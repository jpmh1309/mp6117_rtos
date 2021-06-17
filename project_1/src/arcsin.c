#include <math.h>
#include <stdio.h>
#include <stdlib.h>

long double next(long double a, long double x, int i)
{
    return a*((pow((long double)(2*i-1), 2)) / ((2*i)*(2*i+1)*pow(x, 2)));
}

long double arcsin(long double x, int n)
{
  long double a = 0;
  int i = 0;
  long double sum = 0;
  // a = x;
  for (int i = 1; i <= n; i++) {
    sum += a;
    a = next(a, x, i);
  }
  return 2*sum;
}

int main()
{
   // printf() displays the string inside quotation
   printf("Hello, World!\n");
   printf("Hello, World!\n");
   printf("arcsin: %f \n", 2*asin(1.0));
   printf("arcsin: %LF \n", arcsin(1.0, 23500));
   printf("pi: %lf \n", M_PI);
   return 0;
}
