#include "glstateholder.h"

int UniformValue::textureUnit = 0;

UniformValue::UniformValue( UniformVar & v )
{
	type = v.type;
	name = v.name;
	typeString = v.typeString;
	memcpy( mat4, v.mat4, 4*4*sizeof(int));
	representerTagName = v.representerTagName;
	textureName = v.textureName;
	QFileInfo finfo(v.textureFilename);
	textureFilename = finfo.fileName();
	textureGLStates = v.textureGLStates;
	textureLoaded = false;

	// * if it's a texture, try to load it from the standard path
	if( textureFilename != "" ) {
		QDir textureDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
		if (textureDir.dirName() == "debug" || textureDir.dirName() == "release" || textureDir.dirName() == "plugins"  ) textureDir.cdUp();
#elif defined(Q_OS_MAC)
		if (textureDir.dirName() == "MacOS") { for(int i=0;i<4;++i){ textureDir.cdUp(); if(textureDir.exists("textures")) break; } }
#endif
		textureDir.cd("textures");
		updateUniformVariableValuesFromDialog( 0, 0, QVariant( textureDir.absoluteFilePath(textureFilename)));
	}
}

UniformValue::~UniformValue()
{
	if( textureLoaded )
		glDeleteTextures(1, &textureId);
}


void UniformValue::updateUniformVariableValuesFromDialog( int rowIdx, int colIdx, QVariant newValue )
{
	switch( type ) {
		case INT: ivalue = newValue.toInt(); break;
		case FLOAT: fvalue = newValue.toDouble(); break;
		case BOOL: bvalue = newValue.toBool() != 0; break;
		case VEC2: case VEC3: case VEC4: vec4[colIdx] = newValue.toDouble(); break;
		case IVEC2: case IVEC3: case IVEC4: ivec4[colIdx] = newValue.toInt(); break;
		case BVEC2: case BVEC3: case BVEC4: bvec4[colIdx] = newValue.toBool() != 0; break;
		case MAT2: mat2[rowIdx][colIdx] = newValue.toDouble(); break;
		case MAT3: mat3[rowIdx][colIdx] = newValue.toDouble(); break;
		case MAT4: mat4[rowIdx][colIdx] = newValue.toDouble(); break;

		case SAMPLER1D: case SAMPLER2D: case SAMPLER3D:
		case SAMPLERCUBE: case SAMPLER1DSHADOW: case SAMPLER2DSHADOW:
		{
			QString newPath;
			// * choose the filename with a dialog (55 by convention)
			if( rowIdx == 5 && colIdx == 5 ) {
				QFileDialog fd(0,"Choose new texture");

				QDir texturesDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
				if (texturesDir.dirName() == "debug" || texturesDir.dirName() == "release") texturesDir.cdUp();
#elif defined(Q_OS_MAC)
				if (texturesDir.dirName() == "MacOS") { for(int i=0;i<4;++i){ texturesDir.cdUp(); if(texturesDir.exists("textures")) break; } }
#endif
				texturesDir.cd("textures");
	
				fd.setDirectory(texturesDir);
				fd.move(500, 100);
				if (fd.exec())
				{
					QStringList sels = fd.selectedFiles();
					newPath = sels[0];
				} 
			} else
				newPath = newValue.toString();
			

			// * Load the new texture from given file
			if( textureLoaded ) 
				glDeleteTextures( 1, &textureId);

			QImage img, imgScaled, imgGL;
			QFileInfo finfo(newPath);
			if( finfo.exists() == false ) {
				qDebug() << "Texture" << name << "in" << newPath << ": file do not exists";
			} else
			if( img.load(newPath) == false ) {
				QMessageBox::critical(0, "Meshlab", newPath + ": Unsupported texture format.. implement me!");
			}

			glEnable( GL_TEXTURE_2D );

			// image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
			int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
			int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
			if( !img.isNull() ) imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
			imgGL=QGLWidget::convertToGLFormat(imgScaled);

			glGenTextures( 1, &textureId );
			glBindTexture( GL_TEXTURE_2D, textureId );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );

			textureLoaded = true;
			break;
		}
		case OTHER:
			assert(0);
	}
}


