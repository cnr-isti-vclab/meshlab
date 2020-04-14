#include "filter_sdfgpu.h"
#include <common/gl_defs.h>
#include <GL/glu.h>

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
#define USEVBO_BY_DEFAULT true
#define PIXEL_COUNT_THRESHOLD 100

SdfGpuPlugin::SdfGpuPlugin()
    : mPeelingTextureSize(256),
    mTempDepthComplexity(0),
    mDepthComplexity(0),
    mDepthComplexityWarning(false)
{
    typeList
        << SDF_SDF
        << SDF_DEPTH_COMPLEXITY
        << SDF_OBSCURANCE;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

void SdfGpuPlugin::initParameterSet(QAction *action, MeshModel &/*m*/, RichParameterSet &par)
{
    mAction = ID(action);
    QStringList onPrimitive; onPrimitive.push_back("On vertices"); onPrimitive.push_back("On Faces");
    par.addParam( new RichEnum("onPrimitive", 0, onPrimitive, "Metric:",
        "Choose whether to trace rays from faces or from vertices. " ));
    par.addParam(  new RichInt("numberRays",128, "Number of rays: ",
        "The number of rays that will be casted around "
        "the normals."));
    par.addParam(new RichInt("DepthTextureSize", 512, "Depth texture size",
        "Size of the depth texture for depth peeling. Higher resolutions provide better sampling of the mesh, with a small performance penalty."));
    par.addParam(new RichInt("peelingIteration", 10, "Peeling Iteration",
        "Number of depth peeling iteration. Actually is the maximum number of layers that a ray can hit while traversing the mesh. "
        "For example, in the case of a sphere, you should specify 2 in this parameter. For a torus, specify 4. "
        "<b>For more complex geometry you should run the depth complexity filter to know the exact value</b>."));
    par.addParam(new RichFloat("peelingTolerance", 0.0000001f, "Peeling Tolerance",
        "Depth tolerance used during depth peeling. This is the threshold used to differentiate layers between each others."
        "Two elements whose distance is below this value will be considered as belonging to the same layer."));

    if(mAction != SDF_DEPTH_COMPLEXITY)
        par.addParam(new RichFloat("coneAngle",120,"Cone amplitude", "Cone amplitude around normals in degrees. Rays are traced within this cone."));



    switch(mAction)
    {
    case SDF_OBSCURANCE:
        par.addParam(new RichFloat("obscuranceExponent", 0.1f, "Obscurance Exponent",
            "This parameter controls the spatial decay term in the obscurance formula. "
            "The greater the exponent, the greater the influence of distance; that is: "
            "even if a ray is blocked by an occluder its contribution to the obscurance term is non zero, but proportional to this parameter. "
            "It turs out that if you choose a value of zero, you get the standard ambient occlusion term. "
            "<b>(In this case, only a value of two, in the peeling iteration parameter, has a sense)</b>"));
        break;

    default:
        {
            break;
        }
    }

    if(mAction == SDF_SDF)
    {
        par.addParam(new RichBool("removeFalse",true,"Remove false intersections","For each"
            "ray we check the normal at the point of intersection,"
            "and ignore intersections where the normal at the intersection"
            "points is in the same direction as the point-of-origin"
            "(the same direction is defined as an angle difference less"
            "than 90) "));

        par.addParam(new RichBool("removeOutliers",false,"Remove outliers","The outliers removal is made on the fly with a supersampling of the depth buffer. "
            "For each ray that we trace, we take multiple depth values near the point of intersection and we output only the median of these values. "
            "Some mesh can benefit from this additional calculation. "));
    }
}

QString SdfGpuPlugin::filterName(FilterIDType filterId) const
{
    switch(filterId)
    {
    case SDF_SDF                   :  return QString("Shape Diameter Function");
    case SDF_DEPTH_COMPLEXITY      :  return QString("Depth complexity");
    case SDF_OBSCURANCE            :  return QString("Volumetric obscurance");

    default : assert(0);
    }

    return QString("");
}

QString SdfGpuPlugin::filterInfo(FilterIDType filterId) const
{
    switch(filterId)
    {
    case SDF_SDF                   :  return QString("Calculate the SDF (<b>shape diameter function</b>) on the mesh, you can visualize the result colorizing the mesh. "
                                          "The SDF is a scalar function on the mesh surface and represents the neighborhood diameter of the object at each point. "
                                          "Given a point on the mesh surface,"
                                          "several rays are sent inside a cone, centered around the point's inward-normal, to the other side of the mesh. The result is a weighted sum of all rays lengths. "
                                          "For further details, see the reference paper:<br>"
                                          "<b>Shapira Shamir Cohen-Or,<br>"
                                          "Consistent Mesh Partitioning and Skeletonisation using the shaper diamter function, Visual Comput. J. (2008)</b> ");
    case SDF_DEPTH_COMPLEXITY      :  return QString("Calculate the depth complexity of the mesh, that is: the maximum number of layers that a ray can hit while traversing the mesh. To have a correct value, you should specify and high value in the peeling iteration parameter. "
                                          "You can read the result in the MeshLab log window. <b>If warnings are not present, you have the exact value, otherwise try increasing the peeling iteration parameter. After having calculated the correct value,"
                                          "you can ignore further warnings that you may get using that value.</b>. ");
    case SDF_OBSCURANCE            :  return QString("Calculates obscurance coefficients for the mesh. Obscurance is introduced to avoid the "
                                          "disadvantages of both classical ambient term and ambient occlusion. "
                                          "In ambient occlusion, totally occluded parts of the mesh are black. "
                                          "Instead obscurance, despite still based on a perfectly diffuse light coming "
                                          "from everywhere, accounts for multiple bounces of indirect illumination by means "
                                          "of a function of both the openness of a point and the distance to his occluder (if any). "
                                          "Obscurance is inversely proportional to the number of ray casted from the point "
                                          "that hit an occluder and proportional to the distance a ray travels before hitting the occluder. "
                                          "You can control how much the distance factor influences the final result with the obscurance exponenent (see help below). "
                                          "Obscurance is a value in the range [0,1]. For further details see the reference paper:<br>"
                                          "<b>Iones Krupkin Sbert Zhukov <br> "
                                          "Fast, Realistic Lighting for Video Games <br>"
                                          "IEEECG&A 2003</b> ");

    default : assert(0);
    }

    return QString("");
}

bool SdfGpuPlugin::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet & pars, vcg::CallBackPos *cb)
{
    MeshModel* mm = md.mm();

    //RETRIEVE PARAMETERS
    mOnPrimitive  = (ONPRIMITIVE) pars.getEnum("onPrimitive");
    // assert( mOnPrimitive==ON_VERTICES && "Face mode not supported yet" );
    unsigned int numViews     = pars.getInt("numberRays");
    int          peel         = pars.getInt("peelingIteration");
    mTolerance                = pars.getFloat("peelingTolerance");
    mPeelingTextureSize       = pars.getInt("DepthTextureSize");

    if(mAction != SDF_DEPTH_COMPLEXITY)
        mMinCos                 = vcg::math::Cos(math::ToRad(pars.getFloat("coneAngle")/2.0));

    std::vector<Point3f> coneDirVec;

    if(mAction == SDF_OBSCURANCE)
        mTau = pars.getFloat("obscuranceExponent");
    else if(mAction==SDF_SDF)
    {
        mRemoveFalse     = pars.getBool("removeFalse");
        mRemoveOutliers  = pars.getBool("removeOutliers");
    }
    //MESH CLEAN UP
    setupMesh( md, mOnPrimitive );

    //glContext->makeCurrent();
    //GL INIT
    if(!initGL(*mm)) return false;

    //
    if(mOnPrimitive==ON_VERTICES)
        vertexDataToTexture(*mm);
    else
        faceDataToTexture(*mm);

    //Uniform sampling of directions over a sphere
    std::vector<Point3f> unifDirVec;
    GenNormal<float>::Fibonacci(numViews,unifDirVec);

    Log(0, "Number of rays: %i ", unifDirVec.size() );
    Log(0, "Number of rays for GPU outliers removal: %i ", coneDirVec.size() );

    coneDirVec.clear();

    vector<int>  mDepthDistrib(peel,0);
    //Do the actual calculation of sdf or obscurance for each ray
    unsigned int tracedRays = 0;
    for(vector<vcg::Point3f>::iterator vi = unifDirVec.begin(); vi != unifDirVec.end(); vi++)
    {
        (*vi).Normalize();
        TraceRay(peel, (*vi), md.mm());
        cb(100*((float)tracedRays/(float)unifDirVec.size()), "Tracing rays...");

        glContext->makeCurrent();

        ++tracedRays;
        mDepthComplexity = std::max(mDepthComplexity, mTempDepthComplexity);

        mDepthDistrib[mTempDepthComplexity]++;
        mTempDepthComplexity = 0;
    }

    //read back the result texture and store result in the mesh
    if(mAction == SDF_OBSCURANCE)
    {
        if(mOnPrimitive == ON_VERTICES)
            applyObscurancePerVertex(*mm,unifDirVec.size());
        else
            applyObscurancePerFace(*mm,unifDirVec.size());
    }
    else if(mAction == SDF_SDF)
    {
        if(mOnPrimitive == ON_VERTICES)
            applySdfPerVertex(*mm);
        else
            applySdfPerFace(*mm);

    }



    Log(0, "Mesh depth complexity %i (The accuracy of the result depends on the value you provided for the max number of peeling iterations, \n if you get warnings try increasing"
        " the peeling iteration parameter)\n", mDepthComplexity );

    //Depth complexity distribution log. Useful to know which is the probability to find a number of layers looking at the mesh or scene.
    Log(0, "Depth complexity             NumberOfViews\n", mDepthComplexity );
    for(int j = 0; j < peel; j++)
    {
        Log(0, "   %i                             %i\n", j, mDepthDistrib[j] );
    }

    //Clean & Exit
    releaseGL(*mm);
    //glContext->doneCurrent();
    mDepthComplexity = 0;

    return true;
}

