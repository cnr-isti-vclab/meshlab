#ifndef EDIT_VASE_UTILS_H
#define EDIT_VASE_UTILS_H

#include <common/interfaces.h> // Lots of meshlab stuff

namespace vcg{

/// Converts a Point into a string
QString toString( const Point3f& p );
/// Converts a Point into a string
QString toString( const Point3i& p );
/// Clams float values
float myclamp(float val, float low, float high);
/// Draws an opengl box given center and edge length showing only wireframe
void gl3DBox(Point3f& center, float edgel, bool wireframe=true);
void gl3DBox(Point3i& center, float edgel, bool wireframe=true);
/// Scales a point by "s" isotropically
Point3f myscale( const Point3f& p, float s );
/// Intersection between a ray and the box (PBRT p.179)
} // Namespace vcg
#endif // EDIT_VASE_UTILS_H
