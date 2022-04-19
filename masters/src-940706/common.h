#include <stdio.h>

typedef struct {
  short time_len;
  short size_len;
  short pkt_len;
  short blank;
} file_hdr;

extern void get_hdr(FILE*, file_hdr*);

