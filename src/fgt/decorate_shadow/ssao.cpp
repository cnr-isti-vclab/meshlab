#include "ssao.h"

SSAO::SSAO():DecorateShader()
{
    this->_depth = 0;

    this->_normalMap = 0;
    this->_normalMapFrag = 0;
    this->_normalMapVert = 0;
    this->_normalMapShaderProgram = 0;

    this->_ssao= 0;
    this->_ssaoVert = 0;
    this->_ssaoFrag = 0;
    this->_ssaoShaderProgram = 0;

    this->_blurH = 0;
//    this->_blurV = 0;
    this->_blurVert = 0;
    this->_blurFrag = 0;
    this->_blurShaderProgram = 0;
    this->_fbo = 0;
}

SSAO::~SSAO(){
    glDetachShader(this->_normalMapShaderProgram, this->_normalMapVert);
    glDetachShader(this->_normalMapShaderProgram, this->_normalMapFrag);

    glDeleteShader(this->_normalMapVert);
    glDeleteShader(this->_normalMapFrag);
    glDeleteProgram(this->_normalMapShaderProgram);

    glDetachShader(this->_ssaoShaderProgram, this->_ssaoVert);
    glDetachShader(this->_ssaoShaderProgram, this->_ssaoFrag);

    glDeleteShader(this->_ssaoVert);
    glDeleteShader(this->_ssaoFrag);
    glDeleteProgram(this->_ssaoShaderProgram);

    glDetachShader(this->_blurShaderProgram, this->_blurVert);
    glDetachShader(this->_blurShaderProgram, this->_blurFrag);

    glDeleteShader(this->_blurVert);
    glDeleteShader(this->_blurFrag);
    glDeleteProgram(this->_blurShaderProgram);

    glDeleteFramebuffersEXT(1, &(this->_depth));
    glDeleteTexturesEXT(1, &(this->_normalMap));
    glDeleteTexturesEXT(1, &(this->_ssao));

    glDeleteTexturesEXT(1, &(this->_blurH));
  //  glDeleteTexturesEXT(1, &(this->_blurV));
    glDeleteFramebuffersEXT(1, &_fbo);
}

bool SSAO::init()
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

void SSAO::runShader(MeshModel& m, GLArea* gla){
            /***********************************************************/
            //GENERAZIONE SHADOW MAP
            /***********************************************************/
//            glEnable(GL_POLYGON_OFFSET_FILL);
//            glPolygonOffset(1.0, 1.0);
GLfloat g_mModelView[16];
        GLfloat g_mProjection[16];
            vcg::Box3f bb = m.cm.bbox;
                vcg::Point3f center;
                center = bb.Center();

                float diag = bb.Diag();

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
                    glLoadIdentity();

                    vcg::Matrix44f rotation;
                    vcg::Similarityf track = gla->trackball.track;
                    track.rot.ToMatrix(rotation);
                    glMultMatrixf(rotation.transpose().V());

                    //traslate the model in the center
                    glTranslatef(-center[0],-center[1],-center[2]);
                    glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);

            this->bind();
            glUseProgram(this->_normalMapShaderProgram);

            /*GLuint farLoc = glGetUniformLocation(this->_normalMapShaderProgram, "farPlane");
            glUniform1f(farLoc, gla->farPlane);

            GLuint nearLoc = glGetUniformLocation(this->_normalMapShaderProgram, "nearPlane");
            glUniform1f(nearLoc, gla->nearPlane);

            GLuint diagLoc = glGetUniformLocation(this->_normalMapShaderProgram, "diag");
            glUniform1f(diagLoc, m.cm.bbox.Diag());
*/

            RenderMode rm = gla->getCurrentRenderMode();
            glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
           // glDisable(GL_POLYGON_OFFSET_FILL);
            this->printColorMap(this->_normalMap, "_normalMap.png");
            //this->unbind();
            glUseProgram(0);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        /***********************************************************/
        //SSAO PASS
        /***********************************************************/
        /*GLint depthFuncOld;
        glGetIntegerv(GL_DEPTH_FUNC, &depthFuncOld);
        glDepthFunc(GL_LEQUAL);*/
        vcg::Matrix44f mvpl = (vcg::Matrix44f(g_mProjection).transpose() * vcg::Matrix44f(g_mModelView).transpose()).transpose();
        glUseProgram(this->_ssaoShaderProgram);

        GLuint matrixLoc = glGetUniformLocation(this->_ssaoShaderProgram, "mvpl");
        glUniformMatrix4fv(matrixLoc, 1, 0, mvpl.V());

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_noise);
        GLuint noiseloc = glGetUniformLocation(this->_ssaoShaderProgram, "rnm");
        glUniform1i(noiseloc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_normalMap);
        GLuint loc = glGetUniformLocation(this->_ssaoShaderProgram, "normalMap");
        glUniform1i(loc, 1);


        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);

        this->printColorMap(this->_ssao, "_ssao1.png");
        //this->unbind();
        glUseProgram(0);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_ssao);
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

        this->printColorMap(this->_blurH, "./_blurOrizzontale.png");
        this->unbind();


        /***********************************************************/
        //BLURRING vertical
        /***********************************************************/
        glUniform2f(scaleLoc, 0.0, scale);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, this->_blurH);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

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


        //glDepthFunc((GLenum)depthFuncOld);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glUseProgram(0);


        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

