#define _POSIX_SOURCE
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
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

void execute_thread(int pid)
{
  printf("Process ID: \n", pid);
}

// void switch_thread()
// {
//
// }

void thread_setup(int num_threads)
{
  for (int i = 0; i < num_threads; i++)
  {
    if (!sigsetjmp(env[i], 1)) {
    create(1);
    }
  }
}

void fcfs_scheduler()
{


}

int main(void)
{
  int num_threads = 5;

  current = 0;
  env = malloc(num_threads * sizeof(sigjmp_buf));

  thread_setup(num_threads);

  return 0;
}
