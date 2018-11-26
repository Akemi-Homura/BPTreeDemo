//
// Created by Dio Joyce on 2018/10/23.
//

#ifndef MY_GTEST_PKGCONFIG_BPTREE_H
#define MY_GTEST_PKGCONFIG_BPTREE_H

# include "BPNode.h"
# include "BPTHelper.h"

template<typename T, typename E>
class BPTree {
public:
    explicit BPTree(int _order);

    void insert(T key, E data);

    void remove(T key);

    bool HasKey(T key);

    BPNode<T> *root();

    E FindData(T key);

    virtual ~BPTree();

private:

    void UpdateParentWhenInsert(BPNode<T> *node, T key);

    void UpdateParent(BPNode<T> *node, T new_key, T old_key);

    bool BorrowFromSibling(BPNode<T> *node, BPNode<T> *sibling, int old_key, bool left);

    void RemoveEntry(BPNode<T> *node, T key);

    void split(BPNode<T> *node);

    void release_data(BPNode<T> *node, T key);

    void RemoveNode(BPNode<T> *node);

    void release_node(BPNode<T> *node);

    bool merge(BPNode<T> *left, BPNode<T> *right, int removed_key);

    BPNode<T> *FindLeaf(T key);

    const int order_;

    BPNode<T> *root_ = nullptr;
};

template<typename T, typename E>
BPTree<T, E>::BPTree(int _order):order_(_order) {

    root_ = new BPNode<T>(order_, kLeaf);

}

template<typename T, typename E>
void BPTree<T, E>::insert(T key, E data) {
    auto leaf = FindLeaf(key);
    if (leaf->HasKey(key)) {
        leaf->entries()[key] = data;
    } else {
        UpdateParentWhenInsert(leaf, key);
        leaf->Insert(key, data);
        if (leaf->IsFull()) {
            split(leaf);
        }
    }
}


template<typename T, typename E>
E BPTree<T, E>::FindData(T key) {
    BPNode<T> *leaf = FindLeaf(key);
    return (E) leaf->FindValue(key);
}

template<typename T, typename E>
BPNode<T> *BPTree<T, E>::FindLeaf(T key) {
    BPNode<T> *res = root_;
    while (res->type() != kLeaf) {
        auto entries = res->entries();
        auto iter = entries.lower_bound(key);
        if (iter == entries.end()) {
            iter--;
        }
        res = (BPNode<T> *) iter->second;
    }
    return res;
}

template<typename T, typename E>
bool BPTree<T, E>::HasKey(T key) {
    BPNode<T> *res = FindLeaf(key);
    return res->HasKey(key);
}

template<typename T, typename E>
void BPTree<T, E>::split(BPNode<T> *node) {
    int L = node->KeySize();
    int left_key_size = (L + 1) / 2;
    auto &origin_entries = node->entries();

    auto *left_node = new BPNode<T>(order_, node->type());
    auto &left_entries = left_node->entries();

    auto it_ = origin_entries.begin();
    for (int i = 0; i < left_key_size; i++) {
        left_entries.insert(*it_);
        it_ = origin_entries.erase(it_);
    }
    if (left_node->type() == kInternal) {
        for (auto item:left_entries) {
            ((BPNode<T> *) item.second)->SetParent(left_node);
        }
    }

    BPNode<T> *parent = node->parent();
    if (parent == nullptr) {
        parent = new BPNode<T>(order_, kInternal);
        node->SetParent(parent);
        if (node == root_) {
            root_ = parent;
        }
        parent->Insert(origin_entries.rbegin()->first, (void *) node);
    }
    parent->Insert(left_entries.rbegin()->first, (void *) left_node);

    left_node->SetParent(parent);
    left_node->SetPre(node->pre());
    left_node->SetNext(node);
    if (node->pre() != nullptr) node->pre()->SetNext(left_node);
    node->SetPre(left_node);

    if (parent->IsFull()) {
        split(parent);
    }
}

template<typename T, typename E>
void BPTree<T, E>::UpdateParentWhenInsert(BPNode<T> *node, T key) {
    while (node->parent() != nullptr && node->parent()->entries().rbegin()->first < key) {
        node = node->parent();
        auto data = node->entries().rbegin()->second;
        node->entries().erase(--node->entries().end());
        node->entries()[key] = data;
    }
}

template<typename T, typename E>
void BPTree<T, E>::remove(T key) {
    auto leaf = FindLeaf(key);
    RemoveEntry(leaf, key);
}

