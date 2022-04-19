#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <getopt.h>

#include "common.h"
#include "demux.h"


/* ----------------------------------------------------- */

c_feature::c_feature(int t, int l, byte *p)
{
  type = t;
  len = l;
  ptr = p;
}

c_feauture::c_feature~()
{
}


/* ----------------------------------------------------- */

c_list::c_list()
{
  head = 0;
  tail = 0;
}

c_list::c_list(void *v)
{
  head = v;
  tail = 0;
}

c_list::c_list(void *v, c_list *t)
{
  head = v;
  tail = t;
}

c_list::c_list~()
{
  delete tail;
}

/* ----------------------------------------------------- */

c_list *demux_enet(byte *ptr, int pkt_size)
{
  feature *hdr = mk_feature(enet, 12, ptr);
  feature *dest = mk_feature(enetDest, 6, ptr);
  feature *src = mk_feature(enetSrc, 6, ptr + 6);
  feature *type;
  list *llc = NULL;
  word *tmp;

  tmp = (word*)(ptr + 12);
  llc = (*tmp > 1500) ? demux_enet_type(ptr + 12, pkt_size - 12)
    : demux_llc(ptr + 14, pkt_size - 14);
  return (prepend(hdr, prepend(dest, prepend(src, llc))));
}

/* ----------------------------------------------------- */

list *demux_hdr(byte *ptr, int time_size, int cnt_size, int pkt_size)
{
  list *l;
  feature *tm = mk_feature(pktTime, time_size, ptr);
  feature *cnt = mk_feature(pktSize, cnt_size, ptr + time_size);
  l = pkt_size ? demux_enet(ptr + time_size + cnt_size, pkt_size) : NULL;
  return (prepend(tm, prepend(cnt, l)));
}

