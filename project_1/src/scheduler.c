
#define _POSIX_SOURCE
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "address.h"

#define STACK_SIZE 4096

struct scheduler_config{
  char* algorithm;
  char* operation_mode;
  int num_threads;
  int quantum;
  int bills;
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
  config->algorithm = "FCFS";
  config->operation_mode = "PREEMPTIVE";
  // Main Thread + Number of Threads
  config->num_threads = 5;
  config->quantum = 1000;
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
