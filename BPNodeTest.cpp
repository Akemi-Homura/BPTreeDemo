//
// Created by Dio Joyce on 2018/10/23.
//

# include "BPNode.h"
# include "gtest/gtest.h"

class BPNodeTest : public ::testing::Test{
protected:
    virtual void SetUp() override {
        n0_ = new BPNode<int>(5,kInternal);
        n1_ = new BPNode<int>(3,kLeaf);

        a = new int(1);
        b = new int(2);
        c = new int(3);

        n0_->Insert(1,(void*)a);
        n0_->Insert(5,(void*)b);
        n0_->Insert(8,(void*)c);
    }

    virtual void TearDown() override {
        delete(n0_);
        delete(n1_);
        delete(a);
        delete(b);
        delete(c);
    }
    int *a,*b,*c;
    BPNode<int>* n0_;
    BPNode<int>* n1_;
};

TEST_F(BPNodeTest, ReleaseData){
    int* tmp = new int(73498574);
    n0_->Insert(233,(void*)tmp);
    delete (int*) n0_->FindValue(233);
}

TEST_F(BPNodeTest, Constuctor){
    EXPECT_EQ(3, n1_->order());
    EXPECT_EQ(kLeaf, n1_->type());
    EXPECT_EQ(0, n1_->KeySize());

    EXPECT_FALSE(n1_->IsMoreThanHalfFull());
    EXPECT_FALSE(n1_->IsHalfFull());
    EXPECT_FALSE(n1_->IsFull());

    EXPECT_EQ(nullptr, n1_->parent());
    EXPECT_EQ(nullptr, n1_->pre());
    EXPECT_EQ(nullptr, n1_->next());
}

TEST_F(BPNodeTest, Insert){
    EXPECT_EQ(5, n0_->order());
    EXPECT_EQ(kInternal, n0_->type());
    EXPECT_EQ(3,n0_->KeySize());

    EXPECT_EQ(nullptr, n0_->parent());
    EXPECT_EQ(nullptr, n0_->pre());
    EXPECT_EQ(nullptr, n0_->next());

    EXPECT_TRUE(n0_->IsMoreThanHalfFull());
    EXPECT_TRUE(n0_->IsHalfFull());
    EXPECT_FALSE(n0_->IsFull());

    EXPECT_EQ(1,*(int*) n0_->FindValue(1));
    EXPECT_EQ(2,*(int*) n0_->FindValue(5));
    EXPECT_EQ(3,*(int*) n0_->FindValue(8));

    EXPECT_TRUE(n0_->HasKey(1));
    EXPECT_TRUE(n0_->HasKey(5));
    EXPECT_TRUE(n0_->HasKey(8));

    EXPECT_FALSE(n0_->HasKey(9));
    EXPECT_FALSE(n0_->HasKey(2));

    n0_->Insert(3, nullptr);
    EXPECT_EQ(3, n0_->entries()[1].first);

    n1_->Insert(103,a);
    EXPECT_EQ(1,n1_->KeySize());
    EXPECT_TRUE(n1_->HasKey(103));
}

TEST_F(BPNodeTest, Remove){
    EXPECT_TRUE(n0_->HasKey(1));
    n0_->Remove(1);
    EXPECT_FALSE(n0_->HasKey(1));
    EXPECT_EQ(2,n0_->KeySize());

    EXPECT_TRUE(n0_->HasKey(5));
    n0_->Remove(5);
    EXPECT_FALSE(n0_->HasKey(5));
    EXPECT_EQ(1,n0_->KeySize());

    EXPECT_TRUE(n0_->HasKey(8));
    n0_->Remove(8);
    EXPECT_FALSE(n0_->HasKey(8));
    EXPECT_EQ(0,n0_->KeySize());
}

TEST_F(BPNodeTest, Set){
    EXPECT_EQ(nullptr, n0_->parent());
    n0_->SetParent(n1_);
    EXPECT_EQ(n1_,n0_->parent());

    auto  pre = new BPNode<int>(5,kLeaf);
    auto  next = new BPNode<int>(5,kLeaf);
    
    EXPECT_EQ(nullptr, n0_->pre());
    EXPECT_EQ(nullptr, n0_->next());
    n0_->SetPre(pre);
    n0_->SetNext(next);
    EXPECT_EQ(pre, n0_->pre());
    EXPECT_EQ(next, n0_->next());
}

TEST_F(BPNodeTest, Entries) {
    EXPECT_EQ(3,n0_->KeySize());
    auto & entries = n0_->entries();
    entries.erase(entries.begin());
    EXPECT_EQ(2, n0_->KeySize());

    EXPECT_EQ(5, n0_->entries().front().first);
    n0_->entries()[0].first = 33;
    EXPECT_EQ(33, n0_->entries()[0].first);
    EXPECT_EQ(33, entries[0].first);
}

TEST_F(BPNodeTest, FindValue){
    EXPECT_EQ(1, *(int*)n0_->FindValue(1));
    EXPECT_EQ(2, *(int*)n0_->FindValue(5));
    EXPECT_EQ(3, *(int*)n0_->FindValue(8));
}

TEST_F(BPNodeTest, FindIndex){
    EXPECT_EQ(0,n0_->FindIndex(1));
    EXPECT_EQ(1,n0_->FindIndex(5));
    EXPECT_EQ(2,n0_->FindIndex(8));
}

TEST_F(BPNodeTest, SiblingAndParent){
    BPNode<int>* pre = new BPNode<int>(5,kLeaf);
    BPNode<int>* next = new BPNode<int>(5,kLeaf);
    BPNode<int>* parent = new BPNode<int>(5,kLeaf);
    
    n0_->SetNext(next);
    n0_->SetPre(pre);
    n0_->SetParent(parent);
    
    EXPECT_EQ(pre, n0_->pre());
    EXPECT_EQ(next, n0_->next());
    EXPECT_EQ(parent, n0_->parent());
}