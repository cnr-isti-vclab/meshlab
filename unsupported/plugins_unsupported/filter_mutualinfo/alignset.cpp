#include <iostream>

#include <GL/glew.h>

#include <QGLContext>
#include <QDomNode>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QGLFramebufferObject>


#include "alignset.h"

#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>
//#include <vcg/math/shot.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <wrap/io_trimesh/import_ply.h>

#include "shutils.h"

using namespace std;

AlignSet::AlignSet(): mode(COMBINE),
target(NULL), render(NULL),
vbo(0), nbo(0), cbo(0), ibo(0), error(0){

  box.SetNull();
  correspList = new QList<PointCorrespondence*>();
  imageRatio=1;
}

AlignSet::~AlignSet() {
  if(target) delete []target;
  if(render) delete []render;
  delete correspList;
}

void AlignSet::initializeGL() {

  programs[COLOR] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");
  programs[NORMALMAP] = createShaders("varying vec3 normal; void main() { normal = gl_NormalMatrix * gl_Normal;	gl_Position = ftransform(); }",
    "varying vec3 normal; void main() { "
    "vec3 color = normalize(normal); color = color * 0.5 + 0.5; gl_FragColor = vec4(color, 1.0); }");
  programs[COMBINE] = createShaders("varying vec3 normal; varying vec4 color; void main() { "
    "normal = gl_NormalMatrix * gl_Normal; gl_Position = ftransform(); color = gl_Color; }",
    "varying vec3 normal; varying vec4 color; void main() { "
    "vec3 ncolor = normalize(normal); ncolor = ncolor * 0.5 + 0.5; "
    "float t = color.x*color.x; gl_FragColor = (1.0-t)*color + t*(vec4(ncolor, 1.0)); }");
  programs[SPECULAR] = createShaders("varying vec3 reflection; void main() { "
    "vec3 normal = normalize(gl_NormalMatrix * gl_Normal); vec4 position = gl_ModelViewMatrix * gl_Vertex; "
    "reflection = reflect(position.xyz, normal); gl_Position = ftransform(); }",
    "varying vec3 reflection; varying vec4 color; void main() { "
    "vec4 ncolor; ncolor.xyz = normalize(reflection); ncolor.w = 1.0; gl_FragColor = ncolor * 0.5 + 0.5; }");
  programs[SILHOUETTE] = createShaders("varying vec4 color; void main() { gl_Position = ftransform(); color = gl_Color; }",
    "varying vec4 color; void main() { gl_FragColor = color; }");

  programs[SPECAMB] = createShaders("varying vec3 reflection; varying vec4 color; void main() { "
    "vec3 normal = normalize(gl_NormalMatrix * gl_Normal); vec4 position = gl_ModelViewMatrix * gl_Vertex; "
    "reflection = reflect(position.xyz, normal); gl_Position = ftransform(); color = gl_Color; }",
    "varying vec3 reflection; varying vec4 color; void main() { "
    "vec3 ncolor = normalize(reflection); ncolor = ncolor * 0.5 + 0.5; "
    "float t = color.x*color.x; gl_FragColor = (1.0-t)*color + t*(vec4(ncolor, 1.0)); }");


  // generate a new VBO and get the associated ID
  glGenBuffersARB(1, &vbo);
  glGenBuffersARB(1, &nbo);
  glGenBuffersARB(1, &cbo);
  glGenBuffersARB(1, &ibo);
}

