#include "variance_shadow_mapping_blur.h"

VarianceShadowMappingBlur::VarianceShadowMappingBlur():DecorateShader()
{
    this->_depth = 0;
    this->_blurH = 0;
    this->_blurV = 0;
    this->_depthVert = 0;
    this->_depthFrag = 0;
    this->_depthShaderProgram = 0;
    this->_objectVert = 0;
    this->_objectFrag = 0;
    this->_objectShaderProgram = 0;
    this->_blurVert = 0;
    this->_blurFrag = 0;
    this->_blurShaderProgram = 0;
    this->_fbo = 0;
}

VarianceShadowMappingBlur::~VarianceShadowMappingBlur(){
    glDetachShader(this->_depthShaderProgram, this->_depthVert);
    glDetachShader(this->_depthShaderProgram, this->_depthFrag);

    glDeleteShader(this->_depthVert);
    glDeleteShader(this->_depthFrag);
    glDeleteProgram(this->_depthShaderProgram);

    glDetachShader(this->_objectShaderProgram, this->_objectVert);
    glDetachShader(this->_objectShaderProgram, this->_objectFrag);

    glDeleteShader(this->_objectVert);
    glDeleteShader(this->_objectFrag);
    glDeleteProgram(this->_objectShaderProgram);

    glDetachShader(this->_blurShaderProgram, this->_blurVert);
    glDetachShader(this->_blurShaderProgram, this->_blurFrag);

    glDeleteShader(this->_blurVert);
    glDeleteShader(this->_blurFrag);
    glDeleteProgram(this->_blurShaderProgram);

    glDeleteFramebuffersEXT(1, &(this->_depth));
    glDeleteTexturesEXT(1, &(this->_shadowMap));
    glDeleteTexturesEXT(1, &(this->_blurH));
    glDeleteTexturesEXT(1, &(this->_blurV));
    glDeleteFramebuffersEXT(1, &_fbo);
}

bool VarianceShadowMappingBlur::init()
{
    GLenum err = glewInit();
    if (!GLEW_OK == err){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("GLEW init failure");
        msgBox.setText(QString("Init GLEW failed."));
        int ret = msgBox.exec();
        return false;
    }
    if(!this->setup()){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("FBO Setup failure");
        msgBox.setText(QString("Failed in creating a Frame Buffer Object."));
        int ret = msgBox.exec();
        return false;
    }
    return compileAndLink();
}

void VarianceShadowMappingBlur::runShader(MeshModel& m, GLArea* gla){
        vcg::Box3f bb = m.cm.bbox;
        vcg::Point3f center;
        center = bb.Center();

        GLfloat g_mModelView[16];
        GLfloat g_mProjection[16];

        float diag = bb.Diag();

        GLfloat lP[4];
        glGetLightfv(GL_LIGHT0, GL_POSITION, lP);
        vcg::Point3f light = -vcg::Point3f(lP[0],lP[1],lP[2]);

        vcg::Matrix44f tm = gla->trackball.Matrix();

        glMatrixMode(GL_PROJECTION);

        glPushMatrix();

            glLoadIdentity();
            glOrtho(-(diag/2),
                     diag/2,
                     -(diag/2),
                     diag/2,
                     -(diag/2),
                     diag/2);

            glGetFloatv(GL_PROJECTION_MATRIX, g_mProjection);
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
            vcg::Point3f u, v;
            //mi seleziona automaticamente un upvector che mi eviti casi degeneri...nel caso vada bene 010 sceglie quello
            vcg::GetUV(light, u, v, vcg::Point3f(0,-1,0));
            glLoadIdentity();
            gluLookAt(0, 0, 0, light[0], light[1], light[2], v[0], v[1], v[2]);

            //get the rotation matrix from the trackball
            vcg::Matrix44f rotation;
            vcg::Similarityf track = gla->trackball.track;
            track.rot.ToMatrix(rotation);
            glMultMatrixf(rotation.transpose().V());

            //traslate the model in the center
            glTranslatef(-center[0],-center[1],-center[2]);
            glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);


            /***********************************************************/
            //GENERAZIONE SHADOW MAP
            /***********************************************************/
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 1.0);


            this->bind();
            glUseProgram(this->_depthShaderProgram);
            RenderMode rm = gla->getCurrentRenderMode();
            glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
            glDisable(GL_POLYGON_OFFSET_FILL);
            //this->getShadowMap();
            //glUseProgram(0);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

