//
// Created by quals on 2018/11/30.
//
#include "src/bpt/BPlusTree.h"
#include <gtest/gtest.h>
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <queue>


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

typedef void (OperationFun)(BPlusTree *, std::vector<std::pair<int, int> >);

typedef std::vector<std::pair<int, int> > (DataGenerator)(int size);

std::vector<std::pair<int, int>> fill_random_num(int size);

std::vector<std::pair<int, int>> fill_ordered_num(int size);

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg);

void InsertFunc(BPlusTree *tree, std::vector<std::pair<int, int> > data);

void SynthesizeFunc(BPlusTree *tree, std::vector<std::pair<int, int>> data);

long get_ms_time(timeval t);

void num_to_str(long v, char *res, int len);

void timeval_str(timeval st, timeval ed, char *res);

void output_node_recursive(BPlusNode *root);

void output_single_node(BPlusNode *node, int layer, int index);

TEST_F(BPlusTreeTest, SynthesizeTest) {
    const int size = 1000000;
    const int order = 5;
    char msg[100];
    sprintf(msg, "Synthesize Test, %d data", size);
    TestFunc(fill_random_num, SynthesizeFunc, size, order, msg);
}

TEST(BPlusTreeDebug, SynthesizeDebug) {
    const int size = 11;
    const int order = 5;
    int data[] = {8, 7, 6, 9, 10, 0, 1, 4, 5, 2, 3};

    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(data[i], data[i]);
        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i]);
        output_node_recursive(tree->root_);
        fflush(stdout);
    }

    for (int i = 0; i < size; i++) {
        ASSERT_TRUE(tree->HasKey(data[i]));
        ASSERT_EQ(data[i], tree->FindValue(data[i]));
    }

    for (int i = 0; i < size; i++) {
        tree->Remove(data[i]);
        printf("\n\n======================\nAfter Remove %d:\n======================\n\n", data[i]);
        output_node_recursive(tree->root_);
        fflush(stdout);
    }

    for (int i = 0; i < size; i++) {
        ASSERT_FALSE(tree->HasKey(data[i]));
    }
}

TEST_F(BPlusTreeTest, OrderedInsert) {
    const int size = 1000000;
    const int order = 5;
    char msg[100];
    sprintf(msg, "Insert %d Ordered integer", size);
    TestFunc(fill_ordered_num, InsertFunc, size, order, msg);
}

TEST_F(BPlusTreeTest, RandomInsert) {
    const int size = 1000000;
    const int order = 5;
    char msg[100];
    sprintf(msg, "Insert %d Random integer", size);
    TestFunc(fill_random_num, InsertFunc, size, order, msg);
}

TEST(BPlusTreeDebug, RandomInsertDebug) {
    const int size = 10;
    const int order = 5;

    BPlusTree *tree = new BPlusTree(order);
    std::vector<std::pair<int, int>> data = fill_random_num(size);

    printf("data: ");
    for (int i = 0; i < data.size(); i++) {
        printf("%d%c", data[i].first, " \n"[i == data.size() - 1]);
    }

    for (int i = 0; i < data.size(); i++) {
        tree->Insert(data[i].first, data[i].second);

        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i].first);
        fflush(stdout);
        output_node_recursive(tree->root_);
    }
}

TEST(BPlusTreeDebug, SpecialDataInsertDebug) {
    const int size = 6;
    const int order = 5;
    int data[] = {4, 3, 5, 2, 1, 0};

    BPlusTree *tree = new BPlusTree(order);
    for (int i = 0; i < size; i++) {
        tree->Insert(data[i], data[i]);
        printf("\n\n======================\nAfter insert %d:\n======================\n\n", data[i]);
        fflush(stdout);
        output_node_recursive(tree->root_);
    }
}

std::vector<std::pair<int, int>> fill_random_num(int size) {
    std::vector<int> v;
    std::vector<std::pair<int, int> > data;
    for (int i = 0; i < size; i++) {
        v.emplace_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);

    for (int i = 0; i < size; i++) {
        data.emplace_back(std::pair<int, int>(v[i], v[i]));
    }
    return data;
}


long get_ms_time(timeval t) {
    return t.tv_sec * 1000000 + t.tv_usec;
}

void num_to_str(long v, char *res, int len) {
    sprintf(res, "%.*lfs", len, v * 1.0 / 1e6);
}

void timeval_str(timeval st, timeval ed, char *res) {
    long st_ms = get_ms_time(st);
    long ed_ms = get_ms_time(ed);

    long diff_time = ed_ms - st_ms;
    num_to_str(diff_time, res, 6);
}


void InsertFunc(BPlusTree *tree, std::vector<std::pair<int, int> > data) {
    for (int i = 0; i < data.size(); i++) {
        tree->Insert(data[i].first, data[i].second);
    }
}

std::vector<std::pair<int, int>> fill_ordered_num(int size) {
    std::vector<std::pair<int, int> > data;
    for (int i = 0; i < size; i++) {
        data.emplace_back(std::pair<int, int>(i, i));
    }
    return data;
}

void TestFunc(DataGenerator *data_generator, OperationFun *operation_func, int size, int order, const char *msg) {
    static char res[20];
    BPlusTree *tree = new BPlusTree(order);

    std::vector<std::pair<int, int> > data = data_generator(size);
    struct timeval st, ed;

    gettimeofday(&st, nullptr);
    operation_func(tree, data);
    gettimeofday(&ed, nullptr);

    timeval_str(st, ed, res);
    printf("%s spend time: %s\n", msg, res);
}

void output_node_recursive(BPlusNode *root) {
    std::queue<std::pair<BPlusNode *, int>> q;
    q.push(std::pair<BPlusNode *, int>(root, 0));
    int index = -1, layer = 0;
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        BPlusNode *now = p.first;
        if (p.second > layer) {
            layer = p.second;
            index = 0;
        } else {
            index++;
        }
        output_single_node(now, layer, index);
        if (now->type_ == Data::kInternal) {
            for (auto t = now->list_->head_; t != nullptr; t = t->next_) {
                q.push(std::pair<BPlusNode *, int>(t->data_.val_.child, layer + 1));
            }
        }
    }
}

void output_single_node(BPlusNode *node, int layer, int index) {
    printf("node_%d_%d %s:\n", layer, index, node->type_ == Data::kInternal ? "Internal" : "Leaf");
    for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
        if (node->type_ == Data::kInternal) {
            printf("%d ", now->data_.key_);
        } else {
            printf("(%d %d) ", now->data_.key_, now->data_.val_.value);
        }
    }
    printf("\n");
}

void SynthesizeFunc(BPlusTree *tree, std::vector<std::pair<int, int>> data) {
    for (auto item:data) {
        tree->Insert(item.first, item.second);
    }

    int size = (int) data.size() / 100;

    if (data.size() < 1000) {
        size = 1000;
    }

    /* HasKey and FoundData */
    for (int i = 0; i < size; i++) {
        EXPECT_TRUE(tree->HasKey(data[i].first));
        EXPECT_EQ(data[i].second, tree->FindValue(data[i].first));
    }

    /* Remove then Test HasKey */
    for (int i = 0; i < size; i++) {
        tree->Remove(data[i].first);
    }

    for (int i = 0; i < size; i++) {
        EXPECT_FALSE(tree->HasKey(data[i].first));
    }

}

