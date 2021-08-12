/*****************************************************************************
* Project Name: Project 2: Real Time Scheduling                              *
* File Name: llf.c                                                           *
* University: Costa Rica Institute of Technology                             *
* Lecture: MP-6117 Real Time Operating Systems                               *
* Students: - David Martinez                                                 *
*           - Jose Martinez                                                  *
* Year: 2021                                                                 *
******************************************************************************/
#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/structs.h"

struct ExecutionContext{
    //Execution time
    int t;
    //Deadlines *
    struct Deadlines * deadlines;
    //Amount of deadlines
    size_t num_deadlines;

};

float llf_schedulabity(configuration config)
{
  float U = 0.0;
  for(unsigned int i=0; i<config.number_tasks; i++)
  {
    U += (float)config.execution_time[i]/(float)config.period[i];
  }
  return U;
}


int ldfCompare(const void *a, const void *b, void * executionContext){
    struct Task *taskA = (struct Task *)a;
    struct Task *taskB = (struct Task *)b;
    struct ExecutionContext *context = (struct ExecutionContext *) executionContext;

    unsigned int deadlineA = UINT_MAX;
    unsigned int deadlineB = UINT_MAX;

    unsigned int laxityA;
    unsigned int laxityB;
    //Check if both taskA and taskB are in waiting state.
    if(taskA->status == COMPLETED)
        return INT_MAX;
    if(taskB->status == COMPLETED)
        return INT_MIN;
    //Get the deadline for taskA
    for(int i =0; i < context->num_deadlines; i++){
        if(context->deadlines[i].deadline > context->t){
            if(context->deadlines[i].task_id == taskA->task_id){
                deadlineA = context->deadlines[i].deadline;
                break;
            }
        }
    }
    //Get the deadline for taskB
    for(int i =0; i < context->num_deadlines; i++){
        if(context->deadlines[i].deadline > context->t){
            if(context->deadlines[i].task_id == taskB->task_id){
                deadlineB = context->deadlines[i].deadline;
                break;
            }
        }
    }
    laxityA = deadlineA - (context->t + (taskA->c - taskA->progress));
    laxityB = deadlineB - (context->t + (taskB->c - taskB->progress));
    if(laxityA == laxityB){
        if(taskA->status == RUNNING) return INT_MIN;
        if(taskB->status == RUNNING) return INT_MAX;
    }
    printf("Context. deadlines: %d, t:%d\n", (int) context->num_deadlines, context->t);
    printf("TaskA. task_id = %d, laxity = %d, deadline = %d, progress = %d\n", taskA->task_id, laxityA, deadlineA, taskA->progress);
    printf("TaskB. task_id = %d, laxity = %d, deadline = %d, progress = %d\n", taskB->task_id, laxityB, deadlineB, taskB->progress);
    return(laxityA - laxityB);
}

