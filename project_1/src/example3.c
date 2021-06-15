#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

sigjmp_buf mark_1;
sigjmp_buf mark_2;

void p(void);
void recover(void);

int main(void)
{
  int i = 5;
  if (sigsetjmp(mark_1,1) != 0) {
    printf("second %d\n", i);
    printf("siglongjmp() has been called\n");
    recover();
    exit(1);
  }
  i++;
  printf("first %d\n", i);
  sigsetjmp(mark_2,1);
  i++;
  printf("third %d\n", i);
  printf("sigsetjmp() has been called\n");
  p();
}

void p(void)
{
   int error = 0;
   error = 9;
   if (error != 0) {
      printf("%d \n", error);
      siglongjmp(mark_2, -1);
    }
    printf("%d \n", error);
}

void recover(void)
{
}
