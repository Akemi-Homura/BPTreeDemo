//
// Created by quals on 2018/11/29.
//

#include "src/serializer/SimpleSerializer.h"
#include "src/bpt/BPlusTree.h"
#include "src/serializer/TreeStructureSerializer.h"
#include "src/ordered_array/OrderedArray.h"
#include <gtest/gtest.h>
#include <src/ordered_array/OrderedArray.h>

typedef void (FuncTreeCompare)(BPlusTree *, BPlusTree *);

FuncTreeCompare CompareData;

FuncTreeCompare CompareStructure;

void get_next_entry(BPlusNode *&node, Data &data, int &index);

template<class T>
void com_ser_deser(int order, int size, SerializeHelperAbstract<T> *serializer, FuncTreeCompare *Comparator);


void CompareNodeRecursive(BPlusNode *n1, BPlusNode *n2);

TEST(SimpleBPlusTest, SmallData) {
    com_ser_deser(5, 10, new SimpleSerializer(), CompareData);
}

TEST(SimpleBPlusTest, MoreData) {
    com_ser_deser(5, 1000000, new SimpleSerializer(), CompareData);
}

TEST(TSSerializerTest, SmallData) {
    com_ser_deser(5, 10, new TreeStructureSerializer(), CompareStructure);
}

TEST(TSSerializerTest, MoreData) {
    com_ser_deser(5, 1000000, new TreeStructureSerializer(), CompareStructure);
}

void CompareData(BPlusTree *t1, BPlusTree *t2) {
    BPlusNode *n1 = t1->GetLeftMostNode();
    BPlusNode *n2 = t2->GetLeftMostNode();
    int in1 = 0, in2 = 0;
    Data d1, d2;
    while (in1 != n1->list_.size_ - 1 && in2 != n2->list_.size_ - 1) {
        get_next_entry(n1, d1, in1);
        get_next_entry(n2, d2, in2);
        EXPECT_EQ(d1.key_, d2.key_);
        EXPECT_EQ(d1.val_.value, d2.val_.value);
    }
    EXPECT_EQ(in1, n1->list_.size_ - 1);
    EXPECT_EQ(in2, n2->list_.size_ - 1);
}

void CompareStructure(BPlusTree *t1, BPlusTree *t2) {
    ASSERT_EQ(t1->size_, t2->size_);
    ASSERT_EQ(t1->order_, t2->order_);
    CompareNodeRecursive(t1->root_, t2->root_);
}

void CompareNodeRecursive(BPlusNode *n1, BPlusNode *n2) {
    ASSERT_EQ(n1->type_, n2->type_);
    ASSERT_EQ(n1->list_.size_, n2->list_.size_);
    for (int i=0;i<n1->list_.size_;i++) {
        ASSERT_EQ(n1->list_[i].key_, n2->list_[i].key_);
        if (n1->type_ == Data::kInternal) {
            CompareNodeRecursive(n1->list_[i].val_.child, n2->list_[i].val_.child);
        } else {
            ASSERT_EQ(n1->list_[i].val_.value, n2->list_[i].val_.value);
        }
    }
}

void get_next_entry(BPlusNode *&node, Data &data, int &index) {
    data = node->list_[index++];
    if (index == node->list_.size_) {
        node = node->right_sibling_;
        if (node != nullptr) {
            index = 0;
        }
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
