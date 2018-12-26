//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSTREE_H
#define BPTDEMO_BPLUSTREE_H

#include <src/ordered_array/OrderedArray.h>
#include "BPlusNode.h"
#include <queue>
#include "src/ordered_array/OrderedArray.h"
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

    void Remove(int key);

    bool HasKey(int key);

    int FindValue(int key);

    BPlusNode *GetLeftMostNode() const;


    ~BPlusTree();

private:
#ifdef MULTITHREAD
    pthread_rwlock_t lock_;
#endif

    void Insert(int key, int value, BPlusNode *now);

    void Remove(int key, BPlusNode *now);

    void Split(BPlusNode *fa, int child_index);

    BPlusNode *FindLeaf(int key);

    bool BorrowFromSibling(BPlusNode *fa, int child_index);

    bool MergeSibling(BPlusNode *fa, int child_index);

    bool CanBorrow(BPlusNode *node);
};

void BPlusTree::Split(BPlusNode *fa, int child_index) {
    size_++;
    auto *node = fa->list_[child_index].val_.child;
    int left_key_size = (node->list_.size_ + 1) / 2;

    auto *right_node = new BPlusNode(node->type_, order_);
    node->list_.Split(left_key_size, &right_node->list_);

    fa->list_.InsertAt(Data(node->list_.back().key_, node), child_index);
    fa->list_[child_index + 1].val_.child = right_node;

    if (node->type_ == Data::kLeaf) {
        right_node->right_sibling_ = node->right_sibling_;
        right_node->left_sibling_ = node;
        if (node->right_sibling_ != nullptr) {
            node->right_sibling_->left_sibling_ = right_node;
        }
        node->right_sibling_ = right_node;
    }
}

BPlusTree::BPlusTree(int order) {
    this->order_ = order;
    root_ = new BPlusNode(Data::kLeaf, order_);
    size_ = 1;
#ifdef MULTITHREAD
    pthread_rwlock_init(&lock_, nullptr);
#endif
}

void BPlusTree::Insert(int key, int value, BPlusNode *now) {
#ifdef MULTITHREAD
    pthread_rwlock_wrlock(&lock_);
#endif
    if (now->type_ != Data::kLeaf) {
        int index = now->list_.FindLowerBound(key);
        if (index == now->list_.size_) {
            index = now->list_.size_ - 1;
            now->list_[index].key_ = key;
        }
        Insert(key, value, now->list_[index].val_.child);
        if (now->list_[index].val_.child->list_.size_ == order_) {
            Split(now, index);
        }
    } else {
        now->list_.Insert(Data(key, value));
    }
    if (now == root_ && now->list_.size_ == order_) {
        size_++;
        root_ = new BPlusNode(Data::kInternal, order_);
        root_->list_.Insert(Data(now->list_.back().key_, now));
        Split(root_, 0);
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
}

void BPlusTree::Remove(int key, BPlusNode *now) {
#ifdef MULTITHREAD
    pthread_rwlock_wrlock(&lock_);
#endif
    if (now->type_ != Data::kLeaf) {
        int index = now->list_.FindLowerBound(key);
        if (index == now->list_.size_) {
            index = now->list_.size_ - 1;
        }
        Remove(key, now->list_[index].val_.child);
        if (now->list_[index].val_.child->list_.size_ < order_ / 2) {
            if (!(BorrowFromSibling(now, index) || MergeSibling(now, index))) {
                std::string error_msg = "Can't remove key";
                error_msg.append(std::to_string(key));
                throw std::runtime_error(error_msg);
            }
            if (now == root_ && now->type_ == Data::kInternal && now->list_.size_ == 1) {
                root_ = now->list_[0].val_.child;
                delete now;
            }
        }
    } else {
        now->list_.Remove(key);
    }
#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
}

BPlusNode *BPlusTree::GetLeftMostNode() const {
    BPlusNode *leaf_head;
    for (leaf_head = root_; leaf_head->type_ != Data::kLeaf; leaf_head = leaf_head->list_[0].val_.child);
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

    return leaf->list_.FindEqual(key) != -1;
}

BPlusNode *BPlusTree::FindLeaf(int key) {
    BPlusNode *now = root_;
    while (now->type_ != Data::kLeaf) {
        int index = now->list_.FindLowerBound(key);
        if (index == now->list_.size_) index = now->list_.size_ - 1;
        now = now->list_[index].val_.child;
    }
    return now;
}

int BPlusTree::FindValue(int key) {
#ifdef MULTITHREAD
    pthread_rwlock_rdlock(&lock_);
#endif

    static char msg[20];
    BPlusNode *leaf = FindLeaf(key);
    int index = leaf->list_.FindEqual(key);

    if (index == -1) {
        sprintf(msg, "The key %d does not exist\n", key);
        throw std::runtime_error(msg);
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
    return leaf->list_[index].val_.value;
}

bool BPlusTree::BorrowFromSibling(BPlusNode *fa, int child_index) {
    auto *node = fa->list_[child_index].val_.child;
    auto *left_sibling = child_index > 0 ? fa->list_[child_index - 1].val_.child : nullptr;
    auto *right_sibling = child_index < fa->list_.size_ - 1 ? fa->list_[child_index + 1].val_.child : nullptr;
    if (CanBorrow(left_sibling)) {
        node->list_.Insert(left_sibling->list_.back());
        left_sibling->list_.RemoveAt(left_sibling->list_.size_ - 1);
        fa->list_[child_index - 1].key_ = left_sibling->list_.back().key_;
        return true;
    }

    if (CanBorrow(right_sibling)) {
        node->list_.Insert(right_sibling->list_[0]);
        right_sibling->list_.RemoveAt(0);
        fa->list_[child_index].key_ = node->list_.back().key_;
        return true;
    }

    return false;
}

bool BPlusTree::MergeSibling(BPlusNode *fa, int child_index) {
    auto *node = fa->list_[child_index].val_.child;
    auto *sibling = child_index < fa->list_.size_ - 1 ? fa->list_[child_index + 1].val_.child : nullptr;

    if (sibling == nullptr) {
        sibling = node;
        node = child_index > 0 ? fa->list_[child_index - 1].val_.child : nullptr;
        if (node == nullptr) return false;
        --child_index;
    }

    size_--;
    node->list_.MergeAppend(&sibling->list_);
    fa->list_[child_index].key_ = node->list_.back().key_;
    fa->list_.RemoveAt(child_index + 1);

    if (node->type_ == Data::kLeaf) {
        node->right_sibling_ = sibling->right_sibling_;
        if (sibling->right_sibling_ != nullptr) sibling->right_sibling_->left_sibling_ = node;

    }

    return true;
}

bool BPlusTree::CanBorrow(BPlusNode *node) {
    return node != nullptr && node->list_.size_ > order_ / 2;
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
            for (int i = 0; i < now->list_.size_; i++) {
                que.push(now->list_[i].val_.child);
            }
        }
        delete now;
    }

#ifdef MULTITHREAD
    pthread_rwlock_unlock(&lock_);
#endif
}

void BPlusTree::Insert(int key, int value) {
    Insert(key, value, root_);
}

void BPlusTree::Remove(int key) {
    Remove(key, root_);
}


#endif //BPTDEMO_BPLUSTREE_H
