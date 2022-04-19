#include <math.h>
#include <limits.h>

#include "common.h"
#include "mathlib.h"

// ------------------------------------------------------------------------

/* Standard Uniform Random Deviates */
/* Copyright 1994, Robert Gentleman and Ross Ihaka */
/* All Rights Reserved */

/* Reference:
 * Wichmann, B. A. and I. D. Hill (1982).
 * Algorithm AS 183: An efficient and portable pseudo-random number generator,
 * Applied Statistics, 31, 188.
 */

// ------------------------------------------------------------------------

inline double uniform()
{
  return drand48();
}

// ------------------------------------------------------------------------

int ix_seed = 123;
int iy_seed = 1234;
int iz_seed = 12345;

double sunif(void)
{
  double value;

//  ix_seed = ix_seed * 171 % 30269;
//  iy_seed = iy_seed * 172 % 30307;
//  iz_seed = iz_seed * 170 % 30323;
//  value = ix_seed / 30269.0 + iy_seed / 30307.0 + iz_seed / 30323.0;
//  return value - (int) value;
  return drand48();
}

// ------------------------------------------------------------------------

/*
 *      Reference:
 *
 *      Ahrens, J.H. and Dieter, U. (1972).
 *      Computer methods for sampling from the exponential and
 *      normal distributions.
 *      Comm. ACM, 15, 873-882.
 *
 *      q[k-1] = sum(alog(2.0)**k/k!) k=1,..,n ,
 *      The highest n (here 8) is determined by q[n-1]=1.0
 *      within standard precision
 */

double q[] =
{
        0.6931471805599453,
        0.9333736875190459,
        0.9888777961838675,
        0.9984959252914960,
        0.9998292811061389,
        0.9999833164100727,
        0.9999985691438767,
        0.9999998906925558,
        0.9999999924734159,
        0.9999999995283275,
        0.9999999999728814,
        0.9999999999985598,
        0.9999999999999289,
        0.9999999999999968,
        0.9999999999999999,
        1.0000000000000000
};


double sexp(void)
{
  double a, u, ustar, umin;
  int i;

  a = 0.0;
  u = sunif();
  while (1)
    {
      u = u + u;
      if (u > 1.0)
        break;
      a = a + q[0];
    }
  u = u - 1.0;

  if (u <= q[0])
    return a + u;

  i = 0;
  ustar = sunif();
  umin = ustar;
  do
    {
      ustar = sunif();
      if (ustar < umin)
        umin = ustar;
      i = i + 1;
    }
  while (u > q[i]);
  return a + umin * q[0];
}

// ------------------------------------------------------------------------

static double snorm_a[32] =
{
        0.0,
        0.03917609,
        0.07841241,
        0.1177699,
        0.1573107,
        0.1970991,
        0.2372021,
        0.2776904,
        0.3186394,
        0.3601299,
        0.4022501,
        0.4450965,
        0.4887764,
        0.5334097,
        0.5791322,
        0.626099,
        0.6744898,
        0.7245144,
        0.7764218,
        0.8305109,
        0.8871466,
        0.9467818,
        1.00999,
        1.077516,
        1.150349,
        1.229859,
        1.318011,
        1.417797,
        1.534121,
        1.67594,
        1.862732,
        2.153875
};

static double snorm_d[31] =
{
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.2636843,
        0.2425085,
        0.2255674,
        0.2116342,
        0.1999243,
        0.1899108,
        0.1812252,
        0.1736014,
        0.1668419,
        0.1607967,
        0.1553497,
        0.1504094,
        0.1459026,
        0.14177,
        0.1379632,
        0.1344418,
        0.1311722,
        0.128126,
        0.1252791,
        0.1226109,
        0.1201036,
        0.1177417,
        0.1155119,
        0.1134023,
        0.1114027,
        0.1095039
};

static double snorm_t[31] =
{
        7.673828e-4,
        0.00230687,
        0.003860618,
        0.005438454,
        0.007050699,
        0.008708396,
        0.01042357,
        0.01220953,
        0.01408125,
        0.01605579,
        0.0181529,
        0.02039573,
        0.02281177,
        0.02543407,
        0.02830296,
        0.03146822,
        0.03499233,
        0.03895483,
        0.04345878,
        0.04864035,
        0.05468334,
        0.06184222,
        0.07047983,
        0.08113195,
        0.09462444,
        0.1123001,
        0.136498,
        0.1716886,
        0.2276241,
        0.330498,
        0.5847031
};

static double snorm_H[31] =
{
        0.03920617,
        0.03932705,
        0.03950999,
        0.03975703,
        0.04007093,
        0.04045533,
        0.04091481,
        0.04145507,
        0.04208311,
        0.04280748,
        0.04363863,
        0.04458932,
        0.04567523,
        0.04691571,
        0.04833487,
        0.04996298,
        0.05183859,
        0.05401138,
        0.05654656,
        0.0595313,
        0.06308489,
        0.06737503,
        0.07264544,
        0.07926471,
        0.08781922,
        0.09930398,
        0.1155599,
        0.1404344,
        0.1836142,
        0.2790016,
        0.7010474
};

