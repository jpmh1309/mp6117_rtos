#include <setjmp.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



struct scheduler_config{
    char* algorithm;
    char* operation_mode;
    int num_threads;
    int quantum;
    int workload;
    int bills;
    int arrival_time;
};

enum thread_state{
  MAIN,
  READY,
  WAITING,
  RUNNING,
  FINISHED
};

struct thread{
    int pid;
    sigjmp_buf env;
    long double pi;
    int current_workload;
    //Lottery schedule
    int bill;
    int i;
    enum thread_state state;
};



void sig_alrm(int);
void run_process_fcfs();
void config_get(struct scheduler_config *config);

struct scheduler_config config;
static int curr_process;
static int first_run;
struct thread * processes;
sigjmp_buf scheduler_buf;

int main(void){
    printf("Setting up the system\n");
    config_get(&config);
    processes = malloc(sizeof(processes)*config.num_threads);
    first_run = 1;
    printf("Setting up the alarm\n");
    signal(SIGALRM, sig_alrm);

    curr_process = -1;
    //Save the state of the main.
    sigsetjmp(scheduler_buf, 1);

    if(first_run){
        curr_process++;
        if(curr_process == config.num_threads -1 ) first_run = 0;
        printf("First Run. Running process: %d\n", curr_process);
        if(!strcmp(config.operation_mode, "PREEMPTIVE")){
            printf("Preemtive mode.\n");
            alarm(0.001);
        }
        run_process_fcfs();
    }
    else{
        //Round robin current process
        //Process #0 might be avoided cause it is the main loop.
        
        curr_process = (curr_process == config.num_threads - 1) ? 0 : curr_process + 1;
        if(processes[curr_process].state != FINISHED){
            printf("Running process: %d\n", curr_process);
            if(!strcmp(config.operation_mode,"PREEMPTIVE")) {
                printf("Preemtive mode.\n");
                alarm(1/1000000000);
            }
            siglongjmp(processes[curr_process].env,1);
        }
    }

    while(1);
}

void run_process_fcfs(){
    processes[curr_process].current_workload = 0;
    processes[curr_process].state = RUNNING;
    while(1){
        processes[curr_process].pi += 2*( 2*pow(-1, processes[curr_process].current_workload ) / (1+2*processes[curr_process].current_workload));
        printf("current_workload: %d\n",processes[curr_process].current_workload );
        if(!strcmp(config.operation_mode, "NON-PREEMPTIVE")){
            //printf("Setup is non-preemptive\n");
            if(processes[curr_process].current_workload % config.quantum == 0){
                printf("Saving status for process\n");
                if(!sigsetjmp(processes[curr_process].env,1)){
                    processes[curr_process].state = WAITING;
                    siglongjmp(scheduler_buf,1);
                }
                else {
                    //printf("Coming back from process\n");
                    processes[curr_process].state = RUNNING;
                }
            }

        }
        if(processes[curr_process].current_workload >= config.workload){
            printf("Process finished!!!\n");
            printf("Pi calculation: %Lf\n", processes[curr_process].pi);
            processes[curr_process].state = FINISHED;
            siglongjmp(scheduler_buf,1);
        }
        processes[curr_process].current_workload ++;
        

    }
}

void sig_alrm(int signo){
    printf("Timer Expired\n");
    //if(!sigsetjmp(scheduler_buf,1)) siglongjmp(scheduler_buf,1);
}


void config_get(struct scheduler_config *config)
{
  char * algorithm = malloc(sizeof(char)*255);
  char * operation_mode = malloc(sizeof(char)*255);
  int process_num;
  int arrival_time;
  int workload;
  int bills;
  int quantum;

  FILE *fp = fopen("config.txt", "r");

  int i = 0;
  fscanf (fp, "%s", algorithm);
  fscanf (fp, "%s", operation_mode);      
  fscanf (fp, "%d", &process_num);      
  fscanf (fp, "%d", &arrival_time);      
  fscanf (fp, "%d", &workload);      
  fscanf (fp, "%d", &bills);      
  fscanf (fp, "%d", &quantum);      
  fclose (fp);

  
  config->algorithm = algorithm;
  config->operation_mode = operation_mode;
  config->num_threads= process_num;
  config->arrival_time = arrival_time;
  config->workload = workload;
  config->bills = bills;
  
  // Main Thread + Number of Threads
  config->quantum = quantum;

  printf("config->algorithm: %s\n", config->algorithm);
  printf("config->operation_mode: %s\n", config->operation_mode);
  printf("config->num_threads: %d\n", config->num_threads);
  printf("config->arrival_time: %d\n", config->arrival_time);
  printf("config->workload: %d\n", config->workload);
  printf("config->bills: %d\n", config->bills);
  printf("config->quantum: %d\n", config->quantum);
}