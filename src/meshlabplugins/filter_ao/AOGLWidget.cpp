#include <GL/glew.h>
#include <QtGui>
#include <QGLWidget>
#include "AOGLWidget.h"
#include "filter_ao.h"

AOGLWidget::AOGLWidget (QWidget * parent, AmbientOcclusionPlugin *_plugin) :QGLWidget (parent)
{
	plugin=_plugin;
	cb=0;
	m=0;
	QGLFormat qFormat = QGLFormat::defaultFormat();
	qFormat.setAlpha(true);
	qFormat.setDepth(true);
	setFormat(qFormat);
	if(!isValid())
	{
		qDebug("Error: Unable to create a new QGLWidget");
		return;
	}
	
}


void AOGLWidget::initializeGL ()
{
	plugin->initGL(cb,m->cm.vn);

	unsigned int widgetSize = std::max(plugin->maxTexSize, plugin->depthTexSize);

	setFixedSize(widgetSize,widgetSize);
}

void AOGLWidget::paintGL ()
{
	qDebug("Start Painting window size %i %i", width(), height());
	plugin->processGL(this,*m);
	hide();
	qDebug("End Painting");
}

