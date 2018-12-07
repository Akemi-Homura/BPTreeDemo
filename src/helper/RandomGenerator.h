//
// Created by quals on 2018/12/6.
//

#ifndef BPTDEMO_RANDOMGENERATOR_H
#define BPTDEMO_RANDOMGENERATOR_H

#include <random>

class RandomGenerator {
public:
    static RandomGenerator *GetInstance();

    int random_unint(int min, int max);

private:
    static RandomGenerator *instance_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;

    RandomGenerator() {
        std::random_device rd_;
        gen_ = std::mt19937(rd_());
    }

};

RandomGenerator *RandomGenerator::instance_ = nullptr;

RandomGenerator *RandomGenerator::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new RandomGenerator;
    }
    return instance_;
}

int RandomGenerator::random_unint(int min, int max) {
    if (min > max) {
        std::swap(min, max);
    }
    dis_ = std::uniform_int_distribution<>(min, max);
    return dis_(gen_);
}


#endif //BPTDEMO_RANDOMGENERATOR_H
