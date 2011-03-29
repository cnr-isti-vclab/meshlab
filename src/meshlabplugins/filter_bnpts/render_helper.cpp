#include <GL/glew.h>

#include <QGLContext>
#include <QGLFramebufferObject>

#include "render_helper.h"

using namespace std;

RenderHelper::RenderHelper(): 
mode(FLAT),
render(NULL), 
color(NULL),
normal(NULL),
depth(NULL), 
vbo(0), 
nbo(0), 
cbo(0), 
ibo(0)
{

}


RenderHelper::~RenderHelper() 
{
  if(render) delete []target;
  if(color)  delete []render;
  if(normal) delete []target;
  if(depth)  delete []render;
}


void AlignSet::initializeGL() {

  programs[FLAT] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");
  programs[NORMAL] = createShaders("varying vec3 normal; void main() { normal = gl_NormalMatrix * gl_Normal;	gl_Position = ftransform(); }",
    "varying vec3 normal; void main() { "
    "vec3 color = normalize(normal); color = color * 0.5 + 0.5; gl_FragColor = vec4(color, 1.0); }");
  programs[COLOR] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");

  // generate a new VBO and get the associated ID
  glGenBuffersARB(1, &vbo);
  glGenBuffersARB(1, &nbo);
  glGenBuffersARB(1, &cbo);
  glGenBuffersARB(1, &ibo);
}

void AlignSet::renderScene(vcg::Shotf &view, int component) 
{
  QSize fbosize(wt,ht);
  QGLFramebufferObjectFormat frmt;
  frmt.setInternalTextureFormat(GL_RGBA);
  frmt.setAttachment(QGLFramebufferObject::Depth);
  QGLFramebufferObject fbo(fbosize,frmt);

  float _near, _far;
  _near=0.1;
  _far=10000;

  GlShot< vcg::Shot<float> >::GetNearFarPlanes(view, mesh->bbox, _near, _far);
  //assert(_near <= _far);
  if(_near <= 0) _near = 0.1;
  if(_far < _near) _far = 1000;


  GLenum err = glGetError();

  //render to FBO
  fbo.bind();

  glViewport(0, 0, wt, ht);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GlShot< vcg::Shot<float> >::SetView(shot, 0.5*_near, 2*_far);

	err = glGetError();

  bool use_colors = false;
  bool use_normals = false;
  int program = programs[mode]; //standard pipeline
  switch(mode){
   case COLOR:
     use_colors = true;
     break;
   case NORMALMAP:
   case SPECULAR:
     use_normals = true;
     break;
   case COMBINE:
   case SPECAMB:
     use_colors = true;
     use_normals = true;
     break;
   case SILHOUETTE:
     break;
   default: assert(0);
  }
  glDisable(GL_LIGHTING);
  //bind indices
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

  //bind vertices
  glEnable(GL_COLOR_MATERIAL);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
  glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

//    err = glGetError();

  glUseProgram(program);

//    err = glGetError();

  if(use_colors) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, cbo);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
  }
  if(use_normals) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbo);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 0);
  }

//	err = glGetError();


  int start = 0;
  int tot = 30000;
  if (mesh->fn>0)
  {
    while(start < mesh->fn) {
      glDrawElements(GL_TRIANGLES, tot*3, GL_UNSIGNED_INT, (void *)(start*3*sizeof(int)));
      start += tot;
      if(start + tot > mesh->fn)
        tot = mesh->fn - start;
    }
  }
  else glDrawArrays(GL_POINTS, 0, mesh->vn);



  render = new unsigned char[wt*ht];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  switch(component) {
    case 0: glReadPixels( 0, 0, wt, ht, GL_RED, GL_UNSIGNED_BYTE, render); break;
    case 1: glReadPixels( 0, 0, wt, ht, GL_GREEN, GL_UNSIGNED_BYTE, render); break;
    case 2: glReadPixels( 0, 0, wt, ht, GL_BLUE, GL_UNSIGNED_BYTE, render); break;
    case 3: glReadPixels( 0, 0, wt, ht, GL_ALPHA, GL_UNSIGNED_BYTE, render); break;
    case 4: break;
  }

  //err = glGetError();

  glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array
  if(use_colors) glDisableClientState(GL_COLOR_ARRAY);
  if(use_normals) glDisableClientState(GL_NORMAL_ARRAY);

  //err = glGetError();

  // bind with 0, so, switch back to normal pointer operation
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  switch(mode) {
    case SILHOUETTE:
    case COLOR:
    case COMBINE:
    case NORMALMAP: glEnable(GL_LIGHTING); break;
    default: break;
  }

  // standard opengl pipeline is re-activated
  glUseProgram(0);

  GlShot< vcg::Shot<float> >::UnsetView();

  glFinish();
  /*QImage l=fbo.toImage();
  l.save("rendering.jpg");*/
  fbo.release();

}