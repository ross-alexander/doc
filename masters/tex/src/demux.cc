#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <getopt.h>
#include <netinet/in.h>

#include "common.h"
#include "demux.h"

// ----------------------------------------------------------------------------

int min(int a, int b)
{
  return a > b ? b : a;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void byte_dump(int cnt, byte *ptr, FILE *stream)
{
  register int i;
  for (i = 0; i < cnt; i++)
    fprintf(stream, "%02X", ptr[i]);
}

// ----------------------------------------------------------------------------

Feature::Feature(int t, int l, byte *p)
{
  type = t;
  len = l;
  ptr = p;
}

Feature::~Feature()
{
}

void Feature::Dump()
{
  switch (type)
    {
    case LSAPUnknown:
      break;
    case unknownNetwork:
      printf("net-unkown ");
//      byte_dump(f->len, f->ptr, stdout);
      printf(" ");
      break;
    case DSAP:
      printf("DSAP ");
    case pktTime:
    case pktSize:
    case enetDest:
    case enetSrc:
    case LSAP:
    case LSAPSrc:
    case LSAPDest:
    case LSAPCntl:
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
      byte_dump(len, ptr, stdout);
      printf(" ");
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
      fprintf(stdout, "%04X", *((word*)ptr));
      printf(" ");
      break;
    case DECLANBridge:
      printf("DEC LAN Bridge ");
      break;
    case DECLAT:
      printf("DEC LAT ");
      break;
    default:
      printf("%d ** ", type);
      printf(" ");

    case enet:
    case SNAP:
      break;

    }
}

// ----------------------------------------------------------------------------

FList::FList()
{
  head = 0;
  tail = 0;
}

FList::FList(Feature *f)
{
  head = f;
  tail = 0;
}

FList::FList(Feature *f, FList *t)
{
  head = f;
  tail = t;
}

FList::~FList()
{
  delete tail;
}

void FList::operator +=(Feature *f)
{
  FList *tmp = new FList();
  *tmp = *this;
  tail = tmp;
  head = f;
}

FList *FList::operator+(Feature *f)
{
  FList *tmp = new FList(f, this);
  return tmp;
}

void FList::Dump()
{
  if (head) head->Dump();
  if (tail) tail->Dump(); else fprintf(stdout, "\n");
}

// ----------------------------------------------------------------------------

FList *DemuxAppletalk(byte *ptr, int size)
{
  Feature *ddp = new Feature(DDP, min(size, 13), ptr);
/*
  feature *src_net, *dest_net, *src_id, *dest_id, *src_sock, *dest_sock, *type;
  list *tmp = NULL;
  if (size >= 13)
    {
      if (ptr[12] == 0x03)
        tmp = prepend(new Feature(ATP, min(4, size - 13), ptr + 13), tmp);
      tmp = prepend(new Feature(DDPType, 1, ptr + 12), tmp);
    }
  if (size >= 12)
    tmp = prepend(new Feature(DDPSrcSocket, 1, ptr + 11), tmp);
  if (size >= 11)
    tmp = prepend(new Feature(DDPDestSocket, 1, ptr + 10), tmp);
  if (size >= 10)
    tmp = prepend(new Feature(DDPSrcNode, 1, ptr + 9), tmp);
  if (size >= 9)
    tmp = prepend(new Feature(DDPDestNode, 1, ptr + 8), tmp);
  if (size >= 8)
    tmp = prepend(new Feature(DDPSrcNet, 2, ptr + 6), tmp);
  if (size >= 6)
    tmp = prepend(new Feature(DDPDestNet, 2, ptr + 4), tmp);
  return prepend(ddp, tmp);
*/
  return new FList(ddp);
}

// ----------------------------------------------------------------------------

FList *DemuxIP(byte *ptr, int size)
{
  Feature *ip = new Feature(IP, min(size, 20), ptr);
  FList *tmp = NULL;
/*

  if (size >= 20)
    tmp = prepend(new Feature(IPDest, 4, ptr + 16), tmp);
  if (size >= 16)
    tmp = prepend(new Feature(IPSrc, 4, ptr + 12), tmp);
  if (size >= 10)
    tmp = prepend(new Feature(IPProtocol, 1, ptr + 9), tmp);
  return prepend(ip, tmp);
*/
  return new FList(ip);
}

// ----------------------------------------------------------------------------

FList *DemuxARP(byte *ptr, int size)
{
  Feature *arp = new Feature(ARP, min(size, 28), ptr);
/*
  FList *tmp = NULL;

  if (size >= 8)
    tmp = prepend(new Feature(ARPop, 2, ptr + 6), tmp);
  if (size >= 7)
    tmp = prepend(new Feature(ARPpln, 1, ptr + 5), tmp);
  if (size >= 6)
    tmp = prepend(new Feature(ARPhln, 1, ptr + 4), tmp);
  if (size >= 4)
    tmp = prepend(new Feature(ARPpro, 2, ptr + 2), tmp);
  if (size >= 2)
    tmp = prepend(new Feature(ARPhrd, 2, ptr + 0), tmp);
  return prepend(arp, tmp);
*/
  return new FList(arp);
}

// ----------------------------------------------------------------------------

FList *DemuxENETtype(byte *ptr, int size)
{
  word tmp;
  Feature *type = new Feature(DIXType, 2, ptr);
  FList *net = NULL;

  if (size >= 2)
    {
      tmp = *((word*)(ptr));
      if (tmp == 0x0800)
        net = DemuxIP(ptr + 2, size - 2);
      else if (tmp == 0x809B)
        net = DemuxAppletalk(ptr + 2, size - 2);
      else if (tmp == 0x9000)
        net = new FList(new Feature(Xerox, size - 2, ptr + 2));
      else if ((tmp == 0x0806) || (tmp == 0x80F3))
        net = DemuxARP(ptr + 2, size - 2);
      else if (tmp == 0x6002)
        net = new FList(new Feature(DECMOPRemoteConsole, size - 2, ptr + 2));
      else if (tmp == 0x6003)
        net = new FList(new Feature(DECNETphIVRoute, size - 2, ptr + 2));
      else if (tmp == 0x6004)
        net = new FList(new Feature(DECLAT, size - 2, ptr + 2));
      else if (tmp == 0x6007)
        net = new FList(new Feature(DECLAVC, size - 2, ptr + 2));
      else if (tmp == 0x8038)
        net = new FList(new Feature(DECLANBridge, size - 2, ptr + 2));
      else
        net = new FList(new Feature(unknownNetwork, size - 2, ptr + 2));
    }
  return *net + type;
}

// ----------------------------------------------------------------------------

FList *DemuxDSAP(byte *ptr, int size)
{
  Feature *dsap = new Feature(DSAP, size, ptr);
  return new FList(dsap);
}

// ----------------------------------------------------------------------------

FList *DemuxSNAP(byte *ptr, int size)
{
  Feature *snap = new Feature(SNAP, 5, ptr);
  Feature *org = new Feature(SNAPOrg, 3, ptr);
  return *(*DemuxENETtype(ptr + 3, size - 3) + org) + snap;
}

// ----------------------------------------------------------------------------

FList *DemuxLLC(byte *ptr, int size)
{
  Feature *hdr = new Feature(LSAP, 3, ptr);
  Feature *src = new Feature(LSAPSrc, 1, ptr);
  Feature *dest = new Feature(LSAPDest, 1, ptr + 1);
  Feature *cntl = new Feature(LSAPCntl, 1, ptr + 2);
  FList *snap = NULL;

  if (ptr[0] == 0xAA && ptr[1] == 0xAA && size >= 5)
    snap = DemuxSNAP(ptr + 3, size - 3);
  else if (ptr[0] == 0xFF && ptr[1] == 0xFF)
    snap = DemuxDSAP(ptr + 3, size - 3);
  else
    snap = *(new FList) + (new Feature(LSAPUnknown, 3, ptr));
  return *(*(*snap + dest) + src) + hdr;
}

// ----------------------------------------------------------------------------

FList *DemuxENET(byte *ptr, int pkt_size)
{
  Feature *hdr  = new Feature(enet, 12, ptr);
  Feature *dest = new Feature(enetDest, 6, ptr);
  Feature *src  = new Feature(enetSrc, 6, ptr + 6);

  FList *llc = NULL;
  word *tmp;

  tmp = (word*)(ptr + 12);
  llc = (*tmp > 1500) ? DemuxENETtype(ptr + 12, pkt_size - 12)
    : DemuxLLC(ptr + 14, pkt_size - 14);
  return *(*(*llc + hdr) + dest) + src;
}

// ----------------------------------------------------------------------------

FList *DemuxHdr(byte *ptr, int time_size, int cnt_size, int pkt_size)
{
  Feature *tm = new Feature(pktTime, time_size, ptr);
  Feature *cnt = new Feature(pktSize, cnt_size, ptr + time_size);
  FList *list = pkt_size ? DemuxENET(ptr + time_size + cnt_size, pkt_size) : NULL;
  return *(*list + cnt) + tm;
}

// ----------------------------------------------------------------------------

FList* DemuxLine(byte *ptr, int time_size, int cnt_size, int pkt_size)
{
  FList *list = DemuxHdr(ptr, time_size, cnt_size, pkt_size);
  return list;
}

// ---------------------------------------------------------------------------

Cond* ProcessCondition(Cond *l, char *str)
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
  type = (char*)alloca(type_len + 1);
  op = (char*)alloca(op_len + 1);
  value = (char*)alloca(value_len + 1);

  strncpy(type, str, type_len);
  type[type_len] = '\0';

  strncpy(op, str + type_len, op_len);
  op[op_len] = '\0';

  strncpy(value, str + type_len + op_len, value_len);
  value[value_len] = '\0';
  
  return AddCondition(l, type, op, value);
}

