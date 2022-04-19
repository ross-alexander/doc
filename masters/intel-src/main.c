/* **********************************************************************
*
*  main
*
*  version 0.0.1
*    27 March 1993
*    Add hwtimer code for accurate timings based on the Interval counter.
*
*
********************************************************************** */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <bios.h>
#include <dos.h>
#include "common.h"

extern void hwt_init(void);  /* In hwtimer.asm */
extern void hwt_fin(void);
extern unsigned long hw_time(void);

static PDRec *pd;

extern int refresh, cur_bf;
extern long pkt_count;
extern long bf_cnt;
extern byte *buffer[2];

int main(int argc, char *argv[])
{
  FILE *stream;
  int cnt = 0, i, store_len;
  time_t seconds;
  char *fname = "trace.icr";
  word handle;
  word hdr[4];

  for (i = 0; i < argc; i++)
    if (argv[i][0] == '-')
      switch (argv[i][1])
        {
        case 'o':
          fname = argv[++i];
          break;
        }

  pd = PDInit();
  hwt_init();
  if (!pd->nIntf)
    {
      printf("No usable packet drivers were found.\n");
      return 1;
    }
  handle = DriverAdd(0);

  stream = fopen(fname, "ab");
  hdr[0] = htons(sizeof(clock_t));
  hdr[1] = htons(sizeof(int));
  hdr[2] = htons(MAX_KEEP_PKT);
  hdr[3] = htons(0);
  fwrite(hdr, sizeof(word), 4, stream);

  store_len = MAX_KEEP_PKT + sizeof(clock_t) + sizeof(int);
  do
    if (refresh)
      {
        fwrite(buffer[refresh-1], store_len, MAX_KEEP_CNT, stream);
        refresh = 0;
        cnt++;
      }
  while (!kbhit() && (cnt < 1000));
  DriverRemove(0, handle);
  fwrite(buffer[cur_bf], store_len, bf_cnt, stream);
  fclose(stream);
  hwt_fin();
  printf("\n---------------------------\n");

/*
  for (i = 0; i < pkt_count; i++)
    {
      int j;
      for (j = 0; j < 36; j++)
        printf("%02X", buffer[i*store_len + j]);
      printf("\n");
    }
*/
  printf("Pkt count = %ld\n", pkt_count);
  return 0;
}