bool SdfGpuPlugin::initGL(MeshModel& mm)
{
    const unsigned int numVertices = mm.cm.vn;
    const unsigned int numFaces    = mm.cm.fn;

    unsigned int numElems;
    if(mOnPrimitive==ON_VERTICES)
        numElems = numVertices;
    else
        numElems = numFaces;

    this->glContext->makeCurrent();

    //SET DEFAULT OPENGL STUFF
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    ::glClearColor(0,0,0,0);
    glClearDepth(1.0);

    if (!initializeGLextensions_notThrowing())
    {
        Log(0, "Error initializing OpenGL extensions.");
        return false;
    }

    //CHECK HARDWARE CAPABILITIES
    if (!glExtensionsHasARB_vertex_shader() || !glExtensionsHasARB_fragment_shader())
    {
        if (!glExtensionsHasEXT_vertex_shader())
        {
            Log(0, "Your hardware doesn't support Shaders, which are required for hw occlusion");
            return false;
        }
    }
    if ( !glExtensionsHasEXT_framebuffer_object() )
    {
        Log(0, "Your hardware doesn't support FBOs, which are required for hw occlusion");
        return false;
    }

    if ( glExtensionsHasARB_texture_float() )
    {
        if ( !glExtensionsHasEXT_gpu_shader4() )   //Only DX10-grade cards support FP32 blending
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

    GLint maxColorAttachments;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxColorAttachments);



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
    if ((maxTexSize*maxTexSize) < numElems)
    {
        Log(0, "That's a really huge model, I can't handle it in hardware, sorry..");
        return false;
    }

    for( mResTextureDim = 16; mResTextureDim*mResTextureDim < numElems; mResTextureDim *= 2 ){}

    mNumberOfTexRows = ceil( ((float)numElems) / ((float)mResTextureDim));

    Log(0, "Mesh has %i vertices\n", numVertices );
    Log(0, "Mesh has %i faces\n", numFaces);
    Log(0, "Number of tex rows used %i",mNumberOfTexRows);
    Log(0, "Result texture is %i X %i = %i", mResTextureDim, mResTextureDim, mResTextureDim*mResTextureDim);

    mVertexCoordsTexture  = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );
    mVertexNormalsTexture = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );

    mResultTexture      = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );
    mDirsResultTexture  = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mResTextureDim, mResTextureDim, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );


    mFboResult->attachTexture( mResultTexture->format().target(), mResultTexture->id(), GL_COLOR_ATTACHMENT0_EXT );
    mFboResult->attachTexture( mDirsResultTexture->format().target(), mDirsResultTexture->id(), GL_COLOR_ATTACHMENT1_EXT );

    //clear first time
    mFboResult->bind();
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers(2, buffers);
    glClear(GL_COLOR_BUFFER_BIT);
    mFboResult->unbind();

    //We use 3 FBOs to avoid z-fighting in sdf and obscurance calculation, see TraceRays function for details
    for(int i = 0; i < 3; i++)
    {
        mDepthTextureArray[i]  = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mPeelingTextureSize, mPeelingTextureSize, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT ),
            TextureParams( GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE ) );

        mColorTextureArray[i] = new FloatTexture2D( TextureFormat( GL_TEXTURE_2D, mPeelingTextureSize, mPeelingTextureSize, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT ), TextureParams( GL_NEAREST, GL_NEAREST ) );

        mFboArray[i]->attachTexture( mColorTextureArray[i]->format().target(), mColorTextureArray[i]->id(), GL_COLOR_ATTACHMENT0_EXT );
        mFboArray[i]->attachTexture( mDepthTextureArray[i]->format().target(), mDepthTextureArray[i]->id(), GL_DEPTH_ATTACHMENT  );

    }

    //Depth peeling shader used for both sdf and obscurance
    mDeepthPeelingProgram = new GPUProgram(":/SdfGpu/shaders/vertexShaderDepthPeeling.vs",":/SdfGpu/shaders/shaderDepthPeeling.fs","");
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
    mSDFProgram->addUniform("normalTextureBack");
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


    glGenQueriesARB( 1, &mOcclusionQuery );

    checkGLError::debugInfo("GL Init failed");

    return true;
}

