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
        t1_ = new BPTree<int, std::string *>(5);
        

        for (int i = 0; i < 10; i++) {
            val[i] = new int(num[i]);
            t0_->insert(key[i], val[i]);
        }
    }

    int key[10] = {1, 2, 3, 5, 6, 8, 9, 11, 13, 15};
    int num[10] = {4, 5, 1, 3, 8, 3, 2, 44, 23, 19};
    int *val[10];

    BPTree<int, int *> *t0_;
    BPTree<int, std::string *> *t1_;
};

TEST_F(BPTreeFixture, Constrantor) {
    t1_ = new BPTree<int, std::string *>(5);
    EXPECT_NE(nullptr, t1_->root());
    EXPECT_EQ(5, t1_->root()->order());
    EXPECT_EQ(kLeaf, t1_->root()->type());
}

TEST_F(BPTreeFixture, Destructor) {
    delete t1_;
    delete t0_;
}

TEST_F(BPTreeFixture, Insert){
    auto root = t0_->root();
    EXPECT_EQ(kInternal,root->type());
    EXPECT_EQ(2, root->KeySize());
    EXPECT_EQ(5, root->entries().front().first);
    EXPECT_EQ(15, root->entries()[1].first);

    auto node2_1 = (BPNode<int> *) root->entries().front().second;
    auto node2_2 = (BPNode<int> *) root->entries()[1].second;
    auto node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
    auto node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
    auto node3_3 = (BPNode<int> *) node2_2->entries()[0].second;
    auto node3_4 = (BPNode<int> *) node2_2->entries()[1].second;
    auto node3_5 = (BPNode<int> *) node2_2->entries()[2].second;

    EXPECT_EQ(kInternal, node2_1->type());
    EXPECT_EQ(kInternal, node2_2->type());
    EXPECT_EQ(kLeaf, node3_1->type());
    EXPECT_EQ(kLeaf, node3_2->type());
    EXPECT_EQ(kLeaf, node3_3->type());
    EXPECT_EQ(kLeaf, node3_4->type());
    EXPECT_EQ(kLeaf, node3_5->type());

    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(root, node2_1->parent());
    EXPECT_EQ(root, node2_2->parent());
    EXPECT_EQ(node2_1, node3_1->parent());
    EXPECT_EQ(node2_1, node3_2->parent());
    EXPECT_EQ(node2_2, node3_3->parent());
    EXPECT_EQ(node2_2, node3_4->parent());
    EXPECT_EQ(node2_2, node3_5->parent());

    EXPECT_EQ(nullptr, node2_1->pre());
    EXPECT_EQ(nullptr, node2_2->next());
    EXPECT_EQ(nullptr, node3_1->pre());
    EXPECT_EQ(nullptr, node3_5->next());

    EXPECT_EQ(node2_1, node2_2->pre());
    EXPECT_EQ(node2_2, node2_1->next());

    EXPECT_EQ(node3_1, node3_2->pre());
    EXPECT_EQ(node3_2, node3_1->next());
    EXPECT_EQ(node3_2, node3_3->pre());
    EXPECT_EQ(node3_3, node3_2->next());
    EXPECT_EQ(node3_3, node3_4->pre());
    EXPECT_EQ(node3_4, node3_3->next());
    EXPECT_EQ(node3_4, node3_5->pre());
    EXPECT_EQ(node3_5, node3_4->next());


    EXPECT_EQ(2, node2_1->KeySize());
    EXPECT_EQ(2, node2_1->entries().front().first);
    EXPECT_EQ(5, node2_1->entries().back().first);

    EXPECT_EQ(3, node2_2->KeySize());
    EXPECT_EQ(8, node2_2->entries().front().first);
    EXPECT_EQ(11, node2_2->entries()[1].first);
    EXPECT_EQ(15, node2_2->entries()[2].first);

    EXPECT_EQ(2, node3_1->KeySize());
    EXPECT_EQ(1, node3_1->entries()[0].first);
    EXPECT_EQ(2, node3_1->entries()[1].first);

    EXPECT_EQ(2, node3_2->KeySize());
    EXPECT_EQ(3, node3_2->entries()[0].first);
    EXPECT_EQ(5, node3_2->entries()[1].first);

    EXPECT_EQ(2, node3_3->KeySize());
    EXPECT_EQ(6, node3_3->entries()[0].first);
    EXPECT_EQ(8, node3_3->entries()[1].first);

    EXPECT_EQ(2, node3_4->KeySize());
    EXPECT_EQ(9, node3_4->entries()[0].first);
    EXPECT_EQ(11, node3_4->entries()[1].first);

    EXPECT_EQ(2,node3_5->KeySize());
    EXPECT_EQ(13, node3_5->entries()[0].first);
    EXPECT_EQ(15, node3_5->entries()[1].first);
}

