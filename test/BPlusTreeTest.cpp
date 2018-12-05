//
// Created by quals on 2018/11/30.
//
#include "src/bpt/BPlusTree.h"
#include <gtest/gtest.h>

class BPlusTreeTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        tree = new BPlusTree(5);
    }

    virtual void TearDown() {
        delete tree;
    }

    BPlusTree *tree;
};

TEST_F(BPlusTreeTest, SimpleTest) {
    for (int i = 0; i < 10; i++) {
        tree->Insert(i, i);
    }
    EXPECT_EQ(4, tree->size_);

    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(tree->HasKey(i));
        EXPECT_EQ(i, tree->FindValue(i));
    }

    tree->Remove(1);
    EXPECT_FALSE(tree->HasKey(1));

    tree->Remove(4);
    EXPECT_FALSE(tree->HasKey(4));

    tree->Remove(9);
    EXPECT_FALSE(tree->HasKey(9));
}