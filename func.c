#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>
#include <time.h>
#include <sys/syscall.h>
#include "func.h"

#ifndef _GNU_SOURCE_
#define _GNU_SOURCE_
#endif

int Push(heap *hp, process *P)
{
    if(hp -> now_size < hp -> MAXsize){
        (hp -> container)[hp -> now_size] = *P;
        (hp -> now_size)++;
        toheap(hp -> container, hp -> now_size);
        return 1;
    }
    else return 0;
}
int Pop(heap *hp)
{
    if(hp -> now_size > 0){
        (hp -> now_size)--;
        swap(hp -> container, ((hp -> container)+(hp -> now_size)));
        toheap(hp -> container, hp -> now_size);
        return 1;
    }
    else return 0;
}
int Front(heap *hp, process *P)
{
    if(hp -> now_size > 0){
        *P = *(hp -> container);
        return 1;
    }
    else return 0;
}
int Empty(heap *hp)
{
    if(hp -> now_size > 0) return 0;
    else return 1;
}

void unit_time()
{
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
    return;
}

void print(process p)
{
    printf("%s %d %d %d\n", p.name, p.pid, p.arrive_t, p.burst_t);
    return;
}

int str_same(char* c1, char* c2)
{
    for(int i = 0; c1[i] == c2[i]; i++){
        if(c1[i] == '\0') return 1;
    }
    return 0;
}

void child_exec(struct sched_param sch_p, process now_p, struct timespec ts_start, struct timespec ts_end)
{
    char *flag = "Project1";
    int total_t = 0;
    int burst_t = now_p.burst_t;
    pid_t now_pid = getpid();

    for(int i = 0; i < burst_t-1; i++){
        unit_time();
        sch_p.sched_priority = 2;
        assert(sched_setscheduler(now_pid, SCHED_FIFO, &sch_p) != -1);
        total_t++;
    }

    unit_time();
    syscall(335, &ts_end);
    total_t++;

    printf("%s %d\n", now_p.name, now_pid);
    syscall(334, flag, now_pid, &ts_start, &ts_end);
    _exit(0);
}

void toheap(process* p, int N)
{
    if(N == 1) return;
    for(int i = (N >> 1) - 1; i >= 0; i--){
        MAXheap(p, N, i);
    }
    return;
}

void MAXheap(process* p, int N, int index)
{
    int L = 0;
    int left = (index << 1) + 1;
    int right = (index << 1) + 2;
    if(left < N && larger_p((p + left), (p + index))){
        L = left;
    }
    else L = index;
    if(right < N && larger_p((p + right), (p + L))){
        L = right;
    }
    if(L != index){
        swap(&p[L], &p[index]);
        MAXheap(p, N, L);
    }
    return;
}

int larger_p(process* p1, process* p2)
{
    if(p1 -> burst_t < p2 -> burst_t || (p1 -> burst_t == p2 -> burst_t && p1 -> arrive_t < p2 -> arrive_t))
        return 1;
    else return 0;
}
int earlier_p(process* p1, process* p2)
{
    if(p1 -> arrive_t < p2 -> arrive_t || (p1 -> arrive_t == p2 -> arrive_t && (strcmp(p1 -> name, p2 -> name) < 0)))
        return 1;
    else return 0;
}

int partition(process* p, int N)
{
    int pivot_i = 0;
    for(int i = 1; i < N; i++){
        if(earlier_p((p + i), (p + pivot_i))){
            for(int j = i; j > pivot_i; j--){
                swap(&p[j], &p[j-1]);
            }
            pivot_i++;
        }
    }
    return pivot_i;
}
void qsort_ready(process* p, int N)
{
    if(N == 1) return;
    int new_pivot = partition(p,N);
    if(new_pivot > 1) qsort_ready(p, new_pivot);
    if(N-new_pivot-1 > 1) qsort_ready(&p[new_pivot+1], N-new_pivot-1);
    return;
 }
void swap(process* p1, process* p2)
{
    process* temp = (process*)malloc(sizeof(process));
    *temp = *p1;
    *p1 = *p2;
    *p2 = *temp;
    return;
}
void print_heap(process* p, int N)
{
    for(int i = 0; i < N; i++)
        print(p[i]);
    return;
}

void enqueue(queue *q, process x)
{
    if(q -> stack1 == NULL && q -> stack2 == NULL)
        q -> front = x;
    q -> end = x;
    push(&q -> stack1, x);
    return;
}
process dequeue(queue *q)
{
    process x;
    if(q -> stack1 == NULL && q -> stack2 == NULL){
        printf("Q is empty!");
        getchar();
        exit(0);
    }
    if(q -> stack2 == NULL){
        while(q -> stack1 != NULL){
            x = pop(&q -> stack1);
            push(&q -> stack2, x);
        }
    }

    x = pop(&q ->stack2);
    if(q -> stack2 != NULL){
        q -> front = q -> stack2 -> data;
    }
    else{
        if (q -> stack1 != NULL){
            process temp;
            while(q -> stack1 != NULL){
                temp = pop(&q -> stack1);
                push(&q -> stack2, temp);
            }
            q -> front = temp;
        }
    }
    return x;
}

void push(s_node** top_ref, process new_data)
{
    s_node* new_node = (s_node*)malloc(sizeof(s_node));
    if(new_node == NULL){
        printf("stack overflow!\n");
        getchar();
        exit(0);
    }
    new_node -> data = new_data;
    new_node -> next = (*top_ref);
    (*top_ref) = new_node;
    return;
}
process pop(s_node** top_ref)
{
    process res;
    s_node *top;
    if(*top_ref == NULL){
        printf("stack overflow!\n");
        getchar();
        exit(0);
    }
    else{
        top = *top_ref;
        res = top->data;
        *top_ref = top->next;
        free(top);
        return res;
    }
}