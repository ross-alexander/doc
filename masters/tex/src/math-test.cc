#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "mathlib.h"

int main()
{
  int arraySize = 4000;
  int *array = new int[arraySize];
  int i;

  srand48(time((time_t*)NULL));
  for (i = 0; i < arraySize; i++)
    array[i] = 0;
  
  double mean = arraySize / 2;
  double deviation = arraySize / 6;

  double sum_x = 0.0;
  double sum_sqx = 0.0;
  
  int n = 10000;
  for (i = 0; i < n; i++)
    {
//      double val = rnorm();
      double val = rgamma(1.0, 0.5);
//      int rescale = (int)(val * deviation + mean);
      int rescale = (int)(val * deviation);
      if (rescale >= 0 && rescale < arraySize)
        array[rescale]++;
      sum_x += val;
      sum_sqx += val * val;
    }
  for (i = 0; i < arraySize; i++)
    fprintf(stdout, "%d\n", array[i]);
  double mn = sum_x / n;
  double var = (sum_sqx - n * mn * mn) / (n-1);
  fprintf(stderr, "%lf %lf\n", mn, var);
}
