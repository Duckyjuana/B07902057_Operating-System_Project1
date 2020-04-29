#ifndef _FUNC_H_
#define _FUNC_H_

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>
#define BUFFSIZE 32

typedef struct {
    char name[BUFFSIZE];
    int arrive_t;
    int burst_t;
    pid_t pid;
} process;

typedef struct {
    int now_size;
    int MAXsize;
    process* container;
} heap;

int Push(heap*, process*);
int Pop(heap*);
int Front(heap*, process*);
int Empty(heap*);

void unit_time();
void print(process p);
int str_same(char* c1, char* c2);
void child_exec(struct sched_param sch_p, process now_p, struct timespec ts_start, struct timespec ts_end);
void swap(process* p1, process* p2);
int larger_p(process* p1, process* p2);
int earlier_p(process* p1, process* p2);
void toheap(process* p, int N);
void MAXheap(process* p, int N, int index);
int partition(process* p, int N);
void qsort_ready(process* p, int N);
void print_heap(process *p, int N);

typedef struct snode {
    process data;
    struct snode *next;
} s_node;
void push(s_node** top_ref, process new_data);
process pop(s_node** top_ref);
typedef struct {
    process front;
    process end;
    s_node *stack1;
    s_node *stack2;
} queue;
void enqueue(queue *q, process x);
process dequeue(queue *q);

#endif