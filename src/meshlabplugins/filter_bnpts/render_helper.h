#ifndef ALIGNSET_H
#define ALIGNSET_H

#include <QString>
#include <QImage>
#include <QGLFramebufferObject>


class QGLFramebufferObject;

class RenderHelper {

 public:

  GLuint vbo, nbo, cbo, ibo;  // vertex buffer object (vertices, normals, colors, indices)

  GLint programs[6];
  
  enum RenderingMode {FLAT=0, NORMAL=1, COLOR=2};
  RenderingMode rendmode;

  //buffers for rendered images 
  unsigned char *render; 
  unsigned char *color; 
  unsigned char *normal; 
  unsigned char *depth; 

  RenderHelper();
  ~RenderHelper();

  void initializeGL();



 private:
  
  GLuint createShaderFromFiles(QString basename); // converted into shader/basename.vert .frag
  GLuint createShaders(const char *vert, const char *frag);

};

#endif