/****************************************************************************************/
//                                      BLURRING
/****************************************************************************************/
     //Preparing to draw quad
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(-(this->_texSize/2),
                     this->_texSize/2,
                     -(this->_texSize/2),
                     this->_texSize/2,
                     -(this->_texSize/2),
                     this->_texSize/2);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            //vcg::Point3f t = track.tra;
            //glTranslated(0,0,t[2]);
            glTranslated(0,0,-1);
        /***********************************************************/
        //BLURRING horizontal
        /***********************************************************/
        glUseProgram(this->_blurShaderProgram);

        GLfloat scale = 1/(this->_texSize * BLUR_COEF);// * SHADOW_COEF);
        //GLfloat scale = (1/(this->_texSize)) * BLUR_COEF * SHADOW_COEF;
        GLuint scaleLoc = glGetUniformLocation(this->_blurShaderProgram, "scale");
        glUniform2f(scaleLoc, scale, 0.0);

        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        GLuint loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBegin(GL_QUADS);
                    glTexCoord2d(0,0);
                    glVertex3f(-this->_texSize/2,-this->_texSize/2,0);
                    glTexCoord2d(1,0);
                    glVertex3f(this->_texSize/2,-this->_texSize/2,0);
                    glTexCoord2d(1,1);
                    glVertex3f(this->_texSize/2,this->_texSize/2,0);
                    glTexCoord2d(0,1);
                    glVertex3f(-this->_texSize/2,this->_texSize/2,0);
            glEnd();

        //this->getBlurH();
        //this->unbind();


        /***********************************************************/
        //BLURRING vertical
        /***********************************************************/
        glUniform2f(scaleLoc, 0.0, scale);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, this->_blurH);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBegin(GL_QUADS);
                    glTexCoord2d(0,0);
                    glVertex3f(-this->_texSize/2,-this->_texSize/2,0);
                    glTexCoord2d(1,0);
                    glVertex3f(this->_texSize/2,-this->_texSize/2,0);
                    glTexCoord2d(1,1);
                    glVertex3f(this->_texSize/2,this->_texSize/2,0);
                    glTexCoord2d(0,1);
                    glVertex3f(-this->_texSize/2,this->_texSize/2,0);
            glEnd();

			//this->getBlurV();
        this->unbind();
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        /***********************************************************/
        //OBJECT PASS
        /***********************************************************/
        GLint depthFuncOld;
        glGetIntegerv(GL_DEPTH_FUNC, &depthFuncOld);
        glDepthFunc(GL_LEQUAL);
        vcg::Matrix44f mvpl = (vcg::Matrix44f(g_mProjection).transpose() * vcg::Matrix44f(g_mModelView).transpose()).transpose();
        glUseProgram(this->_objectShaderProgram);

        GLuint matrixLoc = glGetUniformLocation(this->_objectShaderProgram, "mvpl");
        glUniformMatrix4fv(matrixLoc, 1, 0, mvpl.V());

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_blurV);


        loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 0);
//        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glPushAttrib(GL_COLOR_BUFFER_BIT);
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE); // to avoid the fact that when saving a snapshot we get semitransparent shadowed areas.
        m.Render(rm.drawMode, rm.colorMode, vcg::GLW::TMNone);
			  glPopAttrib();
        //this->getBlurH();
        glUseProgram(0);

				glDepthFunc((GLenum)depthFuncOld);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
				
				
   
/****************************************************************************************/
//                                      BLURRING END
/****************************************************************************************/

        int error = glGetError();
}

bool VarianceShadowMappingBlur::setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;

        //genero il frame buffer object
        glGenFramebuffersEXT(1, &_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        //genero la texture di colore che sara la mia variance shadow map.
        glGenTextures(1, &this->_shadowMap);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB,  this->_texSize, this->_texSize, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_FLOAT, NULL);
        //attacco al FBO la texture di colore
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);

        //genero la texture di blur orizzontale.
        glGenTextures(1, &this->_blurH);
        glBindTexture(GL_TEXTURE_2D, this->_blurH);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB,  this->_texSize, this->_texSize, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_FLOAT, NULL);
        //e la texture per il blur orizzontale
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, this->_blurH, 0);

        //genero la texture di blur verticale.
        glGenTextures(1, &this->_blurV);
        glBindTexture(GL_TEXTURE_2D, this->_blurV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB,  this->_texSize, this->_texSize, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_FLOAT, NULL);
        //e la texture per il blur verticale
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, this->_blurV, 0);

        //genero il render buffer per il depth buffer
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);

        //e il depth buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);

        GLenum drawBuffers[] = {this->_shadowMap, this->_blurH, this->_blurV};
        glDrawBuffersARB(3, drawBuffers);

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}