void SdfGpuPlugin::faceDataToTexture(MeshModel &m)
{
    unsigned int texSize = mResTextureDim*mResTextureDim*4;

    GLfloat *facePosition= new GLfloat[texSize];
    GLfloat *faceNormals = new GLfloat[texSize];
    vcg::Point3<CMeshO::ScalarType> n;

    //Copies each face's position and normal in new vectors
    for (int i=0; i < m.cm.fn; ++i)
    {
        //face position
        facePosition[i*4+0] = (m.cm.face[i].P(0).X() + m.cm.face[i].P(1).X() + m.cm.face[i].P(2).X())*(1.0/3.0);
        facePosition[i*4+1] = (m.cm.face[i].P(0).Y() + m.cm.face[i].P(1).Y() + m.cm.face[i].P(2).Y())*(1.0/3.0);
        facePosition[i*4+2] = (m.cm.face[i].P(0).Z() + m.cm.face[i].P(1).Z() + m.cm.face[i].P(2).Z())*(1.0/3.0);
        facePosition[i*4+3] = 1.0;

        //Normal vector for each face
        n = m.cm.face[i].N();

        faceNormals[i*4+0] = n.X();
        faceNormals[i*4+1] = n.Y();
        faceNormals[i*4+2] = n.Z();
        faceNormals[i*4+3] = 0.0;
    }

    //Write vertex coordinates
    mVertexCoordsTexture->bind();
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mResTextureDim, mResTextureDim, 0, GL_RGBA, GL_FLOAT, facePosition);

    //Write normal directions
    mVertexNormalsTexture->bind();
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mResTextureDim, mResTextureDim, 0, GL_RGBA, GL_FLOAT, faceNormals);

    delete [] faceNormals;
    delete [] facePosition;


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

