#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define MAX_PRIORITY 10  
#define QUANTUM 10 // time slice for RR execution


struct node *priority_queues[MAX_PRIORITY + 1] = {NULL};  


static int tsk_num = 0;  
static int *first_Resp_Time;  
static int *rem_Burst;  


void add(char *name, int priority, int burst) {
    if (priority < 1 || priority > MAX_PRIORITY || burst <= 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

 
    Task *newTask = (Task *)malloc(sizeof(Task));
    newTask->name = strdup(name);
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = tsk_num;  //task ID

    // Insert task into priority queue
    insert(&priority_queues[priority], newTask);
    tsk_num++;
}

/**
 * Priority-based RR scheduling
 * Tasks with higher priority are executed first
 * Each task gets QUANTUM (time slice)
 */
void schedule() {
    if (tsk_num == 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

    int current_time = 0;  
    int tot_wait = 0;      
    int tot_turnaround = 0; 
    int tot_resp = 0;      
    int comp_tsk = 0; 


    // Tracking response time and remaining burst time
    first_Resp_Time = (int *)malloc(tsk_num * sizeof(int));
    rem_Burst = (int *)malloc(tsk_num * sizeof(int));


    // Initialize tracking arrays
    for (int i = 0; i < tsk_num; i++) {
        first_Resp_Time[i] = -1;  //  task has not started
        rem_Burst[i] = 0;
    }

    // highest to lowest priority
    for (int p = MAX_PRIORITY; p >= 1; p--) {
        struct node *taskList = priority_queues[p];

        // Reverse the list for FIFO order
        struct node *reversed = NULL;
        while (taskList) {
            struct node *next = taskList->next;
            taskList->next = reversed;
            reversed = taskList;
            taskList = next;
        }
        priority_queues[p] = reversed;

        // Process each task in the priority queue
        while (priority_queues[p]) {  
            struct node *curr = priority_queues[p];
            struct node *prev = NULL;

            while (curr) {
                Task *task = curr->task;
                int slice = (task->burst > QUANTUM) ? QUANTUM : task->burst; // time slice

                // task starts running
                if (first_Resp_Time[task->tid] == -1) {
                    first_Resp_Time[task->tid] = current_time;
                    tot_resp += current_time;
                    rem_Burst[task->tid] = task->burst;  
                }

                // Run task for the time slice
                run(task, slice);
                current_time += slice;
                task->burst -= slice;

                // Task is completed, calculation and remove it
                if (task->burst == 0) {
                    tot_turnaround += current_time;
                    tot_wait += (current_time - rem_Burst[task->tid]);
                    comp_tsk++;

                    // Remove task from queue
                    if (prev == NULL) {
                        priority_queues[p] = curr->next;
                    } else {
                        prev->next = curr->next;
                    }

                    struct node *temp = curr;
                    curr = curr->next;

                    // Free memory for completed task
                    free(temp->task->name);
                    free(temp->task);
                    free(temp);
                } else {
                    // Move to next task
                    prev = curr;
                    curr = curr->next;
                }
            }
        }
    }

    if (comp_tsk > 0) {
        printf("\nAverage Waiting Time = %.2f\n", (float)tot_wait / comp_tsk);
        printf("Average Turnaround Time = %.2f\n", (float)tot_turnaround / comp_tsk);
        printf("Average Response Time = %.2f\n", (float)tot_resp / comp_tsk);
    }

    // Free allocated memory
    free(first_Resp_Time);
    free(rem_Burst);
}
