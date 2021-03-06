//
// Created by Dio Joyce on 2018/10/23.
//

#ifndef MY_GTEST_PKGCONFIG_BPNODE_H
#define MY_GTEST_PKGCONFIG_BPNODE_H

#include <vector>
#include <iostream>
#include "BPTHelper.h"

enum BPNodeType {
    kLeaf, kInternal
};

template <typename T>
class BPNode {
public:
//    using iter_type=typename std::vector<std::pair<T,void*>>::iterator;

    explicit BPNode(int _order, BPNodeType _type);

    BPNodeType type() const;

    int order() const;

    int KeySize() const;

    int MaxEntryNum() const;

    int MinEntryNum() const;

    bool IsFull() const;

    bool IsHalfFull() const;

    bool IsMoreThanHalfFull() const;

    bool HasKey(T key) const;

    std::vector<std::pair<T, void *>> & entries();

    void Insert(T key,void*);

    void Remove(T key);

    void SetParent(BPNode<T>* node);

    void SetPre(BPNode<T>*  node);

    void SetNext(BPNode<T>* node);

    void* FindValue(T key)const {
        if(!HasKey(key)){
            NoSuchKeyError(key);
        }
        return entries_[FindIndex(key)].second;
    }

    int FindIndex(T key) const{
        int index;
        for(index=0;index<entries_.size();index++){
            if(key <= entries_[index].first){
                break;
            }
        }
        return index;
    }

    BPNode<T>* parent();

    BPNode<T>* pre();

    BPNode<T>* next();

private:
    const int order_;
    const BPNodeType type_;
    const int max_entry_num_;
    const int min_entry_num_;

    BPNode<T>* parent_ = nullptr;
    BPNode<T>* pre_ = nullptr;
    BPNode<T>* next_ = nullptr;
    std::vector<std::pair<T,void*> > entries_;
};


template<typename T>
BPNode<T>::BPNode(int _order, BPNodeType _type):order_(_order), type_(_type)
        ,max_entry_num_(order_), min_entry_num_((order_+1) / 2){

}

template<typename T>
BPNodeType BPNode<T>::type() const {
    return type_;
}

template<typename T>
int BPNode<T>::order() const {
    return order_;
}

template<typename T>
int BPNode<T>::KeySize() const {
    return (int)entries_.size();
}

template<typename T>
bool BPNode<T>::IsFull() const {
    return KeySize() >= max_entry_num_;
}

template<typename T>
bool BPNode<T>::IsHalfFull() const {
    return KeySize() >= order_ / 2;
}

template<typename T>
bool BPNode<T>::IsMoreThanHalfFull() const {
    return KeySize() > order_ / 2;
}

template<typename T>
void BPNode<T>::Insert(T key, void * value) {
    auto iter = entries_.begin();
    for(;iter!=entries_.end() && key > iter->first; iter++);
    entries_.insert(iter, std::pair<T,void*>(key,value));
}

template<typename T>
void BPNode<T>::Remove(T key) {
    if(!HasKey(key)){
        NoSuchKeyError(key);
    }
    entries_.erase(FindIndex(key) + entries_.begin());
}


template<typename T>
BPNode<T> *BPNode<T>::parent() {
    return parent_;
}

template<typename T>
BPNode<T> *BPNode<T>::pre() {
    return pre_;
}

template<typename T>
BPNode<T> *BPNode<T>::next() {
    return next_;
}

template<typename T>
bool BPNode<T>::HasKey(T key) const {
    auto iter = FindIndex(key) + entries_.begin();
    return iter != entries_.end() && iter->first == key;
}

template<typename T>
void BPNode<T>::SetParent(BPNode<T> *node) {
    parent_ = node;
}

template<typename T>
void BPNode<T>::SetPre(BPNode<T> *node) {
    pre_ = node;
}

template<typename T>
void BPNode<T>::SetNext(BPNode<T> *node) {
    next_ = node;
}

template<typename T>
std::vector<std::pair<T, void *>> & BPNode<T>::entries() {
    return entries_;
}

template<typename T>
int BPNode<T>::MaxEntryNum() const {
    return max_entry_num_;
}

template<typename T>
int BPNode<T>::MinEntryNum() const {
    return min_entry_num_;
}

#endif //MY_GTEST_PKGCONFIG_BPNODE_H
