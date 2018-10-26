#include <iostream>
#include "BPNode.h"

template <typename E>
void release(void* data){
    delete (E) data;
}

int main() {
    void* a = (void*) new int(2);
    release<int*>(a);
    return 0;
}