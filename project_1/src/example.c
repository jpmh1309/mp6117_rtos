#define _POSIX_SOURCE
#define _XOPEN_SOURCE   600
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <unistd.h>

#define SECOND 100
#define STACK_SIZE 100
sigjmp_buf env[2];

void switchThreads()
{
  static int curThread = 0;
  int ret_val = sigsetjmp(env[curThread],1);
  printf("SWITCH: ret_val=%d\n", ret_val);
  if (ret_val == 1) {
    return;
  }
  curThread = 1 - curThread;
  siglongjmp(env[curThread],1);
}

void f()
{
  int i=0;
  while(1) {
    ++i;
    printf("in f (%d)\n",i);
    if (i % 3 == 0) {
      printf("f: switching\n");
      switchThreads();
    }
    usleep(SECOND);
  }
}

void g()
{
  int i=0;
  while(1) {
    ++i;
    printf("in f (%d)\n",i);
    if (i % 3 == 0) {
      printf("f: switching\n");
      switchThreads();
    }
    usleep(SECOND);
  }
}

char stack1[STACK_SIZE];
char stack2[STACK_SIZE];
typedef unsigned long address_t; // 64bit arch version
#define JB_SP 6 //
#define JB_PC 7 //

void setup()
{
  address_t sp, pc;
  sp = (address_t)stack1 + STACK_SIZE - sizeof(address_t);
  pc = (address_t)f;
  sigsetjmp(env[0],1);
  (env[0]->__jmpbuf)[JB_SP] = sp;
  (env[0]->__jmpbuf)[JB_PC] = pc;
  sigemptyset(&env[0]->__saved_mask); // empty signal mask

  // address_t sp, pc;
  sp = (address_t)stack2 + STACK_SIZE - sizeof(address_t);
  pc = (address_t)g;
  sigsetjmp(env[1],1);
  (env[0]->__jmpbuf)[JB_SP] = sp;
  (env[0]->__jmpbuf)[JB_PC] = pc;
  sigemptyset(&env[1]->__saved_mask); // empty signal mask
}

int main() {
  setup();
  siglongjmp(env[0],1);
  return 0;
}
