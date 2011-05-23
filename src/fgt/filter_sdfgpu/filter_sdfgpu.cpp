#include "filter_sdfgpu.h"

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/intersection.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/spatial_hashing.h>
#include <wrap/qt/to_string.h>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <stdio.h>
#include <assert.h>
using namespace std;
using namespace vcg;

#define SDF_MAX_TEXTURE_SIZE 1024
#define PI 3.14159265358979323846264;
#define USEVBO_BY_DEFAULT false


SdfGpuPlugin::SdfGpuPlugin()
: mPeelingTextureSize(256)
{
    typeList
    << SDF_SDF
    << SDF_CORRECTION_THIN_PARTS
    << SDF_OBSCURANCE;

    foreach(FilterIDType tt , types())
            actionList << new QAction(filterName(tt), this);

}

void SdfGpuPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &par)
{

    qDebug() << "called here!";
    QStringList onPrimitive; onPrimitive.push_back("On vertices"); onPrimitive.push_back("On Faces");
   par.addParam( new RichEnum("onPrimitive", 0, onPrimitive, "Metric:",
                    "Choose whether to trace rays from faces or from vertices. " ));
   par.addParam(  new RichInt("numberRays",128, "Number of rays: ",
                    "The standard deviation of the rays that will be casted around "
                    "the anti-normals."));
   par.addParam(new RichInt("DepthTextureSize", 512, "Depth texture size",
                    "Size of the depth texture for depth peeling"));
   par.addParam(new RichInt("peelingIteration", 4, "Peeling Iteration",
                                "Number of depth peeling iteration"));
   par.addParam(new RichFloat("peelingTolerance", 0.0000001f, "Peeling Tolerance",
                            "Depth tolerance used during depth peeling" ));
   par.addParam(new RichFloat("coneAngle",120,"Cone amplitude", "Cone amplitude around vertex normal. Rays are traced within this cone."));

   mAction = ID(action);

   switch(mAction)
    {
            case SDF_CORRECTION_THIN_PARTS:
                 par.addParam(new RichFloat("minDist", 1.0f, "Min distance",
                                 "Min distance to check too thin part of the mesh" ));
                 break;

            case SDF_OBSCURANCE:
                 par.addParam(new RichFloat("obscuranceExponent", 0.0f, "Obscurance Exponent",
                                          "Parameter that increase or decrease the exponential rise in obscurance function" ));
                 break;

            default:
                {
                    break;
                }
    }

   par.addParam(new RichBool("useVBO",USEVBO_BY_DEFAULT,"Use VBO if supported","By using VBO, Meshlab loads all the vertex structure in the VRam, greatly increasing rendering speed (for both CPU and GPU mode). Disable it if problem occurs"));

   if(mAction != SDF_OBSCURANCE)
   {
        par.addParam(new RichBool("removeFalse",false,"Remove false intersections","For each"
                                  "ray we check the normal at the point of intersection,"
                                  "and ignore intersections where the normal at the intersection"
                                  "points is in the same direction as the point-of-origin"
                                  "(the same direction is defined as an angle difference less"
                                  "than 90) "));

        par.addParam(new RichBool("removeOutliers",false,"Remove outliers","Remove outliers"));
   }
}

QString SdfGpuPlugin::filterName(FilterIDType filterId) const
{
        switch(filterId)
        {
                case SDF_SDF                   :  return QString("Shape diameter function");
                case SDF_CORRECTION_THIN_PARTS :  return QString("Correction of thin parts");
                case SDF_OBSCURANCE            :  return QString("Ambient obscurance");

                default : assert(0);
        }

        return QString("");
}

QString SdfGpuPlugin::filterInfo(FilterIDType filterId) const
{
        switch(filterId)
        {
                case SDF_SDF                   :  return QString("Calculate the shape diameter function on the mesh, you can visualize the result colorizing the mesh");
                case SDF_CORRECTION_THIN_PARTS :  return QString("Checks and corrects too thin parts of the model");
                case SDF_OBSCURANCE            :  return QString("Generates environment obscurances values for the loaded mesh");

                default : assert(0);
        }

        return QString("");
}

