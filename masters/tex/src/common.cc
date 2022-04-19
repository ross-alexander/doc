#include <alloca.h>
#include <stdio.h>
#include <string.h>
#ifdef sun
#include <sys/types.h>
#endif
#include <netinet/in.h>
#include <unistd.h>

#include "common.h"

void get_hdr(FILE *stream, file_hdr *hdr)
{
  fread(&hdr->time_len, 1, sizeof(short), stream);
  fread(&hdr->size_len, 1, sizeof(short), stream);
  fread(&hdr->pkt_len, 1, sizeof(short), stream);
  fread(&hdr->blank, 1, sizeof(short), stream);

  hdr->time_len = ntohs(hdr->time_len);
  hdr->size_len = ntohs(hdr->size_len);
  hdr->pkt_len = ntohs(hdr->pkt_len);
}

void put_hdr(FILE *stream, file_hdr *hdr)
{
  short time, count, data, blank;
  time = htons(hdr->time_len);
  count = htons(hdr->size_len);
  data = htons(hdr->pkt_len);
  blank = htons(hdr->blank);

  fwrite(&time, 1, sizeof(short), stream);
  fwrite(&count, 1, sizeof(short), stream);
  fwrite(&data, 1, sizeof(short), stream);
  fwrite(&blank, 1, sizeof(short), stream);
}

/* ----------------------------------------------------------------------- */

Ext* CheckExtension(char *fname, char *rtn, Ext *exts[])
{
  char tmp[10];
  int i = 0, length = strlen(fname), extlen;
  Ext *dflt = exts[0];

  while (Ext *ext = exts[i++])
    {
      strcpy(tmp, ".");
      strcat(tmp, ext->ext);
      if (ext->flags & EXT_DEFAULT) dflt = ext;
      if (length > (extlen = strlen(tmp)) &&
          (!strcmp(fname + length - extlen, tmp)))
        {
          strncpy(rtn, fname, length - extlen);
          rtn[length - extlen] = '\0';
          return ext;
        }
    }
  strcpy(rtn, fname);
  return dflt;
}

/* ----------------------------------------------------------------------- */

#define READTEXT_MAX 1000

int read_text_file(int count, int max, int width, s_data *mtx, FILE *stream)
{
  int *ptr, *block, tmp;
  int i = 0;

#ifdef DEBUG
  fprintf(stderr, ".");
#endif
  block = (int*)alloca(sizeof(int) * width * max);
  while ((i < max) && !feof(stream))
    {
      ptr = block + i * width;
      for (int j = 0; j < width; j++)
        {
          fscanf(stream, "%d", &tmp);
          ptr[j] = tmp;
        }
      i++;
    }
  if (feof(stream))
    {
      i--;
      mtx->ptr = (int*) new int [width * (i + count)];
      memcpy(((int*)mtx->ptr) + count * width, block, i * width * sizeof(int));
      return count + i;
    }
  else
    {
      int j = read_text_file(count + max, max, width, mtx, stream);
      memcpy(((int*)mtx->ptr) + count * width, block, max * width * sizeof(int));
      return j;
    }
}

int read_text_file_float(int count, int max, int width, s_data *mtx, FILE *stream)
{
  double *ptr, *block, tmp;
  int i = 0;
  block = (double*)alloca(sizeof(double) * width * max);
  while ((i < max) && !feof(stream))
    {
      ptr = block + i * width;
      for (int j = 0; j < width; j++)
        {
          fscanf(stream, "%lf", &tmp);
          ptr[j] = tmp;
        }
      i++;
    }
  if (feof(stream))
    {
      i--;
      mtx->ptr = (double*) new double [width * (i + count)];
      memcpy(((double*)mtx->ptr) + count * width, block, i * width * sizeof(double));
      return count + i;
    }
  else
    {
      int j = read_text_file_float(count + max, max, width, mtx, stream);
      memcpy(((double*)mtx->ptr) + count * width, block, max * width * sizeof(double));
      return j;
    }
}

int ReadFileText(int width, s_data *mtx, FILE *stream)
{
  return read_text_file(0, READTEXT_MAX, width, mtx, stream);
}
int ReadFileTextFloat(int width, s_data *mtx, FILE *stream)
{
  return read_text_file_float(0, READTEXT_MAX, width, mtx, stream);
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
#ifdef DEBUG
  fprintf(stderr, "length = %ld\n", len);
#endif
  ptr = new unsigned char[len];
  fread(ptr, sizeof(unsigned char), len, stream);
  tmp = (int*)ptr;
  mtx->rows = tmp[len / sizeof(int) - 2];
  mtx->columns = tmp[len / sizeof(int) - 1];
  mtx->ptr = (int*)ptr;
#ifdef DEBUG
  fprintf(stderr, "read_binary_file %ld %ld %0lX\n", mtx->rows, mtx->columns,
          mtx->ptr);
#endif
}

void ReadFileBinary(s_data *mtx, FILE *stream)
{
  read_binary_file(mtx, stream);
}

/* ----------------------------------------------------------------------- */

void dump_mtx(s_data *mtx)
{
  int i,j;
  for (i = 0; i < mtx->rows; i++)
    {
      for (j = 0; j < mtx->columns; j++)
        fprintf(stdout, "%8d  ", ((int*)mtx->ptr)[i * mtx->columns + j]);
      fprintf(stdout, "\n");
    }
}

void DumpMtx(s_data *mtx)
{
  dump_mtx(mtx);
}

void DumpMtxFloat(s_data *mtx)
{
  int i,j;
  for (i = 0; i < mtx->rows; i++)
    {
      for (j = 0; j < mtx->columns; j++)
        fprintf(stdout, "%0lf ", ((double*)mtx->ptr)[i * mtx->columns + j]);
      fprintf(stdout, "\n");
    }
}
