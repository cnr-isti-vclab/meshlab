/**
 *
 * Author: Ricardo Marroquim
 *
 * Data created: 2007-11-19
 *
 **/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "surfel.hpp"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <cfloat>

typedef enum 
  {
    PYRAMID_POINTS,
    PYRAMID_POINTS_COLOR,
    PYRAMID_TEMPLATES
  } point_render_type_enum;

using namespace std;

static const Point3f default_color (0.8, 0.4, 0.2);
static const Point3f bg_color (1.0, 1.0, 1.0);
static const Point3f black_color (0.0, 0.0, 0.0);

typedef Surfel<double> Surfeld;
typedef vector<Surfeld>::iterator surfelVectorIter;
typedef vector<Surfeld>::const_iterator surfelVectorIterConst;

class Object
{
 public:
  
  Object() { }
   
  Object(int id_num) : id(id_num)  {}
      
  ~Object();

  void render ( void ) const;

  vector<Surfeld> * getSurfels ( void ) { return &surfels; }

  void clearSurfels ( void );

  int getRendererType ( void ) { return renderer_type; }
  void setRendererType ( int type );

  void setId ( int id_num ) { id = id_num; }
  int getId ( void ) { return id; }

  int numberPoints ( void ) const { return number_points; }

  Point3f eye;

 private:

  void setPyramidPointsDisplayList ( void );
  void setPyramidPointsColorDisplayList ( void );
  void setPyramidPointsArrays( void );
  void setPyramidPointsArraysColor( void );

  void normalizeQuality( void );

  double max_quality, min_quality;

  // Object group identification number.
  int id;
 
  // Rendering type.
  int renderer_type;

  /// Number of samples.
  int number_points;

  GLuint pointsDisplayList;

  // Vector of surfels belonging to this object.
  vector<Surfeld> surfels;

};

#endif
