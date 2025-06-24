#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define QUANTUM 10   // Time slice for RR
#define MIN_PRIORITY 1    
#define MAX_PRIORITY 10   

struct node *task_list = NULL;  // Head of the task queue
struct node *tail = NULL;       // Pointer to the last task in queue
int tsk_num = 0;  // Total number of tasks added



typedef struct {
    int firstRunTime;   // First time the task starts execution
    int arrivalTime;    // Arrival time 
    int totalRunTime;   // Total execution time
} TaskInfo;


TaskInfo taskInfo[100];  // excusion details 100 tasks max


void add(char *name, int priority, int burst) {

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY || burst <= 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }


    Task *newTask = (Task *)malloc(sizeof(Task));
    newTask->name = strdup(name);
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = tsk_num;  //task ID


    // New node for the task
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->task = newTask;
    newNode->next = NULL;

    // Add task to the queue FIFO
    if (task_list == NULL) {
        task_list = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }

    // Execution tracking
    taskInfo[tsk_num].firstRunTime = -1;  // not started 
    taskInfo[tsk_num].arrivalTime = 0;    // tasks arrive at time 0
    taskInfo[tsk_num].totalRunTime = 0;   // No execution

    tsk_num++;
}

/**
 * RR scheduling with time quantum.
 * Each task gets  time slice
 */
void schedule() {

    if (task_list == NULL) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

    int current_time = 0;  
    int tot_wait = 0;      
    int tot_turnaround = 0;
    int tot_resp = 0;    

    while (task_list != NULL) {
        struct node *prev = NULL;
        struct node *curr = task_list;

        int tasksRemaining = 0;  // unfinished tasks 

        while (curr != NULL) {
            Task *task = curr->task;
            int taskId = task->tid;
            int executeTime = (task->burst > QUANTUM) ? QUANTUM : task->burst;

            // First execution time for response time calculation
            if (taskInfo[taskId].firstRunTime == -1) {
                taskInfo[taskId].firstRunTime = current_time;
                tot_resp += (current_time - taskInfo[taskId].arrivalTime);
            }

            // Execute the task for time slice
            run(task, executeTime);
            current_time += executeTime;
            task->burst -= executeTime;
            taskInfo[taskId].totalRunTime += executeTime;

            struct node *nextTask = curr->next;

            if (task->burst == 0) {
                tot_turnaround += (current_time - taskInfo[taskId].arrivalTime);
                tot_wait += (current_time - taskInfo[taskId].arrivalTime - taskInfo[taskId].totalRunTime);

                // Remove the task from list
                if (prev == NULL) {
                    task_list = nextTask;
                } else {
                    prev->next = nextTask;
                }

                free(curr->task->name);
                free(curr->task);
                free(curr);

                curr = nextTask;
            } else {
                tasksRemaining++;

                // Move unfinished task to end of the queue
                if (prev == NULL) {
                    task_list = nextTask;
                } else {
                    prev->next = nextTask;
                }

                tail->next = curr;
                tail = curr;
                curr->next = NULL;

                curr = nextTask;
            }
        }

        if (tasksRemaining == 0) {
            break;  // no ramaining tasks,exit
        }
    }


    if (tsk_num > 0) {
        printf("\nAverage Waiting Time = %.2f\n", (float)tot_wait / tsk_num);
        printf("Average Turnaround Time = %.2f\n", (float)tot_turnaround / tsk_num);
        printf("Average Response Time = %.2f\n", (float)tot_resp / tsk_num);
    }
}
