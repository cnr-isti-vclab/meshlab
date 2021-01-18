/*
	double gasdev (idum)
	int    *idum

 	  returns a normally distributed deviate with zero mean and unit variance.


	A + B * gasdev (idum)

	  is a normally distributed deviate with mean A and standard deviation B.


	idum -	set to any negative integer to initialize or reinitialize
		the random number generator.

	(From the Numerical Recipes in C)
*/

#include <math.h>

#define M1 259200
#define IA1 7141
#define IC1 54773
#define RM1 (1.0/M1)
#define M2 134456
#define IA2 8121
#define IC2 28411
#define RM2 (1.0/M2)
#define M3 243000
#define IA3 4561
#define IC3 51349

double    ran1 (idum)
    int      *idum;
{
    static long ix1,
              ix2,
              ix3;
    static double r[98];
    double    temp;
    static int iff = 0;
    int       j;

    if (*idum < 0 || iff == 0) {
	iff = 1;
	ix1 = (IC1 - (*idum)) % M1;
	ix1 = (IA1 * ix1 + IC1) % M1;
	ix2 = ix1 % M2;
	ix1 = (IA1 * ix1 + IC1) % M1;
	ix3 = ix1 % M3;
	for (j = 1; j <= 97; j++) {
	    ix1 = (IA1 * ix1 + IC1) % M1;
	    ix2 = (IA2 * ix2 + IC2) % M2;
	    r[j] = (ix1 + ix2 * RM2) * RM1;
	}
	*idum = 1;
    }
    ix1 = (IA1 * ix1 + IC1) % M1;
    ix2 = (IA2 * ix2 + IC2) % M2;
    ix3 = (IA3 * ix3 + IC3) % M3;
    j = 1 + ((97 * ix3) / M3);
    temp = r[j];
    r[j] = (ix1 + ix2 * RM2) * RM1;
    return temp;
}

#undef M1
#undef IA1
#undef IC1
#undef RM1
#undef M2
#undef IA2
#undef IC2
#undef RM2
#undef M3
#undef IA3
#undef IC3

double    gasdev (idum)
    int      *idum;
{
    static int iset = 0;
    static double gset;
    double    fac,
              r,
              v1,
              v2;

    if (iset == 0) {
	do {
	    v1 = 2.0 * ran1 (idum) - 1.0;
	    v2 = 2.0 * ran1 (idum) - 1.0;
	    r = v1 * v1 + v2 * v2;
	} while (r >= 1.0);
	fac = sqrt (-2.0 * log (r) / r);
	gset = v1 * fac;
	iset = 1;
	return v2 * fac;
    } else {
	iset = 0;
	return gset;
    }
}
