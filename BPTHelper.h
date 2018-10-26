//
// Created by Dio Joyce on 2018/10/24.
//

#ifndef BPTDEMO_BPTHELPER_H
#define BPTDEMO_BPTHELPER_H


#include <sstream>
#include "iostream"
#include <stdexcept>
static std::ostringstream ss;

template<typename T>
void NoSuchKeyError(T key) {
    ss.clear();
    ss<<"The key '" << key << "' is not in this node" << std::endl;
    throw std::runtime_error(ss.str());
//    std::cerr << "The key '" << key << "' is not in this node" << std::endl;
//    exit(1);
}

template<typename T>
void RemoveKeyFailed(T key) {
    std::cerr << "Remove key '" << key << "' failed" << std::endl;
    exit(1);
}

#endif //BPTDEMO_BPTHELPER_H