double snorm(void)
{
  double s, u, w, y, ustar, aa, tt;
  int i;

  u = sunif();
  s = 0.0;
  if (u > 0.5)
    s = 1.0;
  u = u + u - s;
  u *= 32.0;
  i = (int) u;
  if (i == 32)
    i = 31;
  if (i != 0)
    {
      ustar = u - i;
      aa = snorm_a[i - 1];
      while (ustar <= snorm_t[i - 1])
        {
          u = sunif();
          w = u * (snorm_a[i] - aa);
          tt = (w * 0.5 + aa) * w;
          while (1)
            {
              if (ustar > tt)
        	{
        	  y = aa + w;
        	  return (s == 1.0) ? -y : y;
        	}
              u = sunif();
              if (ustar < u)
        	break;
              tt = u;
              ustar = sunif();
            }
          ustar = sunif();
        }
      w = (ustar - snorm_t[i - 1]) * snorm_H[i - 1];
    }
  else
    {
      i = 6;
      aa = snorm_a[31];
      while (1)
        {
          u = u + u;
          if (u >= 1.0)
            break;
          aa = aa + snorm_d[i - 1];
          i = i + 1;
        }
      u = u - 1.0;
      while (1)
        {
          w = u * snorm_d[i - 1];
          tt = (w * 0.5 + aa) * w;
          while (1)
            {
              ustar = sunif();
              if (ustar > tt)
        	{
        	  y = aa + w;
        	  return (s == 1.0) ? -y : y;
        	}
              u = sunif();
              if (ustar < u)
        	break;
              tt = u;
            }
          u = sunif();
        }
    }
  y = aa + w;
  return (s == 1.0) ? -y : y;
}

// ------------------------------------------------------------------------

#define C1        	0.398942280401433
#define C2        	0.180025191068563
#define g(x)        	(C1*exp(-x*x/2.0)-C2*(a-fabs(x)))
#define max(a,b)        (((a)>(b))?(a):(b))
#define min(a,b)        (((a)<(b))?(a):(b))

static double a =  2.216035867166471;

inline double runif()
{
  return drand48();
//  return sunif();
}

double rnorm(void)
{
  double t, u1, u2, u3;
  double runif();

  u1 = runif();
  if(u1 < 0.884070402298758)
    {
      u2 = runif();
      return a * (1.13113163544180 * u1 + u2 - 1);
    }
  else if (u1 >= 0.973310954173898)
    while (1)
      {
        u2 = runif();
        u3 = runif();
        t = (a * a - 2 * log(u3));
        if(u2 * u2 < (a * a) / t)
          return (u1 < 0.986655477086949) ? sqrt(t) : -sqrt(t);
      }
  else if (u1 >= 0.958720824790463)
    while (1)
      {
        u2 = runif();
        u3 = runif();
        t = a - 0.630834801921960 * min(u2, u3);
        if (max(u2, u3) <= 0.755591531667601)
          return (u2 < u3) ? t : -t;
        if (0.034240503750111 * fabs(u2 - u3) <= g(t))
          return (u2 < u3) ? t : -t;
      }
  else if (u1 >= 0.911312780288703)
    while (1)
      {
        u2 = runif();
        u3 = runif();
        t = 0.479727404222441 + 1.105473661022070 * min(u2, u3);
        if (max(u2, u3) <= 0.872834976671790)
          return (u2 < u3) ? t : -t;
        if (0.049264496373128 * fabs(u2 - u3) <= g(t))
          return (u2 < u3) ? t : -t;
      }
  else
    while (1)
      {
        u2 = runif();
        u3 = runif();
        t = 0.479727404222441 - 0.595507138015940 * min(u2, u3);
        if (max(u2, u3) <= 0.805577924423817)
          return (u2 < u3) ? t : -t;
      }
}

static double a1 = 0.3333333;
static double a2 = -0.250003;
static double a3 = 0.2000062;
static double a4 = -0.1662921;
static double a5 = 0.1423657;
static double a6 = -0.1367177;
static double a7 = 0.1233795;
static double e1 = 1.0;
static double e2 = 0.4999897;
static double e3 = 0.166829;
static double e4 = 0.0407753;
static double e5 = 0.010293;
static double q1 = 0.04166669;
static double q2 = 0.02083148;
static double q3 = 0.00801191;
static double q4 = 0.00144121;
static double q5 = -7.388e-5;
static double q6 = 2.4511e-4;
static double q7 = 2.424e-4;
static double sqrt32 = 5.656854;

static double aa = 0.0;
static double aaa = 0.0;