TEST_F(BPTreeFixture, HasKey){
    for(int i=0;i<10;i++){
        EXPECT_TRUE(t0_->HasKey(key[i]));
    }
    for(int i=0;i<10000;i++){
        bool flag = false;
        for(int j=0;j<10;j++){
            if(key[j] == i){
                flag = true;
                break;
            }
        }
        if(flag)continue;
        EXPECT_FALSE(t0_->HasKey(i));
    }
}

TEST_F(BPTreeFixture, FindData){
    for(int i=0;i<10;i++){
        EXPECT_EQ(*val[i], *(int*)t0_->FindData(key[i]));
    }
}

TEST_F(BPTreeFixture, Remove){
    t0_->remove(11);
    auto root = t0_->root();
    EXPECT_EQ(2, root->KeySize());
    EXPECT_EQ(5, root->entries().front().first);
    EXPECT_EQ(15, root->entries()[1].first);

    auto node2_1 = (BPNode<int> *) root->entries().front().second;
    auto node2_2 = (BPNode<int> *) root->entries()[1].second;
    auto node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
    auto node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
    auto node3_3 = (BPNode<int> *) node2_2->entries()[0].second;
    auto node3_4 = (BPNode<int> *) node2_2->entries()[1].second;

    EXPECT_EQ(kInternal, root->type());
    EXPECT_EQ(kInternal, node2_1->type());
    EXPECT_EQ(kInternal, node2_2->type());
    EXPECT_EQ(kLeaf, node3_1->type());
    EXPECT_EQ(kLeaf, node3_2->type());
    EXPECT_EQ(kLeaf, node3_3->type());
    EXPECT_EQ(kLeaf, node3_4->type());

    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(root, node2_1->parent());
    EXPECT_EQ(root, node2_2->parent());
    EXPECT_EQ(node2_1, node3_1->parent());
    EXPECT_EQ(node2_1, node3_2->parent());
    EXPECT_EQ(node2_2, node3_3->parent());
    EXPECT_EQ(node2_2, node3_4->parent());

    EXPECT_EQ(nullptr, node2_1->pre());
    EXPECT_EQ(nullptr, node2_2->next());
    EXPECT_EQ(node2_1, node2_2->pre());
    EXPECT_EQ(node2_2, node2_1->next());

    EXPECT_EQ(nullptr, node3_1->pre());
    EXPECT_EQ(nullptr, node3_4->next());
    EXPECT_EQ(node3_1, node3_2->pre());
    EXPECT_EQ(node3_2, node3_1->next());
    EXPECT_EQ(node3_2, node3_3->pre());
    EXPECT_EQ(node3_3, node3_2->next());
    EXPECT_EQ(node3_3, node3_2->next());
    EXPECT_EQ(node3_3, node3_4->pre());
    EXPECT_EQ(node3_4, node3_3->next());

    EXPECT_EQ(2, node2_1->KeySize());
    EXPECT_EQ(2, node2_1->entries().front().first);
    EXPECT_EQ(5, node2_1->entries().back().first);

    EXPECT_EQ(2, node2_2->KeySize());
    EXPECT_EQ(9, node2_2->entries().front().first);
    EXPECT_EQ(15, node2_2->entries()[1].first);

    EXPECT_EQ(2, node3_1->KeySize());
    EXPECT_EQ(1, node3_1->entries()[0].first);
    EXPECT_EQ(2, node3_1->entries()[1].first);

    EXPECT_EQ(2, node3_2->KeySize());
    EXPECT_EQ(3, node3_2->entries()[0].first);
    EXPECT_EQ(5, node3_2->entries()[1].first);

    EXPECT_EQ(3, node3_3->KeySize());
    EXPECT_EQ(6, node3_3->entries()[0].first);
    EXPECT_EQ(8, node3_3->entries()[1].first);
    EXPECT_EQ(9, node3_3->entries()[2].first);

    EXPECT_EQ(2, node3_4->KeySize());
    EXPECT_EQ(13, node3_4->entries()[0].first);
    EXPECT_EQ(15, node3_4->entries()[1].first);

    t0_ = new BPTree<int,int*>(4);
    for(int i=0;i<4;i++){
        t0_->insert(key[i],val[i]);
    }
    root = t0_->root();
    node2_1 = (BPNode<int> *) root->entries().front().second;
    node2_2 = (BPNode<int> *) root->entries()[1].second;
    EXPECT_EQ(2,root->KeySize());
    EXPECT_EQ(2, root->entries()[0].first);
    EXPECT_EQ(5, root->entries()[1].first);

    EXPECT_EQ(2, node2_1->KeySize());
    EXPECT_EQ(1, node2_1->entries()[0].first);
    EXPECT_EQ(2, node2_1->entries()[1].first);

    EXPECT_EQ(2, node2_2->KeySize());
    EXPECT_EQ(3, node2_2->entries()[0].first);
    EXPECT_EQ(5, node2_2->entries()[1].first);

    EXPECT_EQ(node2_1, node2_2->pre());
    EXPECT_EQ(node2_2, node2_1->next());
    EXPECT_EQ(root, node2_1->parent());

    t0_->remove(2);
    root = t0_->root();

    /*
     * parent
     */
    EXPECT_EQ(nullptr, root->parent());

    /*
     * type
     */
    EXPECT_EQ(kLeaf, root->type());

    /*
     * structure
     */
    EXPECT_EQ(3, root->KeySize());
    EXPECT_EQ(1, root->entries()[0].first);
    EXPECT_EQ(3, root->entries()[1].first);
    EXPECT_EQ(5, root->entries()[2].first);

    EXPECT_EQ(nullptr, root->pre());
    EXPECT_EQ(nullptr, root->next());
}

