#ifndef VARIANCE_SHADOW_MAPPING_BLUR_H
#define VARIANCE_SHADOW_MAPPING_BLUR_H

#include <cassert>
#include <QString>
#include <QImage>
#include <QFile>
#include <GL/glew.h>
#include <meshlab/interfaces.h>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>
#include <decorate_shader.h>

#define BLUR_COEF 0.25
#define SHADOW_COEF 0.5

class VarianceShadowMappingBlur : public DecorateShader
{

public:
        VarianceShadowMappingBlur();
        ~VarianceShadowMappingBlur();

        bool init();
        void runShader(MeshModel&, GLArea*);

private:
        bool compileAndLink();
        bool setup();
        void bind();
        void unbind();
        void getBlur();
        void getShadowMap();

        GLuint _depth;
        GLuint _blur;

        GLuint _fbo;
        GLuint _objectShaderProgram;
        GLuint _objectVert, _objectFrag;
        GLuint _depthShaderProgram;
        GLuint _depthVert, _depthFrag;
        GLuint _blurShaderProgram;
        GLuint _blurVert, _blurFrag;
};

#endif // VARIANCE_SHADOW_MAPPING_BLUR_H
