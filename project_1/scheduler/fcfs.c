#include <setjmp.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define STACK_SIZE 4096

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
		"rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

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
    char stack[STACK_SIZE];
    void *(function);
};



void sig_alrm(int);
void run_process_fcfs();
void config_get(struct scheduler_config *config);

struct scheduler_config config;
static int curr_process;
static int first_run;
struct thread * processes;
sigjmp_buf scheduler_buf;


void create_process(void * function, int p){


        address_t sp, pc;
        processes[p].current_workload = 0;
        processes[p].state = MAIN;
}

int main(void){
    printf("Setting up the system\n");
    config_get(&config);
    processes = malloc(sizeof(processes)*config.num_threads);
    int i;
    for(i = 0; i < config.num_threads; i++ ){
        create_process(run_process_fcfs,i);
    }
    first_run = 1;
    //printf("Setting up the alarm\n");
    signal(SIGALRM, sig_alrm);

    curr_process = 0;
    //Save the state of the main.
    sigsetjmp(scheduler_buf, 1);

    
    curr_process = (curr_process == config.num_threads - 1) ? 0 : curr_process + 1;
    if(processes[curr_process].current_workload < config.workload){
        //printf("Running process: %d\n", curr_process);
        if(!strcmp(config.operation_mode,"PREEMPTIVE")) {
            //printf("Preemtive mode.\n");
            ualarm(config.quantum,0);
        }
        //siglongjmp(processes[curr_process].env,1);
        run_process_fcfs();
    }
    else{
        ualarm(config.quantum,0);
        siglongjmp(scheduler_buf,1);
    }
    while(1);
}
void run_process_ls(){
    
}
void run_process_fcfs(){
    //printf("being called\n");
    processes[curr_process].state = RUNNING;
    while(1){
        processes[curr_process].pi += 2*( 2*pow(-1, processes[curr_process].current_workload ) / (1+2*processes[curr_process].current_workload));
        //printf("current_workload: %d\n",processes[curr_process].current_workload );
        if(processes[curr_process].current_workload >= config.workload){
            printf("Process finished!!!\n");
            printf("Pi calculation: %Lf\n", processes[curr_process].pi);
            processes[curr_process].state = FINISHED;
            siglongjmp(scheduler_buf,1);
        }
        processes[curr_process].state = RUNNING;
        if(!strcmp(config.operation_mode, "NON-PREEMPTIVE")){
            //printf("Setup is non-preemptive\n");
            if(processes[curr_process].current_workload % config.quantum == 0){
                //printf("Saving status for process\n");
                    processes[curr_process].current_workload ++;
                    processes[curr_process].state = WAITING;
                    siglongjmp(scheduler_buf,1);
            }

        }
        processes[curr_process].current_workload ++;
    }
}

void sig_alrm(int signo){
    processes[curr_process].state = WAITING;
    //printf("Timer Expired\n");
    siglongjmp(scheduler_buf,1);
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