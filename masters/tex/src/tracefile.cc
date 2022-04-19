#include <stdio.h>
#ifdef sun
#include <sys/types.h>
#endif
#include <netinet/in.h>

#include "common.h"

TraceFile::TraceFile(FILE *f) {stream = f; };

TraceFile::~TraceFile() {};

void TraceFile::ReadHdr()
{
  fread(&time_len, 1, sizeof(short), stream);
  fread(&size_len, 1, sizeof(short), stream);
  fread(&pkt_len, 1, sizeof(short), stream);
  fread(&blank, 1, sizeof(short), stream);
  
  time_len = ntohs(time_len);
  size_len = ntohs(size_len);
  pkt_len = ntohs(pkt_len);
  lineLen = time_len + size_len + pkt_len;
}

void TraceFile::WriteHdr()
{
  short time, count, data, blank;
  time = htons(time_len);
  count = htons(size_len);
  data = htons(pkt_len);
  blank = htons(blank);

  fwrite(&time, 1, sizeof(short), stream);
  fwrite(&count, 1, sizeof(short), stream);
  fwrite(&data, 1, sizeof(short), stream);
  fwrite(&blank, 1, sizeof(short), stream);
}

int TraceFile::ReadLine(unsigned char *ptr)
{
  int result = fread(ptr, sizeof(byte), lineLen, stream);
  return result;
}
