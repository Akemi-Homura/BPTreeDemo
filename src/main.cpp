#include <iostream>
#include <sys/time.h>
#include "src/bpt/BPlusTree.h"
#include "src/ordered_array/OrderedArray.h"
#include <queue>
#include <unistd.h>
#include <sys/times.h>

//#include "BPNode.h"
//#include "BPTree.h"
//
//long get_ms_time(timeval t) {
//    return t.tv_sec * 1000000 + t.tv_usec;
//}
//
//void num_to_str(long v, char *res, int len) {
//    sprintf(res, "%.*lfs", len, v * 1.0 / 1e6);
//}
//
//void timeval_str(timeval st, timeval ed, char *res) {
//    long st_ms = get_ms_time(st);
//    long ed_ms = get_ms_time(ed);
//
//    long diff_time = ed_ms - st_ms;
//    num_to_str(diff_time, res, 6);
//}
void pr_times(const char *msg, clock_t real, struct tms *tmsstart, struct tms *tmsend);
int main() {
//    std::vector<int *> val;
//    for (int i = 0; i < 10000000; i++) {
//        val.push_back(new int(i));
//    }
//    BPTree<int, int *> t = BPTree<int, int *>(5);
//
//    char res[20];
//    struct timeval st, ed;
//    gettimeofday(&st, NULL);
//    for (int i = 0; i < 10000000; i++) {
//        t.insert(i, val[i]);
//    }
//    gettimeofday(&ed, NULL);
//    timeval_str(st, ed, res);
//    printf("spend time: %s\n", res);
    BPlusTree *tree = new BPlusTree(5);
    const int size = 10000000;
    struct tms tmsstart, tmsend;
    clock_t start, end;
    start = times(&tmsstart);
    for (int i = 0; i < size; i++) {
        tree->Insert(i, i);
//        printf("\n\n======================\nAfter insert %d:\n======================\n\n", i);
//        output_node_recursive(tree->root_);
    }
    end = times(&tmsend);
    pr_times("Ordered Insert ", end - start, &tmsstart, &tmsend);

    return 0;
}

void pr_times(const char *msg, clock_t real, struct tms *tmsstart, struct tms *tmsend) {
    static long clktck = 0;
    if (clktck == 0) {   /* fetch clock ticks per second first time */
        if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
            perror("sysconf error");
            exit(1);
        }
    }

    printf("====================================\n");
    printf("%s time usage:\n", msg);
    printf(" real: %7.2f\n", real / (double) clktck);
    printf(" user: %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
    printf(" sys:  %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
    printf(" child user: %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
    printf(" child sys:  %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);
    printf("====================================\n");
}