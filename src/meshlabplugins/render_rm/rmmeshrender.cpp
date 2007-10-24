#include "rmmeshrender.h"
#include <QtOpenGL>
#include <QtGui/QImage>



static int vp[4];

const PluginInfo& RmMeshShaderRenderPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr("September 2007");
	ai.Version = "1.0";
	ai.Author = "Fusco Francesco, Giacomo Galilei";
	return ai;
}

void RmMeshShaderRenderPlugin::initActionList() {

	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release" || shadersDir.dirName() == "plugins"  )
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for(int i=0;i<4;++i){
			shadersDir.cdUp();
			if(shadersDir.exists("shadersrm")) break;
		}
	}
#endif

	if(!shadersDir.cd("shadersrm"))  {
		QMessageBox::information(0, "MeshLAb", "Unable to find the render monkey shaders directory.\n" "No shaders will be loaded.");
		return;
	}


	int errors = 0;
	int successes = 0;

	foreach (QString fileName, shadersDir.entryList(QDir::Files)) {
		if (fileName.endsWith(".rfx")) {
			RmXmlParser * parser = new RmXmlParser( shadersDir.absoluteFilePath(fileName) );
			if( parser -> parse() == false ) {
				qDebug() << "Unable to load RmShader from" << shadersDir.absoluteFilePath(fileName) << ": " << parser -> errorString();
				delete parser;
				errors += 1;
				continue;
			}

			rmsources.insert(fileName, parser);
			successes += 1;

			QAction * qa = new QAction(fileName, this); 
			qa->setCheckable(false);
			actionList << qa;
		}
	}
	//qDebug() << (successes+errors) << "RmShaders parsed. Opened correctly" << successes << "rmshaders, and get" << errors <<"errors";
}



void RmMeshShaderRenderPlugin::Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{
#ifdef DEBUG
  qDebug() << "Init";
#endif
  glGetIntegerv(GL_VIEWPORT, vp);
	if( dialog ) {
		dialog->close();
		delete dialog;
	}

	RmXmlParser * parser = rmsources.value(a->text());
	assert(parser);

	gla->makeCurrent();
  GLenum err = glewInit();
	if (GLEW_OK == err) {
		if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {

      holder.reset();
			dialog = new RmShaderDialog(&holder, parser, gla, rm);
			dialog->move(10,100);
			dialog->show();
		}
	}

	// * clear errors, if any
	glGetError();
}
void RmMeshShaderRenderPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla)
{

  if( holder.needUpdateInGLMemory )
    holder.updateUniformVariableValuesInGLMemory();

  if( holder.isSupported()){

    /* Handle single pass filters */
    if (holder.passNumber() == 1){ 
      glEnable(GL_TEXTURE_2D);
      holder.updateUniformVariableValuesInGLMemory(0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      holder.usePassProgram(0);
      return;
    }
    glViewport(0,0,FBO_SIZE, FBO_SIZE); /* FIXME */

    if (holder.currentPass >= holder.passNumber())
      holder.currentPass = -1;

    if (holder.currentPass < 0)
      holder.currentPass = 0;
#ifdef DEBUG
    qDebug() << " Rendering [" << holder.currentPass+1 << "/" << holder.passNumber() << "]";
#endif
    if (holder.currentPass >= 0){ 
      holder.executePass(holder.currentPass);
    }

  }
  holder.currentPass++;

  // * clear errors, if any
  glGetError();
}


Q_EXPORT_PLUGIN(RmMeshShaderRenderPlugin)

