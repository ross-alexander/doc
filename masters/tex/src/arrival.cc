#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#ifdef sun
#include <sys/types.h>
#endif
#include <netinet/in.h>
#include <unistd.h>

#include "common.h"

#define INTERVAL 10.0 // 10 ms
#define SLOT_MAX 2000

// ----------------------------------------------------------------------------

struct o_struct {
  int time, count, octets;
};

struct Param {
  int verbose;
  int binary;
  int cut;
  double cuttime;
  double slotsize;
  FILE *output;
};

// ----------------------------------------------------------------------------

void CutAggregate(TraceFile &hdr, char *fname, Param &p)
{
  int read = 1, eof = 0, octets = 0, count = 0, i = 1, j = 0, k = 0, tm;
  register double tmp;
  double slot_mod = p.slotsize >= 1000 ? p.slotsize / 1000 : p.slotsize;
  o_struct o;
  FILE *output;
  char filetmp[256];
  byte *bf = new byte [hdr.lineLen];

  while (!eof)
    {
      if (p.binary)
        {
          strcpy(filetmp, fname);
          sprintf(filetmp, "%s-%dms-%d.abf", fname, (int)(p.slotsize), i);
          if (!(output = fopen(filetmp, "wb")))
            return;
        }
      else
        {
          strcpy(filetmp, fname);
          sprintf(filetmp, "%s-%dms-%d.atf", fname, (int)(p.slotsize), i);
          if (!(output = fopen(filetmp, "w")))
            return;
        }
      if (p.verbose) fprintf(stderr, "Opening output file %s\n", filetmp);
      octets = count = 0;
      while (1)
        {
          if (read)
            if (eof = !hdr.ReadLine(bf))
              break;
            else
              ;
          else
            read = 1;

          tm = *((int*)bf);
          tmp = tm;
          if (tmp > i * p.cuttime)
            {
              read = 0;
              break;
            }
          while (1)
            if (tmp > (j + 1) * p.slotsize)
              {
        	if (p.binary)
        	  {
        	    o.time = (int)(j * slot_mod);
        	    o.count = count;
        	    o.octets = octets;
        	    fwrite(&o, sizeof(o_struct), 1, output);
        	  }
        	else
        	  fprintf(output, "%9d %5d %7d\n", (int)(j * slot_mod), count, octets);
        	j++;
        	k++;
        	octets = count = 0;
              }
            else
              {
        	octets += ntohs(*(short*)(bf + hdr.time_len));
        	count++;
        	break;
              }
        }
      octets = count = 0;
      while (i * p.cuttime > (j + 1) * p.slotsize)
        {
          if (p.binary)
            {
              o.time = (int)(j * slot_mod);
              o.count = count;
              o.octets = octets;
              fwrite(&o, sizeof(o_struct), 1, output);
            }
          else
            fprintf(output, "%9d %5d %7d\n", (int)(j * slot_mod), count, octets);
          j++;
          k++;
        }
      if (p.binary)
        {
          int a = 3;
          o.time = (int)(j++ * slot_mod);
          o.count = count;
          o.octets = octets;
          fwrite(&o, sizeof(o_struct), 1, output);
          fwrite(&++k, sizeof(int), 1, output);
          fwrite(&a, sizeof(int), 1, output);
        }
      else
        {
          fprintf(output, "%9d %5d %7d\n", (int)(j++ * slot_mod), count, octets);
        }
      if (p.verbose) fprintf(stderr, "Line count = %d\n", ++k);
      fclose(output);
      i++;
      k = 0;
    }
}

// ----------------------------------------------------------------------------
void Aggregate(TraceFile &hdr, char *fname, Param &p)
{
  int octets = 0, count = 0, j = 0, tm;
  register double tmp;
  double slot_mod = p.slotsize >= 1000 ? p.slotsize / 1000 : p.slotsize;
  o_struct o;
  byte *bf;
  FILE *output;
  char filetmp[256];

  if (p.binary)
    {
      strcpy(filetmp, fname);
      sprintf(filetmp, "%s-%dms.abf", fname, (int)(p.slotsize));
      if (!(output = fopen(filetmp, "wb")))
        return;
    }
  else
    {
      strcpy(filetmp, fname);
      sprintf(filetmp, "%s-%dms.atf", fname, (int)(p.slotsize));
      if (!(output = fopen(filetmp, "w")))
        return;
    }
  p.output = output;
  if (p.verbose) fprintf(stderr, "Opening output file %s\n", filetmp);

  bf = new byte [hdr.lineLen];

  while (hdr.ReadLine(bf))
    {
      tm = *((int*)bf);
      tmp = tm;
      while (1)
        if (tmp > (j + 1) * p.slotsize)
          {
            if (p.binary)
              {
        	o.time = (int)(j * slot_mod);
        	o.count = count;
        	o.octets = octets;
        	fwrite(&o, sizeof(o_struct), 1, p.output);
              }
            else
              fprintf(p.output, "%9d %5d %7d\n", (int)(j * slot_mod), count, octets);
            j++;
            octets = count = 0;
          }
        else
          {
            octets += ntohs(*(short*)(bf + hdr.time_len));
            count++;
            break;
          }
    }
  if (p.binary)
    {
      int a = 3;
      o.time = (int)(j++ * slot_mod);
      o.count = count;
      o.octets = octets;
      fwrite(&o, sizeof(o_struct), 1, p.output);
      fwrite(&j, sizeof(int), 1, p.output);
      fwrite(&a, sizeof(int), 1, p.output);
    }
  else
    {
      fprintf(p.output, "%9d %5d %7d\n", (int)(j++ * slot_mod), count, octets);
    }
  if (p.verbose) fprintf(stderr, "Line count = %d\n", j);
  fclose(output);
}

// ----------------------------------------------------------------------------

extern char *optarg;
extern int optind;


int main(int argc, char *argv[])
{
  int c;
  Param p = {0, 0, 0, 0.0, INTERVAL, NULL};

  while ((c = getopt(argc, argv, "vbt:s:")) != -1)
    switch(c)
      {
      case 't':
        p.slotsize = atof(optarg);
        break;
      case 'v':
        p.verbose = 1;
        break;
      case 'b':
        p.binary = 1;
        break;
      case 's':
        p.cut = 1;
        p.cuttime = atof(optarg) * 1000.0;
        break;
      }

  for (; optind < argc; optind++)
    {
      FILE *input;
      char tmp[256], fname[256];
      Ext exttra = {"tra", 1, 1};
      Ext *rtn, *exts[] = {&exttra, NULL};

      rtn = CheckExtension(argv[optind], fname, exts);
      switch(rtn->id)
        {
        case 1:
          strcpy(tmp, fname);
          strcat(tmp, ".");
          strcat(tmp, rtn->ext);
          input = fopen(tmp, "rb");
          break;
        default:
          input = NULL;
          break;
        }
      if (!input) break;
      if (p.verbose) fprintf(stderr, "Opening input file %s\n", tmp);
      TraceFile trace(input);
      trace.ReadHdr();
      if (p.cut)
        CutAggregate(trace, fname, p);
      else
        Aggregate(trace, fname, p);
      fclose(input);
    }
}
