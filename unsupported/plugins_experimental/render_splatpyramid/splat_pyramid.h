/**
 * Header file for application.cc
 *
 * PyramidPointRendererPlugin interface independent layer
 * 
 * Author: Ricardo Marroquim
 *
 * Data created: 20-12-07
 *
 **/

#ifndef SPLATPYRAMIDPLUGIN_H
#define SPLATPYRAMIDPLUGIN_H

//OpenGL and GLUT includes
// Standard headers
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <cassert>

using namespace std;

//IO includes
#include <sstream>
#include <iomanip>
#include <cmath>

#include <list>
#include <vector>

#include <QObject>
#include <QAction>
#include <QString>

#include <GL/glew.h>
#include <common/interfaces.h>
#include <vcg/math/matrix44.h>
#include <meshlab/glarea.h>

#include "pyramid_point_renderer_base.h"
#include "pyramid_point_renderer.h"
#include "pyramid_point_renderer_color.h"
//#include "pyramid_point_renderer_er.h"

#include "shaderStructs.h"
#include "dialog.h"

class PyramidPointRendererPlugin : public QObject, public MeshRenderInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshRenderInterface)
	
  bool mIsSupported;
  QList <QAction *> actionList;

  void createPointRenderer( void );
  void drawPoints ( void );

 public :

  //  PyramidPointRendererPlugin( GLint default_mode = PYRAMID_POINTS );

  PyramidPointRendererPlugin();
  ~PyramidPointRendererPlugin();

  //void draw ( void );
  void reshape ( int w, int h );

  void changeRendererType ( int type );
  void changeMaterial( int mat );

  int getNumberPoints ( void );

  void setGpuMask ( int m );
  void setPerVertexColor ( bool b );
  void setAutoRotate ( bool r );

  void setBackFaceCulling ( bool c );
  void setEllipticalWeight ( bool b );
  void setQualityPerVertex ( bool c );

  void setQualityThreshold ( double q );
  void setReconstructionFilter ( double s );
  void setPrefilter ( double s );
  void setDepthTest ( bool d );
  
  void increaseSelected ( void );
  void decreaseSelected ( void );


  /// Implementations from inherited class
  QList<QAction *> actions ()
	{
	  if(actionList.isEmpty()) initActionList();
	  return actionList;
	}

  void initActionList();
  Dialog *sDialog;
  
  //  virtual bool isSupported() {return mIsSupported;}
  virtual bool isSupported() {return true;}
  virtual void Init(QAction *a, MeshDocument &m, RenderMode &rm, QGLWidget *gla);
  virtual void Render(QAction *a, MeshDocument &md, RenderMode &rm, QGLWidget *gla);
  //  virtual void Draw(QAction *, MeshModel &, RenderMode &, QGLWidget *) {}
  virtual int passNum() { return 1; }

 private :

  // Generic class, is instanced as one of the inherited classes (rendering algorithms)
  PointBasedRenderer *point_based_render;

  int canvas_width, canvas_height;
  int windows_width, windows_height;

  float clipRatioNear, clipRatioFar;
  float fov;
  float scale_factor;

  // Lists of objects (usually one ply file is associated to one object in list)
  vector<Object> objects;

  // Determines which rendering class to use (Pyramid points, with color per vertex, templates version ...)
  // see objects.h for the complete list (point_render_type_enum).
  GLint render_mode;

  // Flags on/off
  bool show_points;
  bool rotating;

  int selected;

  /***** Frames per second and Surfels per second vars ******/
  double sps, fps;
  int fps_loop;
  double start_time, end_time;
  int timing_profile;

  /*************************************/

};

#endif
