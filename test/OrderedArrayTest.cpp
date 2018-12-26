//
// Created by quals on 2018/11/27.
//

# include "src/ordered_array/OrderedArray.h"
# include <gtest/gtest.h>
#include <src/ordered_array/OrderedArray.h>

class OrderedArrayTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        array = new OrderedArray(10);
    }

    virtual void TearDown() {
        delete array;
    }

    OrderedArray *array;
};

void output_array(OrderedArray* array);



TEST_F(OrderedArrayTest, SimpleTest) {
    for (int i = 0; i < 10; i++) {
        Data data(i, i);
        array->Insert(data);
    }
    EXPECT_EQ(array->size_, 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(i, (*array)[i].key_);
        EXPECT_EQ(i, (*array)[i].val_.value);
    }

    array->Remove(6);
    EXPECT_EQ(-1, array->FindEqual(6));
    EXPECT_EQ(9, array->size_);

    auto *right = new OrderedArray(10);
    array->Split(3, right);
    EXPECT_EQ(6, right->size_);
    EXPECT_EQ(3, array->size_);

    right->Insert(Data(-1,-1));
    EXPECT_EQ(-1, (*right)[0].key_);
    EXPECT_EQ(-1, (*right)[0].val_.value);

    array->Insert(Data(12,12));
    EXPECT_EQ(12, (*array)[array->size_-1].key_);
    EXPECT_EQ(12, (*array)[array->size_-1].val_.value);
}

void output_array(OrderedArray *array) {
    for(int i=0;i<array->size_;i++){
        printf("%d%c",(*array)[i].key_," \n"[i == array->size_ - 1]);
    }
}