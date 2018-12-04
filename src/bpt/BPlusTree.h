//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSTREE_H
#define BPTDEMO_BPLUSTREE_H

#include "BPlusNode.h"
#include "src/ordered_list/OrderedLinkList.h"

class BPlusTree {
public:
    int order_;
    BPlusNode *root_;
    /* num of nodes */
    int size_;

    BPlusTree(int order);

    void Insert(int key, int value);

    void Split(BPlusNode *node);

    BPlusNode *GetLeftMostNode() const;
};

void BPlusTree::Split(BPlusNode *node) {
    size_++;
    int left_key_size = (node->list_->size_ + 1) / 2;
    BPlusNode *left_node = new BPlusNode(node->type_);
    node->list_->Split(left_key_size, left_node->list_);

    if (node->type_ == Data::kInternal) {
        for (auto now = left_node->list_->head_; now != nullptr; now = now->next_) {
            now->data_.val_.child->parent_ = left_node;
        }
    }

    if (node->parent_ == nullptr) {
        size_++;
        node->parent_ = new BPlusNode(Data::kInternal);
        if (node == root_) {
            root_ = node->parent_;
        }
        node->entry_in_parent_ = node->parent_->list_->Insert(Data(node->list_->head_->data_.key_, node));
    }
    left_node->entry_in_parent_ =
            node->parent_->list_->Insert(Data(left_node->list_->tail_->data_.key_, left_node));

    left_node->parent_ = node->parent_;

    if (node->type_ == Data::kLeaf) {
        left_node->left_sibling_ = node->left_sibling_;
        left_node->right_sibling_ = node;
        if (node->left_sibling_ != nullptr) {
            node->left_sibling_->right_sibling_ = left_node;
        }
        node->left_sibling_ = left_node;
    }

    if (node->parent_->list_->size_ == order_) {
        Split(node->parent_);
    }
}

BPlusTree::BPlusTree(int order) {
    this->order_ = order;
    root_ = new BPlusNode(Data::kLeaf);
    size_ = 1;
}

void BPlusTree::Insert(int key, int value) {
    BPlusNode *now = root_;
    while (now->type_ != Data::kLeaf) {
        ListNode *entry = now->list_->FindFirstBigger(key);
        entry->data_.key_ = key;
        now = entry->data_.val_.child;
    }
    now->list_->Insert(Data(key, value));
    if (now->list_->size_ == order_) {
        Split(now);
    }
}

BPlusNode *BPlusTree::GetLeftMostNode() const {
    BPlusNode *leaf_head;
    for (leaf_head = root_; leaf_head->type_ != Data::kLeaf; leaf_head = leaf_head->list_->head_->data_.val_.child);
    return leaf_head;
}


#endif //BPTDEMO_BPLUSTREE_H
