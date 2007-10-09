#ifndef RMSHADERRENDERPLUGIN_H
#define RMSHADERRENDERPLUGIN_H

#include <QDir>
#include <QObject>
#include <QAction>
#include <QList>
#include <QFile>
#include <QString>
#include <QApplication>
#include <QMap>
#include <QMessageBox>

#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "parser/RmXmlParser.h"
#include "rmshaderdialog.h"
#include "glstateholder.h"


class RmMeshShaderRenderPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

	// * map between the filename and its parser
	QMap<QString, RmXmlParser*> rmsources;

	QList <QAction *> actionList;

	RmShaderDialog * dialog;
	GLStateHolder holder;

	private:

		void initActionList();
    void myWindowRendering1(int pass);

	public:
		RmMeshShaderRenderPlugin() { dialog = NULL;}
		~RmMeshShaderRenderPlugin() { QMapIterator<QString, RmXmlParser*> i(rmsources); while( i.hasNext() ) { i.next(); delete i.value();} }

		QList<QAction *> actions () { if(actionList.isEmpty()) initActionList(); return actionList; }

		virtual const PluginInfo &Info();
		virtual bool isSupported() {return holder.isSupported();}
		virtual void Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
		virtual void Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
    virtual int passNum() { return holder.passNumber();}
};

#endif