// ----------------------------------------------------------------------------


extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  FILE *stream, *junk;
  int i, opt, junkFlag = 0, verbose = 0;
  Cond *conds = NULL, *cond_tmp;
  char *rules_f_name = NULL, *out_f_name;
  byte *bf;
  Rule *rules, *r;

  while((opt = getopt(argc, argv, "vjf:d:")) != EOF)
      switch(opt)
        {
        case 'd':
          conds = ProcessCondition(conds, optarg);
          break;
        case 'f':
          rules_f_name = optarg;
          break;
        case 'v':
          verbose = 1;
          break;
        case 'j':
          junkFlag = 1;
          break;
        case '?':
          fprintf(stderr, "%s:  [-f rule file] [-jv] [-d extra rule]\n", argv[0]);
          exit(1);
          break;
        }

  rules = parse_rules(rules_f_name);

  for ( ; optind < argc; optind++)
    {
      FILE *input, *output;
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

      TraceFile trace(input);
      trace.ReadHdr();
      byte *bf = new byte[trace.lineLen];

      r = rules;
      while (trace.ReadLine(bf))
        {
          int ok;
          FList *list = DemuxLine(bf, trace.time_len, trace.size_len, trace.pkt_len);
          ok = 0;
          while (r)
            {
              if (check_list(l, r->conds))
        	{
        	  fwrite(bf, sizeof(byte), line_size, r->stream);
        	  ok = 1;
        	  if (verbose) traverse(l);
        	}
              r = r->next;
            }
          if (!ok && junkFlag)
            fwrite(bf, sizeof(byte), line_size, junk);
          free_list(l);
#ifdef DEBUG
          if ((++count % 1000) == 0)
            fprintf(stderr, "%d\n", count);
#endif
          list->Dump();
        }
      fclose(input);
    }
}

