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
            grid.cV(i,j,k).status = 0;
            grid.cV(i,j,k).fdst = 0;
            grid.cV(i,j,k).face = 0;
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

    // Update surface normals
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace( mesh );


    // DEBUG: Update bounding box and print out center
    // vcg::tri::UpdateBounding<CMeshO>::Box(mesh);
    // Check whether we are at right position
    // Point3f center = mesh.bbox.Center();
    // qDebug() << "Isosurface center: " << center[0] << " " << center[1] << " " << center[2] << endl;
}

// We need gridaccell only to retrieve the correct indexing,

void Volume::updateSurfaceCorrespondence( CMeshO& surf, GridAccell& gridAccell, float DELTA ){
    // the array is used to scan a single voxel that contains the triangle in the initialization
    Point3i off[8] = {
        Point3i(0,0,0),
        Point3i(0,0,1),
        Point3i(0,1,0),
        Point3i(0,1,1),
        Point3i(1,0,0),
        Point3i(1,0,1),
        Point3i(1,1,0),
        Point3i(1,1,1) };
    // the array is used to scan the 26-neighborhood
    Point3i off26[26] = {
        // Bottom
        Point3i(-1, -1, -1),
        Point3i(-1,  0, -1),
        Point3i(-1, +1, -1),
        Point3i( 0, -1, -1),
        Point3i( 0,  0, -1),
        Point3i( 0, +1, -1),
        Point3i( 1, -1, -1),
        Point3i( 1,  0, -1),
        Point3i( 1, +1, -1),
        // Center (skip center)
        Point3i(-1, -1,  0),
        Point3i(-1,  0,  0),
        Point3i(-1, +1,  0),
        Point3i( 0, -1,  0),
        Point3i( 0, +1,  0),
        Point3i( 1, -1,  0),
        Point3i( 1,  0,  0),
        Point3i( 1, +1,  0),
        // Top
        Point3i(-1, -1,  1),
        Point3i(-1,  0,  1),
        Point3i(-1, +1,  1),
        Point3i( 0, -1,  1),
        Point3i( 0,  0,  1),
        Point3i( 0, +1,  1),
        Point3i( 1, -1,  1),
        Point3i( 1,  0,  1),
        Point3i( 1, +1,  1),
    };


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
    Point3i o, newo;
    Point3f p, newp;
    float   dist;

    //---------------------------- INITIALIZATION --------------------------
    for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
        // Compute hash index from face center
        CFaceO& f = *(fi);
        p = f.P(0) + f.P(1) + f.P(2);
        p = myscale( p, ONETHIRD );
        gridAccell.pos2off( p, o ); // Convert faces in OBJ space (OK!)
        Point3f fn = f.N();

        // Initialize the exploration queue for this face;
        // compute the distance value for the 8 voxel corners
        // making sure to set the right distance for overlaps
        // Note that we set the flag to 2 (FINISHED) so that these values
        // will never be updated ever again after initialization
        for( int i=0; i<8 /*i<26*/; i++ ){
            // Offset the origin and compute new point
            newo = Point3i(o[0]+off[i][0], o[1]+off[i][1], o[2]+off[i][2]);
            // newo = Point3i(o[0]+off26[i][0], o[1]+off26[i][1], o[2]+off26[i][2]);
            MyVoxel& v = grid.cV(newo[0], newo[1], newo[2]);
            off2pos(newo, newp); // Convert offset to position (OK!)
            dist = vcg::SignedFacePointDistance(f, newp);
            // If has never been touched... insert it
            if( v.status == 0 ){ // && ( tri::Index(surf, f)==35||tri::Index(surf, f)==34) ){
                // Add to active list, save its "band" index
                // and its corresponding face, add it to exploration queue
                v.fdst    = dist;
                v.index   = band.size();
                v.face    = &f;
                v.status  = 2;

                if( newo == Point3i(3,6,5) )
                    qDebug("%d %d %d created!! d=%f %d N=[%f %f %f] ", newo[0], newo[1], newo[2], v.fdst, v.index, fn[0], fn[1], fn[2]);

                pq.push(fabs(v.fdst), v.index);
                band.push_back(newo);


            }
            // If has been inserted already, but an update is needed
            else if( v.status == 2 && fabs(dist) < fabs(v.fdst) ){ // && ( tri::Index(surf, f)==35||tri::Index(surf, f)==34) ){
                //qDebug() << "used to belong to face: " << tri::Index(surf, v.face) <<
                //        " with DST: " << v.fdst << "now chanding it to: " << tri::Index(surf, f) << "with dst: " << dist;

                // Just update its distance value using the key
                v.fdst = dist;
                v.face = &f;
                if( newo == Point3i(3,6,5) )
                    qDebug("%d %d %d updated!! d=%f %d N=[%f %f %f] ", newo[0], newo[1], newo[2], v.fdst, v.index, fn[0], fn[1], fn[2]);
                pq.push(fabs(v.fdst), v.index);
            }
        }
    }

    qDebug() << "with an isosurface of size: " << surf.fn << endl;
    qDebug() << "initial queue size: " << pq.size() << endl;

    // return;

