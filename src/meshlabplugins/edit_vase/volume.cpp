#include "volume.h"
#include "myheap.h" // only required by source
#include "../filter_plymc/plymc.h" // remove bad triangles from marching cubes

using namespace vcg;
/// the array is used to scan a single voxel that contains the triangle in the initialization
const Point3i off[8] = { Point3i(0,0,0), Point3i(0,0,1), Point3i(0,1,0), Point3i(0,1,1),
                                Point3i(1,0,0), Point3i(1,0,1), Point3i(1,1,0), Point3i(1,1,1) };
// the array is used to scan the 26-neighborhood
const Point3i off26[26] = { Point3i(-1, -1, -1), Point3i(-1,  0, -1), Point3i(-1, +1, -1),
                                   Point3i( 0, -1, -1), Point3i( 0,  0, -1), Point3i( 0, +1, -1),
                                   Point3i( 1, -1, -1), Point3i( 1,  0, -1), Point3i( 1, +1, -1),
                                   Point3i(-1, -1,  0), Point3i(-1,  0,  0), Point3i(-1, +1,  0),
                                   Point3i( 0, -1,  0), /*   skip center */  Point3i( 0, +1,  0),
                                   Point3i( 1, -1,  0), Point3i( 1,  0,  0), Point3i( 1, +1,  0),
                                   Point3i(-1, -1,  1), Point3i(-1,  0,  1), Point3i(-1, +1,  1),
                                   Point3i( 0, -1,  1), Point3i( 0,  0,  1), Point3i( 0, +1,  1),
                                   Point3i( 1, -1,  1), Point3i( 1,  0,  1), Point3i( 1, +1,  1) };

void MyVolume::init( int gridsize, int padsize, vcg::Box3f bbox ){
    // Extract length of longest edge
    int maxdimi = bbox.MaxDim();
    Point3f dim = bbox.Dim();
    float maxdim = dim[maxdimi];         // qDebug() << "MaxDim: " << maxdim;
    this->delta = maxdim / ( gridsize ); // qDebug() << "Delta: " << delta;
    this->padsize = padsize;             // qDebug() << "Padsize: " << padsize;
    this->bbox = bbox;                   // qDebug() << "bbox_m: " << bbox.min[0] << " " << bbox.min[1] << " " << bbox.min[2];

    // Debug Stuff
    if( false ){
        Point3i offmin, offmax;
        pos2off( bbox.min, offmin );
        pos2off( bbox.max, offmax );
        qDebug() << "Imin: (" << offmin[0] << " " << offmin[1] << " " << offmin[2] <<")";
        qDebug() << "Imax: (" << offmax[0] << " " << offmax[1] << " " << offmax[2] <<")";
    }

    // Initialize the grid (uniform padding)
    int dimx = ceil( gridsize * bbox.DimX() / maxdim ) + 2*padsize;
    int dimy = ceil( gridsize * bbox.DimY() / maxdim ) + 2*padsize;
    int dimz = ceil( gridsize * bbox.DimZ() / maxdim ) + 2*padsize;

    this->sz = Point3i( dimx, dimy, dimz );
    grid.Init( this->sz );

    // Brute force initialize the grid... VCG doesn't have much...
    for(int i=0;i<size(0);i++)
        for(int j=0;j<size(1);j++)
            for(int k=0;k<size(2);k++){
                grid.Val(i,j,k) = 0;
            }

    // Allocate Slices & Painters
    slices_2D[0] = QPixmap( dimz, dimy );
    slices_2D[1] = QPixmap( dimx,dimz );
    slices_2D[2] = QPixmap( dimx,dimy );
}

QPixmap& MyVolume::getSlice(int dim, int slice){
    assert( dim>=0 && dim<3 );

    float max_val = 0;
    for(int i=0; i<size(0); i++)
        for(int j=0; j<size(1); j++)
            for(int k=0;k<size(2);k++)
                max_val = std::max( grid.Val(i,j,k), max_val );

    if( max_val== 0 )
        max_val = 1;

    // Create a painter for the dimension
    QPainter painter( &slices_2D[dim]);
    // Retrieve appropriate values
    switch( dim ){
        case 0:
            for(int j=0; j<size(1); j++)
                for(int k=0;k<size(2);k++){
                    float val = fabs( grid.Val(slice,j,k) ) / max_val * 255;
                    val = val<0?0:val;
                    val = val>255?255:val;
                    painter.setPen( QColor(val,val,val) );
                    painter.drawPoint(k,j);
                }
            break;
        case 1:
            for(int i=0; i<size(0); i++)
                for(int k=0;k<size(2);k++){
                    float val = fabs( grid.Val(i,slice,k) ) / max_val * 255;
                    val = val<0?0:val;
                    val = val>255?255:val;
                    painter.setPen( QColor(val,val,val) );
                    painter.drawPoint(i,k);
                }
            break;
        case 2:
            for(int i=0; i<size(0); i++)
                for(int j=0;j<size(1);j++){
                    float val = fabs( grid.Val(i,j,slice) ) / max_val * 255;
                    val = val<0?0:val;
                    val = val>255?255:val;
                    painter.setPen( QColor(val,val,val) );
                    // Flip image upside down
                    painter.drawPoint(i,size(1)-j-1);
                }
            break;
    }
    return slices_2D[dim];
}

