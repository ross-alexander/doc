#ifndef _common_
#define _common_

#include "queue.h"
#include <time.h>

/* ----------------------------------------------------------------------
-  essential defined
---------------------------------------------------------------------- */
#define TRUE 1
#define true TRUE
#define FALSE 0
#define false FALSE

typedef unsigned long longword;		/* 32 bits */
typedef unsigned short word;		/* 16 bits */
typedef unsigned char byte;		/*  8 bits */

#define htons(x) intel16(x)
#define htonl(x) intel(x)
#define ntohs(x) intel16(x)
#define ntohl(x) intel(x)
#define checksum(p, len) inchksum(p, len)

/* ----------------------------------------------------------------------
-  packet driver defines
---------------------------------------------------------------------- */
#define MAX_INTF 6
#define MAX_PCOLS 6

#define PD_USABLE 1

#define PD_CLASS_DIX 1
#define PD_CLASS_SLIP 6
#define PD_CLASS_8023 11

#define IP_ADDR_LEN 4
#define ENET_ADDR_LEN 6
#define SLIP_ADDR_LEN 0
#define PPP_ADDR_LEN 0
#define MAX_ADDR_LEN 10

#define ENET_HDR_LEN 14
#define ENET_TLR_LEN 2
#define ENET_MTU 1500

#define SLIP_HDR_LEN 2
#define SLIP_TLR_LEN 2
#define SLIP_MTU 500

#define PPP_HDR_LEN 4
#define PPP_TLR_LEN 2
#define PPP_MTU 1500

#define MAX_KEEP_PKT 58
#define MAX_KEEP_CNT 400

/* ----------------------------------------------------------------------
-  packet driver type defintions
---------------------------------------------------------------------- */
typedef struct {
  int intf, index, flags;
  word type;
  word handle;
  int addrLen;
  byte addr[MAX_ADDR_LEN];
  byte bcastAddr[MAX_ADDR_LEN];
  long pktRcv, pktSent;
  queue rcvQ;
  } protoRec;

typedef struct {
  word vector;
  word PDNumber;
  int number;
  int functionality;
  int class;		/* Packet driver class */
  int version;
  int type;
  int flags;		/* flags */ 
  int nProto;		/* # protocols attached */
  int intfType;
  int hardware_type;	/* ARP hardware type */
  int hdrLen;		/* size of header in octets */
  int tlrLen;		/* size of trailer in octets */
  int maxPktLen;	/* MTU, 1500 ethernet */
  int functions;	/* unicast, broadcast, multicast */
  } intfRec;

typedef struct {
  int nIntf;
  int maxHdr, maxMTU;
  intfRec intf[MAX_INTF];
  protoRec pcol[MAX_INTF][MAX_PCOLS];
  queue rcvQ[MAX_PCOLS];
} PDRec;

/* ----------------------------------------------------------------------
-  assembly prototypes
---------------------------------------------------------------------- */ 
extern unsigned long intel(unsigned long val);
extern unsigned int intel16(unsigned int val);
extern word inchksum(void *ptr, int len);

/* ----------------------------------------------------------------------
-  packet driver prototypes
---------------------------------------------------------------------- */ 
extern PDRec *PDInit();
extern word PDAddProtocol(int intf);
extern void PDRemoveProtocol(int intf, word handle);
extern int PDCheckQueue(void);

#endif