bool UniformValue::updateValueInGLMemory()
{
	switch(type) {
		case INT: glUniform1iARB(location, ivalue); break;
		case FLOAT: glUniform1fARB(location, fvalue); break;
		case BOOL: glUniform1iARB(location, bvalue); break;

		case VEC2: glUniform2fARB(location, vec2[0], vec2[1] ); break;
		case VEC3: glUniform3fARB(location, vec3[0], vec3[1], vec3[2] ); break;
		case VEC4: glUniform4fARB(location, vec4[0], vec4[1], vec4[2], vec4[3] ); break;

		case IVEC2: glUniform2iARB(location, ivec2[0], ivec2[1] ); break;
		case IVEC3: glUniform3iARB(location, ivec3[0], ivec3[1], ivec3[2] ); break;
		case IVEC4: glUniform4iARB(location, ivec4[0], ivec4[1], ivec4[2], ivec4[3] ); break;

		case BVEC2: glUniform2iARB(location, bvec2[0], bvec2[1] ); break;
		case BVEC3: glUniform3iARB(location, bvec3[0], bvec3[1], bvec3[2] ); break;
		case BVEC4: glUniform4iARB(location, bvec4[0], bvec4[1], bvec4[2], bvec4[3] ); break;

		case MAT2: glUniformMatrix2fvARB( location, 1, GL_FALSE, (GLfloat*)mat2 ); break;
		case MAT3: glUniformMatrix3fvARB( location, 1, GL_FALSE, (GLfloat*)mat3 ); break;
		case MAT4: glUniformMatrix4fvARB( location, 1, GL_FALSE, (GLfloat*)mat4 ); break;

		case SAMPLER1D: case SAMPLER2D: case SAMPLER3D: case SAMPLERCUBE: case SAMPLER1DSHADOW: case SAMPLER2DSHADOW:
		{
			if( textureLoaded == false )
				return false;

			if( UniformValue::textureUnit >= GL_MAX_TEXTURE_UNITS ) {
				QMessageBox::critical(0, "Meshlab", "Number of active texture is greater than max number supported (which is" + QString().setNum(GL_MAX_TEXTURE_UNITS) + ")" );
				return false;
			}
			glActiveTexture( GL_TEXTURE0 + UniformValue::textureUnit );
			glBindTexture( GL_TEXTURE_2D, textureId );
			glUniform1iARB( location, UniformValue::textureUnit++ );
			break;
		}
		case OTHER:
			qDebug() << "Type " << UniformVar::getStringFromUniformType(type) << " not updated in arb memory";
			return false;
	}

	// int err = glGetError();
	// if( !err ) qDebug() << "Update done correctly";
	// else qDebug() << "Error occurred while updating: " << err;
	return true;
}


void UniformValue::VarDump()
{
	switch(type) {
		case INT: qDebug() << "    " << name << "(" << location << "): " << ivalue; break;
		case FLOAT: qDebug() << "    " << name << "(" << location << "): " << fvalue; break;
		case BOOL: qDebug() << "    " << name << "(" << location << "): " << bvalue; break;
		case OTHER: qDebug() << "    " << name << "(" << location << "):  OTHER"; break;
		case SAMPLER1D: case SAMPLER2D: case SAMPLER3D: case SAMPLERCUBE: case SAMPLER1DSHADOW: case SAMPLER2DSHADOW: if( representerTagName == "RmRenderTarget" ) qDebug() << "    " << name << "(" << location << "):  RENDER TARGET"; else qDebug() << "    " << name << "(" << location << "): " << textureFilename; break;
		case VEC2: case VEC3: case VEC4: { int n = type == VEC2 ? 2 : (type == VEC3 ? 3 : 4 ); QString val; for( int i = 0; i < n; i++ ) val += " " + QString().setNum(vec4[i]) + (i+1==n?" ":","); qDebug() << "    " << name << "(" << location << "): {" << val.toLatin1().data() << "}"; break; }
		case IVEC2: case IVEC3: case IVEC4: { int n = type == IVEC2 ? 2 : (type == IVEC3 ? 3 : 4 ); QString val; for( int i = 0; i < n; i++ ) val += " " + QString().setNum(ivec4[i]) + (i+1==n?" ":","); qDebug() << "    " << name << "(" << location << "): {" << val.toLatin1().data() << "}"; break; }
		case BVEC2: case BVEC3: case BVEC4: { int n = type == BVEC2 ? 2 : (type == BVEC3 ? 3 : 4 ); QString val; for( int i = 0; i < n; i++ ) val += " " + QString().setNum(bvec4[i]) + (i+1==n?" ":","); qDebug() << "    " << name << "(" << location << "): {" << val.toLatin1().data() << "}"; break; }
		case MAT2: case MAT3: case MAT4: { int n = type == MAT2 ? 2 : (type == MAT3 ? 3 : 4 ); QString val; for( int i = 0; i < n; i++ ) { val += "["; for( int j = 0; j < n; j++ ) val += " " + QString().setNum(vec4[i*n+j]) + (j+1==n?" ":","); val += "]"; } qDebug() << "    " << name << "(" << location << "): { " << val.toLatin1().data() << " }"; break; }
	}
}


