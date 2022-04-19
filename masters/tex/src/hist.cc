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
  int *hist, max, i, smooth = 1, offset = 1, c, width = 2, verbose = 0;

  while ((c = getopt(argc, argv, "vs:w:")) != EOF)
    switch(c)
      {
      case '?':
        printf("%s:  [-s smooth] [-w width] [-v verbose]\n", argv[0]);
        break;
      case 'w':
        width = atoi(optarg);
        break;
      case 's':
        smooth = atoi(optarg);
        break;
      case 'v':
        verbose = 1;
        break;
      }

  Ext *rtn;
  exts[0]->flags = EXT_DEFAULT;

  for (; optind < argc; optind++)
    {
      FILE *input, *stdoutput;
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
          max = 0;
          Vector freq (mtx.rows);
          for (i = 0; i < mtx.rows; i++)
            {
              freq[i] = ((int*)mtx.ptr)[i * mtx.columns + offset];
              if (freq[i] > max) max = (int)(freq[i]);
            }
          delete mtx.ptr;
          if (verbose)
            {
              fprintf(stderr, "count = %d freq.length = %d\n", count, freq.length());
              fprintf(stderr, "Loaded freq Vector.\n");
            }
          sprintf(tmp, "%s-%02d.hst", fname, smooth);
          if (!(stdoutput = fopen(tmp, "w")))
            break;

          hist = new int[++max];
          for (i = 0; i < max; i++) hist[i] = 0;
          for (i = 0; i < freq.length(); i++)
            hist[(int)(freq[i])]++;

          i = 0;
          while (i < max)
            {
              double x = 0.0, y = 0.0;
              for (int j = 0; (j < smooth) && (i+j < max); j++)
        	{
        	  x += i+j;
        	  y += hist[i+j];
        	}
              fprintf(stdoutput, "%6.3lf %6.0lf\n", x/j, y);
              i += smooth;
            }
          fclose(stdoutput);
        }
    }
}

