#include <math.h>
#include <stdlib.h>

/**
  * Generates a random variable uniformly distributed in the range [0,1]
  *
  * @note since rand() returns integers, ranf() returns floats
  * @note re-uses the rand() from <cstdlib> so it is affected by calls
  *       to srand(...) to set the random generator seed.
  */
float ranf(){
  // rand() is guaranteed to generate at least up to RAND_MAX
  // as specified in cstdlib
  float den = RAND_MAX;
  float retval = ( rand() % RAND_MAX ) / den;
  return retval;
}

// boxmuller.c, Implements the Polar form of the Box-Muller Transformation (c) Copyright 1994, Everett F. Carter Jr.
// Permission is granted by the author to use this software for any application provided this copyright notice is preserved.
float box_muller(float m, float s){				        
  float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;
  // use value from previous call
	if (use_last){		        
		y1 = y2;
		use_last = 0;
	}
	else{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );
		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return( m + y1 * s );
}

/**
  * Generates a random variable with gaussian distribution
  *
  * @param mean     the mean of the distribution
  * @param stdev    the standard deviation of the distribution
  *    
  * @note since rand() returns integers, ranf() returns floats
  * @note re-uses the rand() from <cstdlib> so it is affected by calls
  *       to srand(...) to set the random generator seed.
  * @note this is a simple name wrapper for box_mueller.c
  */
float ranfn(float mean, float stdev){
  return box_muller( mean, stdev );
}
