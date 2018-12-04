//
// Created by quals on 2018/11/27.
//

# include "src/ordered_list/OrderedLinkList.h"
# include <gtest/gtest.h>

class OrderedLinkListTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        list = new OrderedLinkList;
    }

    virtual void TearDown() {
        delete list;
    }

    OrderedLinkList *list;
};

TEST_F(OrderedLinkListTest, SimpleTest) {
//    OrderedLinkList *list = new OrderedLinkList;
    for (int i = 0; i < 10; i++) {
        Data data(i, i);
        list->Insert(data);
    }
    ListNode *now = list->head_;
    for (int i = 0; i < 10; i++, now = now->next_) {
        EXPECT_EQ(i, now->data_.key_);
        EXPECT_EQ(i, now->data_.val_.value);
    }

    EXPECT_EQ(4, list->FindFirstBigger(4)->data_.key_);
    EXPECT_EQ(4, list->FindFirstBigger(4)->data_.val_.value);

    list->Remove(6);
    EXPECT_EQ(nullptr, list->FindEqual(6));

    ListNode *left_head = list->head_;
    ListNode *left_tail = list->FindEqual(2);
    ListNode *right_head = left_tail->next_;
    ListNode *right_tail = list->tail_;

    OrderedLinkList *left_list = new OrderedLinkList;
    list->Split(3, left_list);
    EXPECT_EQ(left_head, left_list->head_);
    EXPECT_EQ(left_tail, left_list->tail_);
    EXPECT_EQ(right_head, list->head_);
    EXPECT_EQ(right_tail, list->tail_);

    left_list->Insert(Data(-1,-1));
    EXPECT_EQ(-1, left_list->head_->data_.key_);
    EXPECT_EQ(-1, left_list->head_->data_.val_.value);

    list->InsertLast(Data(12,12));
    EXPECT_EQ(12, list->tail_->data_.key_);
    EXPECT_EQ(12, list->tail_->data_.val_.value);
}