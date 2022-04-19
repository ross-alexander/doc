
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

// ----------------------------------------------------------------------------

extern char *optarg;
extern int optind;


int main(int argc, char *argv[])
{
  int c;
  int verbose;
  int count = 0;

  while ((c = getopt(argc, argv, "v")) != -1)
    switch(c)
      {
      case 'v':
        verbose = 1;
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
      if (verbose) fprintf(stderr, "Opening input file %s\n", tmp);
      TraceFile trace(input);
      trace.ReadHdr();
      byte *bf = new byte [trace.lineLen];
      while (trace.ReadLine(bf)) count++;
      fclose(input);
    }
  fprintf(stdout, "Count = %d\n", count);
}