TEST_F(BPTreeFixture, RemoveAll){
    for(int i=0;i<10;i++){
        t0_->remove(key[i]);
    }
}

TEST_F(BPTreeFixture, Remove2){
    t0_->remove(1);
    auto root = t0_->root();
    auto node2_1 = (BPNode<int>*)root->entries()[0].second;
    auto node3_1 = (BPNode<int>*)node2_1->entries()[0].second;
    auto node3_2 = (BPNode<int>*)node2_1->entries()[1].second;

    EXPECT_EQ(8, root->entries()[0].first);
    EXPECT_EQ(5, node2_1->entries()[0].first);
    EXPECT_EQ(8, node2_1->entries()[1].first);

    EXPECT_EQ(3, node3_1->KeySize());
    EXPECT_EQ(2, node3_1->entries()[0].first);
    EXPECT_EQ(3, node3_1->entries()[1].first);
    EXPECT_EQ(5, node3_1->entries()[2].first);

    EXPECT_EQ(2, node3_2->KeySize());
    EXPECT_EQ(6,node3_2->entries()[0].first);
    EXPECT_EQ(8,node3_2->entries()[1].first);

    EXPECT_EQ(node2_1, node3_2->parent());

    t0_->remove(2);
    EXPECT_EQ(2, node3_1->KeySize());
    EXPECT_EQ(3, node3_1->entries()[0].first);
    EXPECT_EQ(5, node3_1->entries()[1].first);

    t0_->remove(3);
    root = t0_->root();
    node2_1 = (BPNode<int>*)root->entries()[0].second;
    auto node2_2 = (BPNode<int>*)root->entries()[1].second;
    auto node2_3 = (BPNode<int>*)root->entries()[2].second;

    EXPECT_EQ(root, node2_1->parent());
    EXPECT_EQ(root, node2_2->parent());
    EXPECT_EQ(root, node2_3->parent());

    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(3, root->KeySize());
    EXPECT_EQ(8, root->entries()[0].first);
    EXPECT_EQ(11, root->entries()[1].first);
    EXPECT_EQ(15, root->entries()[2].first);

    EXPECT_EQ(3, node2_1->KeySize());
    EXPECT_EQ(5, node2_1->entries()[0].first);
    EXPECT_EQ(6, node2_1->entries()[1].first);
    EXPECT_EQ(8, node2_1->entries()[2].first);

    EXPECT_EQ(2, node2_2->KeySize());
    EXPECT_EQ(9, node2_2->entries()[0].first);
    EXPECT_EQ(11, node2_2->entries()[1].first);

    EXPECT_EQ(2, node2_3->KeySize());
    EXPECT_EQ(13, node2_3->entries()[0].first);
    EXPECT_EQ(15, node2_3->entries()[1].first);

    t0_->remove(5);
    EXPECT_EQ(2, node2_1->KeySize());
    EXPECT_EQ(6, node2_1->entries()[0].first);
    EXPECT_EQ(8, node2_1->entries()[1].first);

    t0_->remove(6);
    node2_1 = (BPNode<int>*)root->entries()[0].second;
    node2_2 = (BPNode<int>*)root->entries()[1].second;
    EXPECT_EQ(2, root->KeySize());
    EXPECT_EQ(11, root->entries()[0].first);
    EXPECT_EQ(15, root->entries()[1].first);

    EXPECT_EQ(3, node2_1->KeySize());
    EXPECT_EQ(8, node2_1->entries()[0].first);
    EXPECT_EQ(9, node2_1->entries()[1].first);
    EXPECT_EQ(11, node2_1->entries()[2].first);

    EXPECT_EQ(2, node2_2->KeySize());
    EXPECT_EQ(13, node2_2->entries()[0].first);
    EXPECT_EQ(15, node2_2->entries()[1].first);

    t0_->remove(8);
    EXPECT_EQ(2, node2_1->KeySize());
    EXPECT_EQ(9, node2_1->entries()[0].first);
    EXPECT_EQ(11, node2_1->entries()[1].first);

    t0_->remove(9);
    root = t0_->root();
    EXPECT_EQ(kLeaf, root->type());
    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(3,root->KeySize());
    EXPECT_EQ(11, root->entries()[0].first);
    EXPECT_EQ(13, root->entries()[1].first);
    EXPECT_EQ(15, root->entries()[2].first);

    t0_->remove(11);
    EXPECT_EQ(kLeaf, root->type());
    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(2,root->KeySize());
    EXPECT_EQ(13, root->entries()[0].first);
    EXPECT_EQ(15, root->entries()[1].first);

    t0_->remove(13);
    EXPECT_EQ(kLeaf, root->type());
    EXPECT_EQ(nullptr, root->parent());
    EXPECT_EQ(1,root->KeySize());
    EXPECT_EQ(15, root->entries()[0].first);
}

