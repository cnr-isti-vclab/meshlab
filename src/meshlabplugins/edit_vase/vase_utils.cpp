#include <vase_utils.h>
#include <string>
#include <stdlib.h> //sprintf

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
void gl3DBox(Point3i& center, float edgel, bool wireframe){
    Point3f _center( center.X(), center.Y(), center.Z() );
    gl3DBox(_center, edgel, wireframe);
}
// Horribly hardcoded box centered at center with side length edgel
void gl3DBox(Point3f& center, float edgel, bool wireframe){
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

}
