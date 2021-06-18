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


struct scheduler_config{
    char* algorithm;
    char* operation_mode;
    int num_threads;
    int quantum;
    int workload;
    int bills;
    int arrival_time;
    int ticket_am;
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
    int tickets[255];
    int ticket_am;
};



void sig_alrm(int);
void run_process_fcfs();
void config_get(struct scheduler_config *config);

struct scheduler_config config;
static int curr_process;
static int first_run;
struct thread * processes;
sigjmp_buf scheduler_buf;

int ticket_am;
void create_process(void * function, int p){


        address_t sp, pc;
        processes[p].current_workload = 0;
        processes[p].state = MAIN;
}
void update_tickets(int process, int lottery_ticket){
    int tickets_copy[processes[process].ticket_am];
    int i =0;
    memcpy(tickets_copy, processes[process].tickets, processes[process].ticket_am*sizeof(int));
    for(int j = 0; j< processes[process].ticket_am; j++){
        if(tickets_copy[j] != lottery_ticket){
            processes[process].tickets[i] = tickets_copy[j];
            i++;
        }
    }
    processes[process].ticket_am = i;
    //Need to implement the creation of new tickets when we are out of tickets

}
void create_all_tickets(){
    int ticket_amm = config.num_threads*config.ticket_am;
    int tickets[255]; 
    //Create tickets by acending numbers to make them unique.
    for(int i = 0; i< ticket_amm; i++) tickets[i] = i;
    //Distribute the tickets to each process.
    for(int i = 0; i< config.num_threads; i++){
        memcpy(processes[i].tickets,&tickets[config.ticket_am*i], sizeof(int)*config.ticket_am);
        processes[i].ticket_am = config.ticket_am;
    }
}

int get_tickets(int * all_tickets){
    int ticket_quantity = 0;
    for(int i =0; i< config.num_threads; i++){
        ticket_quantity += processes[i].ticket_am;
        memcpy(processes[i].tickets, &all_tickets[ticket_quantity], sizeof(int)*processes[i].ticket_am);
    }
    return ticket_quantity;
}
int get_ticket_owner(int ticket){
    int ticket_owner;
    for(int i =0; i<config.num_threads; i++){
        for(int j = 0; j< processes[i].ticket_am; j++){
            if(processes[i].tickets[j] == ticket){
                ticket_owner = i;
                break;
            }
        }
    }
    return ticket_owner;
}
int main(void){
    printf("Setting up the system\n");
    config_get(&config);
    processes = malloc(sizeof(processes)*config.num_threads);
    int i;
    printf("Selected algorithm: %s\n",config.algorithm);
    for(i = 0; i < config.num_threads; i++ ){
        create_process(run_process_fcfs,i);
    }
    if(!strcmp(config.algorithm,"FCFS")){
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
    }
    else if(!strcmp(config.algorithm,"LS")){
        printf("LS mode\n");
        int lottery_tickets[255];
        int ticket_amount;
        int random_ticket_entry;
        int winner_process;

        create_all_tickets();
        signal(SIGALRM, sig_alrm);
        ticket_amount = get_tickets(lottery_tickets);
        //Select a random ticket as a winner!
        random_ticket_entry = rand()%ticket_am;
        //Get the ticket owner.
        winner_process = get_ticket_owner(lottery_tickets[random_ticket_entry]);
        //Remove the ticket from the ticket queue.
        update_tickets(winner_process,lottery_tickets[random_ticket_entry]);
        curr_process = winner_process;
        ualarm(config.quantum,0);
        run_process_fcfs();
        //Initialice with Tickets.
        


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
  int ticket_am;

  FILE *fp = fopen("config.txt", "r");

  int i = 0;
  fscanf (fp, "%s", algorithm);
  fscanf (fp, "%s", operation_mode);      
  fscanf (fp, "%d", &process_num);      
  fscanf (fp, "%d", &arrival_time);      
  fscanf (fp, "%d", &workload);      
  fscanf (fp, "%d", &bills);      
  fscanf (fp, "%d", &quantum);  
  fscanf (fp, "%d", &ticket_am); 
  fclose (fp);

  
  config->algorithm = algorithm;
  config->operation_mode = operation_mode;
  config->num_threads= process_num;
  config->arrival_time = arrival_time;
  config->workload = workload;
  config->bills = bills;
  config->ticket_am = ticket_am;
  
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