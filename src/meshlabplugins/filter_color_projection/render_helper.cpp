#include <GL/glew.h>

#include <QGLContext>
#include <QGLFramebufferObject>

#include "render_helper.h"

using namespace std;

RenderHelper::RenderHelper() 
{
  rendmode = FLAT;
  
  color = NULL;
  depth = NULL;

  mindepth = 0;
  maxdepth = 0;

  vbo = 0; 
  nbo = 0; 
  cbo = 0;
  ibo = 0;
}


RenderHelper::~RenderHelper() 
{
  if(color != NULL)  delete []color;
  if(depth != NULL)  delete []depth;
}


int RenderHelper::initializeGL(vcg::CallBackPos *cb) 
{
  GLenum err = glewInit();
  if(cb) cb( 0, "GL Initialization");

  if (GLEW_OK != err) 
  {
    //Log(0, "GLEW initialization error!");
    return -1;
  }

  if (!glewIsSupported("GL_EXT_framebuffer_object")) 
  {
    //Log(0, "Graphics hardware does not support FBOs");
    return -1;
  }
  if (!glewIsSupported("GL_ARB_vertex_shader") || !glewIsSupported("GL_ARB_fragment_shader") ||
      !glewIsSupported("GL_ARB_shader_objects") || !glewIsSupported("GL_ARB_shading_language")) 
  {
    //Log(0, "Graphics hardware does not fully support Shaders");
  }

  if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) 
  {
    //Log(0,"Graphics hardware does not support non-power-of-two textures");
    return -1;
  }

  if (!glewIsSupported("GL_ARB_vertex_buffer_object")) 
  {
    //Log(0, "Graphics hardware does not support vertex buffer objects");
    return -1;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GLfloat light_position[] = {-600.0f, 500.0f, 700.0f, 0.0f};
  GLfloat light_ambient[]  = {0.1f,  0.1f, 0.1f, 1.0f};
  GLfloat light_diffuse[]  = {0.8f,  0.8f, 0.8f, 1.0f};
  GLfloat light_specular[] = {0.9f,  0.9f, 0.9f, 1.0f};

  glEnable(GL_LIGHTING);
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);
  glLightfv (GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glEnable  (GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_NORMALIZE);
  glDepthRange (0.0, 1.0);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POLYGON_SMOOTH);
  glShadeModel(GL_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);

  assert(glGetError() == 0);

  if(cb) cb( 10, "GL Initialization done");

  programs[FLAT] = createShaders("void main() { gl_Position = ftransform(); }",
    "void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }");
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

  if(cb) cb( 20, "Shaders created");

  return 0;
}

int RenderHelper::initializeMeshBuffers(MeshModel *mesh, vcg::CallBackPos *cb) 
{
  vcg::Point3f *vertices = new vcg::Point3f[mesh->cm.vn];
  vcg::Point3f *normals = new vcg::Point3f[mesh->cm.vn];
  vcg::Color4b *colors = new vcg::Color4b[mesh->cm.vn];
  unsigned int *indices = new unsigned int[mesh->cm.fn*3];

  for(int i = 0; i < mesh->cm.vn; i++) 
  {
    vertices[i] = mesh->cm.vert[i].P();
    normals[i] = mesh->cm.vert[i].N();
    colors[i] = mesh->cm.vert[i].C();
  }


  for(int i = 0; i < mesh->cm.fn; i++) 
    for(int k = 0; k < 3; k++) 
      indices[k+i*3] =  mesh->cm.face[i].V(k) - &*mesh->cm.vert.begin();

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->cm.vn*sizeof(vcg::Point3f), 
                  vertices, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->cm.vn*sizeof(vcg::Point3f), 
                  normals, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, cbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh->cm.vn*sizeof(vcg::Color4b), 
                  colors, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mesh->cm.fn*3*sizeof(unsigned int), 
                  indices, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


  // it is safe to delete after copying data to VBO
  delete []vertices;
  delete []normals;
  delete []colors;

  if(cb) cb( 40, "Mesh Buffers ready");

  return 0;
}

