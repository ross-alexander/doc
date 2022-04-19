#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "common.h"

void dump(FILE *stream, file_hdr *hdr, int test)
{
  unsigned char *bf;
  unsigned long send_time, recv_time, diff_time;
  int total, cnt = 0;

  total = hdr->time_len + hdr->size_len + hdr->pkt_len;
  bf = malloc(total);
  while (fread(bf, 1, total, stream))
    {
      int i;
      recv_time = ntohl(*(long*)(bf));
      send_time = ntohl(*(long*)(bf + 24));
      diff_time = recv_time - send_time;

      if(test)
	printf("%d %ld %ld %ld ", ntohs(*(short*)(bf + 20)),
	       recv_time, send_time, diff_time);
      else
	printf("%d %ld ",cnt, send_time);
      printf("\n");
      cnt++;
    }
  free(bf);
}

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  int ch, test = 0;
  file_hdr hdr;

  while ((ch = getopt(argc, argv, "s")) != EOF)
    switch(ch)
      {
      case 's':
	test = 1;
	break;
      }
  for (;optind < argc; optind++)
    {
      FILE *stream;
      if (!(stream = fopen(argv[optind], "rb")))
	break;
      get_hdr(stream, &hdr);
      dump(stream, &hdr, test);
      fclose(stream);
    }
}
