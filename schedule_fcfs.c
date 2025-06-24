#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include "list.h"

#define MAX_PRIORITY 10 


// Head pointer of tasks` list
struct node *task_list = NULL;  


void add(char *name, int priority, int burst) {
    if (priority < 1 || priority > MAX_PRIORITY || burst <= 0) {
        printf("Invalid Tasks in Input File\n");
        return;
    }


    // Allocate memory for the new task
    Task *new_task = malloc(sizeof(Task));
    new_task->name = strdup(name);
    new_task->priority = priority;
    new_task->burst = burst;

    // Allocate memory for the new linked list node
    struct node *new_node = malloc(sizeof(struct node));
    new_node->task = new_task; // Assign task to node
    new_node->next = NULL; 

    // add task in FIFO at the end of the list
    if (!task_list) {
        task_list = new_node; //make new node the head
    } else {
        struct node *temp = task_list;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

/**
 * FCFS scheduling
 * Tasks are executed in the order added
 */
void schedule() {
    if (!task_list) {
        printf("Invalid Tasks in Input File\n");
        return;
    }

    int current_time = 0; 
    int tot_wait = 0;  
    int tot_turnaround = 0;
    int tot_resp = 0; 
    int tsk_num = 0; 

    struct node *temp = task_list;

    while (temp) {
        int wait = current_time;  // Waiting time before execution
        int turnaround = wait + temp->task->burst;  // Total time from arrival to completion
       
        
        tot_wait += wait;
        tot_turnaround += turnaround;
        tot_resp += wait;
        tsk_num++;

        run(temp->task, temp->task->burst); // Execute the task
        current_time += temp->task->burst; // Update current time after execution
        temp = temp->next; // Next task in list
    }

   
    printf("Average waiting time = %.2f\n", (float)tot_wait / tsk_num);
    printf("Average turnaround time = %.2f\n", (float)tot_turnaround / tsk_num);
    printf("Average response time = %.2f\n", (float)tot_resp / tsk_num);
}
