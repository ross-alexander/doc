#define FAMILY_ROOT 0x01

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long longword;

enum {
  lPhysical = 1,
  lLink,
  lNetwork,
  lTransport,
  lSession
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

struct Family
{
  int flags, value, root;
  char *str;
};

class Feature {
public:
  int type;
  int len;
  byte *ptr;

  Feature(int, int, byte*);
  ~Feature();
  void Dump();

};

class FList {
 public:
  Feature *head;
  class FList *tail;

  FList();
  FList(Feature*);
  FList(Feature*, class FList*);
  ~FList();
  void operator+=(Feature *);
  class FList *operator+(Feature *);
  void Dump();
};

struct Layer
{
  int value;
  char *str;
};

enum {
  eqaulity = 1,
  inequality
};

struct Operator {
  int value;
  char *str;
};

struct Cond {
  struct Cond *next;
  int left;
  int opr;
  int right;
};

struct Rule {
  struct Rule *next;
  char *name;
  char *ext;
  FILE *stream;
  Cond *conds;
};

extern Operator op_list[];
extern Family family_list[];
extern Layer layer_list[];

Cond* AddCondition(Cond*, char*, char*, char*);
Rule* parse_rules(char*);
