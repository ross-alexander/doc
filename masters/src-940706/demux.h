#define FAMILY_ROOT 0x01

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long longword;

enum {
  physical = 1,
  link,
  network,
  transport,
  session
};

enum {
  pktTime = 1,
  pktSize,
  enet,
  enetDest,
  enetSrc,
  enetSize,
  DIXType,
  LSAP,
  LSAPSrc,
  LSAPDest,
  LSAPCntl,
  LSAPUnknown,
  SNAP,
  SNAPOrg,
  SNAPType,
  IP,
  IPSrc,
  IPDest,
  IPProtocol,
  ARP,
  ARPhrd,
  ARPpro,
  ARPhln,
  ARPpln,
  ARPop,
  DDP,
  DDPSrcNet,
  DDPDestNet,
  DDPSrcNode,
  DDPDestNode,
  DDPSrcSocket,
  DDPDestSocket,
  DDPType,
  ATP,
  Xerox,
  DSAP,
  DECMOPRemoteConsole,
  DECNETphIVRoute,
  DECLAT,
  DECLAVC,
  DECLANBridge,
  unknownLink,
  unknownNetwork
};

typedef struct family
{
  int flags, value, root;
  char *str;
} family;

typedef struct feature
{
  int type, len;
  byte *ptr;
} feature;

typedef struct list
{
  void *head;
  struct list *tail;
} list;

#ifdef __cplusplus
class c_feature {
 public:
  c_feature(int, int, byte*);
  c_feature~();
  int type;
  int len;
  byte *ptr;
};

class c_list {
 public:
  c_list();
  c_list(void*);
  c_list(void*, c_list*);
 private:
  void *head;
  c_list *tail;
};
#endif

typedef struct layer
{
  int value;
  char *str;
} layer;

enum {
  eqaulity = 1,
  inequality
};

typedef struct operator {
  int value;
  char *str;
} operator;

typedef struct cond {
  struct cond *next;
  int left;
  int operator;
  int right;
} cond;

typedef struct rule {
  struct rule *next;
  char *name;
  char *ext;
  FILE *stream;
  cond *conds;
} rule;

extern operator op_list[];
extern family family_list[];
extern layer layer_list[];

extern cond* AddCondition(cond*, char*, char*, char*);
extern rule* parse_rules(char*);