int SdfGpuPlugin::getRequirements(QAction *action)
{
        //no requirements needed
        return 0;
}

bool SdfGpuPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & pars, vcg::CallBackPos *cb)
{
  MeshModel* mm = md.mm();
  CMeshO&    cm = mm->cm;

  //RETRIEVE PARAMETERS
  ONPRIMITIVE  onPrimitive  = (ONPRIMITIVE) pars.getInt("onPrimitive");
  unsigned int numViews     = pars.getInt("numberRays");
  int          peel         = pars.getInt("peelingIteration");
  mTolerance                = pars.getFloat("peelingTolerance");
  mPeelingTextureSize       = pars.getInt("DepthTextureSize");
  mMinCos                   = vcg::math::Cos(math::ToRad(pars.getFloat("coneAngle")/2.0));
  mUseVBO                   = pars.getBool("useVBO");

  assert( onPrimitive==ON_VERTICES && "Face mode not supported yet" );

  std::vector<Point3f> coneDirVec;

  if(mAction == SDF_OBSCURANCE)
    mTau = pars.getFloat("obscuranceExponent");
  else
  {
      if(mAction == SDF_CORRECTION_THIN_PARTS)
            mMinDist = pars.getFloat("minDist");

      mRemoveFalse     = pars.getBool("removeFalse");
      mRemoveOutliers  = pars.getBool("removeOutliers");

      if(mRemoveOutliers)
      {
          //Uniform sampling of cone for GPU outliers removal

          GenNormal<float>::UniformCone(EXTRA_RAYS_REQUESTED, coneDirVec, math::ToRad(45.0), Point3f(0.0,0.0,1.0));


          for(int i = 0; i < EXTRA_RAYS_RESULTED; i++)
          {
              coneDirVec[i].Normalize();
              mConeRays[i]   = coneDirVec[i].X();
              mConeRays[i+1] = coneDirVec[i].Y();
              mConeRays[i+2] = coneDirVec[i].Z();

              Log(0, "Ray%i %f %f %f. Angle: %f", i,coneDirVec[i].X(),
                                                    coneDirVec[i].Y(),
                                                    coneDirVec[i].Z(),
                                                    math::ToDeg(vcg::Angle(coneDirVec[i],Point3f(0.0,0.0,1.0))));
          }
      }
   }
   //MESH CLEAN UP
   setupMesh( md, onPrimitive );

   //GL INIT
   if(!initGL(*mm)) return false;

   //
   vertexDataToTexture(*mm);

  //Uniform sampling of directions over a sphere
  std::vector<Point3f> unifDirVec;
  GenNormal<float>::Uniform(numViews,unifDirVec);


  Log(0, "Number of rays: %i ", unifDirVec.size() );
  Log(0, "Number of rays for GPU outliers removal: %i ", coneDirVec.size() );

  coneDirVec.clear();

  //Do the actual calculation of sdf or obscurance for each ray
  unsigned int tracedRays = 0;
  for(vector<vcg::Point3f>::iterator vi = unifDirVec.begin(); vi != unifDirVec.end(); vi++)
  {
       (*vi).Normalize();
        TraceRay(peel, (*vi), md.mm());
        cb(100*((float)tracedRays/(float)unifDirVec.size()), "Tracing rays...");
        ++tracedRays;
  }

  //read back the result texture and store result in the mesh
  if(mAction == SDF_OBSCURANCE)
      applyObscurance(*mm,unifDirVec.size());
  else
      applySdfHW(*mm,unifDirVec.size());

  //Clean & Exit
  releaseGL(*mm);

  return true;
}

