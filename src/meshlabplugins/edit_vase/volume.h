#ifndef VOLUME_H
#define VOLUME_H

#include <vector> //STD vector
#include <QDebug>
#include <QPainter> // Draw slice
#include <common/interfaces.h> // Lots of meshlab stuff

#include <vcg/complex/trimesh/create/mc_trivial_walker.h> // SimpleVolume
#include <vcg/complex/trimesh/create/marching_cubes.h> // Marching cube
#include "gridaccell.h" // Grid ray accellerator
#include "vase_utils.h"


namespace vcg{

// Forward declare
class GridAccell;

/**
  * This is a trivial extension of the class vcg::SimpleVoxel, it does not only
  * hold a field value, but also a pointer to the face responsible for that field
  * value. If a face needs to move (perpendicular to the normal, like in a balloon)
  * then we will be able to retrieve the necessary movement of this field by visiting
  * the corresponding face and querying the variation at that particular location.
  */
class MyVoxel{
    public:
        float   sfield;   // signed distance from surface
        float   field;   // unsigned distance from surface (used for the band updating)
        CFaceO* face;    // corresponding balloon face
        int     status;  // status: {0: untouched, 1: in queue, 2: popped}
        int     index;   // index of MyVoxel in current active band
        /// Set field to zero and NULL the face pointer
        MyVoxel(){
            sfield  = 0;
            field  = 0;
            face   = 0;
            status = 0;
            index  = 0;
        }
        /// Required by marching cubes
        float &V(){
            return sfield;
        }
};
 /**
  * An implicit  volumetric class:
  *
  * There is a fishy relationship in between the indexing used in the volume class and the
  * one used in the gridaccell class. I think it's related to the way in which marching cubes
  * extracts the isosurface. While gridaccell will have the -.5/+.5 correction, the volume
  * needs to work on integer coordinates with no post-correction. In this way the extracted
  * isosurface is perfectly aligned with the ray-grid created by gridaccell. I would like to
  * investigate this particular relationship but the tests show that it work... and that's
  * all I have time for. If anything can be changed, it's within volume... gridaccell should
  * be left alone as the 3D-DDR is highly dependent on the way in which the grid is
  * structured.
  *
  */
class MyVolume{
    private:
        // Internal uniform grid representation
        SimpleVolume<MyVoxel> grid;
        // Size of the underlying grid (voxels)
        Point3i sz;
        // Padding of the volume
        int padsize;
        // Real dimension of a voxel
        float delta;
        // Volume represented by the grid
        vcg::Box3f bbox;
        // Pre allocated 2D matrixes for slicing
        QPixmap  slices_2D[3];      

    public:

        /// Compute volume-to-surface correspondences (stored in MyVoxel::face) in a band around the surface
        /// up to DELTA away (in object space) inserting the volumetric indexes in the band vector.
        /// Used by initField(CMeshO&, accell)
        void updateSurfaceCorrespondence( CMeshO& balloon_mesh, GridAccell& gridAccell, float DELTA );

        /// Indexes of current band
        std::vector<Point3i> band;

        // Constructors / Factories
        /// Empty, refer to Init(...)
        MyVolume(){ sz=Point3i(0,0,0); }
        /// Initialize the volume
        void init( int gridsize, int padsize, vcg::Box3f bbox );
        /// Retrieves a 2D (image) slice from the volume
        QPixmap& getSlice(int dim, int slice);
        /// Set SEDF according to continuous function
        void initField( const vcg::Box3f&  bbox );
        /// Set SEDF so that MC woudl give "surface" as output
        void initField( CMeshO& surface, GridAccell& accell );
        /// Computes isosurface of current volume
        void isosurface( CMeshO& balloon_mesh, float offset=0 );
        /// Render every cube just for illustration
        void render();
        /// Check sample in range
        bool checkRange( Point3i newo ){
            assert( newo[0] >= 0 && newo[0] < size(0) );
            assert( newo[1] >= 0 && newo[1] < size(1) );
            assert( newo[2] >= 0 && newo[2] < size(2) );
        }


        // Getters
        float getDelta() const{ return delta; }
        vcg::Box3f getBbox() const{ return bbox; }
        Point3i getSize() const{ return sz; }
        int getPadding() const{ return padsize; };

        /// Has this volume been initialized?
        inline bool isInit(){
            return size(0)>0 && size(1)>0 && size(2)>0;
        }

        // Access methods (for assignments and similar)
        MyVoxel& Voxel(const Point3i off){
            return Voxel( off[0], off[1], off[2] );
        }
        MyVoxel& Voxel(const int &x,const int &y,const int &z){
            return grid.V( x, y, z );
        }
        float &Val(const Point3i off){
            return Val( off[0], off[1], off[2] );
        }
        float &Val(const int &x,const int &y,const int &z) {
            return grid.Val(x,y,z);
        }

        // Info functions
        /// Is the offset (no rounding) within the bounds of the grid (+pad)
        bool isIn( const Point3f& off ){
            Point3i ofi( round(off[0]), round(off[1]), round(off[2]) );
            return ofi[0]>=0 && ofi[0]<size(0) &&
                   ofi[1]>=0 && ofi[1]<size(1) &&
                   ofi[2]>=0 && ofi[2]<size(2);
        }
        const Point3i& size(){ return grid.ISize(); }
        inline int size(int dim) { return sz[dim]; }
        /// Converts a general position to a voxel index
        void pos2off( const Point3f& pos, Point3i& off ){
            off[0] = pos2off( pos[0], 0 );
            off[1] = pos2off( pos[1], 1 );
            off[2] = pos2off( pos[2], 2 );
        }
        /// Converts a general position to a voxel index
        int pos2off( float p, int dim ){
            int v = round( (p-bbox.min[dim])/delta + padsize);
            return myclamp( v, 0, size(dim));
        }
        /// Converts a volumetric offset in a 3D space coordinate
        void off2pos( const Point3i& off, Point3f& pos ){
            pos[0] = off2pos( off[0], 0 );
            pos[1] = off2pos( off[1], 1 );
            pos[2] = off2pos( off[2], 2 );
        }
        /// Converts a volumetric offset in a 3D space coordinate
        void off2pos( int i, int j, int k, Point3f& pos ){
            pos[0] = off2pos( i, 0 );
            pos[1] = off2pos( j, 1 );
            pos[2] = off2pos( k, 2 );
        }
        /// Converts an offset on grid in object space coordinate
        float off2pos( int i, int dim ){
            return (i-padsize)*delta + bbox.min[dim];
        }
        /// Returns the bounding box
        const vcg::Box3f& get_bbox(){
            return this->bbox;
        }
        /// Returns a string representing the content of the volume at
        /// a particular slice along dimension dim
        QString toString(int dim=2, int a=5);
};

} // end namespace vcg
#endif // VOLUME_H
