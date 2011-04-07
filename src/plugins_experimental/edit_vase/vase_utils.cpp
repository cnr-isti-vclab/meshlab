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
void drawSegment( Point3f& srt, Point3f& end ){
    glBegin(GL_LINES);
        glVertex3f( srt[0], srt[1], srt[2] );
        glVertex3f( end[0], end[1], end[2] );
    glEnd();
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
float SignedFacePointDistance( CFaceO& f, const Point3f& q, Point3f& closest ){
  float dist=std::numeric_limits<float>::max();
    bool ret = face::PointDistanceBase(f,q,dist,closest);
    assert(ret);
    if(((closest-q)*f.cN())>0) dist=-dist;
    return dist;
}


} // Namespace ::vcg
