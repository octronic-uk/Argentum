#pragma once

#include <float.h>

#define RAND_MAX 0
#define	HUGE_VAL 0
#define PI 3.14159265358979324

double fabs (double x);
double sin(double x);
double cos (double x);
double tan (double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double x,double y);
double floor(double x);
double ceil (double x);
double fmod (double numer, double denom);
double sqrt (double x);
double ldexp (double x, int exp);
double log (double x);
double log2  (double x);
double log10 (double x);
double exp (double x);
int rand (void);
int srand (double x);
double pow (double base, double exponent);
double frexp (double x, int* exp);
