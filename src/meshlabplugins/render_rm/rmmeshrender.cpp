#include "rmmeshrender.h"
#include <QtOpenGL>
#include <QtGui/QImage>

void RmMeshShaderRenderPlugin::myWindowRendering1(int pass){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glColor4f(0,0,0,0);
  glLoadIdentity();             
  
  holder.bindTexture(pass);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);glVertex3f(-1.0f, 1.0f, 0.0f);// Top Left
  glTexCoord2f(1.0f, 1.0f);glVertex3f( 1.0f, 1.0f, 0.0f);// Top Right
  glTexCoord2f(1.0f, 0.0f);glVertex3f( 1.0f,-1.0f, 0.0f);// Bottom Right
  glTexCoord2f(0.0f, 0.0f);glVertex3f(-1.0f,-1.0f, 0.0f);
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW); /* restore the model view */
}

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

    if (holder.currentPass == holder.passNumber())
      holder.currentPass = -1;

    if (holder.currentPass < 0)
      holder.currentPass = 0;

    //qDebug() << " Rendering [" << holder.currentPass+1 << "/" << holder.passNumber() << "]" << holder.released(holder.currentPass);

    if (holder.currentPass == holder.passNumber()-1){
      holder.release(1);
      myWindowRendering1(1); /* nothing to do */
      holder.currentPass++;
      return;
    }

    if (holder.currentPass > 0){ /* A previous pass have an fbo that should be released */
      //qDebug() << "  --release " << holder.currentPass-1;
      holder.release(holder.currentPass-1);
      glUseProgramObjectARB(0); /* Disable frag&shader */
      myWindowRendering1(holder.currentPass-1);
    }

    /* At every pass I release the previous fbo and bind the current */
    //qDebug() << "  ++bind " << holder.currentPass;
    holder.bind(holder.currentPass);
    glClearColor(1.0,1.0,1.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    holder.usePassProgram(holder.currentPass);
  }
  holder.currentPass++;

  // * clear errors, if any
  glGetError();
}


Q_EXPORT_PLUGIN(RmMeshShaderRenderPlugin)

