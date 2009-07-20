#include "variance_shadow_mapping.h"

VarianceShadowMapping::VarianceShadowMapping():DecorateShader()
{
    this->_depth = 0;

    this->_depthVert = 0;
    this->_depthFrag = 0;
    this->_depthShaderProgram = 0;
    this->_objectVert = 0;
    this->_objectFrag = 0;
    this->_objectShaderProgram = 0;
    this->_fbo = 0;
}

VarianceShadowMapping::~VarianceShadowMapping(){
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

    glDeleteFramebuffersEXT(1, &(this->_depth));
    glDeleteTexturesEXT(1, &(this->_shadowMap));
    glDeleteFramebuffersEXT(1, &_fbo);
}

bool VarianceShadowMapping::init()
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

void VarianceShadowMapping::runShader(MeshModel& m, GLArea* gla){
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
            //mi seleziona automaticamente un upvector che mi eviti casi degeneri...
            //nel caso vada bene 010 sceglie quello
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
            m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
            glDisable(GL_POLYGON_OFFSET_FILL);
            this->unbind();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        GLuint loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m.Render(rm.drawMode, rm.colorMode, rm.textureMode);

        glDisable(GL_BLEND);
        glDepthFunc((GLenum)depthFuncOld);
        glUseProgram(0);

        int error = glGetError();
}

bool VarianceShadowMapping::setup()
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        //attacco al FBO la texture di colore
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);

        //genero il render buffer per il depth buffer
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);

        //e il depth buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);


        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}

void VarianceShadowMapping::bind()
{
        assert(_initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texSize, this->_texSize);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void VarianceShadowMapping::unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &_fbo);
}

bool VarianceShadowMapping::compileAndLink(){
    QFile* depthVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsm/depthVSM.vert"));
    QFile* depthFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsm/depthVSM.frag"));
    QFile* objVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsm/objectVSM.vert"));
    QFile* objFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/vsm/objectVSM.frag"));

    depthVert->open(QIODevice::ReadOnly | QIODevice::Text);
    depthFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    objVert->open(QIODevice::ReadOnly | QIODevice::Text);
    objFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray bArray = depthVert->readAll();
    GLint ShaderLen = (GLint) bArray.length();
    GLubyte* ShaderSource = (GLubyte *)bArray.data();

    this->_depthVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_depthVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_depthVert);
    if(!this->printShaderInfoLog(this->_depthVert))
        return false;

    depthVert->close();

    bArray = depthFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    this->_depthFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_depthFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_depthFrag);
    if(!this->printShaderInfoLog(this->_depthFrag))
        return false;

    depthFrag->close();

    this->_depthShaderProgram = glCreateProgram();
    glAttachShader(this->_depthShaderProgram, this->_depthVert);
    glAttachShader(this->_depthShaderProgram, this->_depthFrag);
    glLinkProgram(this->_depthShaderProgram);
    if(!this->printProgramInfoLog(this->_depthShaderProgram))
        return false;


    bArray = objVert->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    this->_objectVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_objectVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectVert);
    if(!this->printShaderInfoLog(this->_objectVert))
        return false;

    objVert->close();

    bArray = objFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    this->_objectFrag= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_objectFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectFrag);
    if(!this->printShaderInfoLog(this->_objectFrag))
        return false;

    objFrag->close();

    this->_objectShaderProgram = glCreateProgram();
    glAttachShader(this->_objectShaderProgram, this->_objectVert);
    glAttachShader(this->_objectShaderProgram, this->_objectFrag);
    glLinkProgram(this->_objectShaderProgram);
    if(!this->printProgramInfoLog(this->_objectShaderProgram))
        return false;

    return true;
}

