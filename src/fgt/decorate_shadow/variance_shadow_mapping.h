#ifndef VARIANCE_SHADOW_MAPPING_H
#define VARIANCE_SHADOW_MAPPING_H

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

class VarianceShadowMapping : public DecorateShader
{

public:
        VarianceShadowMapping();
        ~VarianceShadowMapping();

        bool init();
        void runShader(MeshModel&, GLArea*);

private:
        bool compileAndLink();
        bool setup();
        void bind();
        void unbind();

        GLuint _depth;

        GLuint _fbo;
        GLuint _objectShaderProgram;
        GLuint _objectVert, _objectFrag;
        GLuint _depthShaderProgram;
        GLuint _depthVert, _depthFrag;
};


#endif // VARIANCE_SHADOW_MAPPING_H