void SdfGpuPlugin::releaseGL(MeshModel &/*m*/)
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
        delete mColorTextureArray[i];
    }

    glDeleteQueriesARB( 1, &mOcclusionQuery );

    checkGLError::debugInfo("GL release failed");

    this->glContext->doneCurrent();
}

void SdfGpuPlugin::fillFrameBuffer(bool front,  MeshModel* mm)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace((front)?GL_BACK:GL_FRONT);
    //the most recent GPUs can do double speed Z-only rendering
    //(also alpha test must be turned off, depth replace and texkill must not be used in fragment shader)
    //glColorMask(0, 0, 0, 0);
    if (mm != NULL)
    {        
        MLRenderingData dt;
        MLRenderingData::RendAtts atts;
        atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
        atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
        dt.set(MLRenderingData::PR_SOLID,atts);
        glContext->setRenderingData(mm->id(),dt);
        glContext->drawMeshModel(mm->id());
    }
    //mm->glw.DrawFill<GLW::NMPerVert, GLW::CMNone, GLW::TMNone>();
    //glColorMask(1, 1, 1, 1);
    //delete wid;
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
    vcg::tri::Allocator<CMeshO>::CompactVertexVector(m);
    vcg::tri::Allocator<CMeshO>::CompactFaceVector(m);
    vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(m);
    tri::UpdateBounding<CMeshO>::Box(m);

    //Enable & Reset the necessary attributes
    switch(onPrimitive)
    {
    case ON_VERTICES:
        mm->updateDataMask(MeshModel::MM_VERTQUALITY);
        tri::UpdateQuality<CMeshO>::VertexConstant(m,0);
        break;
    case ON_FACES:
        mm->updateDataMask(MeshModel::MM_FACEQUALITY);
        mm->updateDataMask(MeshModel::MM_FACENORMAL);
        mm->updateDataMask(MeshModel::MM_FACECOLOR);
        tri::UpdateQuality<CMeshO>::FaceConstant(m,0);
        break;
    }

    if(!vcg::tri::HasPerVertexAttribute(m,"maxQualityDir") && onPrimitive == ON_VERTICES)
        mMaxQualityDirPerVertex = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3f>(m,std::string("maxQualityDir"));
    else if(!vcg::tri::HasPerFaceAttribute(m,"maxQualityDir") && onPrimitive == ON_FACES)
        mMaxQualityDirPerFace = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<Point3f>(m,std::string("maxQualityDir"));

    glContext->meshAttributesUpdated(mm->id(),true,MLRenderingData::RendAtts());

}