void VarianceShadowMappingBlur::bind()
{
        assert(_initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texSize, this->_texSize);
        //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void VarianceShadowMappingBlur::unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &_fbo);
}

bool VarianceShadowMappingBlur::compileAndLink(){
    QFile* depthVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/depthVSM.vert"));
    QFile* depthFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/depthVSM.frag"));
    QFile* objVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/objectVSM.vert"));
    QFile* objFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/objectVSM.frag"));
    QFile* blurVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/blurVSM.vert"));
    QFile* blurFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsmb/blurVSM.frag"));

    depthVert->open(QIODevice::ReadOnly | QIODevice::Text);
    depthFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    objVert->open(QIODevice::ReadOnly | QIODevice::Text);
    objFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    blurVert->open(QIODevice::ReadOnly | QIODevice::Text);
    blurFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray bArray = depthVert->readAll();
    GLint ShaderLen = (GLint) bArray.length();
    GLubyte* ShaderSource = (GLubyte *)bArray.data();

    depthVert->close();

    this->_depthVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_depthVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_depthVert);
    if(!this->printShaderInfoLog(this->_depthVert))
        return false;

    bArray = depthFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    depthFrag->close();

    this->_depthFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_depthFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_depthFrag);
    if(!this->printShaderInfoLog(this->_depthFrag))
        return false;

    this->_depthShaderProgram = glCreateProgram();
    glAttachShader(this->_depthShaderProgram, this->_depthVert);
    glAttachShader(this->_depthShaderProgram, this->_depthFrag);
    glLinkProgram(this->_depthShaderProgram);
    if(!this->printProgramInfoLog(this->_depthShaderProgram))
        return false;


    bArray = objVert->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    objVert->close();

    this->_objectVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_objectVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectVert);
    if(!this->printShaderInfoLog(this->_objectVert))
        return false;

    bArray = objFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    objFrag->close();

    this->_objectFrag= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_objectFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectFrag);
    if(!this->printShaderInfoLog(this->_objectFrag))
        return false;

    this->_objectShaderProgram = glCreateProgram();
    glAttachShader(this->_objectShaderProgram, this->_objectVert);
    glAttachShader(this->_objectShaderProgram, this->_objectFrag);
    glLinkProgram(this->_objectShaderProgram);
    if(!this->printProgramInfoLog(this->_objectShaderProgram))
        return false;

    bArray = blurVert->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    blurVert->close();

    this->_blurVert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_blurVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_blurVert);
    if(!this->printShaderInfoLog(this->_blurVert))
        return false;

    bArray = blurFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    blurFrag->close();

    this->_blurFrag= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_blurFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_blurFrag);
    if(!this->printShaderInfoLog(this->_blurFrag))
        return false;

    this->_blurShaderProgram = glCreateProgram();
    glAttachShader(this->_blurShaderProgram, this->_blurVert);
    glAttachShader(this->_blurShaderProgram, this->_blurFrag);
    glLinkProgram(this->_blurShaderProgram);
    if(!this->printProgramInfoLog(this->_blurShaderProgram))
        return false;

    return true;
}

void VarianceShadowMappingBlur::getBlurH()
{
        if (!this->_initOk)
                return;

        QImage img(this->_texSize, this->_texSize, QImage::Format_RGB32);

     unsigned char *tempBuf = new unsigned char[this->_texSize * this->_texSize * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, this->_blurH);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_texSize; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texSize; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }

        delete[] tempBuf;

        img.mirrored().save("./_vsm_blurHTXT.png", "PNG");
}

void VarianceShadowMappingBlur::getBlurV()
{
        if (!this->_initOk)
                return;

        QImage img(this->_texSize, this->_texSize, QImage::Format_RGB32);

     unsigned char *tempBuf = new unsigned char[this->_texSize * this->_texSize * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, this->_blurV);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_texSize; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texSize; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }

        delete[] tempBuf;

        img.mirrored().save("./_vsm_blurVTXT.png", "PNG");
}

void VarianceShadowMappingBlur::getShadowMap(){
        if (!this->_initOk)
                return;

        QImage img(this->_texSize, this->_texSize, QImage::Format_RGB32);

        unsigned char *tempBuf = new unsigned char[this->_texSize * this->_texSize * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_texSize; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texSize; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }

        delete[] tempBuf;
        img.mirrored().save("./_shadowMapTXT.png", "PNG");
    }

