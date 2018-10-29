//
// Created by Dio Joyce on 2018/10/25.
//

#include "BPTree.h"
#include "gtest/gtest.h"
#include <random>
#include <algorithm>
#include <sys/time.h>


class BPTreeMoreDataFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
    }


    int GenerateRangeRandomNum(int a, int b) {
        return a + (std::rand() % ((a >= b) == 0 ? b : (b - a)));
    }

    void Initialize(int num, int order) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        int x = 1;
        for (int i = 0; i < num; i++) {
            int u = 1 + rand() % 10;
            x += u;
            keys_.push_back(x);
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(keys_.begin(), keys_.end(), g);

        t0_ = new BPTree<int, int *>(order);

        for (int i = 0; i < num; i++) {
            auto data = new int(GenerateRangeRandomNum(MIN_DATA, MAX_DATA));
            data_.push_back(data);
            int key = keys_[i];
            EXPECT_FALSE(t0_->HasKey(key));
            t0_->insert(key, data);
            backup_[key] = data;
        }

        for (int i = 0; i < num; i++) {
            EXPECT_TRUE(t0_->HasKey(keys_[i]));
        }

        for (int i = 0; i < num;) {
            int key = keys_[i];
            EXPECT_TRUE(t0_->HasKey(key));
            t0_->remove(key);
//            EXPECT_FALSE(backup_.find(key) == backup_.end());
            const std::map<int, int *>::iterator p = backup_.find(key);
            backup_.erase(p);
//            EXPECT_TRUE(backup_.find(key) == backup_.end());
            i += 1 + std::rand() % (num / 10);
        }
    }

    void Clean() {
        keys_.clear();
        backup_.clear();
        for(int* item: data_){
            delete item;
        }
        data_.clear();

        delete t0_;
    }

    long get_ms_time(timeval t) {
        return t.tv_sec * 1000000 + t.tv_usec;
    }

    void num_to_str(long v, char *res, int len) {
        sprintf(res, "%.*lfs", len, v * 1.0 / 1e6);
    }

    void output_time_spend(timeval st, timeval ed, char *res) {
        long st_ms = get_ms_time(st);
        long ed_ms = get_ms_time(ed);

        long diff_time = ed_ms - st_ms;
        num_to_str(diff_time, res, 6);
    }

    void GenerateTestData(int &key, bool &exist, int *&data) {
        int index = GenerateRangeRandomNum(0, keys_.size());
        key = keys_[index];
        if (backup_.find(key) != backup_.end()) {
            exist = true;
            data = backup_[key];
        } else {
            exist = false;
            data = nullptr;
        }
    }


    int MAX_DATA = 100000;

    int MIN_DATA = 1;

    std::vector<int> keys_;

    std::vector<int *> data_;

    std::map<int, int *> backup_;

    BPTree<int, int *> *t0_;
};

TEST_F(BPTreeMoreDataFixture, Random) {
    int data_size[] = {10, 100, 1000, 10000, 100000, 1000000};
    int order[] = {5, 10, 20, 40, 100};
    struct timeval st_time, ed_time;
    char res[20];
    for (int num : data_size) {
        for (int order_ : order) {
            gettimeofday(&st_time, NULL);
            Initialize(num, order_);
            gettimeofday(&ed_time, NULL);
            output_time_spend(st_time, ed_time, res);
            printf("key num: %d, max child num: %d, speed time: %s\n", num, order_, res);
            int key, *data;
            bool exist;
            for (int k = 0; k < num / 10; k++) {
                GenerateTestData(key, exist, data);
                EXPECT_EQ(exist, t0_->HasKey(key));
                if (exist) {
                    EXPECT_EQ(data, t0_->FindData(key));
                }
            }
            Clean();
        }
    }
}