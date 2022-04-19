#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#include "common.h"
#include "mathlib.h"

// ------------------------------------------------------------------------

struct Params {
  double limit;
  int verbose;
  int nodes;
};

// ------------------------------------------------------------------------

class Node {
private:
  double (*renewalDist)(DistParam &p);
  DistParam renewalParam;

public:
  class Node *next;
  double lastEvent, nextEvent;

  Node()
    {
      next = 0;
      lastEvent = nextEvent = 0.0;
    };

  void SetRenewalParam(DistParam &p) { renewalParam = p; };
  void SetRenewalDist(double (*d)(DistParam &p)) { renewalDist = d; };

  void GenerateEvent()
    {
      double evt = (*renewalDist)(renewalParam);
      lastEvent = nextEvent;
      nextEvent += evt;
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
//    double time = currentTime;
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

void OutputBinary(double t, void *d)
{
  short hdr_size = 0;
  long tmp = (long)t;
  FILE *stream = (FILE*)d;

  fwrite(&tmp, sizeof(long), 1, stream);
  fwrite(&hdr_size, sizeof(short), 1, stream);
}

// ------------------------------------------------------------------------

double DistAbsT(DistParam &p)
{
  return fabs(DistT(p));
}

double DistCutOff(DistParam &p)
{
  register double uni = sunif();
  return exp(log((pow(p.CutOff.B,-p.CutOff.D) - pow(p.CutOff.A,-p.CutOff.D) *
        	  uni) + pow(p.CutOff.A,-p.CutOff.D))/ -p.CutOff.D);
}

// ------------------------------------------------------------------------

extern int optind;
extern char* optarg;

int main(int argc, char *argv[])
{
  int ch;
  Params p = {100000, 0,1};
  file_hdr hdr = {sizeof(long), sizeof(short), 0, 0};
  srand48(time(NULL));
  
  while ((ch = getopt(argc, argv, "hvt:n:")) != -1)
    switch(ch)
      {
      case 'h':
      case '?':
        fprintf(stderr, "%s:  -v [-n nodes] [-t time in seconds]\n", argv[0]);
        exit(1);
        break;
      case 't':
        p.limit = 1000 * atof(optarg);
        break;
      case 'v':
        p.verbose = 1;
      case 'n':
        p.nodes = atoi(optarg);
        break;
      }

  for (; optind < argc; optind++)
    {
      FILE *stream;
      char f_tmp[256];
      sprintf(f_tmp, "%s.tra", argv[optind]);
      stream = fopen(f_tmp, "wb");
      put_hdr(stream, &hdr);

      DistParam d;

      //      d.Exponential.lambda = 0.05;
      //      d.Uniform.mean = 20;

      d.Pareto.alpha = 1.5;
      d.Pareto.beta = 40.0;

      d.T.mean = 0;
      d.T.deviation = 1;
      d.T.freedom = 4;
      
      //      d.CutOff.A = 10.0;
      //      d.CutOff.B = 1000000.0;
      //      d.CutOff.D = 1.5;

      Chain chain;
      Node n, nlist[500];
      //      n.SetRenewalDist(DistExp);
      //      n.SetRenewalDist(DistUniform);
      //      n.SetRenewalDist(DistPareto);
      n.SetRenewalDist(DistAbsT);
      //      n.SetRenewalDist(DistCutOff);

      n.SetRenewalParam(d);
      for (int j = 0; j < p.nodes; j++)
        {
          nlist[j] = n;
          chain.AddNode(&nlist[j]);
        }

      if (p.verbose)
        fprintf(stderr, "Starting simulation on file %s\n", f_tmp);

      while (chain.currentTime < p.limit)
        {
          Node *tmp = chain.RemoveHead();
          chain.UpdateTime(tmp->nextEvent);
          if (chain.currentTime < p.limit)
            OutputBinary(chain.currentTime, stream);
          chain.AddNode(tmp);
        }
      fclose(stream);
    }
}