#if 0
    qDebug() << toString(2, 5); // ORIGINAL FUNCTION
    // INITIALIZATION
    for(int i=0; i<size(0); i++) for(int j=0;j<size(1);j++) for(int k=0;k<size(2);k++)
        grid.Val(i,j,k) = NAN;
    for( unsigned int i=0; i<band.size(); i++ ){
        Point3i o = band[i];
        MyVoxel& v = grid.cV(o[0], o[1], o[2]);
        grid.Val(o[0], o[1], o[2]) = v.fdst;
        //if( status != 2 ) qDebug() << (int) status;
    }
    qDebug() << toString(2, 5);
#endif

    // Expansion front, whenever a new voxel is met:
    // If distance is small enough:
    //  2) add it to the active band (thus inheriting its position as index)
    //  3) add it to the active queue
    //  3) add neighbors or update their distances if improved
#if 1
    Point3f neigh_p;
    Point3i neigh_o;
    float debd;
    int indx;
    while( !pq.empty() ){
        // Retrieve current voxel index and pop it
        debd = pq.top().first;
        indx = pq.top().second;
        pq.pop();
        assert( indx < band.size() );
        // qDebug() << "popped " << indx << " dst: " << debd << " queue size: " << pq.size();

        // if( neigh_o == Point3i(3,4,5) ) qDebug() << "POPPED!!";

        // Retrieve voxel & mark it as visited
        newo =  band.at( indx );
        MyVoxel& v = grid.cV(newo[0], newo[1], newo[2]);
        v.status = 2; // Never visit it again
        CFaceO& f = *(v.face); // Parent supporting face


        if( newo == Point3i(4,6,5) || newo == Point3i(3,6,5) )
            qDebug("%d %d %d popped!! d=%f", newo[0], newo[1], newo[2], debd);

        continue;
        // Visit its neighbors and (update | add) them to queue
        for( int i=0; i<26; i++ ){
            // Neighbor offset
            neigh_o = Point3i(newo[0]+off26[i][0], newo[1]+off26[i][1], newo[2]+off26[i][2]);
            MyVoxel& neigh_v = grid.cV(neigh_o[0], neigh_o[1], neigh_o[2]);
            if( neigh_v.status == 2 ) continue; // skip popped areas
            off2pos(neigh_o, neigh_p); // position
            // gridAccell.off2pos(neigh_o, neigh_p); // position
            dist = vcg::SignedFacePointDistance(f, neigh_p );

            if( neigh_o == Point3i(4,6,5) )
                qDebug() << "#---- Guilty update hit from point: " << vcg::toString(newo);

            // In any case never add samples that go beyond the distance value
            // Note that the padding shuld allow for the voxels within this
            // distance to be reached without creating off2pos assertion error.

            // It has never been touched
            if( neigh_v.status == 0 && fabs(dist) < DELTA ){
//                if( neigh_o == Point3i(3,4,5) ){
//                    qDebug() << "first insertion: " << dist ;
//                }

                neigh_v.fdst    = dist; // set distance
                neigh_v.face    = &f; // save face reference
                neigh_v.status  = 1; // mark as inserted
                neigh_v.index   = band.size(); // compute index
                pq.push(fabs(dist), neigh_v.index); // insert it
                band.push_back(neigh_o); // add to active voxels
            }
            // It has been inserted already, but update is needed
            else if( neigh_v.status == 1 && fabs(dist) < fabs(neigh_v.fdst) && fabs(dist) < DELTA ){
                neigh_v.fdst = dist;
                neigh_v.face = &f;
                pq.push(fabs(dist), neigh_v.index);
            }
        }
    }
#endif


#if 0
    //--- DEBUG! make sure flags are correct (if in band => 2)
    // the substitute the value of stored distance with the one
    // computed my marching, then reconstruct suface and substitute
    // the input mesh
    for(int i=0; i<size(0); i++) for(int j=0;j<size(1);j++) for(int k=0;k<size(2);k++)
        grid.Val(i,j,k) = NAN;
    for( unsigned int i=0; i<band.size(); i++ ){
        Point3i o = band[i];
        MyVoxel& v = grid.cV(o[0], o[1], o[2]);
        grid.Val(o[0], o[1], o[2]) = v.fdst;
        //if( status != 2 ) qDebug() << (int) status;
    }
    qDebug() << toString(2, 5);
#endif
    // Test if isosurface looks similar!! (all face references are voided)
    // isosurface( surf, 0 );

    // qDebug()<< "Band extracted!!!" << endl;
    // exit(0);
}

void Volume::render(){
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