bool SdfGpuPlugin::initGL(MeshModel& mm)
{
    const unsigned int numVertices = mm.cm.vn;

    this->glContext->makeCurrent();

    //SET DEFAULT OPENGL STUFF
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glClearColor(0,0,0,0);
    glClearDepth(1.0);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
            Log(0,(const char*)glewGetErrorString(err));
            return false;
    }

    //CHECK HARDWARE CAPABILITIES
    if (!glewIsSupported("GL_ARB_vertex_shader GL_ARB_fragment_shader"))
    {
            if (!glewIsSupported("GL_EXT_vertex_shader GL_EXT_fragment_shader"))
            {
                    Log(0, "Your hardware doesn't support Shaders, which are required for hw occlusion");
                    return false;
            }
    }
    if ( !glewIsSupported("GL_EXT_framebuffer_object") )
    {
            Log(0, "Your hardware doesn't support FBOs, which are required for hw occlusion");
            return false;
    }

    if ( glewIsSupported("GL_ARB_texture_float") )
    {
            if ( !glewIsSupported("GL_EXT_gpu_shader4") )   //Only DX10-grade cards support FP32 blending
            {
                    Log(0,"Your hardware can't do FP32 blending, and currently the FP16 version is not yet implemented.");
                    return false;
            }
    }
    else
    {
            Log(0,"Your hardware doesn't support floating point textures, which are required for hw occlusion");
            return false;
    }

    //INIT FBOs AND TEXs
    for(int i = 0; i < 3; i++)
    {
        mFboArray[i] = new FramebufferObject();
    }

    mFboResult = new FramebufferObject();

    unsigned int maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&maxTexSize) );
    Log(0, "QUERY HARDWARE FOR: MAX TEX SIZE: %i ", maxTexSize );

    //CHECK MODEL SIZE
    if ((maxTexSize*maxTexSize) < numVertices)
    {
            Log(0, "That's a really huge model, I can't handle it in hardware, sorry..");
            return false;
    }

    for( mResTextureDim = 16; mResTextureDim*mResTextureDim < numVertices; mResTextureDim *= 2 ){}

    Log(0, "Mesh has %i vertices\n", numVertices );
    Log(0, "Result texture is %i X %i = %i", mResTextureDim, mResTextureDim, mResTextureDim*mResTextureDim);

    mVertexCoordsTexture  = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );
    mVertexNormalsTexture = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );

    mResultTexture = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );
    mFboResult->attachTexture( mResultTexture->format().target(), mResultTexture->id(), GL_COLOR_ATTACHMENT0_EXT );

    //clear first time
    mFboResult->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    mFboResult->unbind();

    //We use 3 FBOs to avoid z-fighting in sdf and obscurance calculation, see TraceRays function for details
    for(int i = 0; i < 3; i++)
    {
        mDepthTextureArray[i]  = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mPeelingTextureSize, mPeelingTextureSize, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT ),
                                                     TextureParams( GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE ) );

        mFboArray[i]->attachTexture( mDepthTextureArray[i]->format().target(), mDepthTextureArray[i]->id(), GL_DEPTH_ATTACHMENT  );

        mFboArray[i]->bind();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        mFboArray[i]->unbind();
    }

    //Depth peeling shader used for both sdf and obscurance
    mDeepthPeelingProgram = new GPUProgram("",":/SdfGpu/shaders/shaderDepthPeeling.fs","");
    mDeepthPeelingProgram->enable();
    mDeepthPeelingProgram->addUniform("textureLastDepth");
    mDeepthPeelingProgram->addUniform("tolerance");
    mDeepthPeelingProgram->addUniform("oneOverBufSize");
    mDeepthPeelingProgram->disable();

    //Sdf shader
    mSDFProgram = new GPUProgram("",":/SdfGpu/shaders/calculateSdf.frag","");
    mSDFProgram->enable();
    mSDFProgram->addUniform("vTexture");
    mSDFProgram->addUniform("nTexture");
    mSDFProgram->addUniform("depthTextureFront");
    mSDFProgram->addUniform("depthTextureBack");
    mSDFProgram->addUniform("viewDirection");
    mSDFProgram->addUniform("mvprMatrix");
    mSDFProgram->addUniform("viewpSize");
    mSDFProgram->addUniform("texSize");
    mSDFProgram->addUniform("minCos");
    mSDFProgram->addUniform("depthTexturePrevBack");
    mSDFProgram->addUniform("firstRendering");
    mSDFProgram->addUniform("mvprMatrixINV");
    mSDFProgram->addUniform("removeFalse");
    mSDFProgram->addUniform("coneRays");
    mSDFProgram->addUniform("removeOutliers");
    mSDFProgram->disable();

    //Obscurance shader
    mObscuranceProgram = new GPUProgram("",":/SdfGpu/shaders/obscurances.frag","");
    mObscuranceProgram->enable();
    mObscuranceProgram->addUniform("vTexture");
    mObscuranceProgram->addUniform("nTexture");
    mObscuranceProgram->addUniform("depthTextureFront");
    mObscuranceProgram->addUniform("depthTextureBack");
    mObscuranceProgram->addUniform("depthTextureNextBack");
    mObscuranceProgram->addUniform("viewDirection");
    mObscuranceProgram->addUniform("mvprMatrix");
    mObscuranceProgram->addUniform("viewpSize");
    mObscuranceProgram->addUniform("texSize");
    mObscuranceProgram->addUniform("tau");
    mObscuranceProgram->addUniform("firstRendering");
    mObscuranceProgram->addUniform("maxDist");      //mesh BB diagonal
    mObscuranceProgram->disable();

    assert(mFboResult->isValid());
    assert(mFboArray[0]->isValid());
    assert(mFboArray[1]->isValid());
    assert(mFboArray[2]->isValid());


    //If required from the user, we use VBO to speed up mesh rendering
    if(mUseVBO)
    {
        mm.glw.SetHint(vcg::GLW::HNUseVBO);
        mm.glw.Update();
    }

    checkGLError::qDebug("GL Init failed");

    return true;
}

