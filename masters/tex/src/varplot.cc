#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "common.h"
#include "vector.h"

// -----------------------------------------------------------------------

double f_log(double x)
{
  return log10(x);
}

// -----------------------------------------------------------------------

Ext edat = {"sta", 1, 0};
Ext *exts[] = {&edat, NULL};

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  s_data mtx;
  int i, count, c, width = 3, verbose = 0;
  exts[0]->flags = EXT_DEFAULT;

  while ((c = getopt(argc, argv, "vw:")) != EOF)
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
      }
  int optbase = optind, filecount = argc - optind;
  Vector vAlpha(filecount), vBeta(filecount);
  for (; optind < argc; optind++)
    {
      FILE *input, *output;
      char tmp[256], fname[256];
      mtx.rows = -1;
      Ext *rtn = CheckExtension(argv[optind], fname, exts);
      switch(rtn->id)
        {
        case 1:
          strcpy(tmp, fname);
          strcat(tmp, ".");
          strcat(tmp, rtn->ext);
          if (!(input = fopen(tmp, "r")))
            break;
          count = ReadFileTextFloat(width, &mtx, input);
          mtx.columns = width;
          mtx.rows = count;
          fclose(input);
          break;
        }
      if (mtx.rows == -1)
        {
          fprintf(stderr, "%s could not be found.\n", tmp);
          break;
        }
      strcpy(tmp, fname);
      strcat(tmp, ".dat");      
      if (!(output = fopen(tmp, "w")))
        {
          fprintf(stderr, "%s could not be opened for writing.\n", tmp);
          break;
        }

//     if (verbose) DumpMtxFloat(&mtx);

      Vector x (mtx.rows), y (mtx.rows);
      for (i = 0; i < mtx.rows; i++)
        {
          x[i] = ((double*)mtx.ptr)[i * mtx.columns];
          y[i] = ((double*)mtx.ptr)[i * mtx.columns + 2];
        }
      delete mtx.ptr;
      if (verbose)
        {
          fprintf(stderr, "count = %d x.length = %d\n", count, x.length());
          fprintf(stderr, "Loaded x,y Vectors.\n");
        }
      int n = x.length();
      x.map(f_log);
      y.map(f_log);
      for (int i = 0; i < n; i++)
        fprintf(output,"%lf %lf\n", x[i], y[i]);
    }
}
