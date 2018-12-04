//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_BPLUSNODE_H
#define BPTDEMO_BPLUSNODE_H

#include "src/ordered_list/OrderedLinkList.h"

class BPlusNode {
public:
    OrderedLinkList *list_;
    BPlusNode *left_sibling_;
    BPlusNode *right_sibling_;
    BPlusNode *parent_;
    ListNode *entry_in_parent_;
    int index;
    Data::DataType type_;

    BPlusNode(Data::DataType type) {
        list_ = new OrderedLinkList;
        type_ = type;
        left_sibling_ = right_sibling_ = parent_ = nullptr;
        parent_ = nullptr;
    }
    
};

#endif //BPTDEMO_BPLUSNODE_H
