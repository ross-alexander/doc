/* **********************************************************************
*
*  Packet driver interface code
*
*  version 0.0.1
*    Written by Ross Alexander, 17 Dec 1992
*    Modefied for a general packet receiver on 29 March 1994
*
********************************************************************** */

#pragma inline

#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "queue.h"
#include "common.h"

#define MAX_BUFFER 50
#define GRAB_SIZE 40

/* ----------------------------------------------------------------------
-	packet driver defines						-
---------------------------------------------------------------------- */
#define CARRY 1

#define INT_FIRST 0x60
#define INT_LAST 0x80

#define PKT_LINE "PKT DRVR"
#define MAX_DRIVERS 6
#define MAX_PROTOCOLS 5

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
/* ----------------------------------------------------------------------
-  external function prototypes
---------------------------------------------------------------------- */
extern long hw_time(void);

/* ----------------------------------------------------------------------
-	packet driver external prototypes				-
---------------------------------------------------------------------- */
extern void pktentry0();
extern void pktentry1();
extern void pktentry2();
extern void pktentry3();
/* ----------------------------------------------------------------------
-	packet driver global variables
---------------------------------------------------------------------- */
void  (*pkentry[])() = { pktentry0, pktentry1 };
PDRec pd;

long cur_bf, bf_cnt = 0, pkt_count = 0, init_tick;
int refresh = 0;
byte *buffer[2];

/* **********************************************************************
*
*	pkt_rcv_1
*
*  Function used to received packets from the packet drivers.
*
*  Inputs:
*    unsigned int handle - the handle the packet was received on (each
*                          protocol/port combination is	given a handle by
*                          the packet drivers)
*    unsigned int len - the length of the incoming packet
*
*    Outputs:
*      unsigned char* - a pointer to a buffer where the can be stored
*                       in memory
*
********************************************************************** */
byte far *pkt_rcv_1(word len)
{
  byte *tmp;
  if (!(bf_cnt < MAX_KEEP_CNT))
    {
      bf_cnt = 0;
      refresh = cur_bf + 1;
      cur_bf = 1 - cur_bf;
    }
  tmp = buffer[cur_bf] + (sizeof(clock_t) + sizeof(int) + MAX_KEEP_PKT)*bf_cnt;
  *((long*)(tmp)) = htonl(hw_time());
  *((int*)(tmp + sizeof(clock_t))) = htons(len);
  if (MAX_KEEP_PKT)
    return tmp + (sizeof(clock_t) + sizeof(int));
  else
    {
      bf_cnt++;
      pkt_count++;
      return NULL;
    }
}

/* **********************************************************************
*
*  pkt_rcv_2
*
*  Called once packet has been stored in main memory.  Currently the code
*  does a linear search through the ports and protocols to match the handle.
*  Because of the limited number of ports and protocols (not more than twenty
*  at most), there is little reason to make the search more efficient.
*
*  Inputs:
*    word intf - the interface the packet has arrived on
*    word handle - the handle the packet was received on
*    word len - the length of the incoming packet
*    byte *buff - the address of the buffer
*
********************************************************************** */
void pkt_rcv_2()
{
  bf_cnt++;
  pkt_count++;
}

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
 	  switch(pd.intf[i].class)
	    {
	    case PD_CLASS_DIX:
	      pd.intf[i].flags = PD_USABLE;
	      pd.intf[i].hdrLen = ENET_HDR_LEN;
	      pd.intf[i].tlrLen = ENET_TLR_LEN;
	      pd.intf[i].maxPktLen = ENET_MTU;
	      i++;
	      break;
	    case PD_CLASS_SLIP:
	      pd.intf[i].flags = PD_USABLE;
	      pd.intf[i].hdrLen = SLIP_HDR_LEN;
	      pd.intf[i].tlrLen = SLIP_TLR_LEN;
	      pd.intf[i].maxPktLen = SLIP_MTU;
	      i++;
	      break;

	    default:
	      pd.intf[i].flags = 0x00;
	      break;
	    }
	}
    }
  return i;
}

/* 
*/
int DriverAdd(int i)
{
  struct REGPACK regs;
  int error, result, addr_len;
  word temp, handle;
  byte addr[MAX_ADDR_LEN];
  temp = ntohs(0x0800);

  regs.r_ax = PD_ACCESS_TYPE | PD_CLASS_DIX;
  regs.r_bx = 0xFFFF;		/* pd.intf[i].type; */
  regs.r_cx = 0;
  regs.r_dx = pd.intf[i].PDNumber;
  regs.r_ds = FP_SEG(&temp);
  regs.r_si = FP_OFF(&temp);
  regs.r_es = FP_SEG(pkentry[i]);
  regs.r_di = FP_OFF(pkentry[i]);
  intr(pd.intf[i].vector, &regs);
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
  intr(pd.intf[i].vector, &regs);
  addr_len = regs.r_cx;
 
  switch(pd.intf[i].functionality)
    {
    case PD_FN_EXT:
    case PD_FN_EXT_HP:
      {
	regs.r_ax = PD_SET_RCV_MODE;
	regs.r_bx = handle;
	regs.r_cx = PD_RCV_ALL;
	intr(pd.intf[i].vector, &regs);
	break;
      }
    case PD_FN_BASIC:
    case PD_FN_HP:
    default:
      break;
    }
  return handle;
}

void DriverRemove(int i, word handle)
{
  struct REGPACK regs;
  regs.r_ax = PD_RELEASE_TYPE;
  regs.r_bx = handle;
  intr(pd.intf[i].vector, &regs);
}

/* **********************************************************************
*
*  PDInit
*
********************************************************************** */
PDRec *PDInit()
{
  int numDrivers, maxHdr, maxLen, store_len;
  register int i, temp;

  pd.nIntf = CheckDrivers();

  for (maxHdr = maxLen = i = 0; i < pd.nIntf; i++)
    {
      temp = pd.intf[i].maxPktLen + pd.intf[i].hdrLen + pd.intf[i].tlrLen;
      maxLen = maxLen > temp ? maxLen : temp;
      maxHdr = maxHdr > pd.intf[i].hdrLen ? maxHdr : pd.intf[i].hdrLen;
    }
  pd.maxMTU = maxLen;
  pd.maxHdr = maxHdr;
  maxLen += sizeof(clock_t) + sizeof(int);
  store_len = MAX_KEEP_PKT + sizeof(clock_t) + sizeof(int);
  buffer[0] = malloc(MAX_KEEP_CNT * store_len + maxLen);
  buffer[1] = malloc(MAX_KEEP_CNT * store_len + maxLen);
  cur_bf = 0;
  return &pd;
}
