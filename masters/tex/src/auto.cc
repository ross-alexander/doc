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
  int c, offset = 1, width = 2, verbose = 0, limit = 20000, step = 1;

  while ((c = getopt(argc, argv, "vl:s:w:")) != EOF)
    switch(c)
      {
      case '?':
        printf("%s:  No useful information.\n", argv[0]);
        break;
      case 'w':
        width = atoi(optarg);
        break;
      case 'v':
        verbose = 1;
        break;
      case 's':
        step = atoi(optarg);
        break;
      case 'l':
        limit = atoi(optarg);
        break;
      }

  Ext *rtn;
  exts[0]->flags = EXT_DEFAULT;

  for (; optind < argc; optind++)
    {
      FILE *input, *output;
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
          strcat(tmp, ".acr");
          if (!(output = fopen(tmp, "w")))
            break;

          int v_length = freq.length();
          int n = v_length;
          double freq_x = freq.fold(f_identity, f_add, 0.0);
          double freq_x2 = freq.fold(f_square, f_add, 0.0);
          double mean = freq_x / n;
          double var = freq_x2 - 2*mean*freq_x + n*mean*mean;

          for (i = 0; (i <= v_length / 2) && (i < limit); i += step)
            {
              n = v_length - i;
              
              double x1 = 0.0;
              double x2 = 0.0;
              double x1_x2 = 0.0;

              for (int j = 0; j < n; j++)
        	{
        	  x1 += freq[j];
        	  x2 += freq[j + i];
        	  x1_x2 += freq[j] * freq[j + i];
        	}
              double cov = (x1_x2 - mean*x1 - mean*x2 + n*mean*mean) / var;

              fprintf(output, "%d %lf\n", v_length - n, cov);
              fflush(output);
            }
          fclose(output);
        }
    }
}