void SdfGpuPlugin::vertexDataToTexture(MeshModel &m)
{
        unsigned int texSize = mResTextureDim*mResTextureDim*4;

        GLfloat *vertexPosition= new GLfloat[texSize];
        GLfloat *vertexNormals = new GLfloat[texSize];
        vcg::Point3<CMeshO::ScalarType> vn;

        //Copies each vertex's position and normal in new vectors
        for (int i=0; i < m.cm.vn; ++i)
        {
                //Vertex position
                vertexPosition[i*4+0] = m.cm.vert[i].P().X();
                vertexPosition[i*4+1] = m.cm.vert[i].P().Y();
                vertexPosition[i*4+2] = m.cm.vert[i].P().Z();
                vertexPosition[i*4+3] = 1.0;

                //Normal vector for each vertex
                vn = m.cm.vert[i].N();
                vertexNormals[i*4+0] = vn.X();
                vertexNormals[i*4+1] = vn.Y();
                vertexNormals[i*4+2] = vn.Z();
                vertexNormals[i*4+3] = 0.0;
        }

         //Write vertex coordinates
        mVertexCoordsTexture->bind();
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mResTextureDim, mResTextureDim, 0, GL_RGBA, GL_FLOAT, vertexPosition);

        //Write normal directions
        mVertexNormalsTexture->bind();
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mResTextureDim, mResTextureDim, 0, GL_RGBA, GL_FLOAT, vertexNormals);

        delete [] vertexNormals;
        delete [] vertexPosition;
}

void SdfGpuPlugin::releaseGL(MeshModel &m)
{
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete mDeepthPeelingProgram;
    delete mSDFProgram;
    delete mObscuranceProgram;
    delete mFboResult;
    delete mResultTexture;
    delete mVertexCoordsTexture;
    delete mVertexNormalsTexture;

    for(int i = 0; i < 3; i++)
    {
        delete mFboArray[i];
        delete mDepthTextureArray[i];
    }

    if (mUseVBO)
    {
        m.glw.ClearHint(vcg::GLW::HNUseVBO);
    }

    checkGLError::qDebug("GL release failed");

    this->glContext->doneCurrent();
}

