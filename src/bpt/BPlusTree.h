//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSTREE_H
#define BPTDEMO_BPLUSTREE_H

#include <src/ordered_list/OrderedLinkList.h>
#include "BPlusNode.h"
#include "src/ordered_list/OrderedLinkList.h"
#include <exception>
#include <stdexcept>

class BPlusTree {
public:
    int order_;
    BPlusNode *root_;
    /* num of nodes */
    int size_;

    explicit BPlusTree(int order);

    void Insert(int key, int value);

    void Split(BPlusNode *node);

    bool HasKey(int key);

    int FindValue(int key);

    BPlusNode *GetLeftMostNode() const;

    void Remove(int key);

private:
    BPlusNode *FindLeaf(int key);

    void UpdateParent(BPlusNode *node);

    void RemoveEntry(BPlusNode *node, ListNode *entry);

    void UpdateEntryAfterRemove(BPlusNode *node);

    void RemoveEntry(BPlusNode *node, int key);

    bool BorrowFromSibling(BPlusNode *node);

    bool MergeSibling(BPlusNode *node);

    bool CanBorrow(BPlusNode *node);
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
        node->entry_in_parent_ = node->parent_->list_->Insert(Data(node->list_->tail_->data_.key_, node));
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
        if (entry == nullptr) entry = now->list_->head_;
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

bool BPlusTree::HasKey(int key) {
    BPlusNode *leaf = FindLeaf(key);
    return leaf->list_->FindEqual(key) != nullptr;
}

BPlusNode *BPlusTree::FindLeaf(int key) {
    BPlusNode *now = root_;
    while (now->type_ != Data::kLeaf) {
        auto entry = now->list_->FindLowerBound(key);
        if (entry == nullptr) entry = now->list_->tail_;
        now = entry->data_.val_.child;
    }
    return now;
}

int BPlusTree::FindValue(int key) {
    static char msg[20];
    BPlusNode *leaf = FindLeaf(key);
    auto entry = leaf->list_->FindEqual(key);

    if (entry == nullptr) {
        sprintf(msg, "The key %d does not exist\n", key);
        throw std::runtime_error(msg);
    }
    return entry->data_.val_.value;
}

void BPlusTree::Remove(int key) {
    BPlusNode *leaf = FindLeaf(key);
    RemoveEntry(leaf, key);
}

void BPlusTree::UpdateParent(BPlusNode *node) {
    for (; node->parent_ != nullptr &&
           node->entry_in_parent_->data_.key_ != node->list_->tail_->data_.key_; node = node->parent_) {
        node->entry_in_parent_->data_.key_ = node->list_->tail_->data_.key_;
    }
}

void BPlusTree::RemoveEntry(BPlusNode *node, ListNode *entry) {
    node->list_->Remove(entry);
    UpdateEntryAfterRemove(node);
}

void BPlusTree::UpdateEntryAfterRemove(BPlusNode *node) {
    // update entry_in_parent
    if (node->list_->size_ > 0) UpdateParent(node);
    if (node == root_ || node->list_->size_ >= order_ / 2) return;
    if (BorrowFromSibling(node)) return;
    if (MergeSibling(node)) return;
    throw std::runtime_error("Can't Remove key");
}

void BPlusTree::RemoveEntry(BPlusNode *node, int key) {
    node->list_->Remove(key);
    UpdateParent(node);
}

bool BPlusTree::BorrowFromSibling(BPlusNode *node) {
    auto left_sibling = node->entry_in_parent_->pre_->data_.val_.child;
    auto right_sibling = node->entry_in_parent_->next_->data_.val_.child;
    if (CanBorrow(left_sibling)) {
        node->list_->Insert(left_sibling->list_->tail_->data_);
        left_sibling->list_->Remove(left_sibling->list_->tail_);
        UpdateParent(left_sibling);
        return true;
    }

    if (CanBorrow(right_sibling)) {
        node->list_->InsertLast(right_sibling->list_->head_->data_);
        right_sibling->list_->Remove(right_sibling->list_->head_);
        UpdateParent(right_sibling);
        return true;
    }

    return false;
}

bool BPlusTree::MergeSibling(BPlusNode *node) {
    bool merge_left_flag = true;
    auto selected_sibling = node->entry_in_parent_->pre_->data_.val_.child;
    if (selected_sibling == nullptr) {
        selected_sibling = node->entry_in_parent_->next_->data_.val_.child;
        merge_left_flag = false;
    }

    if (merge_left_flag) {
        node->list_->Insert(selected_sibling->list_);
    } else {
        node->list_->InsertLast(selected_sibling->list_);
    }
    if (node->type_ == Data::kInternal) {
        for (auto now = selected_sibling->list_->head_; now != nullptr; now = now->next_) {
            now->data_.val_.child->parent_ = node;
        }
    } else {
        if (merge_left_flag) {
            node->left_sibling_ = selected_sibling->left_sibling_;
            if (selected_sibling->left_sibling_ != nullptr) selected_sibling->left_sibling_->right_sibling_ = node;
        } else {
            node->right_sibling_ = selected_sibling->right_sibling_;
            if (selected_sibling->right_sibling_ != nullptr) selected_sibling->right_sibling_->left_sibling_ = node;
            node->entry_in_parent_ = selected_sibling->entry_in_parent_;
        }
    }
    //TODO delete selected_sibling

    RemoveEntry(node->parent_, node->entry_in_parent_->pre_);
}

bool BPlusTree::CanBorrow(BPlusNode *node) {
    return node != nullptr && node->list_->size_ > order_ / 2;
}


#endif //BPTDEMO_BPLUSTREE_H
