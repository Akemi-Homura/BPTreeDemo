//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSTREE_H
#define BPTDEMO_BPLUSTREE_H

#include <src/ordered_list/OrderedLinkList.h>
#include "BPlusNode.h"
#include <queue>
#include "src/ordered_list/OrderedLinkList.h"
#include <exception>
#include <stdexcept>
#include <pthread.h>

class BPlusTree {
public:
    int order_;
    BPlusNode *root_;
    /* num of nodes */
    int size_;

    explicit BPlusTree(int order);

    void Insert(int key, int value);

    bool HasKey(int key);

    int FindValue(int key);

    BPlusNode *GetLeftMostNode() const;

    void Remove(int key);

    ~BPlusTree();

private:
#ifdef MULTITHREAD
    pthread_rwlock_t lock_;
#endif

    void Split(BPlusNode *node);

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
    do {
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
        node=node->parent_;
    } while(node->list_->size_ == order_);

}

BPlusTree::BPlusTree(int order) {
    this->order_ = order;
    root_ = new BPlusNode(Data::kLeaf);
    size_ = 1;
#ifdef MULTITHREAD
    pthread_rwlock_init(&lock_, nullptr);
#endif
}

void BPlusTree::Insert(int key, int value) {
#ifdef MULTITHREAD
    pthread_rwlock_wrlock(&lock_);
#endif

    BPlusNode *now = root_;
    while (now->type_ != Data::kLeaf) {
        ListNode *entry = now->list_->FindLowerBound(key);
        if (entry == nullptr) {
            entry = now->list_->tail_;
            entry->data_.key_ = key;
        }
        now = entry->data_.val_.child;
    }
    now->list_->Insert(Data(key, value));
    if (now->list_->size_ == order_) {
        Split(now);
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
}

BPlusNode *BPlusTree::GetLeftMostNode() const {
    BPlusNode *leaf_head;
    for (leaf_head = root_; leaf_head->type_ != Data::kLeaf; leaf_head = leaf_head->list_->head_->data_.val_.child);
    return leaf_head;
}

bool BPlusTree::HasKey(int key) {
#ifdef MULTITHREAD
    pthread_rwlock_rdlock(&lock_);
#endif

    BPlusNode *leaf = FindLeaf(key);

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif

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
#ifdef MULTITHREAD
    pthread_rwlock_rdlock(&lock_);
#endif

    static char msg[20];
    BPlusNode *leaf = FindLeaf(key);
    auto entry = leaf->list_->FindEqual(key);

    if (entry == nullptr) {
        sprintf(msg, "The key %d does not exist\n", key);
        throw std::runtime_error(msg);
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
    return entry->data_.val_.value;
}

void BPlusTree::Remove(int key) {
#ifdef MULTITHREAD
    pthread_rwlock_wrlock(&lock_);
#endif

    BPlusNode *leaf = FindLeaf(key);
    RemoveEntry(leaf, key);

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
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
    if (node == root_ || node->list_->size_ >= order_ / 2) {
        if (node->list_->size_ > 0) UpdateParent(node);
        return;
    }
    if (BorrowFromSibling(node)) return;
    if (MergeSibling(node)) return;
    throw std::runtime_error("Can't Remove key");
}

void BPlusTree::RemoveEntry(BPlusNode *node, int key) {
    node->list_->Remove(key);
    UpdateEntryAfterRemove(node);
}

bool BPlusTree::BorrowFromSibling(BPlusNode *node) {
    auto left_sibling = node->entry_in_parent_->pre_ ? node->entry_in_parent_->pre_->data_.val_.child : nullptr;
    auto right_sibling = node->entry_in_parent_->next_ ? node->entry_in_parent_->next_->data_.val_.child : nullptr;
    if (CanBorrow(left_sibling)) {
        node->list_->Insert(left_sibling->list_->tail_->data_);
        left_sibling->list_->Remove(left_sibling->list_->tail_);

        if (node->type_ == Data::kInternal) {
            node->list_->head_->data_.val_.child->parent_ = node;
            node->list_->head_->data_.val_.child->entry_in_parent_ = node->list_->head_;
        }

        UpdateParent(left_sibling);
        return true;
    }

    if (CanBorrow(right_sibling)) {
        node->list_->InsertLast(right_sibling->list_->head_->data_);
        right_sibling->list_->Remove(right_sibling->list_->head_);

        if (node->type_ == Data::kInternal) {
            node->list_->tail_->data_.val_.child->parent_ = node;
            node->list_->tail_->data_.val_.child->entry_in_parent_ = node->list_->tail_;
        }

        UpdateParent(node);
        return true;
    }

    return false;
}

bool BPlusTree::MergeSibling(BPlusNode *node) {
    auto sibling = node->entry_in_parent_->pre_ ? node->entry_in_parent_->pre_->data_.val_.child : nullptr;
    if (sibling == nullptr) {
        sibling = node;
        node = node->entry_in_parent_->next_ ? node->entry_in_parent_->next_->data_.val_.child : nullptr;
        if (node == nullptr) return false;
    }

    size_--;
    node->list_->Insert(sibling->list_);

    if (node->type_ == Data::kInternal) {
        for (auto now = sibling->list_->head_; now != nullptr; now = now->next_) {
            now->data_.val_.child->parent_ = node;
        }
    } else {
        node->left_sibling_ = sibling->left_sibling_;
        if (sibling->left_sibling_ != nullptr) sibling->left_sibling_->right_sibling_ = node;

    }
    /* check if become root */
    if (node->parent_ == root_ && node->parent_->list_->size_ <= order_ / 2) {
        delete root_;
        root_ = node;
        node->parent_ = nullptr;
        node->entry_in_parent_ = nullptr;
        size_--;
    } else {
        UpdateParent(node);
        sibling->list_->head_ = sibling->list_->tail_ = nullptr;
        delete sibling;
        RemoveEntry(node->parent_, node->entry_in_parent_->pre_);
    }
    return true;
}

bool BPlusTree::CanBorrow(BPlusNode *node) {
    return node != nullptr && node->list_->size_ > order_ / 2;
}

BPlusTree::~BPlusTree() {
#ifdef MULTITHREAD
    pthread_rwlock_wrlock(&lock_);
#endif

    std::queue<BPlusNode *> que;
    que.push(root_);
    while (!que.empty()) {
        auto *now = que.front();
        que.pop();
        if (now->type_ == Data::kInternal) {
            for (auto item = now->list_->head_; item; item = item->next_) {
                que.push(item->data_.val_.child);
            }
        }
        delete now;
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
}


#endif //BPTDEMO_BPLUSTREE_H