void SdfGpuPlugin::fillFrameBuffer(bool front,  MeshModel* mm)
{
   glClear(GL_DEPTH_BUFFER_BIT);
   glEnable(GL_CULL_FACE);
   glCullFace((front)?GL_BACK:GL_FRONT);
   //the most recent GPUs can do double speed Z-only rendering
   //(also alpha test must be turned off, depth replace and texkill must not be used in fragment shader)
   glColorMask(0, 0, 0, 0);
   mm->glw.DrawFill<GLW::NMNone, GLW::CMNone, GLW::TMNone>();
   glColorMask(1, 1, 1, 1);

   glDisable(GL_CULL_FACE);
}

void SdfGpuPlugin::setupMesh(MeshDocument& md, ONPRIMITIVE onPrimitive )
{
    MeshModel* mm = md.mm();
    CMeshO& m     = mm->cm;

    //If on vertices, do some cleaning first
    if( onPrimitive == ON_VERTICES )
    {
      int dup = tri::Clean<CMeshO>::RemoveDuplicateVertex(m);
      int unref =  tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
      if (dup > 0 || unref > 0) Log("Removed %i duplicate and %i unreferenced vertices\n",dup,unref);
    }

    //Updating mesh metadata
    tri::UpdateBounding<CMeshO>::Box(m);
    vcg::tri::Allocator<CMeshO>::CompactVertexVector(m);
    vcg::tri::Allocator<CMeshO>::CompactFaceVector(m);
   // vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);

    //Enable & Reset the necessary attributes
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
}

void SdfGpuPlugin::setCamera(Point3f camDir, Box3f &meshBBox)
{
    GLfloat d = (meshBBox.Diag()/2.0) * 1.1,
            k = 0.1f;
    Point3f eye = meshBBox.Center() + camDir * (d+k);

    glViewport(0.0, 0.0, mPeelingTextureSize, mPeelingTextureSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-d, d, -d, d, k, k+(2.0*d) );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.X(), eye.Y(), eye.Z(),
                      meshBBox.Center().X(), meshBBox.Center().Y(), meshBBox.Center().Z(),
                      0.0, 1.0, 0.0);
}

void SdfGpuPlugin::useDepthPeelingShader(FramebufferObject* fbo)
{
   glUseProgram(mDeepthPeelingProgram->id());
   mDeepthPeelingProgram->setUniform1f("tolerance", mTolerance);
   mDeepthPeelingProgram->setUniform2f("oneOverBufSize", 1.0f/mPeelingTextureSize, 1.0f/mPeelingTextureSize);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, fbo->getAttachedId(GL_DEPTH_ATTACHMENT));
   mDeepthPeelingProgram->setUniform1i("textureLastDepth",0);
}

