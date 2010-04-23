#include <vase_utils.h>
#include <string>
#include <stdlib.h> //sprintf
#include <vcg/space/triangle3.h>

namespace vcg{


QString toString( const Point3f& p ){
    QString s;
    s.sprintf("%f %f %f", p[0], p[1], p[2]);
    return s;
}
QString toString( const Point3i& p ){
    QString s;
    s.sprintf("%d %d %d", p[0], p[1], p[2]);
    return s;
}

float myclamp(float val, float low, float high) {
    if (val < low) return low;
    else if (val > high) return high;
    else return val;
}
Point3f myscale( const Point3f& p, float s ){
    Point3f scaled;
    scaled[0] = p[0]*s;
    scaled[1] = p[1]*s;
    scaled[2] = p[2]*s;
    return scaled;
}
void drawBox(Point3i& center, float edgel, bool wireframe){
    Point3f _center( center.X(), center.Y(), center.Z() );
    drawBox(_center, edgel, wireframe);
}
// Horribly hardcoded box centered at center with side length edgel
void drawBox(Point3f& center, float edgel, bool wireframe){
    float hedge = edgel/2;
    // Lower Z plane
    if(!wireframe){
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
    }
    else
        glBegin(GL_LINE_LOOP);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]-hedge);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]-hedge);
    glEnd();
    // Upper Z plane
    if(!wireframe)
        glBegin(GL_QUADS);
    else
        glBegin(GL_LINE_LOOP);
        glNormal3f(0, 0, +1);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]+hedge);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]+hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]+hedge);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]+hedge);
    glEnd();
    // -x plane
    if(!wireframe){
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
    }
    else
        glBegin(GL_LINE_LOOP);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]+hedge);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]-hedge);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]+hedge);
    glEnd();
    // +x plane
    if(!wireframe){
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
    }
    else
        glBegin(GL_LINE_LOOP);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]+hedge);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]+hedge);
    glEnd();
    // +y plane
    if(!wireframe){
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
    }
    else
        glBegin(GL_LINE_LOOP);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]+hedge);
        glVertex3f(center[0]-hedge, center[1]+hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]+hedge, center[2]+hedge);
    glEnd();
    // -y plane
    if(!wireframe){
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
    }
    else
        glBegin(GL_LINE_LOOP);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]+hedge);
        glVertex3f(center[0]-hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]-hedge);
        glVertex3f(center[0]+hedge, center[1]-hedge, center[2]+hedge);
    glEnd();
}


float SignedFacePointDistance( CFaceO& f, const Point3f& q ){
    Point3f closest;
    return SignedFacePointDistance( f, q, closest );
}

float SignedFacePointDistance( CFaceO& f,  CMeshO::PerVertexAttributeHandle<Point3f> MCIH, const Point3f& q, const Point3i& qi,Point3f& closest ){
  Point3f vvi[3];
  for(int i=0;i<3;++i)
    vvi[i]=MCIH[f.V(i)];
  // MC generate vertexes that always have 2 integer coord and one floating.
  for(int i=0;i<3;++i) // foreach vertex
  {
    int interceptAxis = -1;
    for(int j=0;j<3;++j) // foreach axis
        if( floor(vvi[i][j]) != vvi[i][j] )
          interceptAxis = j;
    // now check if this vertex and the query point are alinged on the interceptAxis
    bool aligned=true;
    for(int j=0;j<3;++j) // foreach axis
        if(j!=interceptAxis)
          if(vvi[i][j] != qi[j]) aligned=false;

    if(aligned)
    {
      assert(vvi[i][(interceptAxis+1)%3] == qi[(interceptAxis+1)%3]);
      assert(vvi[i][(interceptAxis+2)%3] == qi[(interceptAxis+2)%3]);
      return f.V(i)->P()[interceptAxis]-q[interceptAxis];
    }
  }
  return std::numeric_limits<float>::max();
  }

float SignedFacePointDistance( CFaceO& f, const Point3f& q, Point3f& closest ){
  float dist=std::numeric_limits<float>::max();
    bool ret = face::PointDistanceBase(f,q,dist,closest);
    assert(ret);

    if(((closest-q)*f.cN())>0) dist=-dist;
    return dist;

//    float dist;
//    const Point3f& p0 = f.P(0);
//    const Point3f& p1 = f.P(1);
//    const Point3f& p2 = f.P(2);
//
//    Point3f clos[3];
//    float distv[3];
//    Point3f clos_proj;
//    float distproj;
//    // Point3f closest;
//
//    ///find distance on the plane
//    vcg::Plane3<float> plane;
//    plane.Init(p0,p1,p2);
//    clos_proj=plane.Projection(q);
//
//    ///control if inside/outside
//    Point3f n=(p1-p0)^(p2-p0);
//    Point3f n0=(p0-clos_proj)^(p1-clos_proj);
//    Point3f n1=(p1-clos_proj)^(p2-clos_proj);
//    Point3f n2=(p2-clos_proj)^(p0-clos_proj);
//    distproj=(clos_proj-q).Norm();
//    if (((n*n0)>=0)&&((n*n1)>=0)&&((n*n2)>=0)){
//        closest=clos_proj;
//        dist=distproj;
//        // Determine the sign of the distance, then flip sign if needed
//        Point3f connvect = q-closest; // vector connecting query to proj point
//        if( connvect.dot( f.N() ) < 0 )
//            dist *= -1;
//        return dist;
//    }
//
//    //distance from the edges
//    vcg::Segment3<float> e0=vcg::Segment3<float>(p0,p1);
//    vcg::Segment3<float> e1=vcg::Segment3<float>(p1,p2);
//    vcg::Segment3<float> e2=vcg::Segment3<float>(p2,p0);
//    clos[0]=ClosestPoint<float>( e0, q);
//    clos[1]=ClosestPoint<float>( e1, q);
//    clos[2]=ClosestPoint<float>( e2, q);
//
//    distv[0]=(clos[0]-q).Norm();
//    distv[1]=(clos[1]-q).Norm();
//    distv[2]=(clos[2]-q).Norm();
//    int min=0;
//
//    ///find minimum distance
//    for (int i=1;i<3;i++)
//    {
//        if (distv[i]<distv[min])
//            min=i;
//    }
//
//    closest=clos[min];
//    dist=distv[min];
//
//    // Determine the sign of the distance, then flip sign if needed
//    Point3f connvect = q-closest; // vector connecting query to proj point
//    if( connvect.dot( f.N() ) < 0 )
//        dist *= -1;
//
//    return dist;
}
/// This was buggy... never debugged..
//Point3f baricenter(Point3f* points, ...){
//    va_list ap;
//    Point3f bari;
//    Point3f* p;
//    float count=0;
//    va_start(ap, p);
//    while (p) {
//       bari += *p;
//       count++;
//       p = va_arg(ap, Point3f*);
//    }
//    va_end(ap);
//    return myscale(bari, 1.0/count);
//}
void drawSegment( Point3f& srt, Point3f& end ){
    glBegin(GL_LINES);
        glVertex3f( srt[0], srt[1], srt[2] );
        glVertex3f( end[0], end[1], end[2] );
    glEnd();
}

} // Namespace ::vcg
