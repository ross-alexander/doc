#include "vector.h"

#include <stdio.h>

#ifndef INLINE
#define INLINE
#endif

Vector::Vector()
{
  size = 0;
  ptr = 0;
  ref = 0;
  refptr = &ref;
}

Vector::Vector(int s)
{
  size = s;
  ptr = new double[s];
  ref = 1;
  refptr = &ref;
}

Vector::~Vector()
{
  (*refptr)--;
  if (!(*refptr))
    delete ptr;
}

Vector::Vector(Vector &v)
{
  size = v.size;
  ptr = v.ptr;
  refptr = v.refptr;
  (*refptr++);
}

INLINE Vector &Vector::operator=(Vector &v)
{
  size = v.size;
  ptr = v.ptr;
  refptr = v.refptr;
  (*refptr++);
  return *this;
}

Vector Vector::operator()()
{
  Vector tmp = Vector(size);
  for (register int i = 0; i < size; i++)
    tmp[i] = ptr[i];
  return tmp;
}

Vector Vector::operator()(int start)
{
  Vector tmp = Vector(size - start);
  for (register int i = start; i < size; i++)
    tmp[i - start] = ptr[i];
  return tmp;
}

Vector Vector::operator()(int start, int length)
{
  Vector tmp = Vector(length);
  for (register int i = 0; i < length; i++)
    tmp[i] = ptr[i + start];
  return tmp;
}

INLINE double &Vector::operator[](int i)
{
  static double zero = 0.0;
#ifdef DEBUG
  if (i >= size)
    {
      fprintf(stderr, "Illegal access %d\n", i);
      exit(1);
    }
#endif
  return i < size ? ptr[i] : zero;
}

INLINE int Vector::length()
{
  return size;
}

INLINE void Vector::map(double (*f)(double))
{
  int i;
  for (i = 0;i < size; i++)
    ptr[i] = (*f)(ptr[i]);
}

INLINE double Vector::fold(double (*f)(double),
        	    double (*g)(double, double), double initial)
{
  for (int i = 0; i < size; i++)
    initial = (*g)((*f)(ptr[i]), initial);
  return initial;
}

Vector Vector::aggregate(int i)
{
  register s = size / i;
  double sum;
  Vector tmp(s);
  for (int j = 0; j < s; j++)
    {
      sum = 0;
      for (int k = 0; k < i; k++)
        sum += ptr[j * i + k];
      tmp[j] = sum / i;
    }
  return tmp;
}

// ----------------------------------------------------------------------------

double Vector::sum_ident()
{
  register int i;
  register double s = 0.0;
  for (i = 0; i < size; i++)
    s += ptr[i];
  return s;
}

double Vector::sum_square()
{
  register int i;
  register double s = 0.0;
  for (i = 0; i < size; i++)
    s += ptr[i] * ptr[i];
  return s;
}

// ----------------------------------------------------------------------------

double f_identity(double x)
{
  return x;
}

double f_square(double x)
{
  return x*x;
}

double f_add(double x, double y)
{
  return x + y;
}
