/* types */

#include <QGLFramebufferObject>

#define FBO_TYPE GL_TEXTURE_2D
//#define FBO_FORMAT GL_RGBA16F_ARB
#define FBO_FORMAT GL_RGBA32F_ARB

/** A pixels_struct specifies a rectangle of pixels 
 * in one or two buffers of one framebuffer object 
 **/
typedef struct 
{
  GLfloat x;
  GLfloat y;
  GLfloat width;
  GLfloat height;
  //  QGLFramebufferObject* fbo; /* framebuffer object handle */  
  GLuint* fbo; /* framebuffer object handle */  

  GLuint buffers[3]; /* e.g. GL_BACK or GL_COLOR_ATTACHMENT0_EXT */
  GLuint textures[3]; /* texture handle */
  int buffersCount;
} pixels_struct;

typedef enum 
  {
    RS_BUFFER0,
    RS_BUFFER1,
    RS_BUFFER2,
    RS_BUFFER3,
    RS_BUFFER4,
    RS_BUFFER5,
    RS_TIME
  } render_state_enum;

typedef enum 
  {
    EMPTY,
    PROJECTION,
    ANALYSIS,
    COPY,
    SYNTHESIS,
    PHONG,
    SHOW
  } phase_enum;

typedef enum 
  {
    FBS_SYSTEM_PROVIDED,
    FBS_APPLICATION_CREATED,
    FBS_UNDEFINED
  } framebuffer_state_enum;
