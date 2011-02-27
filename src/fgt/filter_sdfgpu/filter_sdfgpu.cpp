//#include <GL/glew.h>
#include "filter_sdfgpu.h"
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/edges.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/intersection.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/math/matrix33.h>
#include <wrap/qt/to_string.h>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>

#include <stdio.h>
using namespace std;
using namespace vcg;


#define SDF_MAX_TEXTURE_SIZE 1024

SdfGpuPlugin::SdfGpuPlugin()
: SingleMeshFilterInterface("Compute SDF GPU"),
  depthPeelingShaderProgram(-1),
  fboSize(256),
  colorFormat(GL_RGBA32F_ARB),
  dataTypeFP(GL_FLOAT),
  maxTexSize(16)
{

}

void SdfGpuPlugin::initParameterSet(MeshDocument&, RichParameterSet& par)
{
  qDebug() << "called here!";
  QStringList onPrimitive; onPrimitive.push_back("On vertices"); onPrimitive.push_back("On Faces");
  par.addParam( new RichEnum("onPrimitive", 0, onPrimitive, "Metric:",
                             "Choose whether to trace rays from faces or from vertices. "
                             "Recall that tracing from vertices will use vertex normal "
                             "estimation."));

  par.addParam(  new RichInt("numberRays", 10, "Number of rays: ",
                             "The standard deviation of the rays that will be casted around "
                             "the anti-normals. Remember that most sampled directions are "
                             "expected to fall within 3x this value."));
 /* par.addParam(new RichFloat("lowQuantile", .1, "Bottom quantile",
                             "We will throw away the set of ray distances for each cone which distance "
                             "value falls under this quantile. Value in between [0,1]. 0 Implies all "
                             "values are kept"));
  par.addParam(new RichFloat("hiQuantile", .9, "Top quantile",
                             "We will throw away the set of ray distances for each cone which distance "
                             "value falls under this quantile. Value in between [0,1]. 1 Implies all "
                             "values are kept"));*/

  par.addParam(new RichInt("peelingIteration", 4, "Peeling Iteration",
                             "Number of depth peeling iteration"));

  par.addParam(new RichFloat("peelingTolerance", 0.0005f, "Peeling Tolerance",
                             "We will throw away the set of ray distances for each cone which distance " ));
}


bool SdfGpuPlugin::applyFilter(MeshDocument& md, RichParameterSet& pars, vcg::CallBackPos* cb){

  //--- Retrieve parameters
  ONPRIMITIVE  onPrimitive = (ONPRIMITIVE) pars.getInt("onPrimitive");
  unsigned int numViews = pars.getInt("numberRays");
  int          peel = pars.getInt("peelingIteration");
  float        tolerance = pars.getFloat("peelingTolerance");
  assert( onPrimitive==ON_VERTICES && "Face mode not supported yet" );

  MeshModel* mm = md.mm();
  CMeshO&    cm = mm->cm;


  //---GL init
  initGL(cm.vn);
  setupMesh( md, onPrimitive );
  checkGLError::qDebug("GL Init failed");

  std::vector<Point3f> unifDirVec;
  GenNormal<float>::Uniform(numViews,unifDirVec);

  vector<vcg::Point3f>::iterator vi;
  for(vi = unifDirVec.begin(); vi != unifDirVec.end(); vi++)
    TraceRays(peel, tolerance, *vi, md.mm());

  checkGLError::qDebug("Depth peeling failed");

  //mesh clean up
  //md.mm()->glw.ClearHint(vcg::GLW::HNUseVBO);

  delete mDeepthPeelingProgram;
  releaseGL();

  checkGLError::qDebug("GL release failed");

  return true;
}

bool SdfGpuPlugin::initGL(unsigned int numVertices)
{
    this->glContext->makeCurrent();

    //******* SET DEFAULT OPENGL STUFF **********/
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glClearColor(0,0,1,0);
    glClearDepth(1.0);

    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
            Log(0,(const char*)glewGetErrorString(err));
            return false;
    }
    //**********INIT FBOs for depth peeling***********
    vsA = new Vscan(fboSize, fboSize);
    vsB = new Vscan(fboSize, fboSize);

    vsA->init();
    vsB->init();

    //******* QUERY HARDWARE FOR: MAX TEX SIZE ********/
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&maxTexSize) );

    Log(0, "QUERY HARDWARE FOR: MAX TEX SIZE: %i ", maxTexSize );
    maxTexSize = std::min(maxTexSize, (unsigned int)SDF_MAX_TEXTURE_SIZE);

    if ((maxTexSize*maxTexSize) < numVertices)
    {
            Log(0, "That's a really huge model, I can't handle it in hardware, sorry..");
            return false;
    }

    mDeepthPeelingProgram = new GPUProgram("",":/SdfGpu/shaders/shaderDepthPeeling.fs","");
    mDeepthPeelingProgram->enable();
    mDeepthPeelingProgram->addUniform("textureLastDepth");
    mDeepthPeelingProgram->addUniform("tolerance");
    mDeepthPeelingProgram->addUniform("oneOverBufSize");
    mDeepthPeelingProgram->disable();
}

