#include "volume.h"
#include "myheap.h" // only required by source

using namespace vcg;

void Volume::init( int gridsize, int padsize, vcg::Box3f bbox ){
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

QPixmap& Volume::getSlice(int dim, int slice){
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

void Volume::Set_SEDF( const vcg::Box3f&  inbbox ){
    // Triangulate the bounding box
    if( true ){
        Point3f pos;
        float d;
        float sgn;
        for(int i=0; i<size(0); i++) for(int j=0;j<size(1);j++) for(int k=0;k<size(2);k++){
            // Get 3D coordinate of current voxel & compute distance to bbox
            off2pos( i,j,k, pos );
            d = DistancePoint3Box3( pos, inbbox );
            sgn = inbbox.IsInEx( pos )==true?-1:1;
            grid.Val(i,j,k) = sgn*d;
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
    // Completely fill volume with zeros
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
    else{
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
}

void Volume::isosurface( CMeshO& mesh, float offset ){
    // Run marching cubes on regular lattice
    typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<MyVoxel> >	MyWalker;
    typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
    MyWalker walker;
    MyMarchingCubes	mc(mesh, walker);
    // Extract isoband at "offset" distance
    walker.BuildMesh<MyMarchingCubes>(mesh, this->grid, mc, offset);

    // Rescale the marching cube mesh
    for(CMeshO::VertexIterator vi=mesh.vert.begin();vi!=mesh.vert.end();vi++){
        (*vi).P()[0] = ((*vi).P()[0]-padsize) * delta + bbox.min[0];
        (*vi).P()[1] = ((*vi).P()[1]-padsize) * delta + bbox.min[1];
        (*vi).P()[2] = ((*vi).P()[2]-padsize) * delta + bbox.min[2];
    }
    // Update bounding box and show center
    vcg::tri::UpdateBounding<CMeshO>::Box(mesh);

    // Check whether we are at right position
    // Point3f center = mesh.bbox.Center();
    // qDebug() << "Isosurface center: " << center[0] << " " << center[1] << " " << center[2] << endl;
}

// We need gridaccell only to retrieve the correct indexing
void Volume::updateSurfaceCorrespondence( CMeshO& surf, GridAccell& gridAccell, float DELTA ){
    // Voxels belonging to the active band
    vector< Point3i > band;

    // TODO: do we need a better estimator?
    // we move at most by 1 voxel, so we need to update at least the 2 neighborhood
    // of a voxel on each side so that the implicit function will be correct, this
    // resulting in 2 voxels per side or 2+2+1 per face.
    band.reserve(5*surf.fn);
    MinHeap<float> pq(5*surf.fn);

    // Initialize the exploration queue
    float ONETHIRD=1.0/3.0;
    Point3i o;
    Point3i o1, o2, o3, o4, o5, o6, o7, o8;
    Point3f p1, p2, p3, p4, p5, p6, p7, p8;
    Point3f fcenter;
    for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
        // Compute hash index
        CFaceO& f = *(fi);
        fcenter = f.P(0) + f.P(1) + f.P(2);
        fcenter = myscale( fcenter, ONETHIRD );
        gridAccell.pos2off( fcenter, o );

        // Compute the distance value for the 8 voxel corners
        o1 = Point3i(o[0]+0, o[1]+0, o[2]+0); gridAccell.off2pos(o1, p1);
        // vcg::TrianglePointDistance();
        o2 = Point3i(o[0]+0, o[1]+0, o[2]+1); gridAccell.off2pos(o2, p2);
        o3 = Point3i(o[0]+0, o[1]+1, o[2]+0); gridAccell.off2pos(o3, p3);
        o4 = Point3i(o[0]+0, o[1]+1, o[2]+1); gridAccell.off2pos(o4, p4);
        o5 = Point3i(o[0]+1, o[1]+0, o[2]+0); gridAccell.off2pos(o5, p5);
        o6 = Point3i(o[0]+1, o[1]+0, o[2]+1); gridAccell.off2pos(o6, p6);
        o7 = Point3i(o[0]+1, o[1]+1, o[2]+0); gridAccell.off2pos(o7, p7);
        o8 = Point3i(o[0]+1, o[1]+1, o[2]+1); gridAccell.off2pos(o8, p8);

        // Add data element
        //band.push_back( off );
        //grid.Val( off );
    }

    // Expansion front, whenever a new voxel is met:
    // 1) Compute distance to current element
    // If distance is small enough:
    //  2) add it to the active band (thus inheriting its position as index)
    //  3) add it to the active queue
    while( !pq.empty() ){
        // Point3i curr = pq.top();
        pq.pop();

        // only add untouched voxels outside active band
        if(false){}



    }
}


void Volume::render(){
    Point3f p;
    for(int i=padsize;i<size(0)-padsize;i++)
        for(int j=padsize;j<size(1)-padsize;j++)
            for(int k=padsize;k<size(2)-padsize;k++){
                off2pos(i,j,k,p);
                    gl3DBox(p, .95*delta);
                }
}
QString Volume::toString(int dim, int a){
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
