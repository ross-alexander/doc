#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "vector.h"

Ext eAtf = {"atf", 1, 0}, eAbf = {"abf", 2, 0};
Ext *exts[] = {&eAtf, &eAbf, NULL};

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  FILE *input;
  char tmp[256];
  s_data mtx;
  int offset = 1, ch, width = 3, verbose = 0, count;
  exts[0]->flags = EXT_DEFAULT;
  Ext *rtn;

  while ((ch = getopt(argc, argv, "vo:w:")) != EOF)
    switch(ch)
      {
      case '?':
        printf("%s:  [-v(erbose)] [-w width] [-o offset]\n", argv[0]);
        exit(1);
        break;
      case 'w':
        width = atoi(optarg);
        break;
      case 'o':
        offset = atoi(optarg);
        break;
      case 'v':
        verbose = 1;
        break;
      }

  for (int i = optind; i < argc; i++)
    {
      mtx.rows = -1;
      rtn = CheckExtension(argv[i], tmp, exts);
      switch(rtn->id)
        {
        case 1:
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
          int v_length = freq.length();
          int n = v_length;
          double freq_x = freq.fold(f_identity, f_add, 0.0);
          double freq_x2 = freq.fold(f_square, f_add, 0.0);
          double mean = freq_x / n;
          double var = (n == 1) ? (freq_x2 - 2*mean*freq_x + n*mean*mean) / n
            : (freq_x2 - 2*mean*freq_x + n*mean*mean) / (n-1);
          fprintf(stdout, "%-20s E[x] = %9.3lf  Var(n-1)[x] = %9.3lf\n", tmp, mean, var);
        }
    }
}
