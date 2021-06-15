#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
  return sum;
}

int main()
{
   // printf() displays the string inside quotation
   printf("Hello, World!\n");
   printf("Hello, World!\n");
   printf("arcsin: %lf \n", 2*asin(1.0));
   printf("arcsin: %lf \n", arcsin(-1.0, 17808));
   printf("pi: %lf \n", M_PI);
   return 0;
}
