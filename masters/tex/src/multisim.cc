#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#include "common.h"

// ------------------------------------------------------------------------

#define D_EXP 1
#define D_UNI 2
#define D_DET 3
#define D_GAM 4
#define D_CHI 5
#define D_TDI 6
#define D_NOR 7

struct Params {
  double limit;
  int count;
  int verbose;
};

// ------------------------------------------------------------------------

class Node {
private:
  DistParam rateParam, intervalParam, renewalParam;

  double (*rateDist)(DistParam &);
  double (*intervalDist)(DistParam &);
  double (*renewalDist)(DistParam &);

public:
  class Node *next;
  double intervalStart, intervalLength;
  double lastEvent, nextEvent;

  Node()
    {
      next = 0;
      intervalStart = intervalLength = lastEvent = nextEvent = 0.0;
    };

  void SetIntervalParam(DistParam &p) { intervalParam = p; };
  void SetIntervalDist(double (*d)(DistParam &)) { intervalDist = d; };
  void SetRateParam(DistParam &p) { rateParam = p; };
  void SetRateDist(double (*d)(DistParam &)) { rateDist = d; };
  void SetRenewalParam(DistParam &p) { renewalParam = p; };
  void SetRenewalDist(double (*d)(DistParam &)) { renewalDist = d; };

  void GenerateEvent()
    {
      double evt = (*rateDist)(rateParam);
      double intervalEnd = intervalStart + intervalLength;
      lastEvent = nextEvent;
      nextEvent += evt;

      while (nextEvent >= intervalEnd)
        {
          double renew = (*renewalDist)(renewalParam);
          double intv = (*intervalDist)(intervalParam);
          intervalStart = intervalEnd + renew;
          intervalLength = intv;
          intervalEnd = intervalStart + intervalLength;
          nextEvent = intervalStart + evt;
        }
    };
};

class Chain {
private:
  class Node *head;
public:
  double currentTime;

  Chain()
    {
      head = 0;
      currentTime = 0.0;
    };
  AddNode(Node *node)
    {
      Node **back = &head, *tmp = head;
      double time = currentTime;
      if (node->nextEvent <= currentTime)
        node->GenerateEvent();
      while (tmp && (tmp->nextEvent < node->nextEvent))
        {
          back = &tmp->next;
          tmp = tmp->next;
        }
      node->next = tmp;
      *back = node;
    };
  Node *RemoveHead()
    {
      Node *tmp = head;
      head = tmp->next;
      return tmp;
    };
  void UpdateTime(double t) { currentTime = t; };
};


// ------------------------------------------------------------------------

inline double DistFiles(double mean)
{
  double small = 100, large = 600;
  double prop = (small + large - mean) / large;
  double uni = uniform();
  return uni < prop ? small : large;
}

// ------------------------------------------------------------------------

void Simulate(Params *p, void (*output)(double, void*), void* data)
{
  int i;
  Chain chain;
  Node node, *tmp, nodeList[500];
  DistParam interval, rate, renewal;

  interval.Deterministic.value = 100.0;
  rate.Deterministic.value = 5.0;
  renewal.Pareto.alpha = 1.5;
  renewal.Pareto.beta = 100.0;

  node.SetIntervalParam(interval);
  node.SetIntervalDist(DistDet);
  node.SetRateParam(rate);
  node.SetRateDist(DistDet);
  node.SetRenewalParam(renewal);
  node.SetRenewalDist(DistPareto);

  for (i = 0; i < p->count; i++)
    {
      nodeList[i] = node;
      chain.AddNode(&nodeList[i]);
    }

  while (chain.currentTime < p->limit)
    {
      tmp = chain.RemoveHead();
      chain.UpdateTime(tmp->nextEvent);
      if (chain.currentTime < p->limit)
        output(chain.currentTime, data);
      chain.AddNode(tmp);
    }
}

// ------------------------------------------------------------------------

void OutputBinary(double t, void *d)
{
  short hdr_size = 0;
  long tmp = (long)t;
  FILE *stream = (FILE*)d;

  fwrite(&tmp, sizeof(long), 1, stream);
  fwrite(&hdr_size, sizeof(short), 1, stream);

}

// ------------------------------------------------------------------------

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  int ch, i, verbose = 0;
  Params p = {100000.0, 100, 0};
  file_hdr hdr = {sizeof(long), sizeof(short), 0, 0};
 
  while ((ch = getopt(argc, argv, "vc:t:")) != EOF)
    switch(ch)
      {
      case 't':
        p.limit = 1000.0 * atof(optarg);
        break;
      case 'v':
        p.verbose = 1 - p.verbose;
        break;
      case 'c':
        p.count = atoi(optarg);
        break;
      }
  while (optind < argc)
    {
      FILE *input, *output;
      int length;
      char fname[256], ftmp[256];

      length = strlen(argv[optind]);
      if ((length > 4) && !strcmp(argv[optind] + length - 4, ".tra"))
        length -= 4;
      strncpy(fname, argv[optind], length);
      fname[length] = '\0';

      strcpy(ftmp, fname);
      strcat(ftmp, ".tra");
      output = fopen(ftmp, "wb");

      put_hdr(output, &hdr);
      srandom(time((time_t*)0));
      Simulate(&p, OutputBinary, (void*)output);
      fclose(output);
      optind++;
    }
}
