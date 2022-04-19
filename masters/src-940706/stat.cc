#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#ifndef sun
  #include <getopt.h>
#endif

typedef struct {
  int rows, columns;
  int *data;
} s_data;

class vector {
public:
  vector(int);
  ~vector();
  int length();
  double &operator[](int);
  vector &operator()();
  vector &operator()(int);
  vector &operator()(int, int);
  vector &operator=(vector&);
  void map(double (*)(double));
  double fold(double (*)(double), double (*)(double, double), double);
  vector aggregate(int);
private:
  int size;
  double *ptr;
};

vector::vector(int s)
{
  size = s;
  ptr = new double[s];
}

vector::~vector()
{
  delete ptr;
}

vector &vector::operator()()
{
  vector &tmp = vector(size);
  for (register int i = 0; i < size; i++)
    tmp[i] = ptr[i];
  return tmp;
}

vector &vector::operator()(int start)
{
  vector &tmp = vector(size - start);
  for (register int i = start; i < size; i++)
    tmp[i - start] = ptr[i];
  return tmp;
}

vector &vector::operator()(int start, int length)
{
  vector &tmp = vector(length);
  for (register int i = 0; i < length; i++)
    tmp[i] = ptr[i + start];
  return tmp;
}

inline vector &vector::operator=(vector &v)
{
  int l;
  l = v.length();
  vector &tmp = vector(l);
  for (int i = 0; i < l; i++)
    tmp[i] = v[i];
  return tmp;
}

inline double &vector::operator[](int i)
{
  static double zero = 0.0;
  return i < size ? ptr[i] : zero;
}

inline int vector::length()
{
  return size;
}

inline void vector::map(double (*f)(double))
{
  int i;
  for (i = 0;i < size; i++)
    ptr[i] = (*f)(ptr[i]);
}

inline double vector::fold(double (*f)(double),
		    double (*g)(double, double), double initial)
{
  for (int i = 0; i < size; i++)
    initial = (*g)((*f)(ptr[i]), initial);
  return initial;
}

inline vector vector::aggregate(int i)
{
  double sum;
  vector tmp = vector(size / i);
  for (int j = 0; j < size / i; j++)
    {
      sum = 0;
      for (int k = 0; k < i; k++)
	sum += ptr[j * i + k];
      tmp[j] = sum / i;
    }
  return tmp;
}

/* ----------------------------------------------------------------------- */

inline double f_identity(double x)
{
  return x;
}

inline double f_square(double x)
{
  return x*x;
}

inline double f_add(double x, double y)
{
  return x + y;
}

/* ----------------------------------------------------------------------- */

int read_text_file(int count, int max, int width, s_data *mtx, FILE *stream)
{
  int *ptr, *block, tmp;
  int i = 0, j;

#ifdef DEBUG
  fprintf(stderr, ".");
#endif

  block = (int*)alloca(sizeof(int) * width * max);
  while ((i < max) && !feof(stream))
    {
      ptr = block + i * width;
      for (j = 0; j < width; j++)
	{
	  fscanf(stream, "%d", &tmp);
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
      mtx->data = (int*)malloc(sizeof(int) * width * (i + count));
      memcpy(mtx->data + count * width, block, i * width * sizeof(int));
      return count + i;
    }
  else
    {
      j = read_text_file(count + max, max, width, mtx, stream);
      memcpy(mtx->data + count * width, block, max * width * sizeof(int));
      return j;
    }
}


/* ----------------------------------------------------------------------- */

void read_binary_file(s_data *mtx, FILE *stream)
{
  long len;
  unsigned char *ptr;
  int *tmp;
  fseek(stream, 0, SEEK_END);
  len = ftell(stream);
  fseek(stream, 0, SEEK_SET);
  ptr = (unsigned char*)malloc(len);
  fread(ptr, sizeof(unsigned char), len, stream);
  tmp = (int*)ptr;
  mtx->rows = tmp[len / sizeof(int) - 2];
  mtx->columns = tmp[len / sizeof(int) - 1];
  mtx->data = (int*)ptr;
#ifdef DEBUG
  fprintf(stderr, "read_binary_file %d %d\n", mtx->rows, mtx->columns);
#endif
}

/* ----------------------------------------------------------------------- */

void dump_mtx(s_data *mtx)
{
  int i,j;
  for (i = 0; i < mtx->rows; i++)
    {
      for (j = 0; j < mtx->columns; j++)
	fprintf(stdout, "%8.1lf  ", mtx->data[i * mtx->columns + j]);
      fprintf(stdout, "\n");
    }
}

/* ----------------------------------------------------------------------- */

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  s_data mtx;
  int c, binary = 0, width = 2, dump = 0;

  while ((c = getopt(argc, argv, "bdw:")) != EOF)
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
	break;
      case 'b':
	binary = 1;
	break;
      }
  for (; optind < argc; optind++)
    {
      FILE *stream;
      int count, i;

      if (binary)
	{
	  if (!(stream = fopen(argv[optind], "rb")))
	    break;
	  read_binary_file(&mtx, stream);
	  count = mtx.rows;
	}
      else
	{
	  if (!(stream = fopen(argv[optind], "r")))
	    break;
	  count = read_text_file(0, 1000, width, &mtx, stream);
	  mtx.columns = width;
	  mtx.rows = count;
	}
      fclose(stream);
#ifdef DEBUG
      fprintf(stderr, "count = %d\n", count);
#endif
      if (dump) dump_mtx(&mtx);

      vector freq = vector(mtx.rows);
      for (i = 0; i < mtx.rows; i++)
	freq[i] = mtx.data[i * mtx.columns + 1];

      for (i = 1; i <= freq.length() / 10; i++)
	{
	  vector tmp = freq.aggregate(i);
	  double mean = tmp.fold(f_identity, f_add, 0.0) / tmp.length();
	  double x_squared = tmp.fold(f_square, f_add, 0.0) / tmp.length();
	  double var = (x_squared - mean * mean);
	  fprintf(stdout, "%d %lf %lf %lf\n", i, mean, x_squared, var);
	  tmp.~vector();
	}
    }
}
