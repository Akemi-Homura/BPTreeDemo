//
// Created by quals on 2018/12/24.
//
#include "src/bpt/BPlusTree.h"
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <queue>
typedef void (OperationFun)(BPlusTree *, std::vector<std::pair<int, int> >);

typedef std::vector<std::pair<int, int> > (DataGenerator)(int size);

std::vector<std::pair<int, int>> fill_random_num(int size);

std::vector<std::pair<int, int>> fill_ordered_num(int size);

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg);

void InsertFunc(BPlusTree *tree, std::vector<std::pair<int, int> > data);

void output_node_recursive(BPlusNode *root);

void output_single_node(BPlusNode *node, int layer, int index);

void pr_times(const char *msg, clock_t real, struct tms *tmsstart, struct tms *tmsend);


int main(){
    const int size = 10000000;
    const int order = 5;
    char msg[100];
    sprintf(msg, "Insert %d Ordered integer", size);
    TestFunc(fill_ordered_num, InsertFunc, size, order, msg);
}

std::vector<std::pair<int, int>> fill_random_num(int size) {
    std::vector<int> v;
    std::vector<std::pair<int, int> > data;
    for (int i = 0; i < size; i++) {
        v.emplace_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);

    for (int i = 0; i < size; i++) {
        data.emplace_back(std::pair<int, int>(v[i], v[i]));
    }
    return data;
}


void InsertFunc(BPlusTree *tree, std::vector<std::pair<int, int> > data) {
    for (int i = 0; i < data.size(); i++) {
        tree->Insert(data[i].first, data[i].second);
    }
}

std::vector<std::pair<int, int>> fill_ordered_num(int size) {
    std::vector<std::pair<int, int> > data;
    for (int i = 0; i < size; i++) {
        data.emplace_back(std::pair<int, int>(i, i));
    }
    return data;
}

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg) {
    BPlusTree *tree = new BPlusTree(order);

    std::vector<std::pair<int, int> > data = data_generator(size);
    struct tms tmsstart, tmsend;
    clock_t start, end;

    start = times(&tmsstart);
    operation_func(tree, data);
    end = times(&tmsend);

    pr_times(msg, end - start, &tmsstart, &tmsend);
}

void pr_times(const char *msg, clock_t real, struct tms *tmsstart, struct tms *tmsend) {
    static long clktck = 0;
    if (clktck == 0) {   /* fetch clock ticks per second first time */
        if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
            perror("sysconf error");
            exit(1);
        }
    }

    printf("====================================\n");
    printf("%s time usage:\n", msg);
    printf(" real: %7.2f\n", real / (double) clktck);
    printf(" user: %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
    printf(" sys:  %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
    printf(" child user: %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
    printf(" child sys:  %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);
    printf("====================================\n");
}

void output_node_recursive(BPlusNode *root) {
    std::queue<std::pair<BPlusNode *, int>> q;
    q.push(std::pair<BPlusNode *, int>(root, 0));
    int index = -1, layer = 0;
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        BPlusNode *now = p.first;
        if (p.second > layer) {
            layer = p.second;
            index = 0;
        } else {
            index++;
        }
        output_single_node(now, layer, index);
        if (now->type_ == Data::kInternal) {
            for (auto t = now->list_->head_; t != nullptr; t = t->next_) {
                q.push(std::pair<BPlusNode *, int>(t->data_.val_.child, layer + 1));
            }
        }
    }
}

void output_single_node(BPlusNode *node, int layer, int index) {
    printf("node_%d_%d %s:\n", layer, index, node->type_ == Data::kInternal ? "Internal" : "Leaf");
    for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
        if (node->type_ == Data::kInternal) {
            printf("%d ", now->data_.key_);
        } else {
            printf("(%d %d) ", now->data_.key_, now->data_.val_.value);
        }
    }
    printf("\n");
}