void RenderHelper::renderScene(vcg::Shotf &view, MeshModel *mesh, RenderingMode mode, float camNear = 0, float camFar = 0) 
{
  int wt = view.Intrinsics.ViewportPx[0];
  int ht = view.Intrinsics.ViewportPx[1];

  rendmode = mode;

  QSize fbosize(wt, ht);
  QGLFramebufferObjectFormat frmt;
  frmt.setInternalTextureFormat(GL_RGBA);
  frmt.setAttachment(QGLFramebufferObject::Depth);
  QGLFramebufferObject fbo(fbosize,frmt);

  float _near, _far;

  if((camNear == 0) || (camFar == 0))  // if not provided by caller, then evaluate using bbox
  {
    _near=0.1;
    _far=20000;

    GlShot< vcg::Shot<float> >::GetNearFarPlanes(view, mesh->cm.bbox, _near, _far);
    if(_near <= 0) _near = 0.1;
    if(_far < _near) _far = 1000;
  }
  else
  {
    _near = camNear;
    _far  = camFar;
  }

  assert(_near <= _far);

  GLenum err = glGetError();

  //render to FBO
  fbo.bind();

  glViewport(0, 0, wt, ht);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GlShot< vcg::Shot<float> >::SetView(view, _near, _far);

	err = glGetError();

  bool use_colors=false;
  bool use_normals=false;

  if(rendmode == NORMAL)
    use_normals = true;

  if(rendmode == COLOR)
    use_colors = true;

  int program = programs[rendmode]; 

  glDisable(GL_LIGHTING);
  //bind indices
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);

  //bind vertices
  glEnable(GL_COLOR_MATERIAL);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glEnableClientState(GL_VERTEX_ARRAY);                 // activate vertex coords array
  glVertexPointer(3, GL_FLOAT, 0, 0);                   // last param is offset, not ptr

  err = glGetError();

  glUseProgram(program);

  err = glGetError();

  if(use_colors) 
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, cbo);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
  }
  if(use_normals) 
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbo);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 0);
  }

  err = glGetError();


  if (mesh->cm.fn > 0)
  {
    glDrawElements(GL_TRIANGLES, mesh->cm.fn*3, GL_UNSIGNED_INT, (void *)0); 
  }
  else 
  {
    glDrawArrays(GL_POINTS, 0, mesh->cm.vn);
  }

  if(color != NULL)  delete []color;
  if(depth != NULL)  delete []depth;

  color  = new unsigned char[wt * ht * 3];
  depth  = new float [wt * ht];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glReadPixels( 0, 0, wt, ht, GL_RGB, GL_UNSIGNED_BYTE, color);
  glReadPixels( 0, 0, wt, ht, GL_DEPTH_COMPONENT, GL_FLOAT, depth);


  //----- convert depth in world units
  mindepth =  1000000;
  maxdepth = -1000000;
	for(int pixit = 0; pixit<wt*ht; pixit++)
	{
	 if (depth[pixit] == 1.0)
		depth[pixit] = 0;
	 else
		depth[pixit] = _near*_far / (_far - depth[pixit]*(_far-_near));

   // min and max for normalization purposes (e.g. weighting)
   if(depth[pixit] > maxdepth)
     maxdepth = depth[pixit];
   if(depth[pixit] > maxdepth)
     maxdepth = depth[pixit];
	}

  //-----

/*
{                      // debug dump buffer to pfm (portable float map, open with hdrview o hdrshop) to check values
 FILE* miofile;
 int res;
 char buff[64];

 miofile = fopen("depth.pfm", "wb");

 fprintf(miofile,"PF\n");
 fprintf(miofile,"%i %i\n",wt,ht);

 fprintf(miofile,"-1.000000\n");

 for(int kk=0; kk< wt*ht; kk++)
 {
  res = fwrite(&(depth[kk]), sizeof(float), 1, miofile);
  res = fwrite(&(depth[kk]), sizeof(float), 1, miofile);
  res = fwrite(&(depth[kk]), sizeof(float), 1, miofile);
 }

 fclose(miofile);
}
*/

  err = glGetError();

  glDisableClientState(GL_VERTEX_ARRAY);                // deactivate vertex array
  if(use_colors) glDisableClientState(GL_COLOR_ARRAY);
  if(use_normals) glDisableClientState(GL_NORMAL_ARRAY);

  err = glGetError();

  // bind with 0, so, switch back to normal pointer operation
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  glEnable(GL_LIGHTING);

  // standard opengl pipeline is re-activated
  glUseProgram(0);

  GlShot< vcg::Shot<float> >::UnsetView();

  glFinish();
  QImage l=fbo.toImage();
  l.save("rendering.jpg");

  fbo.release();
}


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

GLuint RenderHelper::createShaderFromFiles(QString name) 
{
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

GLuint RenderHelper::createShaders(const char *vert, const char *frag) 
{
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