//TEST_F(BPTreeFixture, Insert) {
//    t0_ = new BPTree<int, int *>(4);
//    auto root = t0_->root();
//    for (int i = 0; i < 3; i++) {
//        t0_->insert(key[i], val[i]);
//    }
//    EXPECT_EQ(3, root->KeySize());
//    for (int i = 0; i < 3; i++) {
//        EXPECT_EQ(key[i], root->entries()[i].first);
//        EXPECT_EQ(val[i], *(int *) root->entries()[i].second);
//    }
//    t0_->insert(key[3], val[3]);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(2, root->entries().front().first);
//    EXPECT_EQ(5, root->entries().back().first);
//
//    auto node2_1 = (BPNode<int> *) root->entries().front().second;
//    auto node2_2 = (BPNode<int> *) root->entries().back().second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(1, node2_1->entries().front().first);
//    EXPECT_EQ(2, node2_1->entries().back().first);
//    EXPECT_EQ(4, *(int *) node2_1->entries().front().second);
//    EXPECT_EQ(5, *(int *) node2_1->entries().back().second);
//
//    EXPECT_EQ(2, node2_2->KeySize());
//    EXPECT_EQ(3, node2_2->entries().front().first);
//    EXPECT_EQ(5, node2_2->entries().back().first);
//
//    t0_->insert(key[4], val[4]);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(2, root->entries().front().first);
//    EXPECT_EQ(6, root->entries().back().first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries().back().second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(1, node2_1->entries().front().first);
//    EXPECT_EQ(2, node2_1->entries().back().first);
//
//    EXPECT_EQ(3, node2_2->KeySize());
//    EXPECT_EQ(3, node2_2->entries().front().first);
//    EXPECT_EQ(5, node2_2->entries()[1].first);
//    EXPECT_EQ(6, node2_2->entries()[2].first);
//
//    t0_->insert(key[5], val[5]);
//    root = t0_->root();
//    EXPECT_EQ(3, root->KeySize());
//    EXPECT_EQ(2, root->entries().front().first);
//    EXPECT_EQ(5, root->entries()[1].first);
//    EXPECT_EQ(8, root->entries()[2].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    auto node2_3 = (BPNode<int> *) root->entries()[2].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(1, node2_1->entries().front().first);
//    EXPECT_EQ(2, node2_1->entries().back().first);
//
//    EXPECT_EQ(2, node2_2->KeySize());
//    EXPECT_EQ(3, node2_2->entries().front().first);
//    EXPECT_EQ(5, node2_2->entries()[1].first);
//
//    EXPECT_EQ(2, node2_3->KeySize());
//    EXPECT_EQ(6, node2_3->entries()[0].first);
//    EXPECT_EQ(8, node2_3->entries()[1].first);
//
//    t0_->insert(key[6], val[6]);
//    root = t0_->root();
//    EXPECT_EQ(3, root->KeySize());
//    EXPECT_EQ(2, root->entries().front().first);
//    EXPECT_EQ(5, root->entries()[1].first);
//    EXPECT_EQ(9, root->entries()[2].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    node2_3 = (BPNode<int> *) root->entries()[2].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(1, node2_1->entries().front().first);
//    EXPECT_EQ(2, node2_1->entries().back().first);
//
//    EXPECT_EQ(2, node2_2->KeySize());
//    EXPECT_EQ(3, node2_2->entries().front().first);
//    EXPECT_EQ(5, node2_2->entries()[1].first);
//
//    EXPECT_EQ(3, node2_3->KeySize());
//    EXPECT_EQ(6, node2_3->entries()[0].first);
//    EXPECT_EQ(8, node2_3->entries()[1].first);
//    EXPECT_EQ(9, node2_3->entries()[2].first);
//
//    t0_->insert(key[7], val[7]);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(5, root->entries().front().first);
//    EXPECT_EQ(11, root->entries()[1].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    auto node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
//    auto node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
//    auto node3_4 = (BPNode<int> *) node2_2->entries()[0].second;
//    auto node3_5 = (BPNode<int> *) node2_2->entries()[1].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(2, node2_1->entries().front().first);
//    EXPECT_EQ(5, node2_1->entries().back().first);
//
//    EXPECT_EQ(2, node2_2->KeySize());
//    EXPECT_EQ(8, node2_2->entries().front().first);
//    EXPECT_EQ(11, node2_2->entries()[1].first);
//
//    EXPECT_EQ(2, node3_1->KeySize());
//    EXPECT_EQ(1, node3_1->entries()[0].first);
//    EXPECT_EQ(2, node3_1->entries()[1].first);
//
//    EXPECT_EQ(2, node3_2->KeySize());
//    EXPECT_EQ(3, node3_2->entries()[0].first);
//    EXPECT_EQ(5, node3_2->entries()[1].first);
//
//    EXPECT_EQ(2, node3_4->KeySize());
//    EXPECT_EQ(6, node3_4->entries()[0].first);
//    EXPECT_EQ(8, node3_4->entries()[1].first);
//
//    EXPECT_EQ(2, node3_5->KeySize());
//    EXPECT_EQ(9, node3_5->entries()[0].first);
//    EXPECT_EQ(11, node3_5->entries()[1].first);
//
//    t0_->insert(key[8], val[8]);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(5, root->entries().front().first);
//    EXPECT_EQ(13, root->entries()[1].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
//    node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
//    node3_4 = (BPNode<int> *) node2_2->entries()[0].second;
//    node3_5 = (BPNode<int> *) node2_2->entries()[1].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(2, node2_1->entries().front().first);
//    EXPECT_EQ(5, node2_1->entries().back().first);
//
//    EXPECT_EQ(2, node2_2->KeySize());
//    EXPECT_EQ(8, node2_2->entries().front().first);
//    EXPECT_EQ(13, node2_2->entries()[1].first);
//
//    EXPECT_EQ(2, node3_1->KeySize());
//    EXPECT_EQ(1, node3_1->entries()[0].first);
//    EXPECT_EQ(2, node3_1->entries()[1].first);
//
//    EXPECT_EQ(2, node3_2->KeySize());
//    EXPECT_EQ(3, node3_2->entries()[0].first);
//    EXPECT_EQ(5, node3_2->entries()[1].first);
//
//    EXPECT_EQ(2, node3_4->KeySize());
//    EXPECT_EQ(6, node3_4->entries()[0].first);
//    EXPECT_EQ(8, node3_4->entries()[1].first);
//
//    EXPECT_EQ(3, node3_5->KeySize());
//    EXPECT_EQ(9, node3_5->entries()[0].first);
//    EXPECT_EQ(11, node3_5->entries()[1].first);
//    EXPECT_EQ(13, node3_5->entries()[2].first);
//
//    t0_->insert(key[9], val[9]);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(5, root->entries().front().first);
//    EXPECT_EQ(15, root->entries()[1].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
//    node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
//    node3_4 = (BPNode<int> *) node2_2->entries()[0].second;
//    node3_5 = (BPNode<int> *) node2_2->entries()[1].second;
//    auto node3_6 = (BPNode<int> *) node2_2->entries()[2].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(2, node2_1->entries().front().first);
//    EXPECT_EQ(5, node2_1->entries().back().first);
//
//    EXPECT_EQ(3, node2_2->KeySize());
//    EXPECT_EQ(8, node2_2->entries().front().first);
//    EXPECT_EQ(11, node2_2->entries()[1].first);
//    EXPECT_EQ(15, node2_2->entries()[2].first);
//
//    EXPECT_EQ(2, node3_1->KeySize());
//    EXPECT_EQ(1, node3_1->entries()[0].first);
//    EXPECT_EQ(2, node3_1->entries()[1].first);
//
//    EXPECT_EQ(2, node3_2->KeySize());
//    EXPECT_EQ(3, node3_2->entries()[0].first);
//    EXPECT_EQ(5, node3_2->entries()[1].first);
//
//    EXPECT_EQ(2, node3_4->KeySize());
//    EXPECT_EQ(6, node3_4->entries()[0].first);
//    EXPECT_EQ(8, node3_4->entries()[1].first);
//
//    EXPECT_EQ(2, node3_5->KeySize());
//    EXPECT_EQ(9, node3_5->entries()[0].first);
//    EXPECT_EQ(11, node3_5->entries()[1].first);
//
//    EXPECT_EQ(2,node3_6->KeySize());
//    EXPECT_EQ(13, node3_6->entries()[0].first);
//    EXPECT_EQ(15, node3_6->entries()[1].first);
//
//    t0_->insert(4, nullptr);
//    root = t0_->root();
//    EXPECT_EQ(2, root->KeySize());
//    EXPECT_EQ(5, root->entries().front().first);
//    EXPECT_EQ(15, root->entries()[1].first);
//
//    node2_1 = (BPNode<int> *) root->entries().front().second;
//    node2_2 = (BPNode<int> *) root->entries()[1].second;
//    node3_1 = (BPNode<int> *) node2_1->entries()[0].second;
//    node3_2 = (BPNode<int> *) node2_1->entries()[1].second;
//    node3_4 = (BPNode<int> *) node2_2->entries()[0].second;
//    node3_5 = (BPNode<int> *) node2_2->entries()[1].second;
//    node3_6 = (BPNode<int> *) node2_2->entries()[2].second;
//
//    EXPECT_EQ(2, node2_1->KeySize());
//    EXPECT_EQ(2, node2_1->entries().front().first);
//    EXPECT_EQ(5, node2_1->entries().back().first);
//
//    EXPECT_EQ(3, node2_2->KeySize());
//    EXPECT_EQ(8, node2_2->entries().front().first);
//    EXPECT_EQ(11, node2_2->entries()[1].first);
//    EXPECT_EQ(15, node2_2->entries()[2].first);
//
//    EXPECT_EQ(2, node3_1->KeySize());
//    EXPECT_EQ(1, node3_1->entries()[0].first);
//    EXPECT_EQ(2, node3_1->entries()[1].first);
//
//    EXPECT_EQ(3, node3_2->KeySize());
//    EXPECT_EQ(3, node3_2->entries()[0].first);
//    EXPECT_EQ(4, node3_2->entries()[1].first);
//    EXPECT_EQ(5, node3_2->entries()[2].first);
//
//    EXPECT_EQ(2, node3_4->KeySize());
//    EXPECT_EQ(6, node3_4->entries()[0].first);
//    EXPECT_EQ(8, node3_4->entries()[1].first);
//
//    EXPECT_EQ(2, node3_5->KeySize());
//    EXPECT_EQ(9, node3_5->entries()[0].first);
//    EXPECT_EQ(11, node3_5->entries()[1].first);
//
//    EXPECT_EQ(2,node3_6->KeySize());
//    EXPECT_EQ(13, node3_6->entries()[0].first);
//    EXPECT_EQ(15, node3_6->entries()[1].first);
//}