#ifndef __GLSTATEHOLDER_H__
#define __GLSTATEHOLDER_H__

#include "parser/RmPass.h"
#include "parser/UniformVar.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QVariant>
#include <QFileDialog>
#include <QApplication>
#include <QImage>
#include <QMessageBox>

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFramebufferObject>
#include <meshlab/meshmodel.h>


#include <assert.h>

#define FBO_SIZE 512



// * Extends the uniform variable class of the RM Parser
// * to add information such as memory location and
// * actual value
class UniformValue : public UniformVar
{
	public:
		static int textureUnit;


		UniformValue( UniformVar & var );
		virtual ~UniformValue();

		// * the arb memory location
		int location;

		// * texture mappings
		bool textureLoaded;
		GLuint textureId;


		void updateUniformVariableValuesFromDialog( int rowIdx, int colIdx, QVariant newValue );
		bool updateValueInGLMemory();
		void VarDump();
};

// * It's the descriptor of a pass: it has the list 
// * of uniform variable with their value and memory
// * location, and the ARB program handler
class GLStatePassHolder : public QObject
{
	Q_OBJECT

	public:
		QString passName;
    QString modelName;

	private:
	GLhandleARB vhandler;
	GLhandleARB fhandler;

	bool setVertexProgram;
	bool setFragmentProgram;
	QString lastError;

	GLhandleARB program;

	QMap<QString, UniformValue*> uniformValues;

	public:
		GLStatePassHolder( RmPass & pass );
		~GLStatePassHolder( );

		bool compile();
		bool link();

		bool hasVertexProgram() { return setVertexProgram; }
		bool hasFragmentProgram() { return setFragmentProgram; }

		QString & getLastError() { return lastError; }

		void updateUniformVariableValuesFromDialog( QString varname, int rowIdx, int colIdx, QVariant newValue );
		bool updateUniformVariableValuesInGLMemory();
    bool adjustSampler2DUniformVar(QString varname, GLuint texId);

		void VarDump();

		void useProgram();
    void execute();
};

// * This class hold the state of the rmshader render:
// * it has e list of passes and for each pass it keeps the 
// * list of uniform variable with their memory location and
// * actual value, and the ARB program handler
class GLStateHolder : public QObject
{
	Q_OBJECT

	QList<GLStatePassHolder*> passes;
  GLuint* passTextures;
  QGLFramebufferObject* fbo;
	QString lastError;
  GLint currentDrawbuf;
	bool supported;

	public:

		bool needUpdateInGLMemory;
    int currentPass;

		GLStateHolder( ) { fbo = NULL; needUpdateInGLMemory = true; supported = false; currentPass = -1;}
		GLStateHolder( QList<RmPass> & passes ) { fbo = NULL; setPasses(passes); needUpdateInGLMemory = true; supported = false; }
		~GLStateHolder( );

		void setPasses( QList<RmPass> & passes );

		bool compile();
		bool link();
		QString & getLastError() { return lastError; }

		void updateUniformVariableValuesFromDialog( QString passname, QString varname, int rowIdx, int colIdx, QVariant newValue );

		bool isSupported() { return supported; }
	
		bool updateUniformVariableValuesInGLMemory();
    bool updateUniformVariableValuesInGLMemory(int pass_idx);
		void VarDump();

		int passNumber() { return passes.size(); }
    inline void reset(){ needUpdateInGLMemory = true; };
		void usePassProgram( int i ) { passes[i] -> useProgram(); }
    bool executePass(int i );

  private:
    void genPassTextures();
};


#endif