template<typename T, typename E>
void BPTree<T, E>::UpdateParent(BPNode<T> *node, T new_key, T old_key) {
    while (node->parent() != nullptr && node->parent()->HasKey(old_key)) {
        node = node->parent();
        auto data = node->entries()[old_key];
        node->entries().erase(old_key);
        node->entries()[new_key] = data;
    }
}

template<typename T, typename E>
bool BPTree<T, E>::BorrowFromSibling(BPNode<T> *node, BPNode<T> *sibling, int old_key, bool left) {
    if (sibling == nullptr) return false;
    if (node->parent() != sibling->parent()) return false;
    if (sibling->IsMoreThanHalfFull()) {
        std::map<T, void *> &sibling_entries = sibling->entries();
        std::map<T, void *> &node_entries = node->entries();

        if (left) {
            auto sibling_last_iter = sibling_entries.rbegin();
            node_entries.insert(*sibling_last_iter);
            sibling_entries.erase(--sibling_entries.end());
            UpdateParent(sibling, sibling_entries.rbegin()->first, node_entries.begin()->first);
            ((BPNode<T> *) node_entries.begin()->second)->SetParent(node);
        } else {
            auto sibling_first_iter = sibling_entries.begin();
            node_entries.insert(*sibling_first_iter);
            sibling_entries.erase(sibling_first_iter);
            UpdateParent(node, node_entries.rbegin()->first, old_key);
            ((BPNode<T> *) node_entries.rbegin()->second)->SetParent(node);
        }
        return true;
    }
    return false;
}

template<typename T, typename E>
bool BPTree<T, E>::merge(BPNode<T> *left, BPNode<T> *right, int removed_key) {
    if (left == nullptr || right == nullptr) return false;
    if (left->parent() != right->parent()) return false;

    auto &left_entries = left->entries();
    auto &right_entries = right->entries();

    for (auto it_ = left_entries.begin(); it_ != left_entries.end(); it_++) {
        right_entries.insert(*it_);
    }
    if (left->type() == kInternal) {
        for (auto item : left_entries) {
            ((BPNode<T> *) item.second)->SetParent(right);
        }
    }
    right->SetPre(left->pre());
    if (left->pre() != nullptr) left->pre()->SetNext(right);

    if (right->parent() == root_ && root_->KeySize() <= 2) {
        root_ = right;
        RemoveNode(right->parent());
        right->SetParent(nullptr);
    } else {
        RemoveEntry(right->parent(), left_entries.size() == 0 ? removed_key : left_entries.rbegin()->first);
    }
    RemoveNode(left);
    return true;
}

template<typename T, typename E>
void BPTree<T, E>::RemoveEntry(BPNode<T> *node, T key) {

    node->Remove(key);
    if (node->KeySize()) UpdateParent(node, node->entries().rbegin()->first, key);

    if (node->type() == kLeaf && node == root_)return;
    if (node->IsHalfFull())return;
    if (BorrowFromSibling(node, node->pre(), key, true)) return;
    if (BorrowFromSibling(node, node->next(), node->KeySize() == 0 ? key : node->entries().rbegin()->first,
                          false))
        return;
    if (merge(node->pre(), node, key)) return;
    if (merge(node, node->next(), key)) return;
    RemoveEntry(node->parent(), key);
    RemoveNode(node);
}

template<typename T, typename E>
void BPTree<T, E>::release_data(BPNode<T> *node, T key) {
    if (node->type() == kLeaf) {
        void *address = node->FindValue(key);
        delete (E) address;
    } else {
        auto node_to_release = (BPNode<T> *) node->FindValue(key);
        release_node(node_to_release);
    }
}

template<typename T, typename E>
void BPTree<T, E>::release_node(BPNode<T> *node) {
    for (auto item: node->entries()) {
        if (node->type() != kLeaf) {
            auto node_to_release = (BPNode<T> *) item.second;
            release_node(node_to_release);
        }
    }
    delete node;
}

template<typename T, typename E>
void BPTree<T, E>::RemoveNode(BPNode<T> *node) {
    delete node;
}

template<typename T, typename E>
BPNode<T> *BPTree<T, E>::root() {
    return root_;
}

template<typename T, typename E>
BPTree<T, E>::~BPTree() {
    release_node(root_);
}


#endif //MY_GTEST_PKGCONFIG_BPTREE_H
