
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

enum thread_state {
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

// void thread_create(void *function, struct thread_metadata data)
// {
//   thread_counter++;
//   address_t sp, pc;
//
//   list_of_threads[thread_counter].id = thread_counter;
//   list_of_threads[thread_counter].function = function;
//   list_of_threads[thread_counter].data = data;
//   list_of_threads[thread_counter].state = READY;
// 	list_of_threads[thread_counter].data.actual_workload = 0;
//
//   sp = (address_t)list_of_threads[thread_counter].stack + STACK_SIZE - sizeof(address_t);
//   pc = (address_t)list_of_threads[thread_counter].function;
//
//   sigsetjmp(list_of_threads[thread_counter].env,1);
//   (list_of_threads[thread_counter].env->__jmpbuf)[JB_SP] = translate_address(sp);
//   (list_of_threads[thread_counter].env->__jmpbuf)[JB_PC] = translate_address(pc);
//   sigemptyset(&list_of_threads[thread_counter].env->__saved_mask);
// }

void scheduler_setup(struct scheduler_config config,
                     struct thread *scheduler_thread)
{
  for(int i = 0; i < config.num_threads; i++)
  {
    scheduler_thread[i].pid = i;
    scheduler_thread[i].function = NULL;
    scheduler_thread[i].pi = 0.0;
    sigsetjmp(scheduler_thread[i].env,1);
  }
}

void config_get(struct scheduler_config *config)
{
  config->algorithm = "FCFS";
  config->operation_mode = "PREEMPTIVE";
  config->num_threads = 20;
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

  scheduler_setup(config, scheduler_thread);

  // for(int i = 0; i < num_threads; i++)
  //   scheduler_thread[i].pid = i;

  for(int i = 0; i < config.num_threads; i++)
    printf("%d\n", scheduler_thread[i].pid);

  // for (size_t i = 1; i < number_of_threads+1; i++) {
	// 	thread_generic_tmp.workload = thread_workloads[i-1];
	//   thread_generic_tmp.bills = thread_bills[i-1];
	// 	thread_generic_tmp.result = 0;
	// 	create_hilo(compute_arcsin, thread_generic_tmp);
	// }

  // thread_setup();
  return 0;

}
