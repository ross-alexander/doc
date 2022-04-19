#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

#define HWT_TICK 0.2145527

typedef struct {
  short time, count, data, blank;
  int total;
} file_hdr;

void get_hdr(FILE *stream, file_hdr *hdr)
{
  fread(&hdr->time, 1, sizeof(short), stream);
  fread(&hdr->count, 1, sizeof(short), stream);
  fread(&hdr->data, 1, sizeof(short), stream);
  fread(&hdr->blank, 1, sizeof(short), stream);

  hdr->total = 0;
  hdr->total += hdr->time = ntohs(hdr->time);
  hdr->total += hdr->count = ntohs(hdr->count);
  hdr->total += hdr->data = ntohs(hdr->data);
}

void put_hdr(FILE *stream, file_hdr *hdr)
{
  short time, count, data, blank;
  time = htons(hdr->time);
  count = htons(hdr->count);
  data = htons(hdr->data);
  blank = htons(hdr->blank);

  fwrite(&time, 1, sizeof(short), stream);
  fwrite(&count, 1, sizeof(short), stream);
  fwrite(&data, 1, sizeof(short), stream);
  fwrite(&blank, 1, sizeof(short), stream);
}

void fix(FILE *stream, FILE *output, file_hdr *hdr, int test)
{
  unsigned char *bf;
  unsigned long send_time = 0, send_old, recv_time = 0, recv_old;
  int cnt = 0;

  bf = malloc(hdr->total);
  while (fread(bf, 1, hdr->total, stream))
    {
      int i;
      send_old = send_time;
      recv_old = recv_time;
      recv_time = ntohl(*(long*)(bf));
      send_time = ntohl(*(long*)(bf + 24));
      
      if (send_old > send_time)
	send_time += 256;
      if (recv_old > recv_time)
	recv_time += 256;

      (*(long*)(bf)) = recv_time;

      recv_time = recv_time * HWT_TICK;
      send_time = send_time * HWT_TICK;

      if (test) (*(long*)(bf + 24)) = send_time;

      fwrite(bf, 1, hdr->total, output);
    }
  free(bf);
}

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  int ch, speed_test = 0;
  file_hdr hdr;
  char *output_name = "correction.out";

  while ((ch = getopt(argc, argv, "so:")) != EOF)
    switch(ch)
      {
      case 'o':
	output_name = optarg;
	break;
      case 's':
	speed_test = 1;
	break;
      }
  for (;optind < argc; optind++)
    {
      FILE *input, *output;
      char tmp[256], fname[256];
      int length;

      length = strlen(argv[optind]);
      if ((length > 4) && !strcmp(argv[optind] + length - 4, ".icr"))
	length -= 4;
      strncpy(fname, argv[optind], length);
      fname[length] = '\0';

      strcpy(tmp, fname);
      strcat(tmp, ".icr");
      if (!(input = fopen(tmp, "rb")))
	break;

      strcpy(tmp, fname);
      strcat(tmp, ".tra");
      if (!(output = fopen(tmp, "wb")))
	break;

      get_hdr(input, &hdr);
      put_hdr(output, &hdr);
      fix(input, output, &hdr, speed_test);
      fclose(input);
      fclose(output);
    }
}