void SdfGpuPlugin::setCamera(Point3f camDir, Box3f meshBBox)
{
    GLfloat d = (meshBBox.Diag()/2.0),
        k = 0.1f;
    Point3f eye = meshBBox.Center() + camDir * (d+k);

    mScale = 2*k+(2.0*d);

    glViewport(0.0, 0.0, mPeelingTextureSize, mPeelingTextureSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-d, d, -d, d, /*k*/0, mScale );

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

    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,mResTextureDim,mNumberOfTexRows);

    GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers(2, buffers);

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
    // glDepthMask(GL_FALSE);

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
    glBindTexture(GL_TEXTURE_2D, fboBack->getAttachedId(GL_COLOR_ATTACHMENT0));
    mSDFProgram->setUniform1i("normalTextureBack",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mVertexCoordsTexture->id());
    mSDFProgram->setUniform1i("vTexture",3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mVertexNormalsTexture->id());
    mSDFProgram->setUniform1i("nTexture",4);

    //previous depth layer, see TraceRays function for an explanation
    if(fboPrevBack)
    {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, fboPrevBack->getAttachedId(GL_DEPTH_ATTACHMENT));
        mSDFProgram->setUniform1i("depthTexturePrevBack",5);

    }



    // Set view direction
    mSDFProgram->setUniform3f("viewDirection", cameraDir.X(), cameraDir.Y(), cameraDir.Z());

    // Set ModelView-Projection Matrix
    mSDFProgram->setUniformMatrix4fv( "mvprMatrix", mv_pr_Matrix_f, 1, GL_FALSE );

    vcg::Matrix44f mvprINV(mv_pr_Matrix_f);
    mvprINV.transposeInPlace();
    mvprINV=vcg::Inverse(mvprINV);
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
        mSDFProgram->setUniform1i("removeOutliers",1);
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
    //  glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);

}

void SdfGpuPlugin::applySdfPerVertex(MeshModel &m)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.vn; ++i)
    {
        //weighted average: sdf sum is in the red channel and the weights sum in the green one
        m.cm.vert[i].Q() = mScale*((result[i*4+1]>0.0) ? (result[i*4] / result[i*4+1]) : 0.0);
    }

    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.vn; ++i)
    {
        //weighted average: sdf sum is in the red channel and the weights sum in the green one
        Point3f dir = Point3f(result[i*4], result[i*4+1], result[i*4+2]);
        vcg::Normalize(dir);
        mMaxQualityDirPerVertex[i] = dir;
        //  Log(0,"vertice %i: %f %f %f",i,dir.X(),dir.Y(),dir.Z());
    }


    mFboResult->unbind();

    delete [] result;
}

