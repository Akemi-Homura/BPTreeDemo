//
// Created by quals on 2018/11/30.
//
#include "src/bpt/BPlusTree.h"
#include <gtest/gtest.h>
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <sys/times.h>
#include <queue>
#include <src/ordered_array/OrderedArray.h>


class BPlusTreeTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        tree = new BPlusTree(5);
    }

    virtual void TearDown() {
        delete tree;
    }

    BPlusTree *tree;
};


typedef void (OperationFun)(BPlusTree *, std::vector<std::pair<int, int> >);

typedef std::vector<std::pair<int, int> > (DataGenerator)(int size);

std::vector<std::pair<int, int>> fill_random_num(int size);

std::vector<std::pair<int, int>> fill_ordered_num(int size);

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg,
              bool save_data_flag = false);

void InsertFunc(BPlusTree *tree, std::vector<std::pair<int, int> > data);

void output_node_recursive(BPlusNode *root);

void output_single_node(BPlusNode *node, int layer, int index);

void pr_times(const char *msg, clock_t real, struct tms *tmsstart, struct tms *tmsend);

void SynthesizeFunc(BPlusTree *tree, std::vector<std::pair<int, int>> data);

void SaveData(std::vector<std::pair<int, int>> data);

TEST_F(BPlusTreeTest, SimpleTest) {
    for (int i = 0; i < 10; i++) {
        tree->Insert(i, i);
        printf("\n\n======================\nAfter insert %d:\n======================\n\n", i);
        output_node_recursive(tree->root_);
    }
    EXPECT_EQ(4, tree->size_);

    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(tree->HasKey(i));
        EXPECT_EQ(i, tree->FindValue(i));
    }

    tree->Remove(1);
    EXPECT_FALSE(tree->HasKey(1));

    tree->Remove(4);
    EXPECT_FALSE(tree->HasKey(4));

    tree->Remove(9);
    EXPECT_FALSE(tree->HasKey(9));
}

TEST_F(BPlusTreeTest, SynthesizeTest) {
    const int size = 1000000;
    const int order = 5;
    char msg[100];
    sprintf(msg, "Synthesize Test, %d data", size);
    TestFunc(fill_random_num, SynthesizeFunc, size, order, msg);
}

TEST(BPlusTreeDebug, SynthesizeDebug) {
    const int size = 15;
    const int order = 5;
    int data[] = {6, 12, 3, 4, 11, 0, 8, 14, 5, 7, 2, 13, 10, 1, 9};

    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(data[i], data[i]);
        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i]);
        output_node_recursive(tree->root_);
        fflush(stdout);
    }

    for (int i = 0; i < size; i++) {
        ASSERT_TRUE(tree->HasKey(data[i]));
        ASSERT_EQ(data[i], tree->FindValue(data[i]));
    }

    for (int i = 0; i < size; i++) {
        tree->Remove(data[i]);
        printf("\n\n======================\nAfter Remove %d:\n======================\n\n", data[i]);
        output_node_recursive(tree->root_);
        fflush(stdout);
        ASSERT_FALSE(tree->HasKey(data[i]));

    }
}

TEST_F(BPlusTreeTest, OrderedInsert) {
    const int size = 10000000;
    const int order = 7;
    char msg[100];
    sprintf(msg, "Insert %d Ordered integer", size);
    TestFunc(fill_ordered_num, InsertFunc, size, order, msg);
}

TEST_F(BPlusTreeTest, RandomInsert) {
    const int size = 10000000;
    const int order = 7;
    char msg[100];
    sprintf(msg, "Insert %d Random integer", size);
    TestFunc(fill_random_num, InsertFunc, size, order, msg);
}

TEST(BPlusTreeDebug, RandomInsertDebug) {
    const int size = 10;
    const int order = 5;

    BPlusTree *tree = new BPlusTree(order);
    std::vector<std::pair<int, int>> data = fill_random_num(size);

    printf("data: ");
    for (int i = 0; i < data.size(); i++) {
        printf("%d%c", data[i].first, " \n"[i == data.size() - 1]);
    }

    for (int i = 0; i < data.size(); i++) {
        tree->Insert(data[i].first, data[i].second);

        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i].first);
        fflush(stdout);
        output_node_recursive(tree->root_);
    }
}

TEST(BPlusTreeDebug, SpecialDataInsertDebug) {
    const int size = 6;
    const int order = 5;
    int data[] = {4, 3, 5, 2, 1, 0};

    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(data[i], data[i]);
        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i]);
        fflush(stdout);
        output_node_recursive(tree->root_);
    }
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

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg,
              bool save_data_flag) {
    BPlusTree *tree = new BPlusTree(order);

    std::vector<std::pair<int, int> > data = data_generator(size);
    if (save_data_flag) {
        SaveData(data);
    }

    struct tms tmsstart, tmsend;
    clock_t start, end;

    start = times(&tmsstart);
    operation_func(tree, data);
    end = times(&tmsend);

    pr_times(msg, end - start, &tmsstart, &tmsend);
}

void SaveData(std::vector<std::pair<int, int>> data) {
    FILE *file = fopen("data.txt", "w");
    fprintf(file, "%lu\n", data.size());
    for (int i = 0; i < data.size(); i++) {
        fprintf(file, "%d%c", data[i].first, ",\n"[i == data.size() - 1]);
    }
    for (int i = 0; i < data.size(); i++) {
        fprintf(file, "%d%c", data[i].second, ",\n"[i == data.size() - 1]);
    }
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
            for (int i = 0; i < now->list_.size_; i++) {
                q.push(std::pair<BPlusNode *, int>(now->list_[i].val_.child, layer + 1));
            }
        }
    }
}

void output_single_node(BPlusNode *node, int layer, int index) {
    printf("node_%d_%d %s:\n", layer, index, node->type_ == Data::kInternal ? "Internal" : "Leaf");
    for (int i = 0; i < node->list_.size_; i++) {
        if (node->type_ == Data::kInternal) {
            printf("%d ", node->list_[i].key_);
        } else {
            printf("(%d %d) ", node->list_[i].key_, node->list_[i].val_.value);
        }
    }
    printf("\n");
}


void SynthesizeFunc(BPlusTree *tree, std::vector<std::pair<int, int>> data) {
    for (auto item:data) {
        tree->Insert(item.first, item.second);
    }

    int size = (int) data.size();

    /* HasKey and FoundData */
    for (int i = 0; i < size; i++) {
        EXPECT_TRUE(tree->HasKey(data[i].first));
        EXPECT_EQ(data[i].second, tree->FindValue(data[i].first));
    }

    /* Remove then Test HasKey */
    for (int i = 0; i < size; i++) {
        tree->Remove(data[i].first);
        ASSERT_FALSE(tree->HasKey(data[i].first));
    }
}