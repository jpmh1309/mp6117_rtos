#define _POSIX_SOURCE
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

sigjmp_buf * env;
int current;

double next(double a, double x, int i)
{
    return a*((pow((double)(2*i-1), 2)) / ((2*i)*(2*i+1)*pow(x, 2)));
}

double arcsin(double x, int n)
{
  double a = 0;
  int i = 0;
  double sum = 0;
  a = x;
  for (int i = 1; i <= n; i++) {
    sum += a;
    a = next(a, x, i);
  }
  return 2*sum;
}

void execute_thread()
{
  printf("Process ID: %d\n", current);
  if (!sigsetjmp(env[current], 1)) {
    current++;
    if(current == 5) current = 0;
    siglongjmp(env[current], -1);
  }
}

// void switch_thread()
// {
//
// }

void parent()
{
while(1)
fprintf(stderr, "Parent process\n");
}

void child(int *i)
{
while(1)
fprintf(stderr, "Child process %d\n", *i);
}

/* create child context and return to parent */
void create_thread(int i)
{
  if (sigsetjmp(env[i], 1)) {
    child(&current);
    printf("E\n");
  } else {
    printf("D\n");
    char stack[1024];
    siglongjmp(env[0], -1);
  }
}

void thread_setup(int num_threads)
{
  for (int i = 0; i < num_threads-1; i++)
  {
    if (!sigsetjmp(env[0], 1)) {
      create_thread(i+1);
    }
  }
}

void fcfs_scheduler()
{
  signal(SIGALRM, fcfs_scheduler);
  alarm(1);

  execute_thread();
}

int main(void)
{
  int num_threads = 5;

  current = 0;
  env = malloc(num_threads * sizeof(sigjmp_buf));

  signal(SIGALRM, fcfs_scheduler);
  alarm(1);

  thread_setup(num_threads);

  parent();  
}
