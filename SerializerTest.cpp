//
// Created by quals on 2018/11/29.
//

#include "SimpleSerializer.h"
#include "BPlusTree.h"
#include "OrderedLinkList.h"
#include <gtest/gtest.h>

void CompareData(BPlusTree *t1, BPlusTree *t2);


void get_next_entry(BPlusNode *&node, ListNode *&entry);

template<class T>
void com_ser_deser(int order, int size, SerializeHelperAbstract<T> *serializer);

TEST(SimpleBPlusTest, SmallData) {
    com_ser_deser(5, 10, new SimpleSerializer());
}

TEST(SimpleBPlusTest, MoreData) {
    com_ser_deser(5, 100000, new SimpleSerializer());
}

void CompareData(BPlusTree *t1, BPlusTree *t2) {
    BPlusNode *t1_leaf_head = t1->GetLeftMostNode();
    BPlusNode *t2_leaf_head = t2->GetLeftMostNode();
    ListNode *t1_now_entry = t1_leaf_head->list_->head_;
    ListNode *t2_now_entry = t2_leaf_head->list_->head_;
    while (t1_leaf_head != nullptr && t2_leaf_head != nullptr) {
        EXPECT_EQ(t1_now_entry->data_.key_, t2_now_entry->data_.key_);
        EXPECT_EQ(t1_now_entry->data_.val_.value, t2_now_entry->data_.val_.value);

        get_next_entry(t1_leaf_head, t1_now_entry);
        get_next_entry(t2_leaf_head, t2_now_entry);
    }
    EXPECT_EQ(t1_leaf_head, nullptr);
    EXPECT_EQ(t2_leaf_head, nullptr);
    EXPECT_EQ(t1_now_entry, nullptr);
    EXPECT_EQ(t2_now_entry, nullptr);
}

void get_next_entry(BPlusNode *&node, ListNode *&entry) {
    entry = entry->next_;
    if (entry == nullptr) {
        node = node->right_sibling_;
        if (node != nullptr) entry = node->list_->head_;
    }
}

template<class T>
void com_ser_deser(int order, int size, SerializeHelperAbstract<T> *serializer) {
    const char *filename = "tree.txt";
    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(i, i);
    }

    serializer->Serialize(tree, filename);

    BPlusTree *tree_parse_from_file = new BPlusTree(5);
    serializer->Deserialize(tree_parse_from_file, filename);
    CompareData(tree, tree_parse_from_file);
}
