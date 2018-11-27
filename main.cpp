#include <iostream>
#include <sys/time.h>
#include "BPNode.h"
#include "BPTree.h"

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

int main() {
    std::vector<int *> val;
    for (int i = 0; i < 10000000; i++) {
        val.push_back(new int(i));
    }
    BPTree<int, int *> t = BPTree<int, int *>(5);

    char res[20];
    struct timeval st, ed;
    gettimeofday(&st, NULL);
    for (int i = 0; i < 10000000; i++) {
        t.insert(i, val[i]);
    }
    gettimeofday(&ed, NULL);
    output_time_spend(st, ed, res);
    printf("spend time: %s\n", res);
    return 0;
}