double rgamma(double a, double scale)
{
  static double b, c, d, e, p, q, r, s, t, u, v, w, x;
  static double q0, s2, si;
  double ret_val;
  
  if (a < 1.0)
    {
      /* alternate method for parameters a below 1 */
      /* 0.36787944117144232159 = exp(-1) */
      aa = 0.0;
      b = 1.0 + 0.36787944117144232159 * a;
      for (;;)
        {
          p = b * sunif();
          if (p >= 1.0)
            {
              ret_val = -log((b - p) / a);
              if (sexp() >= (1.0 - a) * log(ret_val))
        	break;
            }
          else
            {
              ret_val = exp(log(p) / a);
              if (sexp() >= ret_val)
        	break;
            }
        }
      return scale * ret_val;
    }
  /* Step 1: Recalculations of s2, s, d if a has changed */
  if (a != aa)
    {
      aa = a;
      s2 = a - 0.5;
      s = sqrt(s2);
      d = sqrt32 - s * 12.0;
    }
  /* Step 2: t = standard normal deviate, */
  /* x = (s,1/2)-normal deviate. */
  /* immediate acceptance (i) */
  
  t = snorm();
  x = s + 0.5 * t;
  ret_val = x * x;
  if (t >= 0.0)
    return scale * ret_val;
  
  /* Step 3: u = 0,1 - uniform sample. squeeze acceptance (s) */
  u = sunif();
  if (d * u <= t * t * t) {
    return scale * ret_val;
  }
  /* Step 4: recalculations of q0, b, si, c if necessary */

  if (a != aaa)
    {
      aaa = a;
      r = 1.0 / a;
      q0 = ((((((q7 * r + q6) * r + q5) * r + q4)
              * r + q3) * r + q2) * r + q1) * r;

      /* Approximation depending on size of parameter a */
      /* The constants in the expressions for b, si and */
      /* c were established by numerical experiments */
      
      if (a <= 3.686)
        {
          b = 0.463 + s + 0.178 * s2;
          si = 1.235;
          c = 0.195 / s - 0.079 + 0.16 * s;
        }
      else if (a <= 13.022)
        {
          b = 1.654 + 0.0076 * s2;
          si = 1.68 / s + 0.275;
          c = 0.062 / s + 0.024;
        }
      else
        {
          b = 1.77;
          si = 0.75;
          c = 0.1515 / s;
        }
    }
  /* Step 5: no quotient test if x not positive */

  if (x > 0.0)
    {
      /* Step 6: calculation of v and quotient q */
      v = t / (s + s);
      if (fabs(v) <= 0.25)
        q = q0 + 0.5 * t * t * ((((((a7 * v + a6) * v + a5) * v + a4) * v + a3)
        			 * v + a2) * v + a1) * v;
      else
        q = q0 - s * t + 0.25 * t * t + (s2 + s2) * log(1.0 + v);
      
      /* Step 7: quotient acceptance (q) */

      if (log(1.0 - u) <= q)
        return scale * ret_val;
    }
  /* Step 8: e = standard exponential deviate */
  /* u= 0,1 -uniform deviate */
  /* t=(b,si)-double exponential (laplace) sample */
  
  for (;;)
    {
      e = sexp();
      u = sunif();
      u = u + u - 1.0;
      if (u < 0.0)
        t = b - si * e;
      else
        t = b + si * e;
      /* Step  9:  rejection if t < tau(1) = -0.71874483771719 */
      if (t >= -0.71874483771719)
        {
          /* Step 10:  calculation of v and quotient q */
          v = t / (s + s);
          if (fabs(v) <= 0.25)
            q = q0 + 0.5 * t * t * ((((((a7 * v + a6) * v + a5) * v + a4) * v + a3)
        			     * v + a2) * v + a1) * v;
          else
            q = q0 - s * t + 0.25 * t * t + (s2 + s2) * log(1.0 + v);
          /* Step 11:  hat acceptance (h) */
          /* (if q not positive go to step 8) */
          if (q > 0.0)
            {
              if (q <= 0.5)
        	w = ((((e5 * q + e4) * q + e3) * q + e2) * q + e1) * q;
              else
        	w = exp(q) - 1.0;
              /* if t is rejected */
              /* sample again at step 8 */
              if (c * fabs(u) <= w * exp(e - 0.5 * t * t))
        	break;
            }
        }
    }
  x = s + 0.5 * t;
  return scale * x * x;
}

// ------------------------------------------------------------------------

double DistDet(DistParam &p)
{
  return p.Deterministic.value;
}

double DistExp(DistParam &p)
{
  register double uni = uniform();
  return -log(1 - uni) / p.Exponential.lambda;
}

double DistUniform(DistParam &p)
{
  register double uni = uniform();
  return uni * p.Uniform.mean / 2;
}

double DistGamma(DistParam &p)
{
  return rgamma(p.Gamma.k, p.Gamma.lambda);
}

double DistChiSq(DistParam &p)
{
  return rgamma(p.ChiSquared.freedom / 2, 0.5);
}

double DistNormal(DistParam &p)
{
  return rnorm() * p.Normal.deviation + p.Normal.mean;
}

double DistT(DistParam &p)
{
  double norm = rnorm() * p.T.deviation + p.T.mean;
  double chi = rgamma(p.T.freedom / 2, 0.5);
  return norm / chi;
}

double DistPareto(DistParam &p)
{
  register double uni = uniform();
  double alpha = p.Pareto.alpha;
  double beta = p.Pareto.beta;
  register double result = beta * (exp(-log(1 - uni) / alpha) - 1);
  return result;
}

// ------------------------------------------------------------------------
