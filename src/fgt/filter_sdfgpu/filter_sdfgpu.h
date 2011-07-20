#ifndef FILTER_SDFGPU_H
#define FILTER_SDFGPU_H

#include <QObject>

#include <common/interfaces.h>

#include <gpuProgram.h>
#include <framebufferObject.h>
#include <texture2D.h>

#define EXTRA_RAYS_REQUESTED 10
#define EXTRA_RAYS_RESULTED 40 //must match value in sdf shader

enum ONPRIMITIVE{ON_VERTICES, ON_FACES};

class SdfGpuPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:

    enum{ SDF_SDF, SDF_DEPTH_COMPLEXITY, SDF_OBSCURANCE };

    SdfGpuPlugin();

    QString filterName(FilterIDType filterId) const;

    QString filterInfo(FilterIDType filterId) const;

    int     getRequirements(QAction *action);

    virtual FilterClass getClass()
    {
      return MeshFilterInterface::Generic;
    }

    //Main plugin function
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);

    //Parameters init for user interface
    virtual void initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst);

    //Draw the mesh
    void fillFrameBuffer(bool front,  MeshModel* mm);

    //Mesh setup
    void setupMesh(MeshDocument& md, ONPRIMITIVE onprim );

    //Init OpenGL context
    bool initGL(MeshModel& mm);

    //OpenGL clean up
    void releaseGL(MeshModel &m);

    //Setup camera orientation
    void setCamera(vcg::Point3f camDir, vcg::Box3f &meshBBox);

    //Calculate sdf or obscurance along a ray
    void TraceRay(int peelingIteration, const vcg::Point3f& dir, MeshModel* mm );

    //Enable depth peeling shader
    void useDepthPeelingShader(FramebufferObject* fbo);

    //Position and normal of each vertex are copied to two separate texture, to be used in sdf and obscurance GPU calculation
    void vertexDataToTexture(MeshModel &m);

    //Sdf calculation for each depth peeling iteration
    void calculateSdfHW(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* fboPrevBack, const vcg::Point3f& cameraDir );

    //Copy sdf values from result texture to the mesh (vertex quality)
    void applySdfHW(MeshModel &m, float numberOfRays);

    //Obscurance calculation for each depth peeling iteration
    void calculateObscurance(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* nextFront, const vcg::Point3f& cameraDir, float bbDiag );

    //Copy obscurance values from result texture to the mesh (vertex color)
    void applyObscurance(MeshModel &m, float numberOfRays);

    void preRender(unsigned int peelingIteration);

    bool postRender(unsigned int peelingIteration);

    bool postCalculate(unsigned int peelingIteration);

  protected:

    FilterIDType       mAction;
    unsigned int       mResTextureDim;
    unsigned int       mNumberOfTexRows; //the number of rows of a texture actually used for the result texture
    FloatTexture2D*    mVertexCoordsTexture;
    FloatTexture2D*    mVertexNormalsTexture;
    FramebufferObject* mFboResult;    //Fbo and texture storing the result computation
    FloatTexture2D*    mResultTexture;
    FloatTexture2D*    mDirsResultTexture;
    FramebufferObject* mFboArray[3];  //Fbos and textures for depth peeling
    FloatTexture2D*    mDepthTextureArray[3];
    FloatTexture2D*    mColorTextureArray[3];
    bool               mUseVBO;
    unsigned int       mPeelingTextureSize;
    float              mTolerance;
    float              mMinCos;
    float              mTau;      //obscurance exponent
    float              mMinDist;  //min dist between vertices to check too thin parts
    float              mScale;  //Scaling factor used to setup camera
    GPUProgram*        mDeepthPeelingProgram;
    GPUProgram*        mSDFProgram;
    GPUProgram*        mObscuranceProgram;
    bool               mRemoveFalse;
    bool               mRemoveOutliers;
    float              mConeRays[EXTRA_RAYS_RESULTED*3];
    GLuint             mOcclusionQuery;
    GLuint             mPixelCount;
    unsigned int       mTempDepthComplexity;
    unsigned int       mDepthComplexity;
    CMeshO::PerVertexAttributeHandle<vcg::Point3f> mMaxQualityDir;


};

#endif // FILTER_SDFGPU_H
