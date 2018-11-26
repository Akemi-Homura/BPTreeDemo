//
// Created by Dio Joyce on 2018/10/23.
//

# include "BPNode.h"
# include "gtest/gtest.h"

class BPNodeTest : public ::testing::Test {
protected:
    virtual void SetUp() override {
        n0_ = new BPNode<int>(5, kInternal);
        n1_ = new BPNode<int>(3, kLeaf);

        a = new int(1);
        b = new int(2);
        c = new int(3);

        n0_->Insert(1, (void *) a);
        n0_->Insert(5, (void *) b);
        n0_->Insert(8, (void *) c);
    }

    virtual void TearDown() override {
        delete (n0_);
        delete (n1_);
        delete (a);
        delete (b);
        delete (c);
    }

    int *a, *b, *c;
    BPNode<int> *n0_;
    BPNode<int> *n1_;
};

TEST_F(BPNodeTest, Constrantor) {
    EXPECT_EQ(kInternal, n0_->type());
    EXPECT_EQ(kLeaf, n1_->type());

    EXPECT_EQ(5, n0_->order());
    EXPECT_EQ(3, n1_->order());
}

TEST_F(BPNodeTest, SimpleTest) {
    EXPECT_TRUE(n0_->HasKey(1));
    EXPECT_TRUE(n0_->HasKey(5));
    EXPECT_TRUE(n0_->HasKey(8));

    EXPECT_FALSE(n0_->HasKey(0));

    EXPECT_EQ(3, n0_->KeySize());
    EXPECT_EQ(0, n1_->KeySize());

    EXPECT_EQ(a, (int *) n0_->FindValue(1));
    EXPECT_EQ(b, (int *) n0_->FindValue(5));
    EXPECT_EQ(c, (int *) n0_->FindValue(8));

    EXPECT_EQ(1, *(int *) n0_->FindValue(1));
    EXPECT_EQ(2, *(int *) n0_->FindValue(5));
    EXPECT_EQ(3, *(int *) n0_->FindValue(8));

    // remove
    n0_->Remove(1);
    n0_->Remove(5);
    n0_->Remove(8);
    EXPECT_FALSE(n0_->HasKey(1));
    EXPECT_FALSE(n0_->HasKey(5));
    EXPECT_FALSE(n0_->HasKey(8));
}