void SdfGpuPlugin::applySdfPerFace(MeshModel &m)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.fn; ++i)
    {
        //weighted average: sdf sum is in the red channel and the weights sum in the green one
        m.cm.face[i].Q() = mScale*((result[i*4+1]>0.0) ? (result[i*4] / result[i*4+1]) : 0.0);
    }

    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.fn; ++i)
    {
        //weighted average: sdf sum is in the red channel and the weights sum in the green one
        Point3f dir = Point3f(result[i*4], result[i*4+1], result[i*4+2]);
        vcg::Normalize(dir);
        mMaxQualityDirPerFace[i] = dir;
        //  Log(0,"vertice %i: %f %f %f",i,dir.X(),dir.Y(),dir.Z());
    }


    mFboResult->unbind();

    delete [] result;
}

void SdfGpuPlugin::calculateObscurance(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* nextBack, const vcg::Point3f& cameraDir, float bbDiag)
{
    mFboResult->bind();

    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,mResTextureDim,mNumberOfTexRows);

    GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers(2, buffers);

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

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);


    glUseProgram(mObscuranceProgram->id());
    assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT && "before draw");


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

    assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT && "after draw");

    mFboResult->unbind();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void SdfGpuPlugin::applyObscurancePerVertex(MeshModel &m, float numberOfRays)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for( int i = 0; i < m.cm.vn; i++)
    {
        m.cm.vert[i].Q() = result[i*4]/numberOfRays;
    }
    tri::UpdateColor<CMeshO>::PerVertexQualityGray(m.cm,0.0f,0.0f);

    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.vn; ++i)
    {

        Point3f dir = Point3f(result[i*4], result[i*4+1], result[i*4+2]);
        vcg::Normalize(dir);
        mMaxQualityDirPerVertex[i] = dir;

    }

    mFboResult->unbind();
    delete [] result;
}

void SdfGpuPlugin::applyObscurancePerFace(MeshModel &m, float numberOfRays)
{
    const unsigned int texelNum = mResTextureDim*mResTextureDim;

    GLfloat *result = new GLfloat[texelNum*4];

    mFboResult->bind();

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for(int i = 0; i < m.cm.fn; i++)
    {
        m.cm.face[i].Q() = result[i*4]/numberOfRays;
    }

    tri::UpdateColor<CMeshO>::PerFaceQualityGray(m.cm);

    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glReadPixels(0, 0, mResTextureDim, mResTextureDim, GL_RGBA, GL_FLOAT, result);

    for (int i=0; i < m.cm.fn; ++i)
    {

        Point3f dir = Point3f(result[i*4], result[i*4+1], result[i*4+2]);
        vcg::Normalize(dir);
        mMaxQualityDirPerFace[i] = dir;

    }

    mFboResult->unbind();
    delete [] result;
}


void SdfGpuPlugin::preRender(unsigned int peelingIteration)
{
    if( peelingIteration != 0 )
        glBeginQueryARB( GL_SAMPLES_PASSED_ARB, mOcclusionQuery );

}

bool SdfGpuPlugin::postRender(unsigned int peelingIteration)
{

    if( peelingIteration != 0)
    {
        glEndQueryARB( GL_SAMPLES_PASSED_ARB );

        glGetQueryObjectuivARB( mOcclusionQuery, GL_QUERY_RESULT_ARB, &mPixelCount);
        if(mPixelCount > PIXEL_COUNT_THRESHOLD )
        {
            mTempDepthComplexity++;
            return true;
        }
        else return false;
    }

    return true;
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

        setCamera(dir, Box3f::Construct(mm->cm.bbox));

        preRender(i);

        fillFrameBuffer(i%2==0, mm);

        //Cut off useless passes
        if(!postRender(i))
            return;
        else
            if(i==(peelingIteration-1))
                Log(0,"WARNING: You may have underestimated the depth complexity of the mesh. Run the filter with a higher number of peeling iteration.");

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
            else if(mAction == SDF_SDF)
            {
                if(i>1)
                {
                    //We are interested in vertices belonging to the front layer. Then in the shader, we check that
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

    checkGLError::debugInfo("Error during depth peeling");
}

MeshFilterInterface::FILTER_ARITY SdfGpuPlugin::filterArity( QAction *) const
{
    return MeshFilterInterface::SINGLE_MESH;
}

MESHLAB_PLUGIN_NAME_EXPORTER(SdfGpuPlugin)

