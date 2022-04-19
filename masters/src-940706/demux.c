#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <getopt.h>

#include "common.h"
#include "demux.h"

/* ----------------------------------------------------- */

list *prepend(void *head, list *tail)
{
  list *tmp;
  tmp = malloc(sizeof(*tmp));
  tmp->head = head;
  tmp->tail = tail;
  return tmp;
}

/* ----------------------------------------------------- */

void free_list(list *l)
{
  if (!l) return;
  if (l->head) free(l->head);
  free_list(l->tail);
  free(l);
}

/* ----------------------------------------------------- */

feature *mk_feature(int type, int len, byte *ptr)
{
  register feature *tmp;
  tmp = malloc(sizeof(*tmp));
  tmp->type = type;
  tmp->len = len;
  tmp->ptr = ptr;
  return tmp;
}

/* ----------------------------------------------------- */

byte_dump(int cnt, byte *ptr, FILE *stream)
{
  register int i;
  for (i = 0; i < cnt; i++)
    fprintf(stream, "%02X", ptr[i]);
}

/* ----------------------------------------------------- */

int min(a,b)
{
  return a > b ? b : a;
}

/* ----------------------------------------------------- */

list *demux_appletalk(byte *ptr, int size)
{
  feature *ddp = mk_feature(DDP, min(size, 13), ptr);
  feature *src_net, *dest_net, *src_id, *dest_id, *src_sock, *dest_sock, *type;
  list *tmp = NULL;
  if (size >= 13)
    {
      if (ptr[12] == 0x03)
	tmp = prepend(mk_feature(ATP, min(4, size - 13), ptr + 13), tmp);
      tmp = prepend(mk_feature(DDPType, 1, ptr + 12), tmp);
    }
  if (size >= 12)
    tmp = prepend(mk_feature(DDPSrcSocket, 1, ptr + 11), tmp);
  if (size >= 11)
    tmp = prepend(mk_feature(DDPDestSocket, 1, ptr + 10), tmp);
  if (size >= 10)
    tmp = prepend(mk_feature(DDPSrcNode, 1, ptr + 9), tmp);
  if (size >= 9)
    tmp = prepend(mk_feature(DDPDestNode, 1, ptr + 8), tmp);
  if (size >= 8)
    tmp = prepend(mk_feature(DDPSrcNet, 2, ptr + 6), tmp);
  if (size >= 6)
    tmp = prepend(mk_feature(DDPDestNet, 2, ptr + 4), tmp);
  return prepend(ddp, tmp);
}

/* ----------------------------------------------------- */

list *demux_ip(byte *ptr, int size)
{
  feature *ip = mk_feature(IP, min(size, 20), ptr);
  list *tmp = NULL;

  if (size >= 20)
    tmp = prepend(mk_feature(IPDest, 4, ptr + 16), tmp);
  if (size >= 16)
    tmp = prepend(mk_feature(IPSrc, 4, ptr + 12), tmp);
  if (size >= 10)
    tmp = prepend(mk_feature(IPProtocol, 1, ptr + 9), tmp);
  return prepend(ip, tmp);
}

/* ----------------------------------------------------- */

list *demux_arp(byte *ptr, int size)
{
  feature *arp = mk_feature(ARP, min(size, 28), ptr);
  list *tmp = NULL;

  if (size >= 8)
    tmp = prepend(mk_feature(ARPop, 2, ptr + 6), tmp);
  if (size >= 7)
    tmp = prepend(mk_feature(ARPpln, 1, ptr + 5), tmp);
  if (size >= 6)
    tmp = prepend(mk_feature(ARPhln, 1, ptr + 4), tmp);
  if (size >= 4)
    tmp = prepend(mk_feature(ARPpro, 2, ptr + 2), tmp);
  if (size >= 2)
    tmp = prepend(mk_feature(ARPhrd, 2, ptr + 0), tmp);
  return prepend(arp, tmp);
}

/* ----------------------------------------------------- */

