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

    bool merge(BPNode<T> *left, BPNode<T> *right);

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
        int index = leaf->FindIndex(key);
        leaf->entries()[index].second = data;
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
        auto iter = entries.begin();
        for (; iter != entries.end(); iter++) {
            if (key <= iter->first) {
                break;
            }
        }
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

    left_entries.insert(left_entries.begin(), origin_entries.begin(), origin_entries.begin() + left_key_size);
    origin_entries.erase(origin_entries.begin(), origin_entries.begin() + left_key_size);
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
        parent->Insert(origin_entries.back().first, (void *) node);
    }
    parent->Insert(left_entries.back().first, (void *) left_node);

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
    while (node->parent() != nullptr && node->parent()->entries().back().first < key) {
        node = node->parent();
        node->entries()[node->KeySize() - 1].first = key;
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
        node->entries()[node->FindIndex(old_key)].first = new_key;
    }
}

template<typename T, typename E>
bool BPTree<T, E>::BorrowFromSibling(BPNode<T> *node, BPNode<T> *sibling, int old_key, bool left) {
    if (sibling == nullptr) return false;
    if (node->parent() != sibling->parent()) return false;
    if (sibling->IsMoreThanHalfFull()) {
        auto &sibling_entries = sibling->entries();
        auto &node_entries = node->entries();

        if (left) {
            node_entries.insert(node_entries.begin(), sibling_entries.back());
            sibling_entries.pop_back();
            UpdateParent(sibling, sibling_entries.back().first, node_entries.front().first);
            ((BPNode<T>*)sibling_entries.front().second)->SetParent(node);
        } else {
            node_entries.push_back(sibling_entries.front());
            sibling_entries.erase(sibling_entries.begin());
            UpdateParent(node, node_entries.back().first, old_key);
            ((BPNode<T>*)node_entries.back().second)->SetParent(node);
        }
        return true;
    }
    return false;
}

template<typename T, typename E>
bool BPTree<T, E>::merge(BPNode<T> *left, BPNode<T> *right) {
    if (left == nullptr || right == nullptr) return false;
    if (left->parent() != right->parent()) return false;

    auto &left_entries = left->entries();
    auto &right_entries = right->entries();

    right_entries.insert(right_entries.begin(), left_entries.begin(), left_entries.end());
    if(left->type() == kInternal) {
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
        RemoveEntry(right->parent(), left_entries.back().first);
    }
    RemoveNode(left);
    return true;
}

template<typename T, typename E>
void BPTree<T, E>::RemoveEntry(BPNode<T> *node, T key) {

    node->Remove(key);
    UpdateParent(node, node->entries().back().first, key);

    if(node->type() == kLeaf && node == root_)return;
    if (node->IsHalfFull())return;
    if (BorrowFromSibling(node, node->pre(), key, true)) return;
    if (BorrowFromSibling(node, node->next(), node->KeySize()==0?key:node->entries().back().first, false)) return;
    if (merge(node->pre(), node)) return;
    if (merge(node, node->next())) return;
    RemoveKeyFailed<T>(key);
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
