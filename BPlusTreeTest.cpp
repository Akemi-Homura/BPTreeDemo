//
// Created by quals on 2018/11/30.
//
#include "BPlusTree.h"
#include <gtest/gtest.h>

class BPlusTreeTest : public ::testing::Test{
protected:
    virtual void SetUp(){
        tree = new BPlusTree(5);
    }

    virtual void TearDown(){
        delete tree;
    }

    BPlusTree* tree;
};

TEST_F(BPlusTreeTest, SimpleTest){
    for(int i=0;i<10;i++){
        tree->Insert(i,i);
    }
    EXPECT_EQ(4, tree->size_);
}