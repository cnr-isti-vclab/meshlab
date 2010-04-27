#ifndef GRIDACCELL_H
#define GRIDACCELL_H

#include <common/interfaces.h> // Loads of meshlab stuff
#include "vase_utils.h"
#include "volume.h" // Underlying structure of GridAccell

namespace vcg{

// Forward declaration
class MyVolume;

/**
  * Each ray is a triple of elements, the startpoint/direction
  * of the basic ray, the value of the parameter T along
  * the direction that caused an intersection with face f.
  *
  * If we have no intersection, then f=t=0.
  */
class PokingRay{
public:
    Ray3f    ray;
    float      t;
    CFaceO*    f;
    // Looking forward to the next version of C++-Ox, I cannot stand writing this stupid stuff just because this language is a syntactical mess...
    PokingRay(){
        t = +FLT_MAX;
        f = NULL;
    }
};

/**
  * Basic type of pointer for whole class, used to build a template like class
  * without all the syntactic overload of one
  */
typedef PokingRay* PTRTYPE;
typedef std::vector< PTRTYPE > PointerVector;

/**
  * Each volume cell contains a *set* of pointers which
  * might intersect the geometry defined within the same cell
  */
class GridAccell{
public:
    /// Store all rays (TODO: put back to private....)
    std::vector<PokingRay> rays;
private:
    /// Linear memory for volume Vol[x,y,z]
    std::vector<PointerVector> Vol;
    /// Grid sizes
    Point3i sz;
    /// Bounding box
    Box3f bbox;
    /// Padding of the volume
    int padsize;
    /// Real dimension of a voxel
    float delta;

public:
    // Coordinate change
    /// Converts a general position to a voxel index
    void pos2off( const Point3f& pos, Point3i& off ){
        off[0] = pos2off( pos[0], 0 );
        off[1] = pos2off( pos[1], 1 );
        off[2] = pos2off( pos[2], 2 );
    }
    /// Converts a general position to a voxel index
    int pos2off( float p, int dim ){
        int v = round( (p-bbox.min[dim])/delta + padsize-.5 );
        // return myclamp( v, 0, size(dim)-1);
        assert(v>=0 && v<size(dim));
        return v;
    }
    /// Converts a volumetric offset in a 3D space coordinate
    void off2pos( int i, int j, int k, Point3f& pos ){
        pos[0] = off2pos( i, 0 );
        pos[1] = off2pos( j, 1 );
        pos[2] = off2pos( k, 2 );
    }
    /// Converts a volumetric offset in a 3D space coordinate
    void off2pos( Point3i off, Point3f& pos ){
        pos[0] = off2pos( off[0], 0 );
        pos[1] = off2pos( off[1], 1 );
        pos[2] = off2pos( off[2], 2 );
    }
    /// Converts an offset on grid in object space coordinate
    float off2pos( int i, int dim ){
        assert(i>=0 && i<size(dim));
        return (i-padsize+.5)*delta + bbox.min[dim];
    }
    /// Does not allocate memory, refer to Init
    GridAccell(){ sz=Point3i(0,0,0); }
    /// Allocates memory for a grid of the given size
    void init( MyVolume& v, CMeshO& pcloud );
    /// Accessor for memory grid representation
    inline PointerVector& Val( const int x, const int y, const int z ){
        return Vol[x+y*sz[0]+z*sz[0]*sz[1]];
    }
    /// Has this volume been initialized?
    inline bool isInit(){
        return sz[0]>0 && sz[1]>0 && sz[2]>0;
    }
    /// Size of the volume in specific dimension (padding included)
    inline const int size(int dim){ return sz[dim]; }
    /// Renders the intersections
    void render();
    /// Trace a ray in the current volume updating PointerVectors in the traversed cells
    /// Before starting the marching we pull the ray back of the quantity "off", so that
    /// we are able to test for intersections with triangles which are slightly behind
    /// the surface. The default value is not really zero, it gets converted to -Delta.
    void trace_ray(PokingRay& pray, float off=0);

    /// This function scans through the PokingRay array and reset the correspondence information
    /// and the distance information (used to compute a new correspondence)
    void clearCorrespondences();

};

} // Namespace VCG
#endif // GRIDACCELL_H
