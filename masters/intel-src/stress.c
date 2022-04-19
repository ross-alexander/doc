#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BUFFER 50
#define GRAB_SIZE 40

/* ----------------------------------------------------------------------
-	packet driver defines						-
---------------------------------------------------------------------- */
#define CARRY 1

#define INT_FIRST 0x60
#define INT_LAST 0x80

#define PKT_LINE "PKT DRVR"

#define PD_CLASS_DIX 1
#define PD_CLASS_SLIP 6
#define PD_CLASS_8023 11

#define PD_DRIVER_INFO 0x01FF
#define PD_ACCESS_TYPE 0x0200
#define PD_RELEASE_TYPE 0x0300
#define PD_SEND_PACKET 0x0400
#define PD_GET_ADDRESS 0x0600
#define PD_SET_RCV_MODE 0x1400

#define PD_FN_BASIC 1
#define PD_FN_EXT 2
#define PD_FN_HP 5
#define PD_FN_EXT_HP 6

#define PD_RCV_OFF 1
#define PD_RCV_UNI 2
#define PD_RCV_BCAST 3
#define PD_RCV_LIM_MCAST 4
#define PD_RCV_ALL_MCAST 5
#define PD_RCV_ALL 6

#define MAX_INTF 6
#define MAX_ADDR_LEN 8

/* ----------------------------------------------------------------------
-  general type definitions
---------------------------------------------------------------------- */
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long longword;

#define htons(x) intel16(x)
#define htonl(x) intel(x)
#define ntohs(x) intel16(x)
#define ntohl(x) intel(x)
/* ----------------------------------------------------------------------
-  externs
---------------------------------------------------------------------- */
extern word intel16(word);
extern longword intel(longword);

extern void hwt_init(void);
extern void hwt_fin(void);
extern unsigned long hw_time(void);

extern void pktentry0(void);

/* ----------------------------------------------------------------------
-  packet driver type defintions
---------------------------------------------------------------------- */
typedef struct {
  word vector;
  word PDNumber;
  int number;
  int functionality;
  int class;		/* Packet driver class */
  int version;
  int type;
  int flags;		/* flags */ 
  } intfRec;

typedef struct {
  int nIntf;
  intfRec intf[MAX_INTF];
} PDRec;

/* ----------------------------------------------------------------------
-  global variables
---------------------------------------------------------------------- */
PDRec pd;

/* **********************************************************************
*
*  CheckDrivers
*
*  Runs through the list of available packet drivers and finds those ones
*   which are usable.  Currently on Dec Intel Xerox ethernet cards are usable.
*
*  Output:
*    int - the number of usable packet drivers.
*
********************************************************************** */
int CheckDrivers(void)
{
  int i;
  struct REGPACK regs;
  word packetInt;
  longword far *interrupts;
  char far *temp;

  interrupts = 0L;		/* a PC hack looking through low memory */
  i = 0;

  for (packetInt = INT_FIRST; packetInt <= INT_LAST; packetInt++)
    {
      temp = (char far *)(interrupts[packetInt]);
      if (!strcmp(&temp[3], PKT_LINE))
	{
	  pd.intf[i].vector = packetInt;
	  regs.r_ax = PD_DRIVER_INFO;
	  intr(pd.intf[i].vector, &regs);
	  if (regs.r_flags & CARRY)
	    exit(1);
	  pd.intf[i].functionality = regs.r_ax & 0xFF;
	  pd.intf[i].version = regs.r_bx;
	  pd.intf[i].class = regs.r_cx >> 8;
	  pd.intf[i].type = regs.r_dx;
	  pd.intf[i].PDNumber = regs.r_cx & 0xFF;
	  printf("Packet driver at 0x%0X type %d\n",pd.intf[i].vector,
	          pd.intf[i].class);
	  i++;
	}
    }
  return i;
}

byte far* pkt_rcv_1() {return 0L;}
void pkt_rcv_2() {}

int add_protocol(PDRec *pd, int intf)
{
  struct REGPACK regs;
  int error, result, addr_len;
  word tmp, handle;
  byte addr[MAX_ADDR_LEN];
  tmp = ntohs(0x0801);

  regs.r_ax = PD_ACCESS_TYPE | PD_CLASS_DIX;
  regs.r_bx = 0xFFFF;
  regs.r_cx = sizeof(tmp);
  regs.r_dx = pd->intf[intf].PDNumber;
  regs.r_ds = FP_SEG(&tmp);
  regs.r_si = FP_OFF(&tmp);
  regs.r_es = FP_SEG(pktentry0);
  regs.r_di = FP_OFF(pktentry0);
  intr(pd->intf[intf].vector, &regs);
  handle = regs.r_ax;

  error = regs.r_flags & CARRY;
  if (error)
    {
      printf("Error in ACCESS_TYPE\n");
      return 0;
    }
  regs.r_ax = PD_GET_ADDRESS;
  regs.r_bx = handle;
  regs.r_cx = MAX_ADDR_LEN;
  regs.r_es = FP_SEG(addr);
  regs.r_di = FP_OFF(addr);
  intr(pd->intf[intf].vector, &regs);
  addr_len = regs.r_cx;

  switch(pd->intf[intf].functionality)
    {
    case PD_FN_EXT:
    case PD_FN_EXT_HP:
      {
	regs.r_ax = PD_SET_RCV_MODE;
	regs.r_bx = handle;
	regs.r_cx = PD_RCV_OFF;
	intr(pd->intf[intf].vector, &regs);
	break;
      }
    case PD_FN_BASIC:
    case PD_FN_HP:
    default:
      break;
    }
  return handle;
}

void drop_protocol(PDRec *pd, int intf, word handle)
{
  struct REGPACK regs;
  regs.r_ax = PD_RELEASE_TYPE;
  regs.r_bx = handle;
  intr(pd->intf[intf].vector, &regs);
}
void send_packet(PDRec *pd, int intf, int count, char *bf)
{
  struct REGPACK regs;
/*  int error, result; */

  regs.r_ax = PD_SEND_PACKET;
  regs.r_cx = count;
  regs.r_ds = FP_SEG(bf);
  regs.r_si = FP_OFF(bf);
  intr(pd->intf[intf].vector, &regs);
}

void stress(PDRec *pd, int intf, int size, long count)
{
  word handle;
  byte *bf;
  int i;
  long seq_count = 0;
  
  if(size < 64) size = 64;
  printf("Stressing Network:  count = %ld size = %d\n", count, size);
  bf = (byte*)malloc(size);
  for (i = 0; i < 14; i++) bf[i] = 0xFF;
  handle = add_protocol(pd, intf);
  for (seq_count = 0; seq_count < count; seq_count++)
    {
      *(longword*)(bf[14]) = seq_count;
      *(longword*)(bf[18]) = 0; /* hw_time(); */
      send_packet(pd, intf, size, bf);
/* printf("."); */
    }      
  drop_protocol(pd, intf, handle);
}

int main(int argc, char **argv)
{
  int i, size = 500, count = 1000;
  for (i = 0; i < argc; i++)
    if (argv[i][0] == '-')
      switch (argv[i][1])
        {
        case 's':
          size = atoi(argv[++i]);
          break;
        case 'c':
          count = atoi(argv[++i]);
        }
  hwt_init();
  if (!(pd.nIntf = CheckDrivers()))
    {
      printf("No packet drivers were found.\n");
      exit(1);
    }
  stress(&pd, 0, size, count);
  hwt_fin();
  exit(0);
}
