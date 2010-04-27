#include "gridaccell.h"
using namespace vcg;

// Create a grid accell based on the volume
void GridAccell::init( MyVolume& vol, CMeshO& pcloud ){
    assert( vol.isInit() );

    // Init the data structure
    this->sz = vol.getSize();
    this->Vol.clear(); // Delete anything pre-allocated
    this->Vol.resize(sz[0]*sz[1]*sz[2]);
    this->delta = vol.getDelta();
    this->bbox = vol.getBbox();
    this->padsize = vol.getPadding();
    this->rays.clear();
    this->rays.resize(pcloud.vn);

    // Convert Point+Normal into a ray and store it in GridAccell
    int i=0;
    for(CMeshO::VertexIterator vi=pcloud.vert.begin(); vi!=pcloud.vert.end(); ++vi, ++i)
        rays[i].ray = Ray3f( (*vi).P(), (*vi).N().normalized() );

    // Trace EVERY ray in the volume and hash the intersection
    for(int i=0; i<rays.size(); i++){
        // NACTIVEDEBUG
        // if( i!=12 ) continue;
        trace_ray( rays[i] );
    }
}
void GridAccell::trace_ray(PokingRay& pray, float off){
    // We want to go behind the pixel, you better give me a default of 0,
    // a negative number, or a negative number very close to zero if you
    // want to turn it off.
    assert( off<=0 );

    // Point3i ps; pos2off( p, ps );
    // qDebug() << "Tracing: " << toString(p);
    // qDebug() << "Start offset " << toString(ps);

#if 1
    // We want to test intersections "off" distance behind the ray as well
    float rayT = 1e-20;
    Point3f p = pray.ray.P((off==0)?-this->delta:off);
    Point3f d = pray.ray.Direction();
#else
    // Create a ray copy and make a microscopic step in
    // direction d to avoid zero intersect (similar PBRT)
    float rayT = 1e-20;
    Point3f p = pray.ray.P(rayT);
    Point3f d = pray.ray.Direction();
#endif

    // Set up 3D DDA for current ray
    // NOTE: The -.5*delta is the only difference w.r.t. PBRT as they were using a
    // slightly different indexing method. See NextCrossingT[axis] below. Also note
    // that the -.5*delta causes a bug in the sign of NextCrossingT (which should always
    // positive) thus we can correct it by simply taking the absolute value.
    float NextCrossingT[3], DeltaT[3];
    int Step[3], Out[3], Pos[3];
    for (int axis = 0; axis < 3; ++axis) {
        // Compute current voxel for this axis
        Pos[axis] = pos2off( p[axis], axis );
        if (d[axis] >= 0) {
            NextCrossingT[axis] = fabs( rayT + (off2pos(Pos[axis]+1, axis) - p[axis]-.5*delta) / d[axis] );
            DeltaT[axis] = delta / d[axis];
            Step[axis] = 1;
            Out[axis] = size(axis);
        }
        else {
            // Handle ray with negative direction for voxel stepping
            NextCrossingT[axis] = fabs( rayT + (off2pos(Pos[axis], axis) - p[axis]-.5*delta) / d[axis] );
            DeltaT[axis] = -delta / d[axis];
            Step[axis] = -1;
            Out[axis] = -1;
        }
    }
    // Walk ray through voxel grid
    for (;;){
        // Store pointer to the ray in the structure
        Val( Pos[0], Pos[1], Pos[2] ).push_back( &pray );
        // Advance to next voxel
        int bits = ((NextCrossingT[0] < NextCrossingT[1]) << 2) +
                   ((NextCrossingT[0] < NextCrossingT[2]) << 1) +
                   ((NextCrossingT[1] < NextCrossingT[2]));
        const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
        int stepAxis = cmpToAxis[bits];
        // qDebug() << "STEPAXIS" << stepAxis;
        Pos[stepAxis] += Step[stepAxis];
        if (Pos[stepAxis] == Out[stepAxis])
            break;
        NextCrossingT[stepAxis] += DeltaT[stepAxis];
    }
}
void GridAccell::render(){
    // Don't draw not initialized voumes
    if( !isInit() ) return;

    //--- OpenGL stuff
     glDisable(GL_LIGHTING);
    glColor4f(1.0,0.0,0.0,0.1);

    //--- Visit the volume
    glLineWidth(1.0);
    Point3f p(0,0,0);
    for(int i=0;i<size(0);i++)
        for(int j=0;j<size(1);j++)
            for(int k=0;k<size(2);k++)
                if( Val(i,j,k).size() > 0 ){
                    off2pos(i,j,k,p);
                    vcg::drawBox( p, .95*delta, true );
                }

    //--- Post OpenGL stuff
     glEnable(GL_LIGHTING);
}
void GridAccell::clearCorrespondences(){
    for( unsigned int i=0; i<rays.size(); i++ ){
        rays[i].f = NULL;
        rays[i].t = +FLT_MAX;
    }
}
