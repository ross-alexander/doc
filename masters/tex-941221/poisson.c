#include <stdio.h>
#include <math.h>
#include <getopt.h>

double factorial(double x)
{
  register int y;
  double result = 1.0;
  for (y = 1; y <= x; y++)
    result *= y;
  return result;
}

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  int ch, t;
  double lambda = 14.0;
  int max = 20;

  while ((ch = getopt(argc, argv, "l:m:")) != EOF)
    switch (ch)
      {
      case 'l':
	lambda = atoi(optarg);
	break;
      case 'm':
	max = atoi(optarg);
	break;
      }

  for (t = 0; t <= max; t++)
    {
      double x = t;
      double x_fact = factorial(x);
      fprintf(stdout, "%d %lf\n", t, (exp(-lambda) *  pow(lambda, x) / x_fact));
    }
}
