/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2007                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *   
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/
/****************************************************************************
  History

$Log: not supported by cvs2svn $
Revision 1.1  2007/10/18 08:52:06  benedetti
Initial release.


****************************************************************************/

#include <QtGui>
#include "glarea.h"
#include <wrap/qt/trackball.h>

#include <vcg/math/histogram.h>

//---------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "libfreenect.h"
#include "freenect.h"

#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>
#include <wrap/io_trimesh/export_ply.h>


using namespace vcg;
class CFace;
class CVertex;

GLuint gl_depth_tex,gl_rgb_tex;
bool snapshot = false;
bool pick_depth = false;
 int i_pick,j_pick;
uint16_t t_gamma[2048];
unsigned int n_frames = 0;
unsigned int last_time_on[640*480];
uint16_t last_value_on[640*480];
vcg::Point3f normals[640*480];
vcg::Point3f points[640*480];
uint16_t gl_depths[100][640*480];
uint32_t gl_depths_sum[640*480];
unsigned int curs = 0;

const float ToZ(float  v){return 0.1236 *  tan(v / 2842.5 + 1.1863);
}

vcg::Point3f P( int i,  int j){
    vcg::Point3f p;
    p[2] = ToZ(gl_depths_sum[640*j+i]/100.f);
    if( (p[2]>5.0) ||(p[2]< 0.5))
        p[2]=6.0;
    p[0] = (i - 640 / 2) * p[2] * 0.00186;
    p[1] = (j - 480 / 2) * p[2] * 0.00184;
    return p;
}

vcg::Point3f N(int i,  int j){
    vcg::Point3f n;
    vcg::Point3f p = points[j*640+i];
    vcg::Point3f p0 = (points[j*640+i+1]-p).Normalize();
    vcg::Point3f p1 = (points[(j+1)*640+i]-p).Normalize();
    n = (p0^p1).Normalize();
    return n;
//    n= vcg::Point3f(ToZ(kinect_qt::gl_depth_front[640*j+i+1]) - ToZ(kinect_qt::gl_depth_front[640*j+i-1]),
//                    ToZ(kinect_qt::gl_depth_front[640*(j+1)+i]) -ToZ(kinect_qt::gl_depth_front[640*(j+1)+i])  ,
//                    0.002);
//    n.Normalize();
//    return n;
}

