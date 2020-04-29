#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "func.h"
#include "scheduling.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 32
#endif

int main(int argc, char *argv[]) {
    char schedule_policy[10];
    int N;
    int arrive_t;
    int burst_t;
    fscanf(stdin, "%s", schedule_policy);
    fscanf(stdin, "%d", &N);
    process p_array[N];
    for (int i = 0; i < N; ++i) {
        char pname_buff[BUFF_SIZE];
        fscanf(stdin, "%s %d %d\n", pname_buff, &arrive_t, &burst_t);
        process temp;
        memcpy(temp.name, pname_buff, BUFF_SIZE);
        temp.name[BUFF_SIZE - 1] = 0;
        temp.arrive_t = arrive_t;
        temp.burst_t  = burst_t;
        p_array[i] = temp;
    }
    qsort_ready(p_array, N);
    if(strcmp(schedule_policy, "FIFO") == 0) FIFO(p_array, N);      
    else if(strcmp(schedule_policy, "RR") == 0) RR(p_array, N);
    else if(strcmp(schedule_policy, "SJF") == 0) SJF(p_array, N);
    else PSJF(p_array, N);
    return 0;
}
