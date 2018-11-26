//
// Created by Dio Joyce on 2018/10/24.
//

#include <string>
#include "BPTree.h"
#include "gtest/gtest.h"

class BPTreeFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        t0_ = new BPTree<int, int *>(4);


        for (int i = 0; i < 10; i++) {
            val[i] = new int(num[i]);
            t0_->insert(key[i], val[i]);
        }

    }

    int key[10] = {1, 2, 3, 5, 6, 8, 9, 11, 13, 15};
    int num[10] = {4, 5, 1, 3, 8, 3, 2, 44, 23, 19};
    int *val[10];

    BPTree<int, int *> *t0_;
};

TEST_F(BPTreeFixture, SimpleTest) {
    // after insert
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(t0_->HasKey(key[i]));
        // address equal
        EXPECT_EQ(val[i], t0_->FindData(key[i]));
        // value not be changed
        EXPECT_EQ(num[i], *t0_->FindData(key[i]));
    }

    // remove all
    for (int i = 0; i < 10; i++) {
        t0_->remove(key[i]);
        EXPECT_FALSE(t0_->HasKey(key[i]));
    }

}