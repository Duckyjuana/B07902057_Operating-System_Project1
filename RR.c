#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include "func.h"
#include "scheduling.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TIME_QUANTUM 500

int terminated = 0;
int total_child = 0;
pid_t exit_pid;

void sighandler(int signum)
{
    if (signum == SIGCHLD){
        terminated = 1;
        exit_pid = wait(NULL);
    }
    return;
}

void RR(process* p_array, int N)
{
    total_child = N;
    char *flag = "[Project1]";
    struct timespec ts_start;
    struct timespec ts_end;
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);

    struct sched_param sch_p;
    pid_t scheduler_pid = getpid();
    sch_p.sched_priority = 3;
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1);

    int now_child_pid;
    int now_child_idx;
    int count_child = 0;
    int ptr_now_process;

    int RR_start = -1;
    queue *rdy_q = (queue*)malloc(sizeof(queue*));
    rdy_q -> stack1 = NULL;
    rdy_q -> stack2 = NULL;

    int time_counter = 0;
    while(total_child > 0){
        for(int i = 0; i < N; i++){
            if(time_counter == p_array[i].arrive_t){
                ptr_now_process = i;
                syscall(335, &ts_start);
                p_array[i].pid = fork();
                enqueue(rdy_q, p_array[i]);
                count_child++;
                if(p_array[i].pid == 0) break;
                if(count_child == 1){
                    now_child_idx = i;
                    now_child_pid = rdy_q -> front.pid;
                    RR_start = time_counter;
                }
            }
        }
        if(count_child != 0 && getpid() != scheduler_pid) break;
        if(count_child > 1 && (time_counter - RR_start) % TIME_QUANTUM == 0 && (time_counter != RR_start)){
            if(!(terminated && rdy_q -> front.pid == exit_pid)){
                process temp = dequeue(rdy_q);
                enqueue(rdy_q, temp);
            }
        }
        if(terminated){
            terminated = 0;
            int rm_idx = -1;
            process terminated_child = dequeue(rdy_q);
            total_child--;
            count_child--;
        }
        if(count_child > 0){
            sch_p.sched_priority = 4;
            now_child_pid = rdy_q -> front.pid;
            assert(sched_setscheduler(now_child_pid, SCHED_FIFO, &sch_p) != -1);
        }
        if(count_child == 0) unit_time();
        time_counter++;
    }
    int total_time = 0;
    pid_t cpid;
    if((cpid = getpid()) != scheduler_pid){
        int burst_t = p_array[ptr_now_process].burst_t;
        for(int i = 0; i < burst_t; i++){
            unit_time();
            sch_p.sched_priority = 2;
            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1);
            total_time++;
        }
        unit_time();     
        syscall(335, &ts_end);
        total_time++;
        printf("%s %d\n", p_array[ptr_now_process].name, cpid);
        syscall(334, flag, cpid, &ts_start, &ts_end);       
        _exit(0);
    }
    return;
}