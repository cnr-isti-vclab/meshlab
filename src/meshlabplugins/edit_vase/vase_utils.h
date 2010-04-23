#ifndef EDIT_VASE_UTILS_H
#define EDIT_VASE_UTILS_H

#include <common/interfaces.h> // Lots of meshlab stuff
#include <stdarg.h> // vararg processing

namespace vcg{

/// Converts a Point into a string
QString toString( const Point3f& p );
/// Converts a Point into a string
QString toString( const Point3i& p );
/// Clams float values
float myclamp(float val, float low, float high);
/// Draws an opengl box given center and edge length showing only wireframe
void drawBox(Point3f& center, float edgel, bool wireframe=true);
void drawBox(Point3i& center, float edgel, bool wireframe=true);
/// Draws an opengl line in between two points
void drawSegment( Point3f& start, Point3f& stop );
/// Computes baricenter of set of points (BROKEN)
// Point3f baricenter(Point3f* points, ...);
/// Scales a point by "s" isotropically
Point3f myscale( const Point3f& p, float s );
/// Triangle point intersection, slightly different from the method in VCG to return signed distance
float SignedFacePointDistance( CFaceO& f, const Point3f& q );
float SignedFacePointDistance( CFaceO& f, const Point3f& q, Point3f& a );
float SignedFacePointDistance( CFaceO& f,  CMeshO::PerVertexAttributeHandle<Point3f> MCIH, const Point3f& q, const Point3i& qi,Point3f& closest );

} // Namespace vcg
#endif // EDIT_VASE_UTILS_H
