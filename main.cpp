#include <iostream>
#include <sys/time.h>
#include "BPlusTree.h"
#include "OrderedLinkList.h"
#include <queue>

//#include "BPNode.h"
//#include "BPTree.h"
//
//long get_ms_time(timeval t) {
//    return t.tv_sec * 1000000 + t.tv_usec;
//}
//
//void num_to_str(long v, char *res, int len) {
//    sprintf(res, "%.*lfs", len, v * 1.0 / 1e6);
//}
//
//void output_time_spend(timeval st, timeval ed, char *res) {
//    long st_ms = get_ms_time(st);
//    long ed_ms = get_ms_time(ed);
//
//    long diff_time = ed_ms - st_ms;
//    num_to_str(diff_time, res, 6);
//}
void output_node_recursive(BPlusNode *root);

void output_single_node(BPlusNode *node, int layer, int index);

int main() {
//    std::vector<int *> val;
//    for (int i = 0; i < 10000000; i++) {
//        val.push_back(new int(i));
//    }
//    BPTree<int, int *> t = BPTree<int, int *>(5);
//
//    char res[20];
//    struct timeval st, ed;
//    gettimeofday(&st, NULL);
//    for (int i = 0; i < 10000000; i++) {
//        t.insert(i, val[i]);
//    }
//    gettimeofday(&ed, NULL);
//    output_time_spend(st, ed, res);
//    printf("spend time: %s\n", res);
    BPlusTree *tree = new BPlusTree(5);
    for (int i = 0; i < 10000000; i++) {
        tree->Insert(i, i);
//        printf("\n\n======================\nAfter insert %d:\n======================\n\n", i);
//        output_node_recursive(tree->root_);
    }
    return 0;
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
        if (now->type_ == kInternal) {
            for (auto t = now->list_->head_; t != nullptr; t = t->next_) {
                q.push(std::pair<BPlusNode *, int>(t->data_.val_.child, layer + 1));
            }
        }
    }
}

void output_single_node(BPlusNode *node, int layer, int index) {
    printf("node_%d_%d %s:\n", layer, index, node->type_ == kInternal ? "Internal" : "Leaf");
    for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
        if (node->type_ == kInternal) {
            printf("%d ", now->data_.key_);
        } else {
            printf("(%d %d) ", now->data_.key_, now->data_.val_.value);
        }
    }
    printf("\n");
}