void SdfGpuPlugin::calculateSdfHW(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* fboPrevBack, const vcg::Point3f& cameraDir)
{
    mFboResult->bind();
    glViewport(0, 0, mResTextureDim, mResTextureDim);
    GLfloat mv_pr_Matrix_f[16];  // modelview-projection matrix

    glGetFloatv(GL_MODELVIEW_MATRIX, mv_pr_Matrix_f);
    glMatrixMode(GL_PROJECTION);
    glMultMatrixf(mv_pr_Matrix_f);
    glGetFloatv(GL_PROJECTION_MATRIX, mv_pr_Matrix_f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glUseProgram(mSDFProgram->id());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboFront->getAttachedId(GL_DEPTH_ATTACHMENT));
    mSDFProgram->setUniform1i("depthTextureFront",0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboBack->getAttachedId(GL_DEPTH_ATTACHMENT));
    mSDFProgram->setUniform1i("depthTextureBack",1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mVertexCoordsTexture->id());
    mSDFProgram->setUniform1i("vTexture",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mVertexNormalsTexture->id());
    mSDFProgram->setUniform1i("nTexture",3);

    //previous depth layer, see TraceRays function for an explanation
    if(fboPrevBack)
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, fboPrevBack->getAttachedId(GL_DEPTH_ATTACHMENT));
        mSDFProgram->setUniform1i("depthTexturePrevBack",4);

    }

    // Set view direction
    mSDFProgram->setUniform3f("viewDirection", cameraDir.X(), cameraDir.Y(), cameraDir.Z());

    // Set ModelView-Projection Matrix
    mSDFProgram->setUniformMatrix4fv( "mvprMatrix", mv_pr_Matrix_f, 1, GL_FALSE );

    vcg::Matrix44f mvprINV(mv_pr_Matrix_f);
    mvprINV.transposeInPlace();
    vcg::Invert(mvprINV);
    mSDFProgram->setUniformMatrix4fv( "mvprMatrixINV", mvprINV.V(), 1, GL_TRUE );

    // Set texture Size
    mSDFProgram->setUniform1f("texSize", mPeelingTextureSize);

    // Set viewport Size
    mSDFProgram->setUniform1f("viewpSize", mResTextureDim );

    mSDFProgram->setUniform1f("minCos", mMinCos);

    //just a flag to know how many layers to use for z-fighting removal
    if(fboPrevBack == NULL)
        mSDFProgram->setUniform1i("firstRendering",1);
    else
         mSDFProgram->setUniform1i("firstRendering",0);

    if(mRemoveFalse)
        mSDFProgram->setUniform1i("removeFalse",1);
    else
         mSDFProgram->setUniform1i("removeFalse",0);

    if(mRemoveOutliers)
    {
        mSDFProgram->setUniform1i("removeOutliers",1);
        mSDFProgram->setUniform3fv("coneRays", mConeRays, EXTRA_RAYS_RESULTED);
    }
    else
        mSDFProgram->setUniform1i("removeOutliers",0);

    // Screen-aligned Quad
    glBegin(GL_QUADS);
            glVertex3f(-1.0f, -1.0f, 0.0f); //L-L
            glVertex3f( 1.0f, -1.0f, 0.0f); //L-R
            glVertex3f( 1.0f,  1.0f, 0.0f); //U-R
            glVertex3f(-1.0f,  1.0f, 0.0f); //U-L
    glEnd();

    mFboResult->unbind();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

}

void SdfGpuPlugin::applySdfHW(MeshModel &m, float numberOfRays)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.vn; ++i)
    {
        //weighted average: sdf sum is in the red channel and the weights sum in the green one
        m.cm.vert[i].Q() = (result[i*4+1]>0.0) ? (result[i*4] / result[i*4+1]) : 0.0;
    }

    mFboResult->unbind();

    delete [] result;
}

void SdfGpuPlugin::calculateObscurance(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* nextBack, const vcg::Point3f& cameraDir, float bbDiag)
{
    mFboResult->bind();
    glViewport(0, 0, mResTextureDim, mResTextureDim);
    GLfloat mv_pr_Matrix_f[16];  // modelview-projection matrix

    glGetFloatv(GL_MODELVIEW_MATRIX, mv_pr_Matrix_f);
    glMatrixMode(GL_PROJECTION);
    glMultMatrixf(mv_pr_Matrix_f);
    glGetFloatv(GL_PROJECTION_MATRIX, mv_pr_Matrix_f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glUseProgram(mObscuranceProgram->id());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboFront->getAttachedId(GL_DEPTH_ATTACHMENT));
    mObscuranceProgram->setUniform1i("depthTextureFront",0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboBack->getAttachedId(GL_DEPTH_ATTACHMENT));
    mObscuranceProgram->setUniform1i("depthTextureBack",1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mVertexCoordsTexture->id());
    mObscuranceProgram->setUniform1i("vTexture",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mVertexNormalsTexture->id());
    mObscuranceProgram->setUniform1i("nTexture",3);

    //next depth layer, see TraceRays function for an explanation
    if(nextBack != NULL)
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, nextBack->getAttachedId(GL_DEPTH_ATTACHMENT));
        mObscuranceProgram->setUniform1i("depthTextureNextBack",4);
    }

    // Set view direction
    mObscuranceProgram->setUniform3f("viewDirection", cameraDir.X(), cameraDir.Y(), cameraDir.Z());

    // Set ModelView-Projection Matrix
    mObscuranceProgram->setUniformMatrix4fv( "mvprMatrix", mv_pr_Matrix_f, 1, GL_FALSE );

    // Set texture Size
    mObscuranceProgram->setUniform1f("texSize", mPeelingTextureSize);

    // Set viewport Size
    mObscuranceProgram->setUniform1f("viewpSize", mResTextureDim );

    mObscuranceProgram->setUniform1f("tau", mTau);

    mObscuranceProgram->setUniform1f("maxDist", bbDiag);

    //just a flag to know how many layers to use for z-fighting removal
    if(nextBack == NULL)
        mObscuranceProgram->setUniform1i("firstRendering",1);
    else
         mObscuranceProgram->setUniform1i("firstRendering",0);

    // Screen-aligned Quad
    glBegin(GL_QUADS);
            glVertex3f(-1.0f, -1.0f, 0.0f); //L-L
            glVertex3f( 1.0f, -1.0f, 0.0f); //L-R
            glVertex3f( 1.0f,  1.0f, 0.0f); //U-R
            glVertex3f(-1.0f,  1.0f, 0.0f); //U-L
    glEnd();

    mFboResult->unbind();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void SdfGpuPlugin::applyObscurance(MeshModel &m, float numberOfRays)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for( unsigned int i = 0; i < m.cm.vn; i++)
    {
        m.cm.vert[i].Q() = result[i*4]/numberOfRays;
    }

    tri::UpdateColor<CMeshO>::VertexQualityGray(m.cm);

    mFboResult->unbind();
    delete [] result;
}

