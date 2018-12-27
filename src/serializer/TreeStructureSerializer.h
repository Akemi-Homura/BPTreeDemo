//
// Created by quals on 2018/11/29.
//

#ifndef BPTDEMO_TREESTRUCTSERIALIZER_H
#define BPTDEMO_TREESTRUCTSERIALIZER_H

#include "src/bpt/BPlusTree.h"
#include "SerializeHelperAbstract.h"
#include "src/ordered_array/OrderedArray.h"
#include <queue>
#include <src/ordered_array/OrderedArray.h>

class TreeStructureSerializer : public SerializeHelperAbstract<BPlusTree> {
public:


    bool Serialize(const BPlusTree *tree, const char *filename) override;

    bool Deserialize(BPlusTree *tree, const char *filename) override;

private:
    static const int TInternal = 0;
    static const int TLeaf = 1;

    BPlusNode *ReadNode(FILE *fp, int order);
};


bool TreeStructureSerializer::Serialize(const BPlusTree *tree, const char *filename) {
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d %d\n", tree->order_, tree->size_);
    std::queue<BPlusNode *> que;
    que.push(tree->root_);
    while (!que.empty()) {
        BPlusNode *now = que.front();
        que.pop();
        fprintf(fp, "%d %d\n", now->list_.size_, now->type_ == Data::kInternal ? TInternal : TLeaf);
        for (int j = 0; j < now->list_.size_; j++) {
            if (now->type_ == Data::kInternal) {
                fprintf(fp, "%d%c", now->list_[j].key_, " \n"[j == now->list_.size_ - 1]);
                que.push(now->list_[j].val_.child);
            } else {
                fprintf(fp, "(%d,%d)%c", now->list_[j].key_, now->list_[j].val_.value,
                        " \n"[j == now->list_.size_ - 1]);
            }
        }
    }
    fclose(fp);
    return true;
}

bool TreeStructureSerializer::Deserialize(BPlusTree *tree, const char *filename) {
    FILE *fp = fopen(filename, "r");
    int order, size;
    fscanf(fp, "%d %d", &order, &size);
    tree->order_ = order;
    tree->size_ = size;
    BPlusNode *root = ReadNode(fp, order);
    tree->root_ = root;

    std::queue<BPlusNode *> que;
    que.push(root);
    BPlusNode *leaf_p = nullptr;
    while (!que.empty()) {
        const int size = (int)que.size();
        for (int i = 0; i < size; i++) {
            BPlusNode *now = que.front();
            que.pop();
            if (now->type_ == Data::kInternal) {
                for (int j = 0; j < now->list_.size_; j++) {
                    auto *node = ReadNode(fp, order);
                    now->list_[j].val_.child = node;
                    if (node->type_ == Data::kInternal) {
                        que.push(node);
                    } else {
                        if (leaf_p != nullptr) {
                            leaf_p->right_sibling_ = node;
                            node->left_sibling_ = leaf_p;
                        }
                        leaf_p = node;
                    }
                }
            }
        }
    }
    return true;
}

BPlusNode *TreeStructureSerializer::ReadNode(FILE *fp, int order) {
    int type, size;
    fscanf(fp, "%d %d\n", &size, &type);
    auto *node = new BPlusNode(type == TInternal ? Data::kInternal : Data::kLeaf, order);
    node->list_.size_ = size;
    for (int i = 0; i < node->list_.size_; i++) {
        if (type == TInternal) {
            int key;
            if (fscanf(fp, "%d", &key) == -1) {
                throw std::runtime_error("fscanf failed");
            }
            node->list_[i].key_ = key;
        } else {
            int key, value;
            fscanf(fp, i == node->list_.size_ - 1 ? "(%d,%d)\n" : "(%d,%d) ", &key, &value);
            node->list_[i] = Data(key, value);
        }
    }
    return node;
}


#endif //BPTDEMO_TREESTRUCTSERIALIZER_H
