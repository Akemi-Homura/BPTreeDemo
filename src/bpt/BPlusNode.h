//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSNODE_H
#define BPTDEMO_BPLUSNODE_H

#include "src/ordered_array/OrderedArray.h"

class BPlusNode {
public:
    OrderedArray list_;
    BPlusNode *left_sibling_;
    BPlusNode *right_sibling_;
//    int index;
    Data::DataType type_;
    int order_;

    explicit BPlusNode(Data::DataType type, int order);

    ~BPlusNode();
};

BPlusNode::BPlusNode(Data::DataType type, int order) :list_(order), order_(order){
    type_ = type;
    left_sibling_ = right_sibling_ =  nullptr;
}

BPlusNode::~BPlusNode() {
}

#endif //BPTDEMO_BPLUSNODE_H