void SdfGpuPlugin::TraceRay(int peelingIteration,const Point3f& dir, MeshModel* mm )
{
    unsigned int j = 0;

    for( int i = 0;  i < peelingIteration; i++ )
    {
        if( i == 0 )
              glUseProgram(0);
        else
        {
              if(j > 0)
                  useDepthPeelingShader(mFboArray[j-1]);
               else
                   useDepthPeelingShader(mFboArray[2]);
        }

        mFboArray[j]->bind();

        setCamera(dir, mm->cm.bbox);

        fillFrameBuffer(i%2==0, mm);

        mFboArray[j]->unbind();

         //we use 3 FBOs to avoid z-fighting (Inspired from Woo's shadow mapping method)
         if(i%2)
         {
              //we use the same method as in sdf, see below
              if(mAction==SDF_OBSCURANCE )
              {
                 if(i>1)
                 {
                      int prevBack  = (j+1)%3;
                      int front     = (j==0)? 2 : (j-1);
                      calculateObscurance( mFboArray[front], mFboArray[prevBack], mFboArray[j], dir, mm->cm.bbox.Diag());//front prevBack Back
                 }
                 else
                 {
                     assert(j!=0);
                     calculateObscurance( mFboArray[j-1], mFboArray[j], NULL, dir, mm->cm.bbox.Diag());//front back nextBack

                 }
              }
              else //SDF_SDF, SDF_CORRECTION_THIN_PARTS
              {
                  if(i>1)
                  {
                      //We are interested in vertices belonging to the front layer. Then in the shader we check that
                      //the vertex's depth is greater than the previous depth layer and smaller than the next one.
                      int prevBack  = (j+1)%3;
                      int prevFront = (j==0)? 2 : (j-1);
                      calculateSdfHW( mFboArray[prevFront], mFboArray[j], mFboArray[prevBack],dir );// front back prevback
                  }
                  else
                  {    //we have first and second depth layers, so we can use "second-depth shadow mapping" to avoid z-fighting
                        assert(j!=0);
                      calculateSdfHW( mFboArray[j-1], mFboArray[j], NULL, dir );// front back prevback
                  }
              }
          }

          //increment and wrap around
          j = (j+1) % 3;
   }

    assert(mFboResult->isValid());
    assert(mFboArray[0]->isValid());
    assert(mFboArray[1]->isValid());
    assert(mFboArray[2]->isValid());

    checkGLError::qDebug("Error during depth peeling");
}

Q_EXPORT_PLUGIN(SdfGpuPlugin)

