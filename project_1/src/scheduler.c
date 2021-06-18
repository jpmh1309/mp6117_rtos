
#define _POSIX_SOURCE
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include "address.h"

#define STACK_SIZE 4096
#define MAIN_THREAD 0
#define START_THREAD 1
#define WORKLOAD_TERMS 50

struct scheduler_config{
  char* algorithm;
  char* operation_mode;
  int num_threads;
  int quantum;
  int workload;
  int bills;
};

enum thread_state{
  MAIN,
  READY,
  WAITING,
  RUNNING,
  FINISHED
};

struct thread{
  // THREAD INFO
  int pid;
  sigjmp_buf env;
  char stack[STACK_SIZE]; //**
  void *(function);
  enum thread_state state;
  // DATA
  long double pi;
  int current_workload;
  // LOTTERY SCHEDULING
  int bill;
};

// GLOBAL VARIABLES
int current_thread;
int next_thread;
struct scheduler_config config;
struct thread *scheduler_thread;

void thread_execute();
void thread_finish();
void scheduler();

// FUNCTIONS
void sleepsec(int n) {
	for (size_t i = 0; i < n; i++) {
		//HELLO WORLD
	}
}

void timer_quantum(int quantum_ms, void *function){
	struct itimerval timer;

  int sec = quantum_ms / 1000;
  int usec = (quantum_ms % 1000 ) * 1000;

   /* Configure the timer to expire after X sec... */
   timer.it_value.tv_sec = sec;
   timer.it_value.tv_usec = usec;
   /* ... and every X sec after that. */
   timer.it_interval.tv_sec = sec;
   timer.it_interval.tv_usec = usec;

	if(function != NULL) {signal(SIGALRM, function);}
	setitimer(ITIMER_REAL, &timer, NULL);
}

// long double next(long double a, long double x, int i)
// {
//     return a*((pow((long double)(2*i-1), 2)) / ((2*i)*(2*i+1)*pow(x, 2)));
// }
//
// long double arcsin(long double x, int n)
// {
//   long double a = 0;
//   int i = 0;
//   long double sum = 0;
//   a = x;
//   for (int i = 1; i <= n; i++) {
//     sum += a;
//     a = next(a, x, i);
//   }
//   return 2*sum;
// }


void thread_execute(void)
{
  int count = 0;
  printf("Executing thread\n");
  for(; scheduler_thread[current_thread].current_workload < config.workload; scheduler_thread[current_thread].current_workload++)
  {
    scheduler_thread[current_thread].pi += 2*( 2*pow(-1, scheduler_thread[current_thread].current_workload ) / (1+2*scheduler_thread[current_thread].current_workload));
    // NON-PREEMPTIVE MODE LEAVE CPU
    // count++;
    // if (count==config.quantum) {
    //   scheduler_thread[current_thread].state = WAITING;
    //   scheduler();
    // }
    // sleepsec(10000);
  }
    // sleepsec(1215752192);
    // int count = current_thread.data.workload;
    // int actual_workload = 0;

    // for (int i = 0; i < count; i++) {
    //   for (int n = 0; n < 50; n++) {
    //     current_thread.data.result += 2*( 2*pow(-1, actual_workload) / (1+2*actual_workload));
		// 		current_thread.data.actual_workload = actual_workload;
    //
		// 		// Sleep used to avoid a super-fast termination
		// 		sleepsec(4000);
    //     actual_workload++;
    //   }
    //   if ( ((i+1)%work_limit == 0) && (algorithm == 1) && (opt_mode == 0)) {
    //       printf("Current Workload (%d) , Workload Expired!\n", i);
    //       printf("Workload expired, jumping to next thread!, Partial Result = %LF\n\n", current_thread.data.result);
		// 			current_thread.state = WAITING;
		// 			current_thread.data.actual_workload = actual_workload;
    //       save_current_state();
    //       scheduler();
    //   }
    // }
    // current_thread.data.result = current_thread.data.result*2;
    thread_finish();
}
//

// void save_current_state(struct thread *scheduler_thread){
//   scheduler_thread[current_thread.id].id = current_thread.id;
//   scheduler_thread[current_thread.id].function = current_thread.function;
//   scheduler_thread[current_thread.id].state = current_thread.state;
//   scheduler_thread[current_thread.id].data.result = current_thread.data.result;
// 	scheduler_thread[current_thread.id].data.actual_workload = current_thread.data.actual_workload;
// }

void stop_timer(){
  timer_quantum(0, NULL);
}


void program_finished()
{
  printf("\nNo More Threads!\n\n");
  for (int i = START_THREAD; i < config.num_threads; i++) {
    printf("Final result of Thread #%d is %.30Lf.\n", scheduler_thread[i].pid, scheduler_thread[i].pi);
  }

  // Return to main thread
  siglongjmp(scheduler_thread[MAIN_THREAD].env, 1);
}


