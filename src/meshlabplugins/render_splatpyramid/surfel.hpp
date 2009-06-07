#ifndef SURFEL_HPP_
#define SURFEL_HPP_

/**
 * 
 * Author: Ricardo Marroquim, Felipe Moura
 *
 * Data created: 08-01-2008
 *
 **/

#include <vector>
#include <cmath>

#include <iostream>
#include <wrap/gl/shaders.h>

#include <GL/glew.h>
#include "vcg/space/point3.h"
#include "vcg/space/color4.h"

extern "C" 
{
#include <stdio.h>
#include <strings.h>
}

#include <list>

//#define PI 3.1415926535897932384626433832795

/**
 * Surfel class.
 * A surface element (surfel) is a sample point of the surface.
 * Also known as splats in some implementations.
 * The splat has an elliptical or circular form, and an estimated
 * surface normal at it's center.
 **/


using namespace vcg;

template <class Real > class Surfel
{
public:
		
  typedef std::list<Point3f* >       			ListPtrPoint3;
  typedef typename ListPtrPoint3::iterator  	ListPtrPoint3Iterator;

  Surfel ()
  {
    mCenter = Point3f();
    mNormal = Point3f();
	mQuality = 0.0;
    mSplatRadius = 0.0;
  }
	
  Surfel (const Point3f& position, 
		  const Point3f& normal,
		  const Color4b& color,
		  Real quality,
		  Real radius,
		  unsigned int 	id ) : 	mCenter(position),
								mNormal(normal),
								mColor(color),
								mQuality(quality),
								mSplatRadius(radius),
								mID(id)
  {

  };
	  
	  
  inline const Surfel<Real>& operator= ( const Surfel<Real>& pSurfel)
  {
    this->mCenter    = pSurfel.Center();
    this->mNormal    = pSurfel.Normal(); 
    this->mColor     = pSurfel.Color();
	this->mQuality   = pSurfel.Quality();
	this->mSplatRadius = pSurfel.Radius();
		 
    return ( *this );
  }
	 
  Surfel (const Point3f& position)
  {
    this->mCenter 		= position;
	         
  };	    
	   
  ~Surfel() {};
	
  const Point3f Center () const 
  { 
    return  ( this->mCenter ) ; 
  };
	 
  Real Center(const int axis)  const
  { 
    return ( this->mCenter[axis] ); 
  };
	 
  void SetCenter(const Point3f& pCenter) 
  { 
    this->mCenter = Point3f(pCenter); 
  };
	
  const Point3f Normal(void) const 
  { 
    return (this->mNormal); 
  };
	 
  Real Normal(int axis) const 
  { 
    return ( this->mNormal[axis] ); 
  };
	 
  void SetNormal (const Point3f& normal )
  { 
    this->mNormal = Point3f(normal); 
  };

  unsigned int ID () const 
  { 
    return ( this->mID ); 
  };

  void SetID (unsigned int id) 
  {	
    this->mID = id; 
  };

  const Real Radius (void) const
  { 
    return this->mSplatRadius;
  }

  void SetRadius ( const Real& pRadius ) 
  { 
    this->mSplatRadius = pRadius; 
  };

  const Real Quality (void) const
  { 
    return this->mQuality;
  }

  void SetQuality ( const Real& pQuality )
  { 
    this->mQuality = pQuality;
  };

  Color4b Color (void) const
  { 
    return this->mColor; 
  };

  void SetColor ( const Color4b& pColor ) 
  { 
    this->mColor = pColor; 
  };

	 
  /// I/O operator - output
  inline friend std::ostream& operator << (std::ostream& out, const Surfel &s) 
  {
    out << s.perpendicularError() << " " << s.Center[0] << " " 
	<< s.Center[1] 	<< " " << s.Center[2] << " " 
	<< s.radius() 		<< " " << s.Normal[0] << " " 
	<< s.Normal[1] 	<< " " << s.Normal[2];
	    
    return out;
  };
	 

private:
	 
  /// Point coordinates
  Point3f mCenter;

  /// Estimated surface normal at point sample
  Point3f mNormal;

  /// RGB color
  Color4b mColor;

  /// Quality
  Real mQuality;
	  
  /// Splat radius
  Real mSplatRadius;

  /// Surfel ID
  int mID;

};

#endif