void MyVolume::initField( const vcg::Box3f&  inbbox ){
    // Triangulate the bounding box
    if( true ){
        qDebug() << "constructing EDF of box: [" << vcg::toString(inbbox.min) << " " << vcg::toString(inbbox.max) << "]";
        Point3f pos;
        float d;
        float sgn;
        for(int i=0; i<size(0); i++) for(int j=0;j<size(1);j++) for(int k=0;k<size(2);k++){
            // Get 3D coordinate of current voxel & compute distance to bbox
            off2pos( i,j,k, pos );
            d = DistancePoint3Box3( pos, inbbox );
            sgn = inbbox.IsInEx( pos )==true?-1:1;
            //if(d<2.0*delta)
              grid.Val(i,j,k) = sgn*d;
            //else
            //  grid.Val(i,j,k) = NAN;
            grid.V(i,j,k).status = 0;
            grid.V(i,j,k).face = 0;
        }
    }
    // Completely fill volume with zeros
    else if( false ){
        for(int i=0; i<size(0); i++)
            for(int j=0;j<size(1);j++)
                for(int k=0;k<size(2);k++){
            grid.Val(i,j,k) = 0;
        }
    }
    // Fill sub-portion of volume
    else if( false ){
        for(int i=0; i<size(0); i++)
            for(int j=0;j<size(1);j++)
                for(int k=0;k<size(2);k++)
                    if( i>=2 && i<6 &&
                        j>=2 && j<6 &&
                        k>=2 && k<6 )
                        grid.Val(i,j,k) = -1;
                    else
                        grid.Val(i,j,k) = +1;
    }
    else if( false){
        // Radius in object space
        double r = 1;
        Point3f p;
        Point3f center = bbox.Center();
        qDebug() << "Sphere in: " << center[0] << " " << center[1] << " " << center[2] << endl;
        center[1] = 0;
        for(int i=0;i<size(0);i++)
            for(int j=0;j<size(1);j++)
                for(int k=0;k<size(2);k++){
                    // implicit sphere equation
                    off2pos(i,j,k,p);
                    p -= center;
                    // Cylinder along Y (vertical)
                    grid.Val(i,j,k) = p[0]*p[0] + p[2]*p[2] + p[1]*p[1] - r*r;
                }
    }
    else{
        // Void all volume
        for(int i=0;i<size(0);i++) for(int j=0;j<size(1);j++) for(int k=0;k<size(2);k++)
            grid.Val(i,j,k) = NAN;
        // Radius in object space
        double r = 1.1;
        Point3f p;
        Point3f center = bbox.Center();
        qDebug() << "Cylinder along Z: " << center[0] << " " << center[1] << " " << center[2] << endl;
        center[1] = 0;

        for(int i=0;i<size(0);i++)
            for(int j=0;j<size(1);j++)
                for(int k=getPadding();k<size(2)-getPadding()+1;k++){
                    // implicit cylinder equation
                    off2pos(i,j,k,p);
                    p -= center;
                    // Cylinder along Y (vertical)
                    grid.Val(i,j,k) = p[0]*p[0] + p[1]*p[1] - r*r;
                }
    }
}
void MyVolume::initField( CMeshO& surface, GridAccell& accell ){
    //--- Extract a new iso-surface, which linearly approximates surface in a marching cube-sense
    isosurface( surface, 0 );

    //--- Clear the current band
    for(unsigned int i=0; i<band.size(); i++){
        Point3i& voxi = band[i];
        MyVoxel& v = Voxel(voxi);
        v.status = 0;
        v.face = 0;
        v.index = 0;
        v.field = NAN;
        v.sfield = NAN;
    }
    band.clear();

    //--- Compute active band distances around surface and correspondences
    // Memory estimation: we move at most by 1 voxel, so we need to update at least the 2 neighborhood
    // of a voxel on each side so that the implicit function will be correct, this resulting in 2 voxels
    // per side thus: 2+2+1 per face.
    const float DELTA = 2*getDelta();
    band.reserve(5*surface.fn);
    updateSurfaceCorrespondence( surface, accell, DELTA );
}

