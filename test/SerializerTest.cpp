//
// Created by quals on 2018/11/29.
//

#include "src/serializer/SimpleSerializer.h"
#include "src/bpt/BPlusTree.h"
#include "src/serializer/TreeStructureSerializer.h"
#include "src/ordered_list/OrderedLinkList.h"
#include <gtest/gtest.h>

typedef void (FuncTreeCompare)(BPlusTree *, BPlusTree *);

FuncTreeCompare CompareData;

FuncTreeCompare CompareStructure;

void get_next_entry(BPlusNode *&node, ListNode *&entry);

template<class T>
void com_ser_deser(int order, int size, SerializeHelperAbstract<T> *serializer, FuncTreeCompare *Comparator);


void CompareNodeRecursive(BPlusNode *n1, BPlusNode *n2);

TEST(SimpleBPlusTest, SmallData) {
    com_ser_deser(5, 10, new SimpleSerializer(), CompareData);
}

TEST(SimpleBPlusTest, MoreData) {
    com_ser_deser(5, 100000, new SimpleSerializer(), CompareData);
}

TEST(TSSerializerTest, SmallData) {
    com_ser_deser(5, 10, new TreeStructureSerializer(), CompareStructure);
}

TEST(TSSerializerTest, MoreData) {
    com_ser_deser(5, 100000, new TreeStructureSerializer(), CompareStructure);
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

void CompareStructure(BPlusTree *t1, BPlusTree *t2) {
    ASSERT_EQ(t1->size_, t2->size_);
    ASSERT_EQ(t1->order_, t2->order_);
    CompareNodeRecursive(t1->root_, t2->root_);
}

void CompareNodeRecursive(BPlusNode *n1, BPlusNode *n2) {
    ASSERT_EQ(n1->type_, n2->type_);
    ASSERT_EQ(n1->list_->size_, n2->list_->size_);
    auto now1 = n1->list_->head_, now2 = n2->list_->head_;
    for (; now1 != nullptr && now2 != nullptr; now1 = now1->next_, now2 = now2->next_) {
        ASSERT_EQ(now1->data_.key_, now2->data_.key_);
        if (n1->type_ == Data::kInternal) {
            CompareNodeRecursive(now1->data_.val_.child, now2->data_.val_.child);
        } else {
            ASSERT_EQ(now1->data_.val_.value, now2->data_.val_.value);
        }
    }
    ASSERT_EQ(nullptr, now1);
    ASSERT_EQ(nullptr, now2);
}

void get_next_entry(BPlusNode *&node, ListNode *&entry) {
    entry = entry->next_;
    if (entry == nullptr) {
        node = node->right_sibling_;
        if (node != nullptr) entry = node->list_->head_;
    }
}

template<class T>
void com_ser_deser(int order, int size, SerializeHelperAbstract<T> *serializer, FuncTreeCompare *Comparator) {
    const char *filename = "tree.txt";
    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(i, i);
    }

    serializer->Serialize(tree, filename);

    BPlusTree *tree_parse_from_file = new BPlusTree(5);
    serializer->Deserialize(tree_parse_from_file, filename);
    Comparator(tree, tree_parse_from_file);
}
