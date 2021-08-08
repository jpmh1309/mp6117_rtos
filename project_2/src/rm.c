#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
//There's a hard limit on 256 tasks
//The progress of the task is the amount of computation time that the task already had.
//task_id 0 is reserved for no task running
enum task_status{WAITING, RUNNING, COMPLETED};
struct Task{
    unsigned int task_id;
    unsigned int c;
    unsigned int p;
    unsigned int progress;
    enum task_status status;

};
struct ScheduleTimeUnit{
    unsigned int task_id;
    size_t num_task_arrow;
    unsigned int arrow_task_ids[256];
};
struct Deadlines{
    unsigned int task_id;
    unsigned int deadline;
};
//Function to compare taks priority
int taskPriorityCompare(const void *a, const void *b){
    struct Task *taskA = (struct Task *)a;
    struct Task *taskB = (struct Task *)b;

    return(taskA->p - taskB->p);
}
//Function to compute the lcm of two numbers
int lcm(int a, int b){
    int m = 1;
    while(m%a || m%b) m++;
    return m;
}


// Function should return the schedule for the rate monotonic given the provided tasks.
// task id for the array of tasks should be unique.
// There should be at least one task. 
// For all tasks p >= c
int createRateMonotonicSchedule(struct Task * tasks, size_t num_tasks, struct ScheduleTimeUnit * o_schedule){
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
    printf("num_deadlines: %d\n", (int) num_deadlines);
    //Loop to schedule the tasks
    //Tasks are arranged in priorities. The running_task variable refers to the index of the array and not the task id.
    //Schedule the first task out of the loop
    unsigned int running_task = 0;
    priorityTasks[running_task].status = RUNNING;
    priorityTasks[running_task].progress += 1;
    schedule[0].task_id = priorityTasks[running_task].task_id;
    
    if(priorityTasks[running_task].c == priorityTasks[running_task].progress){
        priorityTasks[running_task].progress = 0;
        priorityTasks[running_task].status = COMPLETED;
        //Aparently just moveing the running_task to the max will make it go to the lower priority in the next loop
        running_task = num_tasks;
    }
    for(int i =1; i< run_period +1; i++){
        //Check if any task has reached a deadline and update the tasks to the running state.
        int deadlineMissed = 0;
        for(int j = 0; j< num_deadlines; j++){
            
            if(deadlines[j].deadline == i){
                printf("Deadline: %d, task_id: %d\n",deadlines[j].deadline, deadlines[j].task_id);
                for(int k = 0; k < num_tasks; k++){
                   if(priorityTasks[k].task_id ==  deadlines[j].task_id){
                       if(priorityTasks[k].status == WAITING 
                        || priorityTasks[k].status == RUNNING){
                            //Deadline was reached. Abort and stop scheduling.
                            run_period = i-1; //Not sure if i or i-1
                            deadlineMissed = 1;
                            printf("Deadline was missed\n");
                            break;
                       }
                       //Update the taks from completed to waiting.
                       else{
                           priorityTasks[k].status = WAITING;
                       }
                   }  
                }                 
            }
        }
        if(deadlineMissed) break;
        //Check if a task with higher priority needs to run.
        for(int j =0; j< running_task; j++){
            if(priorityTasks[j].status == WAITING){
                priorityTasks[running_task].status = WAITING;
                priorityTasks[j].status = RUNNING;
                running_task = j;
                //We just want the highest priority to execute so break out of the loop
                break;

            }
        }
        //Run the task, update the schedule and the running task.
        if(priorityTasks[running_task].status == RUNNING){
            schedule[i].task_id = priorityTasks[running_task].task_id;
            priorityTasks[running_task].progress++;

            //Check if the task finished running:
            if(priorityTasks[running_task].c == priorityTasks[running_task].progress){
                priorityTasks[running_task].progress = 0;
                priorityTasks[running_task].status = COMPLETED;
                //Aparently just moveing the running_task to the max will make it go to the lower priority in the next loop
                running_task = num_tasks;
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

int main(){
    struct Task tasks[3];
    struct ScheduleTimeUnit schedule[255];
    int schedulePeriod = 0;
    //Scheludable scenario
    
    tasks[0].task_id = 1;
    tasks[0].c = 1;
    tasks[0].p = 6;

    tasks[1].task_id = 2;
    tasks[1].c = 2;
    tasks[1].p = 9;

    tasks[2].task_id = 3;
    tasks[2].c = 6;
    tasks[2].p = 18;
    

    //Unschedulable scenario.
    /*
    tasks[0].task_id = 1;
    tasks[0].c = 3;
    tasks[0].p = 6;

    tasks[1].task_id = 2;
    tasks[1].c = 4;
    tasks[1].p = 9;
    */
    schedulePeriod = createRateMonotonicSchedule(tasks, 3, schedule);
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

}