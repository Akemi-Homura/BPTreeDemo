//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_ORDEREDLINKLIST_H
#define BPTDEMO_ORDEREDLINKLIST_H

#include <stdexcept>
#include <string.h>
#include <string>

class BPlusNode;


class Data {
public:
    enum DataType {
        kInternal,
        kLeaf
    };

    int key_;
    union {
        int value;
        BPlusNode *child;
    } val_;

    Data() {}

    explicit Data(int key, int val) {
        key_ = key;
        val_.value = val;
    }

    explicit Data(int key, BPlusNode *child) {
        key_ = key;
        val_.child = child;
    }
};

class OrderedArray {
public:
    Data *array_;

    int size_;

    const int order_;

    explicit OrderedArray(int order);

    ~OrderedArray();

    int Insert(const Data &data);

    int Insert(OrderedArray *rhs);

    int MergeAppend(OrderedArray *rhs);

    Data front();

    Data back();

    void Remove(int key);

    void RemoveAt(int index);

    /**
     * 分裂链表，前index个元素归新的链表，后面的属于原来的链表
     * @param remain_size 分裂点
     * @param rhs 新链表的指针
     */
    void Split(int remain_size, OrderedArray *rhs);


    int FindEqual(int key);

    /**
     * 查找链表中第一个大于等于key的entry，没有则返回nullptr
     * @param key 比较的key
     * @return 查找到的entry的指针
     */
    int FindLowerBound(int key);

    Data &operator[](int n);

    const Data &operator[](int n) const;

    void InsertAt(const Data &data, int index);

private:
    static const int DATA_SIZE = sizeof(Data);

};


OrderedArray::OrderedArray(int order) : order_(order), size_(0) {
    array_ = new Data[order];
}

int OrderedArray::Insert(const Data &data) {
    int index = FindLowerBound(data.key_);
    InsertAt(data, index);
    return index;
}

void OrderedArray::InsertAt(const Data &data, int index) {
    if (index > 0 && data.key_ < array_[index - 1].key_) {
        std::string error_msg = "InsertAt error, ";
        error_msg.append(std::to_string(data.key_));
        error_msg.append("less than previous key ");
        error_msg.append(std::to_string(array_[index - 1].key_));
        throw std::runtime_error(error_msg);
    }
    if (index != size_) {
        memmove(&array_[index + 1], &array_[index], (size_t) (size_ - index) * DATA_SIZE);
    }
    array_[index] = data;
    size_++;
}

void OrderedArray::Remove(int key) {
    int index = FindEqual(key);
    RemoveAt(index);
}

void OrderedArray::RemoveAt(int index) {
    if (index != size_ - 1) {
        memmove(&array_[index], &array_[index + 1], (size_t) (size_ - index - 1) * DATA_SIZE);
    }
    size_--;
}

void OrderedArray::Split(int remain_size, OrderedArray *rhs) {
    memmove(rhs->array_, &array_[remain_size], (size_t) (size_ - remain_size) * DATA_SIZE);
    rhs->size_ = size_ - remain_size;
    size_ = remain_size;
}

int OrderedArray::FindEqual(int key) {
    int ret = -1;
    int mid;
    int L = 0, R = size_ - 1;
    while (L <= R) {
        mid = L + (R - L) / 2;
        if (array_[mid].key_ < key) {
            L = mid + 1;
        } else if (array_[mid].key_ > key) {
            R = mid - 1;
        } else {
            ret = mid;
            break;
        }
    }
    return ret;
}

OrderedArray::~OrderedArray() {
    delete array_;
}

int OrderedArray::FindLowerBound(int key) {
    int half;
    int len = size_;
    int mid;
    int first = 0;
    while (len > 0) {
        half = len >> 1;
        mid = first + half;
        // in the right part
        if (array_[mid].key_ < key) {
            first = mid + 1;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

int OrderedArray::Insert(OrderedArray *rhs) {
    int offset = rhs->size_ * DATA_SIZE;
    memmove(array_ + offset, array_, offset);
    memmove(array_, rhs->array_, offset);
    size_ += rhs->size_;
    return 0;
}

int OrderedArray::MergeAppend(OrderedArray *rhs) {
    memmove(&array_[size_], rhs->array_, (size_t) rhs->size_ * DATA_SIZE);
    size_ += rhs->size_;
    return 0;
}

Data &OrderedArray::operator[](int n) {
    if (n >= size_) {
        std::string error_msg("Index out of range: ");
        error_msg.append(std::to_string(n));
        throw std::runtime_error(error_msg);
    }
    return array_[n];
}

const Data &OrderedArray::operator[](int n) const {
    if (n >= size_) {
        std::string error_msg("Index out of range: ");
        error_msg.append(std::to_string(n));
        throw std::runtime_error(error_msg);
    }
    return array_[n];
}

Data OrderedArray::front() {
    if (size_ == 0) {
        std::string error_msg("Front error, array is empty");
        throw std::runtime_error(error_msg);
    }
    return array_[0];
}

Data OrderedArray::back() {
    if (size_ == 0) {
        std::string error_msg("Back error, array is empty");
        throw std::runtime_error(error_msg);
    }
    return array_[size_ - 1];
}


#endif //BPTDEMO_ORDEREDLINKLIST_H