/****************************************************************************************/
//                                      BLURRING END
/****************************************************************************************/

        int error = glGetError();
}

bool SSAO::setup()
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

        glGenTextures(1, &this->_normalMap);
        glBindTexture(GL_TEXTURE_2D, this->_normalMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB,  this->_texSize, this->_texSize, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_FLOAT, NULL);
        //attacco al FBO la texture di colore
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_normalMap, 0);

        glGenTextures(1, &this->_ssao);
        glBindTexture(GL_TEXTURE_2D, this->_ssao);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB,  this->_texSize, this->_texSize, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_FLOAT, NULL);
        //attacco al FBO la texture di colore
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, this->_ssao, 0);


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
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, this->_blurH, 0);

        //genero il render buffer per il depth buffer
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);

        //e il depth buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);

        //GLenum drawBuffers[] = {this->_normalMap, this->_ssao, this->_blurH};
        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffersARB(3, drawBuffers);

        this->loadNoiseTxt();

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}

void SSAO::bind()
{
        assert(_initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texSize, this->_texSize);
        //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void SSAO::unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &_fbo);
}

bool SSAO::compileAndLink(){
    QFile* normalVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/normalMap.vert"));
    QFile* normalFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/normalMap.frag"));

    QFile* ssaoVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/ssao.vert"));
    QFile* ssaoFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/ssao.frag"));

    QFile* blurVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/blur.vert"));
    QFile* blurFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/ssao/blur.frag"));

    normalVert->open(QIODevice::ReadOnly | QIODevice::Text);
    normalFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    ssaoVert->open(QIODevice::ReadOnly | QIODevice::Text);
    ssaoFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    blurVert->open(QIODevice::ReadOnly | QIODevice::Text);
    blurFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray bArray = normalVert->readAll();
    GLint ShaderLen = (GLint) bArray.length();
    GLubyte* ShaderSource = (GLubyte *)bArray.data();

    normalVert->close();

    this->_normalMapVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_normalMapVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_normalMapVert);
    if(!this->printShaderInfoLog(this->_normalMapVert))
        return false;

    bArray = normalFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    normalFrag->close();

    this->_normalMapFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_normalMapFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(_normalMapFrag);
    if(!this->printShaderInfoLog(this->_normalMapFrag))
        return false;

    this->_normalMapShaderProgram = glCreateProgram();
    glAttachShader(this->_normalMapShaderProgram, this->_normalMapVert);
    glAttachShader(this->_normalMapShaderProgram, this->_normalMapFrag);
    glLinkProgram(this->_normalMapShaderProgram);
    if(!this->printProgramInfoLog(this->_normalMapShaderProgram))
        return false;


    bArray = ssaoVert->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    ssaoVert->close();

    this->_ssaoVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_ssaoVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_ssaoVert);
    if(!this->printShaderInfoLog(this->_ssaoVert))
        return false;

    bArray = ssaoFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    ssaoFrag->close();

    this->_ssaoFrag= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_ssaoFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_ssaoFrag);
    if(!this->printShaderInfoLog(this->_ssaoFrag))
        return false;

    this->_ssaoShaderProgram = glCreateProgram();
    glAttachShader(this->_ssaoShaderProgram, this->_ssaoVert);
    glAttachShader(this->_ssaoShaderProgram, this->_ssaoFrag);
    glLinkProgram(this->_ssaoShaderProgram);
    if(!this->printProgramInfoLog(this->_ssaoShaderProgram))
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

bool SSAO::loadNoiseTxt(){

    QImage image = QImage();
    QString textureName = QString("./noise.png");
    //fileName = textureName;
    if (QFile(textureName).exists())
    {
            image = QImage(textureName);
            //int bestW = pow(2.0,floor(::log(double(image.width()))/::log(2.0)));
            //int bestH = pow(2.0,floor(::log(double(image.height()))/::log(2.0)));
            //QImage imgGL = image.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QImage tmpGL = QGLWidget::convertToGLFormat(image);
            image = QImage(tmpGL);
    }
    // Creates The Texture
    glGenTextures(1, &(this->_noise));
    glBindTexture(GL_TEXTURE_2D, this->_noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, NOISE_WIDTH , NOISE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

    return true;
}