void SdfGpuPlugin::releaseGL()
{
    useDefaultShader();
    useScreenAsDest();

    delete vsA;
    delete vsB;

    this->glContext->doneCurrent();
}

void SdfGpuPlugin::fillFrameBuffer(bool front,  MeshModel* mm)
{
    (front) ? glClearColor(0,1,0,1) : glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glCullFace((front)?GL_BACK:GL_FRONT);

  //  glPushMatrix();

   mm->glw.DrawFill<GLW::NMPerVert, GLW::CMPerVert, GLW::TMNone>();

   // mm->glw.DrawPointsBase<GLW::NMPerVert, GLW::CMPerVert>();
    //glPopMatrix();
}

void SdfGpuPlugin::setupMesh(MeshDocument& md, ONPRIMITIVE onPrimitive )
{
    MeshModel* mm = md.mm();
    CMeshO& m     = mm->cm;

    //--- If on vertices, do some cleaning first
    if( onPrimitive == ON_VERTICES )
    {
      int dup = tri::Clean<CMeshO>::RemoveDuplicateVertex(m);
      int unref =  tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
      if (dup > 0 || unref > 0) Log("Removed %i duplicate and %i unreferenced vertices\n",dup,unref);
    }

    //--- Updating mesh metadata
    tri::UpdateBounding<CMeshO>::Box(m);
    tri::UpdateNormals<CMeshO>::PerFaceNormalized(m);
    tri::UpdateNormals<CMeshO>::PerVertexAngleWeighted(m);
    tri::UpdateNormals<CMeshO>::NormalizeVertex(m);
    tri::UpdateFlags<CMeshO>::FaceProjection(m);

    //--- Enable & Reset the necessary attributes
    switch(onPrimitive)
    {
      case ON_VERTICES:
        mm->updateDataMask(MeshModel::MM_VERTQUALITY);
        tri::UpdateQuality<CMeshO>::VertexConstant(m,0);
        break;
      case ON_FACES:
        mm->updateDataMask(MeshModel::MM_FACEQUALITY);
        tri::UpdateQuality<CMeshO>::FaceConstant(m,0);
        break;
    }

    //--- Use VBO
  //  mm->glw.SetHint(vcg::GLW::HNUseVBO);
    mm->glw.Update();
}

void SdfGpuPlugin::useDepthPeelingShader()
{
    mDeepthPeelingProgram->enable();
    mDeepthPeelingProgram->setUniform1i( "textureLastDepth", 0 );
}

void SdfGpuPlugin::setDepthPeelingTolerance(float t)
{
    mDeepthPeelingProgram->setUniform1f( "tolerance", t );
}

void SdfGpuPlugin::setDepthPeelingSize(const Vscan & scan)
{
    float f[2];
    f[0] = 1.0f/scan.sizeX();
    f[1] = 1.0f/scan.sizeY();
    mDeepthPeelingProgram->setUniform2f("oneOverBufSize", f[0], f[1]);
}


void SdfGpuPlugin::useScreenAsDest()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

void SdfGpuPlugin::useDefaultShader()
{
    glUseProgram(0);
}

void SdfGpuPlugin::setCamera(Point3f camDir, Box3f &meshBBox)
{
    GLfloat d = (meshBBox.Diag()/2.0) * 1.1,
            k = 0.1f;
    Point3f eye = meshBBox.Center() + camDir * (d+k);

    glViewport(0.0, 0.0, fboSize, fboSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-d, d, -d, d, k, k+(2.0*d) );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.X(), eye.Y(), eye.Z(),
                      meshBBox.Center().X(), meshBBox.Center().Y(), meshBBox.Center().Z(),
                      0.0, 1.0, 0.0);
}


void SdfGpuPlugin::TraceRays(int peelingIteration, float tolerance, const Point3f& dir, MeshModel* mm )
{
    for( int i = 0;  i < peelingIteration; i++ )
    {
       if( i == 0 )
              useDefaultShader();
        else
        {
              vsB->useAsSource();
              useDepthPeelingShader();
              setDepthPeelingTolerance(tolerance);
              setDepthPeelingSize(*vsB);
        }

        vsA->useAsDest();
        setCamera(dir, mm->cm.bbox);
        fillFrameBuffer(i%2==0, mm);

        drawVertexMarkers();

        std::swap<Vscan*>(vsA,vsB);
   }
}

void SdfGpuPlugin::drawVertexMarkers()
{
//TODO
}


bool SdfGpuPlugin::checkFramebuffer()
{
        GLenum fboStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

        if ( fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
        {
                switch (fboStatus)
                {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                        Log(0, "FBO Incomplete: Attachment");
                        break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                        Log(0, "FBO Incomplete: Missing Attachment");
                        break;
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                        Log(0, "FBO Incomplete: Dimensions");
                        break;
                case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                        Log(0, "FBO Incomplete: Formats");
                        break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                        Log(0, "FBO Incomplete: Draw Buffer");
                        break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                        Log(0, "FBO Incomplete: Read Buffer");
                        break;
                default:
                        Log(0, "Undefined FBO error");
                        assert(0);
                }

                return false;
        }

        return true;
}


Q_EXPORT_PLUGIN(SdfGpuPlugin)