void fcfs_next_thread(){
  next_thread++;

  // FIXME
  // Reset next_thread and start again
  // if (next_thread == config.num_threads) next_thread = START_THREAD;

  stop_timer();
}

// void ls_next_thread(){
//   next_thread++;
//
//   if (next_thread == number_of_threads+1) {
//     next_thread = MAX_THREAD_NUMBER;
//   }
//
//   stop_timer();
// }

void signal_handler(){
	printf(" -> Time expired, jumping to next thread!, Partial Result = %.5LF, workload= %d\n\n", scheduler_thread[current_thread].pi, scheduler_thread[current_thread].current_workload);
	scheduler_thread[current_thread].state = WAITING;
  // save_current_state();
  scheduler();
}

void scheduler()
{
  bool finished;
  int coming_back = sigsetjmp(scheduler_thread[current_thread].env, 1);

	if(coming_back == 1) return;

  if (!strcmp(config.algorithm, "FCFS"))
  {
    printf("FCFS\n");
    fcfs_next_thread(config);

    // Set timer in preemptive mode
		if (!strcmp(config.operation_mode, "PREEMPTIVE"))
    {
      printf("FCFS-PREEMPTIVE");
      // TODO: FCFS PREEMPTIVE MODE
      timer_quantum(config.quantum, signal_handler);
		}
  } else {
    // TODO: LOTTERY SCHEDULING
    // ls_next_thread();
    printf("LOTTERY SCHEDULING");
  }

  // Check whether the state of all threads is FINISHED or not
  // for(int i = START_THREAD; i<config.num_threads; i++)
  // {
  //   finished &= scheduler_thread[i].state == FINISHED;
  // }

  // Check if all thread are in
  if (next_thread == config.num_threads)
  {
		stop_timer();
    program_finished();
  } else {
		// current_thread = list_of_threads[next_thread];
    current_thread = next_thread;
		scheduler_thread[current_thread].state = RUNNING;
    printf("Running Thread (%d) ", current_thread);
    siglongjmp(scheduler_thread[current_thread].env, 1);
  }
}

void thread_finish(){
  printf(" -> Thread (%d) Finished!\n", current_thread);
  scheduler_thread[current_thread].state = FINISHED;
  // save_current_state();
  scheduler();
}

void threads_create(void *function)
{
  address_t sp, pc;

  for(int i = START_THREAD; i < config.num_threads; i++)
  {
    scheduler_thread[i].pid = i;
    scheduler_thread[i].function = function;
    scheduler_thread[i].pi = 0.0;
    scheduler_thread[i].state = READY;
    scheduler_thread[i].current_workload = 0;

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
  scheduler_thread[MAIN_THREAD].pid = MAIN_THREAD;
  scheduler_thread[MAIN_THREAD].function = NULL;
  scheduler_thread[MAIN_THREAD].state = MAIN;
  sigsetjmp(scheduler_thread[MAIN_THREAD].env, 1);
  // }
}

void config_get()
{
  // FIXME: Get information from file
  config.algorithm = "FCFS";
  config.operation_mode = "PREEMPTIVE";
  // Main Thread + Number of Threads
  config.workload = 5*WORKLOAD_TERMS;
  config.num_threads = START_THREAD + 5;
  config.quantum = 10;

  // config.algorithm = "LOTTERY SCHEDULING";
  // config.operation_mode = "NON-PREEMPTIVE";
  // // Main Thread + Number of Threads
  // config.num_threads = 5;
  // config.quantum = 1000;
}

int main(int argc, char **argv)
{
  int status;
  // struct scheduler_config config;
  // struct thread *scheduler_thread;

  // Get scheduler configuratuion parameters from config.yaml file
  config_get();

  // Create thread structs
  scheduler_thread = malloc(config.num_threads * sizeof(struct thread));

  // scheduler_thread[2].pid  = 1000;

  // for(int i = 0; i < config.num_threads; i++)
  //   printf("%d\n", scheduler_thread[i].pid);

  // Main thread setup
  main_thread_create(scheduler_thread);

  // for(int i = 0; i < num_threads; i++)
  //   scheduler_thread[i].pid = i;

  // for(int i = 0; i < config.num_threads; i++)
  //   printf("%d\n", scheduler_thread[i].pid);

  threads_create(thread_execute);

  for(int i = 0; i < config.num_threads; i++)
  {
    printf("pid %d state %d \n", scheduler_thread[i].pid, scheduler_thread[i].state);
  }

  // Start scheduler
  scheduler();

  //
  // for (size_t i = 1; i < number_of_threads+1; i++) {
	// 	thread_generic_tmp.workload = thread_workloads[i-1];
	//   thread_generic_tmp.bills = thread_bills[i-1];
	// 	thread_generic_tmp.result = 0;
	// 	create_hilo(compute_thread_execute, thread_generic_tmp);
	// }

  // thread_setup();
  return 0;

}
