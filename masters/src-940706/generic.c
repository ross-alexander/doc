#include <stdio.h>
#include <netinet/in.h>
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