//resample image IF too big.
void AlignSet::resize(int max_side) {
  int w = image->width();
  int h = image->height();
  if(image->isNull()) {
    w =  1024;
    h = 768;
  }

  if(w > max_side) {
    h = h*max_side/w;
    w = max_side;
  }
  if(h > max_side) {
    w = w*max_side/h;
    h = max_side;
  }

  wt=w;
  ht=h;

  if(target) delete []target;
  if(render) delete []render;
  target = new unsigned char[w*h];
  render = new unsigned char[w*h];


  if(image->isNull()) return;
  //resize image and store values into render
  QImage im;
  if(w != image->width() || h != image->height())
    im = image->scaled(w, h, Qt::IgnoreAspectRatio); //Qt::KeepAspectRatio);
  else im = *image;
  //im.save("image.jpg");
  assert(w == im.width());
  assert(h == im.height());
  QColor color;
  int offset = 0;
  //equalize image
  int histo[256];
  memset(histo, 0, 256*sizeof(int));
  for (int y = h-1; y >= 0; y--) {
    for (int x = 0; x < w; x++) {
      color.setRgb(im.pixel(x, y));
      unsigned char c = (unsigned char)(color.red() * 0.3f + color.green() * 0.59f + color.blue() * 0.11f);
      target[offset] = c;
      histo[c]++;
      offset++;
    }
  }
#ifdef RESCALE_HISTO
  int cumulative[256];
  cumulative[0] = histo[0];
  for(int i = 1; i < 256; i++)
    cumulative[i] = cumulative[i-1] + histo[i];

  int min = 0;
  int max = 255;
  for(int i = 0; i < 256; i++) {
    if(cumulative[i] > 20) break;
    min = i;
  }

  //invert cumulative..
  cumulative[255] = histo[255];
  for(int i = 254; i >= 0; i--)
    cumulative[i] = cumulative[i+1] + histo[i];

  for(int i = 255; i >= 0; i--) {
    if(cumulative[i] > 20) break;
    max = i;
  }
  assert(max > min);
  //rescale between min and max (should use bresenham but I am lazy
  unsigned char equa[256];
  for(int i = 0; i < 256; i++) {
    if(i < min) equa[i] = 0;
    if(i > max) equa[i] = 255;
    equa[i] = (255*(i - min))/(max - min);
  }
  for(int i = 0; i < w*h; i++)
    target[i] = equa[target[i]];
#endif
}

void AlignSet::renderScene(vcg::Shot<float> &view, int component) {
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


//GLenum err = glGetError();

  //render to FBO
  fbo.bind();

  glViewport(0, 0, wt, ht);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GlShot< vcg::Shot<float> >::SetView(shot, 0.5*_near, 2*_far);

//	err = glGetError();

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

void AlignSet::readRender(int component) {
  QSize fbosize(wt,ht);
  QGLFramebufferObjectFormat frmt;
  frmt.setInternalTextureFormat(GL_RGBA);
  frmt.setAttachment(QGLFramebufferObject::Depth);
  QGLFramebufferObject fbo(fbosize,frmt);

  fbo.bind();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  switch(component) {
  case 0: glReadPixels( 0, 0, width(), height(), GL_RED, GL_UNSIGNED_BYTE, render); break;
  case 1: glReadPixels( 0, 0, width(), height(), GL_GREEN, GL_UNSIGNED_BYTE, render); break;
  case 2: glReadPixels( 0, 0, width(), height(), GL_BLUE, GL_UNSIGNED_BYTE, render); break;
  case 3: glReadPixels( 0, 0, width(), height(), GL_ALPHA, GL_UNSIGNED_BYTE, render); break;
  }
  QImage l=fbo.toImage();
  l.save("puppo.jpg");
  fbo.release();
}

GLuint AlignSet::createShaderFromFiles(QString name) {
  QString vert = "shaders/" + name + ".vert";
  QString frag = "shaders/" + name + ".frag";

  const char *vs_src = ShaderUtils::importShaders(vert.toAscii().data());
  if(!vs_src) {
    cerr << "Could not load shader: " << qPrintable(vert) << endl;
    return 0;
  }

  const char *fs_src = ShaderUtils::importShaders(frag.toAscii().data());
  if(!fs_src) {
    cerr << "Could not load shader: " << qPrintable(frag) << endl;
    return 0;
  }

  return createShaders(vs_src, fs_src);
}

GLuint AlignSet::createShaders(const char *vert, const char *frag) {
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vert, NULL);
  ShaderUtils::compileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &frag, NULL);
  ShaderUtils::compileShader(fs);

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);

  ShaderUtils::linkShaderProgram(prog);
  return prog;
}

