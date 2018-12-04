//
// Created by quals on 2018/11/29.
//

#ifndef BPTDEMO_SIMPLESERIALIZER_H
#define BPTDEMO_SIMPLESERIALIZER_H

#include "SerializeHelperAbstract.h"
#include "src/bpt/BPlusTree.h"
#include "src/ordered_list/OrderedLinkList.h"
#include <fstream>
#include <iostream>

class SimpleSerializer : public SerializeHelperAbstract<BPlusTree> {
public:
    bool Serialize(const BPlusTree *tree, const char *filename) override;

    bool Deserialize(BPlusTree *tree, const char *filename) override;
};

bool SimpleSerializer::Serialize(const BPlusTree *tree, const char *filename) {
    std::ofstream output_stream(filename);
    for (auto *leaf_head = tree->GetLeftMostNode(); leaf_head != nullptr; leaf_head = leaf_head->right_sibling_) {
        for (auto *entry = leaf_head->list_->head_; entry != nullptr; entry = entry->next_) {
            output_stream << entry->data_.key_ << " " << entry->data_.val_.value << std::endl;
        }
    }
    output_stream.close();
    return true;
}

bool SimpleSerializer::Deserialize(BPlusTree *tree, const char *filename) {
    std::ifstream input_stream(filename);
    int key, value;
    while (input_stream >> key >> value) {
        tree->Insert(key, value);
    }
    if (input_stream.bad())
        std::cout << "I/O error while reading\n";
    else if (input_stream.fail())
        std::cout << "Non-integer data encountered\n";
    input_stream.close();
    return true;
}


#endif //BPTDEMO_SIMPLESERIALIZER_H