void MyVolume::isosurface( CMeshO& mesh, float offset ){
    // Run marching cubes on regular lattice
    typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<MyVoxel> >	MyWalker;
    typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
    MyWalker walker;
    MyMarchingCubes	mc(mesh, walker);
    // Extract isoband at "offset" distance
    walker.BuildMesh<MyMarchingCubes>(mesh, this->grid, mc, offset);
    // add an additional attribute for storing the original grid coordinate.
    // so it is easy to detect for each vertex
    CMeshO::PerVertexAttributeHandle<Point3f> MCIH;
    if(!vcg::tri::HasPerVertexAttribute (mesh,std::string("OrigCoord")))
          MCIH = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3f>  (mesh,std::string("OrigCoord"));
    else  MCIH = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>  (mesh,std::string("OrigCoord"));

    // Rescale the marching cube mesh
    for(CMeshO::VertexIterator vi=mesh.vert.begin();vi!=mesh.vert.end();vi++){
        MCIH[vi]=(*vi).P(); // save the original coords
        (*vi).P()[0] = ((*vi).P()[0]-padsize) * delta + bbox.min[0];
        (*vi).P()[1] = ((*vi).P()[1]-padsize) * delta + bbox.min[1];
        (*vi).P()[2] = ((*vi).P()[2]-padsize) * delta + bbox.min[2];
    }

    //--- Remove slivers which might crash the whole system (reuses the one defined by plymc)
    // Paolo: il parametro perc va dato in funzione della grandezza del voxel se gli dai come
    // perc: perc=voxel.side/4 sei safe
    mesh.vert.EnableMark();
    mesh.vert.EnableVFAdjacency();
    mesh.face.EnableVFAdjacency();
    tri::UpdateTopology<CMeshO>::VertexFace( mesh );
    tri::Simplify( mesh, getDelta()/4 );
    // tri::Simplify( mesh, getDelta()/16 );

    //--- The simplify operation removed some vertices
    tri::Allocator<CMeshO>::CompactVertexVector( mesh );
    tri::Allocator<CMeshO>::CompactFaceVector( mesh );

    //--- Update surface normals
    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized( mesh );
    //--- Face orientation, needed to have more robust face distance tests
    tri::UpdateFlags<CMeshO>::FaceProjection(mesh);
}

