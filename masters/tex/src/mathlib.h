/* R Mathematical Function Library Header */
/* Copyright 1994, Robert Gentleman and Ross Ihaka */
/* All Rights Reserved */

#include <math.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

#ifndef drand48
extern "C" double drand48();
#endif

#ifndef srand48
extern "C" void srand48(long);
#endif

extern double snorm(void);
extern double sunif(void);
extern double sexp(void);

extern double rgamma(double, double);
extern double rnorm();

/* 30 Decimal-place constants computed with bc */

#ifndef M_1_SQRT_2PI
#define M_1_SQRT_2PI        0.398942280401432677939946059934
#endif

#ifndef M_1_SQRT_2
#define M_1_SQRT_2        0.707106781186547524400844362105
#endif

#ifndef M_PI
#define M_PI        	3.141592653589793238462643383276
#endif
