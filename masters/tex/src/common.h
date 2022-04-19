#include <stdio.h>

typedef unsigned char byte;

class TraceFile {
private:
  FILE *stream;

public:
  int lineLen;
  short time_len;
  short size_len;
  short pkt_len;
  short blank;

  TraceFile(FILE *f);
  ~TraceFile();
  void ReadHdr();
  void WriteHdr();
  int ReadLine(unsigned char *ptr);
};

typedef struct {
  short time_len;
  short size_len;
  short pkt_len;
  short blank;
} file_hdr;


typedef struct {
  int rows, columns;
  void *ptr;
} s_data;

// -------------------------------------------------------------------------

#define D_EXP 1
#define D_UNI 2
#define D_DET 3
#define D_GAM 4
#define D_CHI 5
#define D_TDI 6
#define D_NOR 7

union DistParam {
  struct {
    double lambda;
  } Exponential;
  struct {
    double alpha;
    double beta;
  } Pareto;
  struct {
    double mean;
  } Uniform;
  struct {
    double value;
  } Deterministic;
  struct {
    double lambda;
    double k;
  } Gamma;
  struct {
    double mean;
    double deviation;
  } Normal;
  struct {
    double freedom;
  } ChiSquared;
  struct {
    double mean;
    double deviation;
    double freedom;
  } T;
  struct {
    double A;
    double B;
    double D;
  } CutOff;
};

extern double uniform();
extern double DistExp(DistParam &);
extern double DistUniform(DistParam &);
extern double DistDet(DistParam &);
extern double DistPareto(DistParam &);
extern double DistNormal(DistParam &);
extern double DistGamma(DistParam &);
extern double DistChiSq(DistParam &);
extern double DistT(DistParam &);

// -------------------------------------------------------------------------

#ifdef __cplusplus
#include <string.h>

#define EXT_DEFAULT 0x01

struct Ext {
  char ext[10];
  int id;
  int flags;
};

extern Ext* CheckExtension(char *fname, char *rtn, Ext *exts[]);
#endif

extern void get_hdr(FILE*, file_hdr*);
extern void put_hdr(FILE*, file_hdr*);
extern int ReadFileText(int, s_data*, FILE*);
extern int ReadFileTextFloat(int, s_data*, FILE*);
extern void ReadFileBinary(s_data*, FILE*);
extern void DumpMtx(s_data*);
extern void DumpMtxFloat(s_data*);