/// gridaccell is needed only to retrieve the correct face=>voxel index
void MyVolume::updateSurfaceCorrespondence( CMeshO& surf, GridAccell& gridAccell, float DELTA ){
    // The capacity of the band is estiamted to be enough to reach DELTA away
    MinHeap<float> pq( band.capacity() );

    //--- INITIALIZATION
    Point3i o, newo;
    Point3f p, newp;
    float   dist;
    CMeshO::PerVertexAttributeHandle<Point3f> MCIH = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>  (surf,std::string("OrigCoord"));
    assert(vcg::tri::HasPerVertexAttribute(surf,std::string("OrigCoord")));

    for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
        // Compute hash index from face center
        CFaceO& f = *(fi);
        p = Barycenter( f );
        // ADEBUG: skip invalid samples
        if( math::IsNAN(p[0]) || math::IsNAN(p[1]) || math::IsNAN(p[2]) )
            continue;
        gridAccell.pos2off( p, o ); // Convert faces in OBJ space (OK!)

        // Initialize the exploration queue for this face: compute the distance value for the 8 corners of the voxel containing
        // the face making sure to set the right distance when a corner is covered by more than one face. Note that we set the flag
        // to 2 (FINISHED) so that these values will never be inserted again in the queue after initialization
        for( int i=0; i<8; i++ ){
            // Offset the origin and compute new point
            newo = Point3i(o[0]+off[i][0], o[1]+off[i][1], o[2]+off[i][2]);
            MyVoxel& v = grid.V(newo[0], newo[1], newo[2]);
            off2pos(newo, newp); // Convert offset to position (OK!)
            float sdist = vcg::SignedFacePointDistance(f, newp) ;
            dist = fabs(sdist);
            // If has never been touched... insert it
            if( v.status == 0 ){
                v.field   = dist;
                //if(math::IsNAN(v.sfield)) v.sfield = sdist;
                v.index   = band.size();
                v.face    = &f;
                v.status  = 2;
                pq.push(dist, v.index);
                band.push_back(newo);
            }
            // It has been inserted already, but an update is needed
            else if( v.status == 2 && dist < v.field ){
                v.field = dist;
                //v.sfield = sdist;
                v.face = &f;
                pq.push(dist, v.index);
            }
        }
    }

    //--- EVOLUTION
    // Expansion front, whenever a new voxel is met, If distance is small enough:
    //  1) add it to the active band (thus inheriting its position as index)
    //  2) add neighbors to queue or update their distances if has improved
    Point3f neigh_p;
    Point3i neigh_o;
    float debd;
    int indx;
    while( !pq.empty() ){
        //--- Retrieve current voxel index and pop it
        debd = pq.top().first;
        indx = pq.top().second;
        if( indx>band.size() ){
            qDebug("bindex %d, bandsz %d", indx, band.size());
            assert( indx < band.size() );
        }
        pq.pop();

        //--- Retrieve voxel & mark it as visited, also compute the real signed distance
        // and set it in the voxel (fast marching used unsigned distance)
        newo =  band.at( indx );
        MyVoxel& v = grid.V(newo[0], newo[1], newo[2]);
        v.status = 2; // Never visit it again
        CFaceO& f = *(v.face); // Parent supporting face
        off2pos(newo, newp);
        v.field = fabs(vcg::SignedFacePointDistance(f, newp));
        if(math::IsNAN(v.sfield)) v.sfield= vcg::SignedFacePointDistance(f, newp);

        //--- Visit its neighbors and (update | add) them to queue
        for( int i=0; i<26; i++ ){
            // Neighbor offset
            neigh_o = Point3i(newo[0]+off26[i][0], newo[1]+off26[i][1], newo[2]+off26[i][2]);
            MyVoxel& neigh_v = grid.V(neigh_o[0], neigh_o[1], neigh_o[2]);
            if( neigh_v.status == 2 ) continue; // skip popped areas
            off2pos(neigh_o, neigh_p);
            dist = fabs( vcg::SignedFacePointDistance(f, neigh_p ) );

            // Never add samples that go beyond the distance value. Note that the padding
            // shuld allow for the voxels within this distance to be reached without creating
            // off2pos assertion error.

            // Only if it has never been touched, also, Never add samples that go beyond the
            // DELTA distance value. Note that the padding shuld allow for the voxels within
            // this distance to be reached without creating off2pos assertion error.
            if( neigh_v.status == 0 && dist < DELTA ){
                neigh_v.field   = dist; // set distance
                neigh_v.face    = &f; // save face reference
                neigh_v.status  = 1; // mark as inserted
                neigh_v.index   = band.size(); // compute index
                pq.push(dist, neigh_v.index); // insert it
                band.push_back(neigh_o); // add to active voxels
            }
            // It has been inserted already, but update is needed
            else if( neigh_v.status == 1 && dist < neigh_v.field && dist < DELTA ){
                neigh_v.field = dist; // set new distance
                neigh_v.face  = &f; // save face reference
                pq.push(dist, neigh_v.index); // add to active voxels
            }
        }
    }


}

void MyVolume::render(){
    if( !this->isInit() ) return;

    qDebug() << "Volume::render()";
    Point3f p;
    MyVoxel v;
    glDisable(GL_LIGHTING);
    int padsize = 0;
    for(int i=padsize;i<size(0)-padsize;i++)
        for(int j=padsize;j<size(1)-padsize;j++)
            for(int k=padsize;k<size(2)-padsize;k++){
                v = grid.cV(i,j,k);
                off2pos(i,j,k,p);
                switch( v.status ){
                    case 0: glColor3f(1.0, 0.0, 0.0); break;
                    case 1: glColor3f(0.0, 1.0, 0.0); break;
                    case 2: glColor3f(0.0, 0.0, 1.0); break;
                }
                vcg::drawBox(p, .95*delta);
            }
    glEnable(GL_LIGHTING);
}
QString MyVolume::toString(int dim, int a){
    assert(dim==2); // code below only slices along z

    QString ret;
    ret.append("\n");
    for(int j=size(1)-1; j>=0; j--){
        QString column;
        for(int i=0; i<size(0); i++){ //scan row first
            QString num;
            num.sprintf("%+10.2f ", grid.Val(i,j,a));
            column.append(num);
        }
        ret.append(column);
        ret.append("\n");
    }
    return ret;
}