// ************** GL STATE PASS HOLDER *************** //

GLStatePassHolder::GLStatePassHolder( RmPass & pass )
{

	passName = pass.getName();

	QString &vprog = pass.getVertex();
	QString &fprog = pass.getFragment();
	
	GLenum err = glewInit();
	if( vprog.isNull() ) {
		setVertexProgram = false;
	} else {
		setVertexProgram = true;
		vhandler = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		QByteArray * c = new QByteArray(vprog.toLocal8Bit());
		const char * vvv = c->data();
		glShaderSourceARB(vhandler, 1, &vvv, NULL);
		delete c;
	}

	if( fprog.isNull() ) {
		setFragmentProgram = false;
	} else {
		setFragmentProgram = true;
		fhandler = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		QByteArray* c = new QByteArray(fprog.toLocal8Bit());
		const char * fff = c->data();
		glShaderSourceARB(fhandler, 1, &fff, NULL);
		delete c;
	}

	for( int i = 0; i < 2; i++ ) 
		for( int j = 0; j < (i == 0 ? pass.vertexUniformVariableSize() : pass.fragmentUniformVariableSize() ); j++ ) {
			UniformVar v = pass.getUniformVariable( j, i == 0 ? RmPass::VERTEX : RmPass::FRAGMENT );
			if( !uniformValues.contains(v.name) )
				uniformValues.insert( v.name, new UniformValue(v));
		}

	program = glCreateProgramObjectARB();
}

GLStatePassHolder::~GLStatePassHolder()
{
	glDeleteObjectARB(program);
	glDeleteObjectARB(vhandler);
	glDeleteObjectARB(fhandler);

	QMapIterator<QString, UniformValue*> it(uniformValues);
	while( it.hasNext() ) {
		it.next();
		delete it.value();
	}
}

bool GLStatePassHolder::compile() 
{
	GLint statusV = 1;
	GLint statusF = 1;
	if( hasVertexProgram() ) {
		glCompileShaderARB(vhandler);
		glGetObjectParameterivARB(vhandler, GL_OBJECT_COMPILE_STATUS_ARB, &statusV);
	}
	if( hasFragmentProgram() ) {
		glCompileShaderARB(fhandler);
		glGetObjectParameterivARB(fhandler, GL_OBJECT_COMPILE_STATUS_ARB, &statusF);
	}

	GLsizei length;
	if( !statusV ) {
		char shlog[2048];
		glGetShaderInfoLog(vhandler, 2048, &length, shlog);
		lastError = "Pass \"" + passName + "\" Vertex Compiling Error (" + QString().setNum(glGetError()) + "):\n" + QString(shlog);
		return false;
	}
	if( !statusF ) {
		char shlog[2048];
		glGetShaderInfoLog(fhandler, 2048, &length, shlog);
		lastError = "Pass \"" + passName + "\" Fragment Compiling Error (" + QString().setNum(glGetError()) + "):\n" + QString(shlog);
		return false;
	}

	return true;
}


