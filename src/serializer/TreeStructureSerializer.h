//
// Created by quals on 2018/11/29.
//

#ifndef BPTDEMO_TREESTRUCTSERIALIZER_H
#define BPTDEMO_TREESTRUCTSERIALIZER_H

#include "src/bpt/BPlusTree.h"
#include "SerializeHelperAbstract.h"
#include "src/ordered_list/OrderedLinkList.h"
#include <queue>

struct BPlusNodeModel {
    int index_;
    int type_;
    int child_size_;
    int *key_;
    int *values_;

    BPlusNodeModel(int type, int index, int child_size) {
        this->index_ = index;
        this->type_ = type;
        this->child_size_ = child_size;
        values_ = new int[child_size];
        key_ = new int[child_size];
    }

    BPlusNodeModel() : BPlusNodeModel(0, 0, 0) {}
};

class TreeStructureSerializer : public SerializeHelperAbstract<BPlusTree> {
public:


    bool Serialize(const BPlusTree *tree, const char *filename) override;

    bool Deserialize(BPlusTree *tree, const char *filename) override;

private:
    const static int TInternal = 0;
    const static int TLeaf = 1;

    void DFS(FILE *fp, BPlusNode *node, int &index);

    BPlusNodeModel get_one_node_model(FILE *fp);

    BPlusNode *BuildNodeRecursiveFromNodeModelArray(BPlusNodeModel *mode_array, int index);

    static void PrintNode(FILE *fp, const BPlusNode *node);

    static void PrintNodeRecursive(FILE *fp, BPlusNode *node);
};

void TreeStructureSerializer::DFS(FILE *fp, BPlusNode *node, int &index) {
    node->index = index;
    if (node->type_ == Data::kInternal) {
        for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
            DFS(fp, now->data_.val_.child, ++index);
        }
    }
}


bool TreeStructureSerializer::Serialize(const BPlusTree *tree, const char *filename) {
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d %d\n", tree->order_, tree->size_);
    int index = 0;
    DFS(fp, tree->root_, index);
    PrintNodeRecursive(fp, tree->root_);
    fclose(fp);
    return true;
}

bool TreeStructureSerializer::Deserialize(BPlusTree *tree, const char *filename) {
    FILE *fp = fopen(filename, "r");
    int node_size, order;
    fscanf(fp, "%d%d", &order, &node_size);
    tree->order_ = order;
    tree->size_ = node_size;

    BPlusNodeModel *model_array = new BPlusNodeModel[node_size * sizeof(BPlusNodeModel)];
    for (int i = 0; i < node_size; i++) {
        BPlusNodeModel model = get_one_node_model(fp);
        model_array[model.index_] = model;
    }
    tree->root_ = BuildNodeRecursiveFromNodeModelArray(model_array, 0);
    fclose(fp);
    return true;
}

BPlusNodeModel TreeStructureSerializer::get_one_node_model(FILE *fp) {
    int node_type, node_index, child_size;
    fscanf(fp, "%d%d%d\n", &node_type, &node_index, &child_size);
    BPlusNodeModel res(node_type, node_index, child_size);
    for (int i = 0; i < child_size; i++) {
        int key, value;
        fscanf(fp, i == child_size - 1 ? "(%d,%d) " : "(%d,%d)\n", &key, &value);
        res.key_[i] = key;
        res.values_[i] = value;
    }
    return res;
}

BPlusNode *
TreeStructureSerializer::BuildNodeRecursiveFromNodeModelArray(BPlusNodeModel *mode_array, int index) {
    BPlusNodeModel model = mode_array[index];
    BPlusNode *res = new BPlusNode(model.type_ == TInternal ? Data::kInternal : Data::kLeaf);

    for (int i = 0; i < model.child_size_; i++) {
        Data data;
        data.key_ = model.key_[i];
        if (res->type_ == Data::kInternal) {
            data.val_.child = BuildNodeRecursiveFromNodeModelArray(mode_array, model.values_[i]);
        } else {
            data.val_.value = model.values_[i];
        }

        res->list_->InsertLast(data);
    }
    return res;
}

void TreeStructureSerializer::PrintNode(FILE *fp, const BPlusNode *node) {
    // output node
    int type = node->type_ == Data::kLeaf ? TLeaf : TInternal;
    fprintf(fp, "%d %d %d\n", type, node->index, node->list_->size_);
    for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
        fprintf(fp, "(%d,%d)%c", now->data_.key_,
                node->type_ == Data::kInternal ? now->data_.val_.child->index : now->data_.val_.value,
                " \n"[now->next_ == nullptr]);
    }
}

void TreeStructureSerializer::PrintNodeRecursive(FILE *fp, BPlusNode *node) {
    PrintNode(fp, node);
    if (node->type_ == Data::kInternal) {
        for (auto now = node->list_->head_; now != nullptr; now = now->next_) {
            PrintNodeRecursive(fp, now->data_.val_.child);
        }
    }
}


#endif //BPTDEMO_TREESTRUCTSERIALIZER_H