list *demux_enet_type(byte *ptr, int size)
{
  word tmp;
  feature *type = mk_feature(DIXType, 2, ptr);
  list *net = NULL;

  if (size >= 2)
    {
      tmp = *((word*)(ptr));
      if (tmp == 0x0800)
	net = demux_ip(ptr + 2, size - 2);
      else if (tmp == 0x809B)
	net = demux_appletalk(ptr + 2, size - 2);
      else if (tmp == 0x9000)
	net = prepend(mk_feature(Xerox, size - 2, ptr + 2), NULL);
      else if ((tmp == 0x0806) || (tmp == 0x80F3))
	net = demux_arp(ptr + 2, size - 2);
      else if (tmp == 0x6002)
	net = prepend(mk_feature(DECMOPRemoteConsole, size - 2, ptr + 2), NULL);
      else if (tmp == 0x6003)
	net = prepend(mk_feature(DECNETphIVRoute, size - 2, ptr + 2), NULL);
      else if (tmp == 0x6004)
	net = prepend(mk_feature(DECLAT, size - 2, ptr + 2), NULL);
      else if (tmp == 0x6007)
	net = prepend(mk_feature(DECLAVC, size - 2, ptr + 2), NULL);
      else if (tmp == 0x8038)
	net = prepend(mk_feature(DECLANBridge, size - 2, ptr + 2), NULL);
      else
	net = prepend(mk_feature(unknownNetwork, size - 2, ptr + 2), NULL);
    }
  return prepend(type, net);
}

/* ----------------------------------------------------- */

list *demux_dsap(byte *ptr, int size)
{
  return prepend(mk_feature(DSAP, size, ptr), NULL);
}

/* ----------------------------------------------------- */

list *demux_snap(byte *ptr, int size)
{
  feature *snap = mk_feature(SNAP, 5, ptr);
  feature *org = mk_feature(SNAPOrg, 3, ptr);
  return prepend(snap, prepend(org, demux_enet_type(ptr + 3, size - 3)));
}

/* ----------------------------------------------------- */

list *demux_llc(byte *ptr, int size)
{
  feature *hdr = mk_feature(LSAP, 3, ptr);
  feature *src = mk_feature(LSAPSrc, 1, ptr);
  feature *dest = mk_feature(LSAPDest, 1, ptr + 1);
  feature *cntl = mk_feature(LSAPCntl, 1, ptr + 2);
  list *snap = NULL;

  if (ptr[0] == 0xAA && ptr[1] == 0xAA && size >= 5)
    snap = demux_snap(ptr + 3, size - 3);
  else if (ptr[0] == 0xFF && ptr[1] == 0xFF)
    snap = demux_dsap(ptr + 3, size - 3);
  else
    snap = prepend(mk_feature(LSAPUnknown, 3, ptr), NULL);
  return prepend(hdr, prepend(src, prepend(dest, prepend(cntl, snap))));
}

/* ----------------------------------------------------- */

list *demux_enet(byte *ptr, int pkt_size)
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

/* ----------------------------------------------------- */

void traverse(list *l)
{
  register feature *f;
  if (!l)
    {
      printf("\n");
      return;
      }
  f = l->head;
  switch (f->type)
    {
    case LSAPUnknown:
      break;
    case unknownNetwork:
      printf("net-unkown ");
      byte_dump(f->len, f->ptr, stdout);
      printf(" ");
      break;
    case DSAP:
      printf("DSAP ");
    case pktTime:
    case pktSize:
    case enet:
    case enetDest:
    case enetSrc:
    case LSAP:
    case LSAPSrc:
    case LSAPDest:
    case LSAPCntl:
    case SNAP:
    case SNAPOrg:
    case DDPSrcSocket:
    case DDPDestSocket:
    case DDPSrcNode:
    case DDPDestNode:
    case DDPSrcNet:
    case DDPDestNet:
    case DDPType:
    case ATP:
    case ARPhrd:
    case ARPpro:
    case ARPhln:
    case ARPpln:
    case ARPop:
    case IPSrc:
    case IPDest:
    case IPProtocol:
/*
      byte_dump(f->len, f->ptr, stdout);
      printf(" ");
*/
      break;
    case IP:
      printf("IP ");
      break;
    case DDP:
      printf("DDP ");
      break;
    case Xerox:
      printf("Xerox");
      break;
    case DIXType:
      byte_dump(f->len, f->ptr, stdout);
      printf(" ");
      break;
    case DECLANBridge:
      printf("DEC LAN Bridge ");
      break;
    case DECLAT:
      printf("DEC LAT ");
      break;
    default:
      printf("%d ** ",f->type);
      byte_dump(f->len, f->ptr, stdout);
      printf(" ");
    }
  traverse(l->tail);
}

/* ----------------------------------------------------- */

list* demux_line(byte *ptr, int time_size, int cnt_size, int pkt_size)
{
  list *l;
  l = demux_hdr(ptr, time_size, cnt_size, pkt_size);
  return l;
}

/* ----------------------------------------------------- */

void demux_block(byte *bf, long bf_size, int time_size,
		 int cnt_size, int pkt_size)
{
  int line_len, cnt, i;

  line_len = time_size + cnt_size + pkt_size;
  cnt = bf_size / line_len;

  for (i = 0; i < cnt; i++)
    {
      printf("%04d : ", i);
      demux_line(bf + i * line_len, time_size, cnt_size, pkt_size);
    }
}