bool GLStatePassHolder::link() 
{
	if( hasVertexProgram() ) glAttachObjectARB( program, vhandler );
	if( hasFragmentProgram() ) glAttachObjectARB( program, fhandler );

	glLinkProgramARB(program);

	GLint linkStatus;
	glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

	if( !linkStatus ) {

		GLsizei length;
		char shlog[2048];
		glGetInfoLogARB(program, 2048, &length, shlog);
		lastError = "Pass \"" + passName + "\" Linking Error (" + QString().setNum(glGetError()) + "):\n" + QString(shlog);
		return false;
	}


	QMapIterator<QString, UniformValue*> it(uniformValues);
	while( it.hasNext() ) {
		it.next();
		if( (it.value() -> location = glGetUniformLocationARB( program, it.key().toLocal8Bit().data())) == -1 ) {
			lastError = "Pass \"" + passName + "\" Linking Error (" + QString().setNum(glGetError()) + "): Unknown memory location for some variable";
			return false;
		}
	}

	return true;
}


void GLStatePassHolder::updateUniformVariableValuesFromDialog( QString varname, int rowIdx, int colIdx, QVariant newValue )
{
	UniformValue * var = uniformValues[varname];
	if( var )
		var -> updateUniformVariableValuesFromDialog( rowIdx, colIdx, newValue );
}


bool GLStatePassHolder::updateUniformVariableValuesInGLMemory()
{
	bool ret = true;
	UniformValue::textureUnit = 0;
	glUseProgramObjectARB(program);
	QMapIterator<QString,UniformValue*> it( uniformValues );
	while( it.hasNext() ) {
		it.next();
		if( !it.value() -> updateValueInGLMemory() ) ret = false;
	}
	return ret;
}

void GLStatePassHolder::VarDump()
{
	qDebug() << "  Pass:" << passName << "with" << uniformValues.size() << "uniform values";
	QMapIterator<QString,UniformValue*> it( uniformValues );
	while( it.hasNext() ) {
		it.next();
		it.value() -> VarDump();
	}
}


// ***************** GL STATE HOLDER ****************** //


GLStateHolder::~GLStateHolder( )
{
	for( int i = 0; i < passes.size(); i++ )
		delete passes[i];
}

void GLStateHolder::setPasses( QList<RmPass> & _passes )
{
	for( int i = 0; i < passes.size(); i++ )
		delete passes[i];
	passes.clear();
	for( int i = 0; i < _passes.size(); i++ )
		passes.append( new GLStatePassHolder(_passes[i]));
}

bool GLStateHolder::compile() 
{
	for( int i = 0; i < passes.size(); i++ )
		if( passes[i] -> compile() == false ) {
			lastError = passes[i] -> getLastError();
			supported = false;
			return false;
		}
	return true;
}

bool GLStateHolder::link() 
{
	for( int i = 0; i < passes.size(); i++ )
		if( passes[i] -> link() == false ) {
			lastError = passes[i] -> getLastError();
			supported = false;
			return false;
		}

	supported = true;
	return true;
}


void GLStateHolder::updateUniformVariableValuesFromDialog( QString passname, QString varname, int rowIdx, int colIdx, QVariant newValue )
{
	needUpdateInGLMemory = true;
	for( int i = 0; i < passes.size(); i++ )
		if( passes[i] -> passName == passname ) {
			passes[i] -> updateUniformVariableValuesFromDialog( varname, rowIdx, colIdx, newValue );
			break;
		}
}

bool GLStateHolder::updateUniformVariableValuesInGLMemory()
{
	bool ret = true;
	needUpdateInGLMemory = false;
	for( int i = 0; i < passes.size(); i++ )
		if( !passes[i] -> updateUniformVariableValuesInGLMemory() )
			ret = false;
	glUseProgramObjectARB(0);
	return ret;
}

void GLStateHolder::VarDump()
{
	qDebug() << "Passes:" << passes.size();
	for( int i = 0; i < passes.size(); i++ )
		passes[i] -> VarDump();
}

