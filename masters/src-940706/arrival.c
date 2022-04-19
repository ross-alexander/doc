#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include "common.h"

#define INTERVAL 10.0 /* 1 ms */
#define SLOT_MAX 2000

#define AGGREGATE 1
#define ARRIVAL 2
#define DUMP 3

typedef unsigned char byte;

/*
void inter_arrival(char *fname, FILE *stream, double slot_size)
{
  short pkt_size, time_size, len_size, blank;
  int line_size, i, old_tm, tm = 0;
  register double tmp;
  register int sl;
  byte bf[1600];
  int slot[SLOT_MAX + 1];

  line_size = time_size + len_size + pkt_size;

  for (i = 0; i < SLOT_MAX; i++)
    slot[i] = 0;
  while (fread(bf,sizeof(byte), line_size, stream))
    {
      old_tm = tm;
      tm = 0;
      for (i = 0; i < time_size; i++)
	tm = (tm << 8) + bf[i];
      tmp = (tm -old_tm) * HWT_TICK;
      sl = tmp / slot_size;
      if (sl < SLOT_MAX)
	slot[sl]++;
      else
	slot[SLOT_MAX]++;

	  printf("%8.4lf %5d\n", tmp, sl);
    }
  for (i = 0; i <= SLOT_MAX; i++)
    printf("%7.2lf %5d\n", (i + 1) * slot_size, slot[i]);
}
*/

/* ------------------------------------------------------------------------ */

void aggregate(file_hdr *hdr, FILE *input, FILE *output, double slot_size)
{
  int line_len, octets = 0, count = 0, i, j = 0, tm;
  register double tmp;
  double slot_mod = slot_size >= 1000 ? slot_size / 1000 : slot_size;
  byte bf[1600];

  line_len = hdr->time_len + hdr->size_len + hdr->pkt_len;

  while (fread(bf, sizeof(byte), line_len, input))
    {
      tm = 0;
      for (i = 0; i < hdr->time_len; i++)
	tm = (tm << 8) + bf[i];
      tmp = tm;
      while (1)
	if (tmp > j * slot_size)
	  {
	    fprintf(output, "%9.2lf %5d %7d\n", j * slot_mod, count, octets);
	    j++;
	    octets = count = 0;
	  }
	else
	  {
	    octets += ntohs(*(short*)(bf + hdr->time_len));
	    count++;
	    break;
	  }
    }
}

/* ------------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;

  file_hdr hdr;
  int show_file_name  = 0, c, length, action = AGGREGATE;
  double interval = INTERVAL;

  while ((c = getopt(argc, argv, "fapt:")) != -1)
    switch(c)
      {
      case 'f':
	show_file_name = 1;
	break;
      case 't':
	interval = atof(optarg);
	break;
      case 'a':
	action = AGGREGATE;
	break;
      case 'p':
	action = ARRIVAL;
	break;
      }

  for (; optind < argc; optind++)
    {
      FILE *input, *output;
      char tmp[256], fname[256];

      length = strlen(argv[optind]);
      if ((length > 4) && !strcmp(argv[optind] + length - 4, ".tra"))
	length -= 4;
      strncpy(fname, argv[optind], length);
      fname[length] = '\0';
      strcpy(tmp, fname);
      strcat(tmp, ".tra");
      if (!(input = fopen(tmp, "rb")))
	break;
      sprintf(tmp, "%s.%dms", fname, (int)interval);
      if (!(output = fopen(tmp, "w")))
	break;
     
      get_hdr(input, &hdr);
      switch (action)
	{
	case AGGREGATE:
	  if (show_file_name) fprintf(output, "%s::\n", fname);
	  aggregate(&hdr, input, output, interval);
	  break;
	}
      fclose(input);
      fclose(output);
    }
}