/* ----------------------------------------------------- */

cond* ProcessCondition(cond *l, char *str)
{
  int type_len = 0, op_len = 0, value_len;
  char c, *type, *op, *value;

  while (1)
    {
      c = str[type_len];
      if ((c >= 'A' && c <= 'Z') ||
	  (c >= 'a' && c <= 'z') ||
	  (c >= '0' && c <= '9'))
	type_len++;
      else
	break;
    }
  while(1)
    {
      c = str[type_len + op_len];
      if (!((c >= 'A' && c <= 'Z') ||
	    (c >= 'a' && c <= 'z') ||
	    (c >= '0' && c <= '9')))
	op_len++;
      else
	break;
    }
  value_len = strlen(str) - type_len - op_len;
  type = alloca(type_len + 1);
  op = alloca(op_len + 1);
  value = alloca(value_len + 1);

  strncpy(type, str, type_len);
  type[type_len] = '\0';

  strncpy(op, str + type_len, op_len);
  op[op_len] = '\0';

  strncpy(value, str + type_len + op_len, value_len);
  value[value_len] = '\0';
  
  return AddCondition(l, type, op, value);
}

/* ----------------------------------------------------- */

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  FILE *stream, *output;
  short time_size, cnt_size, pkt_size, blank;
  int store = 0, verbose = 0, i, opt, line_size;
  cond *conds = NULL, *cond_tmp;
  char *rules_f_name = NULL, *out_f_name;
  byte *bf;
  list *l;
  rule *rules, *r;
  file_hdr *hdr;
#ifdef DEBUG
	  int count = 0;
#endif

  while((opt = getopt(argc, argv, "vf:d:o")) != EOF)
      switch(opt)
	{
	case 'd':
	  conds = ProcessCondition(conds, optarg);
	  break;
	case 'f':
	  rules_f_name = optarg;
	  break;
	case 'o':
	  store = 1;
	  break;
	case 'v':
	  verbose = 1;
	  break;
	case '?':
	  fprintf(stderr, "%s:  [-f rule file] [-ov] [-d extra rule]\n", argv[0]);
	  exit(1);
	  break;
	}

  rules = parse_rules(rules_f_name);

  for (i = optind; i < argc; i++)
    {
      char f_tmp[256], f_name[256];
      int length;

      length = strlen(argv[i]);
      if ((length > 4) && !strcmp(argv[optind] + length - 4, ".tra"))
	length -= 4;
      strncpy(f_name, argv[i], length);
      f_name[length] = '\0';
      strcpy(f_tmp, f_name);
      strcat(f_tmp, ".tra");

      stream = fopen(f_tmp, "rb");
      if (!stream) break;

      fread(&time_size, sizeof(short), 1, stream);
      fread(&cnt_size, sizeof(short), 1, stream);
      fread(&pkt_size, sizeof(short), 1, stream);
      fread(&blank, sizeof(short), 1, stream);
     
      if (store)
	{
	  r = rules;
	  while (r)
	    {
	      strcpy(f_tmp, f_name);
	      strcat(f_tmp, "-");
	      strcat(f_tmp, r->ext);
	      strcat(f_tmp, ".tra");
	      if (!(r->stream = fopen(f_tmp, "wb")))
		{
		  fprintf(stderr, "Cannot open %s for writing.\n", f_tmp);
		  break;
		}
	      fwrite(&time_size, sizeof(short), 1, r->stream);
	      fwrite(&cnt_size, sizeof(short), 1, r->stream);
	      fwrite(&pkt_size, sizeof(short), 1, r->stream);
	      fwrite(&blank, sizeof(short), 1, r->stream);
	      r = r->next;
	    }
	}
      line_size = time_size + cnt_size + pkt_size;
      bf = malloc(line_size);

      while (!feof(stream))
	{
	  rule *r = rules;
	  fread(bf, sizeof(byte), line_size, stream);
	  l = demux_line(bf, time_size, cnt_size, pkt_size);
	  while (r)
	    {
	      if (check_list(l, r->conds))
		{
		  if (store) fwrite(bf, sizeof(byte), line_size, r->stream);
		  if (verbose) traverse(l);
		}
	      r = r->next;
	    }
	  free_list(l);
#ifdef DEBUG
	  if ((++count % 1000) == 0)
	    fprintf(stderr, "%d\n", count);
#endif
	}
      fclose(stream);
      if (store)
	{
	  rule *r = rules;
#ifdef DEBUG
      fprintf(stderr, "Closing output files.\n");
#endif
	  while (r)
	    {
	      fclose(r->stream);
	      r = r->next;
	    }
	}
    }
}