int createLeastLaxitudeFirst(struct Task * tasks, size_t num_tasks, struct ScheduleTimeUnit * o_schedule){
    struct Task priorityTasks[num_tasks];
    size_t run_period;
    //Check that all tasks_id are unique.
    for(int i = 0; i < num_tasks -1; i++){
        for(int j = i+1; j < num_tasks; j++){
            if(tasks[i].task_id == tasks[j].task_id ){
                printf("Error: Input task ID for two tasks is the same\n");
                return -1;
            }
        }
    }

    //Check that for all tasks p >= c
    for(int i = 0; i < num_tasks; i++){
        if(tasks[i].p < tasks[i].c){
            printf("Error: Task period is lower than the computation time\n");
            return -1;
        }
        else if (tasks[i].task_id == 0){
            printf("Error: task_id 0 is reserved for no task running\n");
            return -1;
        }
    }

    //Copy the tasks into another array and reset the progress and mark them as waiting (all are ready to run)
    for(int i = 0; i < num_tasks; i++){
        priorityTasks[i] = tasks[i];
        priorityTasks[i].progress = 0;
        priorityTasks[i].status = WAITING;
    }
    //Sort the input tasks based on their priority (shortest period first)
    qsort(priorityTasks,num_tasks, sizeof(struct Task), taskPriorityCompare);


    //TODO:Compute the tests from [1] and [2]

    //Calculate the lcm of the periods to know for how long the simulation should run.
    run_period = priorityTasks[0].p;
    for(int i =1; i < num_tasks; i++){
        run_period = lcm(run_period, priorityTasks[i].p);
    }

    printf("The run will be a max of: %d time units as it is the lcm. As long as it is schedulable\n", (int)run_period);

    //Create the schedule struct(run_period +1 is because the last period requires arrows)
    struct ScheduleTimeUnit schedule[run_period + 1];
    //Setup where the arrows will be set in the graph
    for(int i =0; i< run_period + 1; i++){

        schedule[i].num_task_arrow = 0;
        for(int j =0; j< num_tasks; j++){
            if(i % priorityTasks[j].p == 0) {
                schedule[i].arrow_task_ids[schedule[i].num_task_arrow] = priorityTasks[j].task_id;
                schedule[i].num_task_arrow++;
            }
        }
    }
    //Deadlines
    size_t num_deadlines = 0;
    for(int i = 0; i < num_tasks; i++){
        num_deadlines += run_period / priorityTasks[i].p;
    }
    printf("num_deadlines: %d\n", (int) num_deadlines);
    struct Deadlines deadlines[num_deadlines];
    num_deadlines = 0;
    for(int i =1; i< run_period + 1; i++){
        for(int j =0; j< num_tasks; j++){
            if(i % priorityTasks[j].p == 0) {
                deadlines[num_deadlines].deadline = i;
                deadlines[num_deadlines].task_id = priorityTasks[j].task_id;
                num_deadlines++;
            }
        }
    }
    //Tasks are arranged in meaning that the shortest deadline is first.
    //The running_task variable refers to the index of the array and not the task id.
    //Schedule the first task out of the loop
    //The first deadline also correlates with deadlines[0]
    unsigned int running_task = 0;
    //currentDeadline is implemented to avoid changing the context in case two tasks have the same deadline
    struct Deadlines currentDeadline = deadlines[0];
    priorityTasks[running_task].status = RUNNING;
    priorityTasks[running_task].progress += 1;
    schedule[0].task_id = priorityTasks[running_task].task_id;
    printf("t: %d. task: %d\n", 0, schedule[0].task_id);
    if(priorityTasks[running_task].c == priorityTasks[running_task].progress){
        priorityTasks[running_task].progress = 0;
        priorityTasks[running_task].status = COMPLETED;
        //Aparently just moveing the running_task to the max will make it go to the lower priority in the next loop
        printf("t: %d, task_id: %d priority changed to COMPLETED.\n", 0,  priorityTasks[running_task].task_id );
        running_task = num_tasks;
    }
    for(int i =1; i< run_period +1; i++){
        printf("\n\nt: %d\n", i);

        int deadlineMissed = 0;

        //Check if any deadline was missed, if not, set the next in waiting priority
        for(int j =0; j < num_deadlines ; j++){
            if(deadlines[j].deadline == i){
                for(int k = 0; k< num_tasks; k++){
                    if(priorityTasks[k].task_id == deadlines[j].task_id){
                        if(priorityTasks[k].status == WAITING
                        || priorityTasks[k].status == RUNNING){
                            run_period = i-1;
                            deadlineMissed = 1;
                            printf("Deadline was missed in time: %d for task_id: %d \n", i, deadlines[j].task_id);
                        }
                        else{
                            priorityTasks[k].status = WAITING;
                            printf("t: %d, task_id: %d priority changed to WAITING due to period\n", i,  priorityTasks[k].task_id );
                            //printf("deadlines[j].deadline: %d\n",deadlines[j].deadline);
                            //printf("The value of j is: %d\n",j);
                            //printf("Printing first 10 deadlines\n");
                            //for(int l =0; l<10; l++)
                                //printf("t:%d deadline for task_id: %d\n",deadlines[l].deadline,deadlines[l].task_id);
                        }
                    }

                }

            }
        }
        if(deadlineMissed) break;
        //Sort the tasks by laxity.
        struct ExecutionContext executionContext;

        executionContext.deadlines = deadlines;
        executionContext.num_deadlines = num_deadlines;
        executionContext.t = i;

        qsort_r(priorityTasks,num_tasks, sizeof(struct Task), ldfCompare, (void * ) &executionContext);
        printf("Printing asorted tasks\n");
        for(int j =0; j< num_tasks; j++){
            printf("task_id: %d, status: %d\n", priorityTasks[j].task_id, priorityTasks[j].status);
        }
        //Move RUNNING task to WATING state:
        for(int j =0; j< num_tasks; j++){
            if(priorityTasks[j].status == RUNNING)
                priorityTasks[j].status = WAITING;
        }
        //Run the task, update the schedule and the running task.
        if(priorityTasks[0].status != COMPLETED){
            priorityTasks[0].status = RUNNING;
            schedule[i].task_id = priorityTasks[0].task_id;
            priorityTasks[0].progress++;

            //Check if the task finished running:
            if(priorityTasks[0].c == priorityTasks[0].progress){
                priorityTasks[0].progress = 0;
                priorityTasks[0].status = COMPLETED;
                printf("t: %d, task_id: %d priority changed to COMPLETED.\n", i,  priorityTasks[0].task_id );
            }
        }
        //If no task is running.
        else{
            schedule[i].task_id = 0;
        }
        printf("t: %d. task: %d\n", i, schedule[i].task_id);
    }
    for(int i =0; i< run_period +1 ; i++){
        o_schedule[i] = schedule[i];
    }

    return run_period +1;
}


/*
int main(){
    struct Task tasks[3];
    struct ScheduleTimeUnit schedule[1024];
    int schedulePeriod = 0;
    //Scheludable scenario

    tasks[0].task_id = 1;
    tasks[0].c = 2;
    tasks[0].p = 6;

    tasks[1].task_id = 2;
    tasks[1].c = 2;
    tasks[1].p = 8;

    tasks[2].task_id = 3;
    tasks[2].c = 3;
    tasks[2].p = 10;


    //Unschedulable scenario.
    /*
    tasks[0].task_id = 1;
    tasks[0].c = 3;
    tasks[0].p = 6;

    tasks[1].task_id = 2;
    tasks[1].c = 4;
    tasks[1].p = 9;
    */

    /*
    schedulePeriod = createLeastLaxitudeFirst(tasks, 3, schedule);
    printf("Reading from the retrurned schedule\n");
    for(int i =0; i< schedulePeriod; i++){
        printf("t: %d. task: %d\n", i, schedule[i].task_id);
        if(schedule[i].num_task_arrow != 0){
            for(int j = 0; j< schedule[i].num_task_arrow; j++){
                printf("Arrow at t: %d for task_id:%d\n", i, schedule[i].arrow_task_ids[j]);
            }
        }
    }

    return 0;

}*/
