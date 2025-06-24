#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include "list.h"

#define MAX_PRIORITY 10  


struct node *tskList = NULL;


void add(char *name, int priority, int burst) {

    if (priority < 1 || priority > MAX_PRIORITY || burst <= 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

    //Allocate memory for the new task
    Task *newTask = malloc(sizeof(Task));
    newTask->name = strdup(name);
    newTask->priority = priority;
    newTask->burst = burst;


    struct node **temp = &tskList;

    // Insert the task in SJF
    while (*temp && (*temp)->task->burst < burst) {
        temp = &((*temp)->next);
    }
    
    insert(temp, newTask); 
}

//Executes tasks in SJF
void schedule() {
    // If no tasks exist
    if (tskList == NULL) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

    struct node *temp = tskList;
    int current_time = 0;  
    int tot_wait = 0;      
    int tot_turnaround = 0;
    int tot_resp = 0;      
    int tsk_num = 0;      

    // Process each task in SJF by burst time
    while (temp) {
        int wait_time = current_time;  // Wait time before execution
        run(temp->task, temp->task->burst);
        current_time += temp->task->burst; // Update current time after execution
        int turnaround_time = current_time; // Total time from arrival to completion

     
        tot_wait += wait_time;
        tot_turnaround += turnaround_time;
        tot_resp += wait_time;
        tsk_num++;

        temp = temp->next;  // Next task 
    }

 
    printf("\nAverage Waiting Time = %.2f\n", (float)tot_wait / tsk_num);
    printf("Average Turnaround Time = %.2f\n", (float)tot_turnaround / tsk_num);
    printf("Average Response Time = %.2f\n", (float)tot_resp / tsk_num);
}
