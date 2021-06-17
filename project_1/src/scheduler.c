
#define _POSIX_SOURCE
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "address.h"
#include <yaml.h>

#define STACK_SIZE 4096

struct scheduler_config{
  char* algorithm;
  char* operation_mode;
  int num_threads;
  int quantum;
  int bills;
  int arrival_time;
  int work_load;
};

enum thread_state{
  MAIN,
  READY,
  WAITING,
  RUNNING,
  FINISHED,
  OPENED
};

struct thread{
  int pid;
  sigjmp_buf env;
  char stack[STACK_SIZE];
  void *(function);
  long double pi;
  enum thread_state state;
};

void arcsin()
{

}

void threads_create(void *function,
                   struct scheduler_config config,
                   struct thread *scheduler_thread)
{
  address_t sp, pc;

  for(int i = 1; i < config.num_threads; i++)
  {
    scheduler_thread[i].pid = i;
    scheduler_thread[i].function = function;
    scheduler_thread[i].pi = 0.0;
    scheduler_thread[i].state = READY;

    sp = (address_t)scheduler_thread[i].stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)scheduler_thread[i].function;

    sigsetjmp(scheduler_thread[i].env,1);
    (scheduler_thread[i].env->__jmpbuf)[JB_SP] = translate_address(sp);
    (scheduler_thread[i].env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&scheduler_thread[i].env->__saved_mask);
  }
}

void main_thread_create(struct thread *scheduler_thread)
{
  // for(int i = 0; i < config.num_threads; i++)
  // {
  scheduler_thread[0].pid = 0;
  scheduler_thread[0].function = NULL;
  scheduler_thread[0].state = MAIN;
  sigsetjmp(scheduler_thread[0].env, 1);
  // }
}

void config_get(struct scheduler_config *config)
{
  char * algorithm = malloc(sizeof(char)*255);
  char * operation_mode = malloc(sizeof(char)*255);
  int process_num;
  int arrival_time;
  int work_load;
  int bills;
  int quantum;

  FILE *fp = fopen("config.txt", "r");

  int i = 0;
  fscanf (fp, "%s", algorithm);
  fscanf (fp, "%s", operation_mode);      
  fscanf (fp, "%d", &process_num);      
  fscanf (fp, "%d", &arrival_time);      
  fscanf (fp, "%d", &work_load);      
  fscanf (fp, "%d", &bills);      
  fscanf (fp, "%d", &quantum);      
  fclose (fp);

  
  config->algorithm = algorithm;
  config->operation_mode = operation_mode;
  config->num_threads= process_num;
  config->arrival_time = arrival_time;
  config->work_load = work_load;
  config->bills = bills;
  
  // Main Thread + Number of Threads
  config->num_threads = 5;
  config->quantum = 1000;

  printf("config->algorithm: %s\n", config->algorithm);
  printf("config->operation_mode: %s\n", config->operation_mode);
  printf("config->num_threads: %d\n", config->num_threads);
  printf("config->arrival_time: %d\n", config->arrival_time);
  printf("config->work_load: %d\n", config->work_load);
  printf("config->bills: %d\n", config->bills);
}

int main(int argc, char **argv)
{
  int status;
  struct scheduler_config config;
  struct thread *scheduler_thread;

  // Get scheduler configuratuion parameters from config.yaml file
  config_get(&config);

  // Create thread structs
  scheduler_thread = malloc(config.num_threads * sizeof(struct thread));

  scheduler_thread[2].pid  = 1000;

  for(int i = 0; i < config.num_threads; i++)
    printf("%d\n", scheduler_thread[i].pid);

  // Main thread setup
  main_thread_create(scheduler_thread);

  // for(int i = 0; i < num_threads; i++)
  //   scheduler_thread[i].pid = i;

  for(int i = 0; i < config.num_threads; i++)
    printf("%d\n", scheduler_thread[i].pid);

  threads_create(arcsin, config, scheduler_thread);

  for(int i = 0; i < config.num_threads; i++)
  {
    printf("pid %d state %d \n", scheduler_thread[i].pid, scheduler_thread[i].state);
  }

  //
  // for (size_t i = 1; i < number_of_threads+1; i++) {
	// 	thread_generic_tmp.workload = thread_workloads[i-1];
	//   thread_generic_tmp.bills = thread_bills[i-1];
	// 	thread_generic_tmp.result = 0;
	// 	create_hilo(compute_arcsin, thread_generic_tmp);
	// }

  // thread_setup();
  return 0;

}
