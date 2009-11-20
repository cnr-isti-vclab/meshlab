#include "ssao.h"

SSAO::SSAO():DecorateShader()
{
    this->_depth = 0;

    this->_color1 = 0;
    this->_normalMapFrag = 0;
    this->_normalMapVert = 0;
    this->_normalMapShaderProgram = 0;

    this->_depthMap = 0;
    this->_depth = 0;

    this->_color2= 0;
    this->_ssaoVert = 0;
    this->_ssaoFrag = 0;
    this->_ssaoShaderProgram = 0;

    this->_blurVert = 0;
    this->_blurFrag = 0;
    this->_blurShaderProgram = 0;
    this->_fbo = 0;
    this->_fbo2 = 0;
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

    glDeleteTexturesEXT(1, &(this->_color1));
    glDeleteTexturesEXT(1, &(this->_depthMap));

    glDeleteFramebuffersEXT(1, &(this->_depth));
    glDeleteTexturesEXT(1, &(this->_color2));
    glDeleteTexturesEXT(1, &(this->_color2));

    glDeleteFramebuffersEXT(1, &_fbo);
    glDeleteFramebuffersEXT(1, &_fbo2);
}

bool SSAO::init()
{
    GLenum err = glewInit();
    if (!GLEW_OK == err){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("GLEW init failure");
        msgBox.setText(QString("Init GLEW failed."));
        msgBox.exec();
        return false;
    }
    if(!this->setup()){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("FBO Setup failure");
        msgBox.setText(QString("Failed in creating a Frame Buffer Object."));
        msgBox.exec();
        return false;
    }
    return compileAndLink();
}

void SSAO::runShader(MeshModel& m, GLArea* gla){

        /***********************************************************/
        //GENERAZIONE NORMAL MAP E DEPTH MAP
        /***********************************************************/
        this->bind();
        glUseProgram(this->_normalMapShaderProgram);
        RenderMode rm = gla->getCurrentRenderMode();

        vcg::Matrix44f mProj, mInverseProj;
        glMatrixMode(GL_PROJECTION);
        glGetFloatv(GL_PROJECTION_MATRIX, mProj.V());
        glMatrixMode(GL_MODELVIEW);

        mProj.transposeInPlace();
        mInverseProj = vcg::Inverse(mProj);

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m.Render(vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone);
        //this->printColorMap(this->_color1, "_normals.png");
        glUseProgram(0);

        /***********************************************************/
        //SSAO PASS
        /***********************************************************/
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo2);
        glUseProgram(this->_ssaoShaderProgram);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_noise);
        GLuint noiseloc = glGetUniformLocation(this->_ssaoShaderProgram, "rnm");
        glUniform1i(noiseloc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_color1);
        GLuint loc = glGetUniformLocation(this->_ssaoShaderProgram, "normalMap");
        glUniform1i(loc, 1);


        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, this->_depthMap);
        loc = glGetUniformLocation(this->_ssaoShaderProgram, "depthMap");
        glUniform1i(loc, 2);

        GLuint matrixLoc = glGetUniformLocation(this->_ssaoShaderProgram, "proj");
        glUniformMatrix4fv(matrixLoc, 1, 0, mProj.transpose().V());

        GLuint invMatrixLoc = glGetUniformLocation(this->_ssaoShaderProgram, "invProj");
        glUniformMatrix4fv(invMatrixLoc, 1, 0, mInverseProj.transpose().V());

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
            glEnd();
        //this->printColorMap(this->_color2, "_ssao.png");
        glUseProgram(0);


/****************************************************************************************/
//                                      BLURRING
/****************************************************************************************/


        /***********************************************************/
        //BLURRING horizontal
        /***********************************************************/
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glUseProgram(this->_blurShaderProgram);

        float blur_coef = 0.8;
        GLfloat scale = 1/(this->_texSize * blur_coef);

        GLuint scaleLoc = glGetUniformLocation(this->_blurShaderProgram, "scale");
        glUniform2f(scaleLoc, scale, 0.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_color2);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
            glEnd();

        /***********************************************************/
        //BLURRING vertical
        /***********************************************************/
        this->unbind();
        glUniform2f(scaleLoc, 0.0, scale);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, this->_color1);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
            glEnd();

        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

/****************************************************************************************/
//                                      BLURRING END
/****************************************************************************************/
}

bool SSAO::setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;

        //genero i 2 framebuffer object che mi servono.
        glGenFramebuffersEXT(1, &_fbo);
        glGenFramebuffersEXT(1, &_fbo2);

        //attacco il primo...adesso le modifiche andranno a modificare solo _fbo
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        glGenTextures(1, &this->_color1);
        glBindTexture(GL_TEXTURE_2D, this->_color1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_color1, 0);

        glGenTextures(1, &this->_depthMap);
        glBindTexture(GL_TEXTURE_2D, this->_depthMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,  this->_texSize, this->_texSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->_depthMap, 0);


        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};

        glDrawBuffersARB(0, drawBuffers);

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);

        if(!this->_initOk)
            return this->_initOk;

        //attacco il secondo fbo...adesso le modifiche andranno a modificare solo _fbo2
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo2);

        //texture per ssao
        glGenTextures(1, &this->_color2);
        glBindTexture(GL_TEXTURE_2D, this->_color2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_color2, 0);

        //genero il render buffer per il depth buffer
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);

        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);

        GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0};

        glDrawBuffersARB(0, drawBuffers2);

        this->loadNoiseTxt();

        err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
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
}

void SSAO::unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
    //QString textureName = QString(":/noise.png");
    QString textureName = QString(":/rand.png");
    //fileName = textureName;
    if (QFile(textureName).exists())
    {
            image = QImage(textureName);
						noiseWidth=image.width();
						noiseHeight=image.height();

            QImage tmpGL = QGLWidget::convertToGLFormat(image);
            image = QImage(tmpGL);
    }
    else {
        qDebug("Warning failed to load noise texture!");
        assert(0);
    }
    // Creates The Texture
    glGenTextures(1, &(this->_noise));
    glBindTexture(GL_TEXTURE_2D, this->_noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseWidth , noiseHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

    return true;
}

void SSAO::printNoiseTxt(){
    QImage img(noiseWidth , noiseHeight, QImage::Format_RGB32);

    unsigned char *tempBuf = new unsigned char[noiseWidth * noiseHeight * 3];
    unsigned char *tempBufPtr = tempBuf;
    glBindTexture(GL_TEXTURE_2D, this->_noise);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
    for (int i = 0; i < noiseWidth; ++i) {
            QRgb *scanLine = (QRgb*)img.scanLine(i);
            for (int j = 0; j < noiseHeight; ++j) {
                    scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                    tempBufPtr += 3;
            }
    }

    delete[] tempBuf;

    img.mirrored().save("_noise.png", "PNG");
}
