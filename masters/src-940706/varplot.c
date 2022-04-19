#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <getopt.h>
#include <math.h>

/* ----------------------------------------------------------------------- */

typedef struct {
  int rows, columns;
  double *data;
} s_data;

/* ----------------------------------------------------------------------- */

int file_read(int count, int max, int width, s_data *mtx, FILE *stream)
{
  double *ptr, *block, tmp;
  int i = 0, j;

#ifdef DEBUG
  fprintf(stderr, ".");
#endif

  block = (double*)alloca(sizeof(double) * width * max);
  while ((i < max) && !feof(stream))
    {
      ptr = block + i * width;
      for (j = 0; j < width; j++)
	{
	  fscanf(stream, "%lf", &tmp);
	  ptr[j] = tmp;
	}
      i++;
    }
  if (feof(stream))
    {
#ifdef DEBUG
      fprintf(stderr,"\n");
#endif
      i--;
      mtx->data = (double*)malloc(sizeof(double) * width * (i + count));
      memcpy(mtx->data + count * width, block, i * width * sizeof(double));
      return count + i;
    }
  else
    {
      j = file_read(count + max, max, width, mtx, stream);
      memcpy(mtx->data + count * width, block, max * width * sizeof(double));
      return j;
    }
}

/* ----------------------------------------------------------------------- */

void log_mtx(FILE *stream, s_data *mtx)
{
  int i,j;
  for (i = 0; i < mtx->rows; i++)
    fprintf(stream, "%lf  %lf\n",
	    log10(mtx->data[i * mtx->columns]),
	    log10(mtx->data[i * mtx->columns + 3]));
}

/* ----------------------------------------------------------------------- */

void dump_mtx(s_data *mtx)
{
  int i,j;
  for (i = 0; i < mtx->rows; i++)
    {
      for (j = 0; j < mtx->columns; j++)
	fprintf(stdout, "%8.3lf  ", mtx->data[i * mtx->columns + j]);
      fprintf(stdout, "\n");
    }
}

/* ----------------------------------------------------------------------- */

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  s_data mtx;
  int c, width = 2, dump = 0;

  while ((c = getopt(argc, argv, "dw:")) != EOF)
    switch(c)
      {
      case '?':
	printf("%s:  No useful information.\n", argv[0]);
	break;
      case 'w':
	width = atoi(optarg);
	break;
      case 'd':
	dump = 1;
      }
  for (; optind < argc; optind++)
    {
      FILE *stream;
      int count, i;

      if (!(stream = fopen(argv[optind], "r")))
	break;
      count = file_read(0, 1000, width, &mtx, stream);
      mtx.columns = width;
      mtx.rows = count; 
      fclose(stream);
#ifdef DEBUG
      fprintf(stderr, "count = %d\n", count);
#endif
      if (dump) dump_mtx(&mtx);
      log_mtx(stdout, &mtx);
    }
}