void GLArea::paintGL()
{
        n_frames++;
         kinect_qt::qbackbuf_mutex.lock();

        while ( kinect_qt::got_frames < 2) {
                kinect_qt::qwait.wait(& kinect_qt::qbackbuf_mutex);
        }

        memcpy( kinect_qt::gl_rgb_front,  kinect_qt::gl_rgb_back, sizeof( kinect_qt::gl_rgb_back));

        for(unsigned int i = 0; i < 640*480;++i)
            gl_depths_sum[i] -= gl_depths[curs][i];

        memcpy( gl_depths[curs],  kinect_qt::gl_depth_front, sizeof( kinect_qt::gl_depth_front));

        for(unsigned int i = 0; i < 640*480;++i)
            gl_depths_sum[i] += gl_depths[curs][i];

        curs=(curs+1)%100;

        if(pick_depth){
            printf("depth %d \n",kinect_qt::gl_depth_front[640*j_pick+i_pick]);
            vcg::Point3f p = P(i_pick,480-j_pick);
            printf("p3: %f %f %f \n",p[0],p[1],p[2]);

            pick_depth = false;
        }
        if(1)   {
            for(unsigned int i = 0; i < 640;++i)
                for(unsigned int j = 0; j < 480;++j){
                    if(kinect_qt::gl_depth_front[640*j +i ]>=2047)
                    {
                        if(n_frames - last_time_on[j*640+i]< 15)
                            kinect_qt::gl_depth_front[640*j +i ] = last_value_on[j*640+i];
                    }else{
                        last_value_on[j*640+i] = kinect_qt::gl_depth_front[640*j +i ];
                        last_time_on[j*640+i] = n_frames;
                    }

            }
        }
        vcg::Color4b im[640*480];
        for (unsigned int i=0; i<FREENECT_FRAME_PIX; i++) {
                int pval = t_gamma[kinect_qt::gl_depth_front[i]];
                int lb = pval & 0xff;
                switch (pval>>8) {
                        case 0:
                                im[i][0] = 255;
                                im[i][1] = 255-lb;
                                im[i][2] = 255-lb;
                                break;
                        case 1:
                                im[i][0] = 255;
                                im[i][1] = lb;
                                im[i][2] = 0;
                                break;
                        case 2:
                                im[i][0] = 255-lb;
                                im[i][1] = 255;
                                im[i][2] = 0;
                                break;
                        case 3:
                                im[i][0] = 0;
                                im[i][1] = 255;
                                im[i][2] = lb;
                                break;
                        case 4:
                                im[i][0] = 0;
                                im[i][1] = 255-lb;
                                im[i][2] = 255;
                                break;
                        case 5:
                                im[i][0] = 0;
                                im[i][1] = 0;
                                im[i][2] = 255-lb;
                                break;
                        default:
                                im[i][0] = 0;
                                im[i][1] = 0;
                                im[i][2] = 0;
                                break;
                }
        }
        for(unsigned int i = 0; i < 640;++i)
            for(unsigned int j = 0; j< 480;++j){
            points[640*j+i] = P(i,480-j);
        }
        for(unsigned int i = 0; i < 640;++i)
            for(unsigned int j = 0; j< 480;++j){
            normals[640*j+i] = N(i,j);
        }

        kinect_qt::qbackbuf_mutex.unlock();
        kinect_qt:: got_frames = 0;


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
if(0)
{

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
        vcg::Color4b c;

   //     glPixelTransferf(GL_RED_SCALE,32.0);
//        glTexImage2D(GL_TEXTURE_2D, 0, 1, 640, 480, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT,  kinect_qt::gl_depth_front);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE,  im);
        glPixelTransferf(GL_RED_SCALE,1.0);

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
        glTexCoord2f(0, 0);  glVertex3f(-1,1,0);
        glTexCoord2f(1, 0); glVertex3f(0,1,0);
        glTexCoord2f(1, 1); glVertex3f(0,-1,0);
        glTexCoord2f(0, 1);glVertex3f(-1,-1,0);
        glEnd();

}// colored
else
{

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glColor3f(1,1,1);
    glViewport(0,0,640,480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float w = 0.3;
    glFrustum( w,-w, w/1.5, -w/1.5,0.5,6.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,0,0,0,1,0,1,0);
//    glPointSize(2.0);
    glBegin(GL_POINTS);

    for(unsigned int i = 0; i < 640*480;++i){
            glNormal(normals[i]);
            glVertex(points[i]);
    }
    glEnd();

}
{
        glEnable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glViewport(0,0,1280,480);
        glBindTexture(GL_TEXTURE_2D,  gl_rgb_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE,  kinect_qt::gl_rgb_front);

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
        glTexCoord2f(0, 0);  glVertex3f(0,1,0);
        glTexCoord2f(1, 0); glVertex3f(1,1,0);
        glTexCoord2f(1, 1); glVertex3f(1,-1,0);
        glTexCoord2f(0, 1);glVertex3f(0,-1,0);
        glEnd();
    }
        if(snapshot){
            snapshot = false;
            CMesh m;
            CMesh::VertexIterator vi = vcg::tri::Allocator<CMesh>::AddVertices(m,640*480);
            for(unsigned int i = 0; vi != m.vert.end(); ++vi,++i){

                (*vi).N() = normals[i];
                (*vi).P() = points[i];
                //if((*vi).P()[2] <0)
//                if(n_frames - last_time_on[i]>3)
//                    vcg::tri::Allocator<CMesh>::DeleteVertex(m,*vi);
            }

            // discard high rms
//            float rms = 0;

                for(unsigned int i = 0; i < 640*480;++i){
//                    vcg::Distribution<float> d;
                       float vmin = 2047,vmax=0;
                    for(unsigned int c = 0; c < 100;++c) {
                        vmin=std::min<float>(vmin,gl_depths[c][i]);
                        vmax=std::max<float>(vmax,gl_depths[c][i]);
                         }
//                            d.Add(gl_depths[c][i]);
//                    rms = d.RMS();
//                    if(rms>500)
                       if((vmax-vmin)>20)
                            vcg::tri::Allocator<CMesh>::DeleteVertex(m,m.vert[i]);
                }

            vcg::tri::UpdateBounding<CMesh>::Box(m);
            vcg::tri::io::ExporterPLY<CMesh>::Save(m,"out.ply",vcg::tri::io::Mask::IOM_VERTNORMAL |vcg::tri::io::Mask::IOM_VERTCOORD);
        }

}

//---------------------------------------------
GLArea::~GLArea(){
     kinect_qt::kinect_thread.terminate();
}
GLArea::GLArea (QWidget * parent)
          :QGLWidget (parent)
{
	drawmode= SMOOTH;
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(50);
}


void GLArea::initializeGL ()
{
  glClearColor(0, 0, 0, 0); 
  glEnable(GL_BLEND);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glGenTextures(1, &gl_depth_tex);
  glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &gl_rgb_tex);
  glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  static bool first = true;
  if(first){
      first = false;
      kinect_qt::start_kinect();
}

  int i;
  for (i=0; i<2048; i++) {
          float v = i/2048.0;
          v = powf(v, 3)* 6;
          t_gamma[i] = v*6*256;
  }
  for (i=0; i<640*480; i++) {
      last_time_on[i]=0;
      last_value_on[i]=2049;
  }
}

void GLArea::resizeGL (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  initializeGL();
 }

void GLArea::keyReleaseEvent (QKeyEvent * e)
{
  e->ignore ();
  if (e->key () == Qt::Key_Control)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::ControlModifier));
  if (e->key () == Qt::Key_Shift)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
  if (e->key () == Qt::Key_Alt)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::AltModifier));
  updateGL ();
}

void GLArea::keyPressEvent (QKeyEvent * e)
{
  e->ignore ();
  if (e->key () == Qt::Key_Control)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::ControlModifier));
  if (e->key () == Qt::Key_Shift)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
  if (e->key () == Qt::Key_Alt)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::AltModifier));

  updateGL ();
}

void GLArea::mousePressEvent (QMouseEvent * e)
{
    e->accept();
    pick_depth = true;
    i_pick = e->x();
    j_pick = e->y();
    snapshot = true;
//  e->accept ();
//  setFocus ();
//  track.MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
//  updateGL ();
}

void GLArea::mouseMoveEvent (QMouseEvent * e)
{
//  if (e->buttons ()) {
//    track.MouseMove (e->x (), height () - e->y ());
//    updateGL ();
//  }
}

void GLArea::mouseReleaseEvent (QMouseEvent * e)
{
//  track.MouseUp (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
//  updateGL ();
}

void GLArea::wheelEvent (QWheelEvent * e)
{
//  const int WHEEL_STEP = 120;
//  track.MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
//  updateGL ();
}
