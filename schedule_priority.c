#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define MAX_PRIORITY 10

// Head pointer of tasks` list
struct node *task_list = NULL; 



void add(char *name, int priority, int burst) {
    if (priority < 1 || priority > MAX_PRIORITY || burst <= 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }


    // Allocate memory for the new task
    Task *newTask = (Task *)malloc(sizeof(Task));
    newTask->name = strdup(name);
    newTask->priority = priority;
    newTask->burst = burst;

    // New node for the task
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->task = newTask;
    newNode->next = NULL;

    // Insert task in priority order and FIFO for same priority
    if (task_list == NULL || task_list->task->priority < priority) {
        newNode->next = task_list;
        task_list = newNode;
        return;
    }

    struct node *temp = task_list;
    struct node *prev = NULL;

    while (temp != NULL && temp->task->priority >= priority) {
        prev = temp;
        temp = temp->next;
    }

    prev->next = newNode;
    newNode->next = temp;
}

/**
 * Execute tasks based on priority scheduling.
 * Higher priority tasks run first, and FIFO for same priority
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
    int tsk_num = 0;     

    struct node *temp = task_list;
    
    while (temp != NULL) {
        Task *task = temp->task;

        int wait_time = current_time; // Wait time before execution
        int turnaround_time = wait_time + task->burst; // Total time from arrival to completion

       
        tot_wait += wait_time;
        tot_turnaround += turnaround_time;
        tot_resp += wait_time;
        tsk_num++;


        // Run the task for full burst time
        run(task, task->burst);
        current_time += task->burst;

        // Remove completed task from the list
        struct node *toDelete = temp;
        temp = temp->next;
        free(toDelete->task->name);
        free(toDelete->task);
        free(toDelete);
    }

  
    if (tsk_num > 0) {
        printf("\nAverage Waiting Time = %.2f\n", (float)tot_wait / tsk_num);
        printf("Average Turnaround Time = %.2f\n", (float)tot_turnaround / tsk_num);
        printf("Average Response Time = %.2f\n", (float)tot_resp / tsk_num);
    }
}
