#include <math.h>
#include <stdlib.h>
#include <vcg/space/ray3.h>
#include <vector>
#include <QFile>
#include <QTextStream>
using namespace std;
using namespace vcg;

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

void generateRayCone( Ray3f& dir, float aperture, int raysPerCone, vector<Ray3f>& cone, vector<float>& coneSdf ){
  // Make sure it's normalized
  dir.Normalize();

  // Matrix that vector around +x: (phi=0,theta=0) to vectors around current direction
  Matrix33<float> relToAbs = RotationMatrix( Point3f(1,0,0), dir.Direction() );
  
  // Reset the vales
  coneSdf.resize(raysPerCone, 0);  
  cone.resize(raysPerCone, Ray3f());  
  
// #define DEBUG_CONERAY
#ifdef DEBUG_CONERAY
  // Tested setting the direction along cardinal axis
  //  dir.SetDirection(Point3f(0,0,1));
  //  dir.SetOrigin(Point3f(0,0,0));
  // raysPerCone = 100;
  // aperture = math::ToRad(0.1);
  FILE* fp;
  if( output )
    fp = fopen("/Users/ata2/Desktop/myrays.txt", "w");
#endif

  
  // First ray is *always* directed along antinormal
  cone[0] = dir;
  
  // Other rays are distributed around it
  for( int i=1; i<raysPerCone; i++ ){
    // Construct an isotropic bi-variate distribution
    // of "giggling" around +x axis
    float theta_giggle = ranfn(0, aperture);
    float phi_giggle   = ranfn(0, aperture);
    Point3f giggle_v;
    giggle_v.FromPolarRad(1, theta_giggle, phi_giggle);
    
    // Transform the ray back in object's coordinate system
    // and add it to the list 
    Ray3f currRay;
    currRay.SetDirection( relToAbs*giggle_v );
    currRay.SetOrigin( dir.Origin() );
    cone[i] = currRay;
    
#ifdef DEBUG_CONERAY
    if( output ){
    Point3f dir = currRay.Direction();
    fprintf( fp, "%f %f %f\n", dir[0], dir[1], dir[2] );}
#endif 
  }  
  
#ifdef DEBUG_CONERAY
  // For some reasons it will crash here (memory)
  // I guess GDB wanted some memory to be freed
  // rather than just exiting like this.
  if(output){
      fclose(fp);
  }// exit(0);  
#endif  
}
