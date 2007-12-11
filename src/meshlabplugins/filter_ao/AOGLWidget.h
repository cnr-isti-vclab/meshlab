/*
 *  editAlignPair.h
 *  editalign_debug
 *
 *  Created by Paolo Cignoni on 11/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __VCG_AOGLWIDGET
#define __VCG_AOGLWIDGET

#include <GL/glew.h>
#include <QObject>
#include <QDialog>
#include <QGLWidget>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class AmbientOcclusionPlugin;

class AOGLWidget: public QGLWidget
{
	Q_OBJECT 
public:
  AOGLWidget (QWidget * parent, AmbientOcclusionPlugin *_plugin);
	AmbientOcclusionPlugin * plugin;
	vcg::CallBackPos *cb;
	MeshModel *m;
	public slots:
protected:
		/// opengl initialization and drawing calls
	void initializeGL ();
  void paintGL ();
	
};

#endif
