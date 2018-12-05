//
// Created by quals on 2018/11/27.
//

#ifndef BPTDEMO_ORDEREDLINKLIST_H
#define BPTDEMO_ORDEREDLINKLIST_H

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


struct ListNode {
    Data data_;
    ListNode *next_;
    ListNode *pre_;

    explicit ListNode(Data data) {
        data_ = data;
        pre_ = next_ = nullptr;
    }
};

class OrderedLinkList {
public:
    ListNode *head_, *tail_;

    int size_;

    OrderedLinkList();

    OrderedLinkList(ListNode *head, ListNode *tail);

    ~OrderedLinkList();

    ListNode *Insert(Data data);

    ListNode *Insert(OrderedLinkList *list);

    ListNode *InsertLast(Data data);

    ListNode *InsertLast(OrderedLinkList *list);

    void Remove(int key);

    /**
     * 删除节点
     * @param node 如果是head或者tail要特殊处理，更新head和tail
     */
    void Remove(ListNode *node);

    /**
     * 分裂链表，前index个元素归新的链表，后面的属于原来的链表
     * @param index 分裂点
     * @param list 新链表的指针
     */
    void Split(int index, OrderedLinkList *list);

    void Split(ListNode *split_node, OrderedLinkList *list);

    ListNode *FindEqual(int key);

    /**
     * 查找链表中第一个大于等于key的entry，没有则返回nullptr
     * @param key 比较的key
     * @return 查找到的entry的指针
     */
    ListNode *FindLowerBound(int key);

    ListNode *FindFirstBigger(int key);

private:
    ListNode *InsertBefore(ListNode *node, Data data);

    ListNode *InsertAfter(ListNode *node, Data data);
};

OrderedLinkList::OrderedLinkList() : OrderedLinkList(nullptr, nullptr) {
}

OrderedLinkList::OrderedLinkList(ListNode *head, ListNode *tail) : head_(head), tail_(tail), size_(0) {
}

ListNode *OrderedLinkList::Insert(Data data) {
    if (head_ == nullptr) {
        head_ = tail_ = new ListNode(data);
        size_++;
        return head_;
    }

    ListNode *lower_bound = FindLowerBound(data.key_);
    return lower_bound != nullptr ? InsertBefore(lower_bound, data) : InsertAfter(tail_, data);
}

ListNode *OrderedLinkList::InsertBefore(ListNode *node, Data data) {
    ListNode *node_to_insert = new ListNode(data);
    size_++;
    node_to_insert->next_ = node;
    node_to_insert->pre_ = node->pre_;
    if (node->pre_ != nullptr) {
        node->pre_->next_ = node_to_insert;
    }
    node->pre_ = node_to_insert;
    if (head_ == node) {
        head_ = node_to_insert;
    }
    return node_to_insert;
}

ListNode *OrderedLinkList::InsertAfter(ListNode *node, Data data) {
    ListNode *node_to_insert = new ListNode(data);
    size_++;
    node_to_insert->pre_ = node;
    node_to_insert->next_ = node->next_;
    if (node->next_ != nullptr) {
        node->next_->pre_ = node_to_insert;
    }
    node->next_ = node_to_insert;
    if (tail_ == node) {
        tail_ = node_to_insert;
    }
    return node_to_insert;
}

void OrderedLinkList::Remove(int key) {
    ListNode *node_to_remove = FindEqual(key);
    Remove(node_to_remove);
}

void OrderedLinkList::Remove(ListNode *node) {
    size_--;
    if (node->pre_ != nullptr) {
        node->pre_->next_ = node->next_;
    }
    if (node->next_ != nullptr) {
        node->next_->pre_ = node->pre_;
    }
    if (node == head_) head_ = node->next_;
    if (node == tail_) tail_ = node->pre_;

    delete node;
}

void OrderedLinkList::Split(int index, OrderedLinkList *list) {
    list->size_ = index;
    size_ -= index;
    ListNode *split_node = head_;
    for (int i = 0; i < index - 1 && split_node != tail_; i++) {
        split_node = split_node->next_;
    }
    Split(split_node, list);
}

void OrderedLinkList::Split(ListNode *split_node, OrderedLinkList *list) {
    list->head_ = head_;
    list->tail_ = split_node;
    head_ = split_node->next_;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    /**
     * Must split split_node and split_node->next !!!!
     **/
    if (head_ != nullptr) head_->pre_ = nullptr;
    list->tail_->next_ = nullptr;

}

ListNode *OrderedLinkList::FindEqual(int key) {
    for (ListNode *now = head_; now != nullptr; now = now->next_) {
        if (key == now->data_.key_) {
            return now;
        }
    }
    return nullptr;
}

ListNode *OrderedLinkList::FindFirstBigger(int key) {
    for (ListNode *now = tail_; now != nullptr; now = now->pre_) {
        if (key >= now->data_.key_) {
            return now;
        }
    }
    return nullptr;
}

OrderedLinkList::~OrderedLinkList() {
    for (ListNode *now = head_; now != nullptr; now = now->next_) {
        delete now;
    }
}

ListNode *OrderedLinkList::FindLowerBound(int key) {
    for (ListNode *now = head_; now != nullptr; now = now->next_) {
        if (key <= now->data_.key_) {
            return now;
        }
    }
    return nullptr;
}

ListNode *OrderedLinkList::InsertLast(Data data) {
    if (head_ == nullptr) {
        head_ = tail_ = new ListNode(data);
        size_++;
        return head_;
    }

    ListNode *firstBigger = FindFirstBigger(data.key_);
    return firstBigger != nullptr ? InsertAfter(firstBigger, data) : InsertBefore(head_, data);

}

ListNode *OrderedLinkList::Insert(OrderedLinkList *list) {
    head_->pre_ = list->tail_;
    list->tail_->next_ = head_;
    head_ = list->head_;
    return head_;
}

ListNode *OrderedLinkList::InsertLast(OrderedLinkList *list) {
    tail_->next_ = list->head_;
    list->head_->pre_ = tail_;
    tail_ = list->tail_;
    return tail_;
}


#endif //BPTDEMO_ORDEREDLINKLIST_H
