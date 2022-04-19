#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "common.h"
#include "vector.h"

// ----------------------------------------------------------------------------

Ext eatf = {"atf", 1, 0}, eabf = {"abf", 2, 0};
Ext *exts[] = {&eatf, &eabf, NULL};

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  s_data mtx;
  int i, logout = 0, offset = 1, limit = 10000, c, width = 2, verbose = 0;

  while ((c = getopt(argc, argv, "vlm:w:")) != EOF)
    switch(c)
      {
      case '?':
        printf("%s:  [-w width] [-v verbose] [-l log output] [-m maximum]\n", argv[0]);
        break;
      case 'w':
        width = atoi(optarg);
        break;
      case 'v':
        verbose = 1;
        break;
      case 'l':
        logout = 1;
        break;
      case 'm':
        limit = atoi(optarg);
        break;
      }

  Ext *rtn;
  exts[0]->flags = EXT_DEFAULT;

  for (; optind < argc; optind++)
    {
      FILE *input, *stdoutput, *logoutput;
      char tmp[256], fname[256];
      int count;
      mtx.rows = -1;
      rtn = CheckExtension(argv[optind], fname, exts);
      switch(rtn->id)
        {
        case 1:
          strcpy(tmp, fname);
          strcat(tmp, ".");
          strcat(tmp, rtn->ext);
          if (!(input = fopen(tmp, "r")))
            break;
          count = ReadFileText(width, &mtx, input);
          mtx.columns = width;
          mtx.rows = count;
          fclose(input);
          break;
        case 2:
          strcpy(tmp, fname);
          strcat(tmp, ".");
          strcat(tmp, rtn->ext);
          if (!(input = fopen(tmp, "rb")))
            break;
          ReadFileBinary(&mtx, input);
          count = mtx.rows;
          fclose(input);
        }

      if (mtx.rows != -1)
        {
          Vector freq (mtx.rows);
          for (int i = 0; i < mtx.rows; i++)
            freq[i] = ((int*)mtx.ptr)[i * mtx.columns + offset];
          delete mtx.ptr;
          if (verbose)
            {
              fprintf(stderr, "count = %d freq.length = %d\n", count, freq.length());
              fprintf(stderr, "Loaded freq Vector.\n");
            }
          strcpy(tmp, fname);
          strcat(tmp, ".sta");
          if (!(stdoutput = fopen(tmp, "w")))
            break;
          if (logout)
            {
              strcpy(tmp, fname);
              strcat(tmp, ".dat");
              if (!(logoutput = fopen(tmp, "w")))
        	break;
            }
          for (i = 1; (i <= freq.length() / 20) && (i < limit); i++)
            {
#ifdef DEBUG
              fprintf(stderr, ".");
#endif
              register s = freq.length() / i;
              double sum;
              Vector tmp(s);
              for (register int j = 0; j < s; j++)
        	{
        	  sum = 0;
        	  for (register int k = 0; k < i; k++)
        	    sum += freq[j * i + k];
        	  tmp[j] = sum / i;
        	}
              double n = tmp.length();
              double mean = tmp.sum_ident() / n;
              double x_squared = tmp.sum_square();
              double var = (x_squared - n * mean * mean) / (n-1);
              fprintf(stdoutput, "%d %lf %lf\n", i, mean, var);
              if (logout) fprintf(logoutput, "%lf %lf\n", log10(i), log10(var));
#ifdef DEBUG
              fflush(output);
#endif
            }
#ifdef DEBUG
          fprintf(stderr, "\n");
#endif
          fclose(stdoutput);
          if (logout) fclose(logoutput);
        }
    }
}

