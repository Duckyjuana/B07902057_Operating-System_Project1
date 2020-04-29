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

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25

void SJF(process* p_array, int N)
{
    char *flag = "[Project1]";
    struct timespec ts_start;
    struct timespec ts_end;
    process *rbegin, *rend, *wbegin, *wend;
    process now;
    rbegin = rend = wbegin = (&p_array[0]);
    wend = (&p_array[N]);

    struct sched_param sch_p;
    sch_p.sched_priority = 3;
    assert(sched_setscheduler(getpid(), SCHED_FIFO, &sch_p) != -1);
    sch_p.sched_priority = 4;
    int t = 0, r =0;
    int total = N;
    int status;
    process H[N];
    heap hp;
    hp.now_size = 0;
    hp.MAXsize = N;
    hp.container = H;
    pid_t pid = getpid();
    while(total > 0){
        while((wbegin != wend) && wbegin -> arrive_t == t){
            syscall(335, &ts_start);
            pid = fork();
            wbegin -> pid = pid;
            if(pid < 0) perror("fork() failed...\n");
            else if(pid == 0){
                pid_t cpid = getpid();
                sch_p.sched_priority = 2;
                for(int i = 0; i < wbegin -> burst_t - 1; i++){
                    unit_time();
                    assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1);
                }
                unit_time();
                syscall(335, &ts_end);
                syscall(334, flag, getpid(), &ts_start, &ts_end);
                printf("%s %d\n", wbegin->name, getpid());
                fflush(stdout);
                exit(0);
            }
            else{
                assert(Push(&hp, wbegin));
                wbegin++;
                rend++;
            }
        }
        if((r == 0) && (!Empty(&hp))){
            assert(Front(&hp, &now));
            assert(Pop(&hp));
            assert(sched_setscheduler(now.pid, SCHED_FIFO, &sch_p) != -1);
            r++;
        }
        else if(r == 1){
            if(waitpid(now.pid, &status, WNOHANG) != 0){
                rbegin++;
                total--;
                r--;
                if(!Empty(&hp)){
                    assert(Front(&hp, &now));
                    assert(Pop(&hp));
                    assert(sched_setscheduler(now.pid, SCHED_FIFO, &sch_p) != -1);
                    r++;
                }
                else unit_time();
            }
            else assert(sched_setscheduler(now.pid, SCHED_FIFO, &sch_p) != -1);
        }
        else unit_time();
        t++;
    }
    return;
}