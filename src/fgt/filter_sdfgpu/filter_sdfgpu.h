#ifndef FILTER_SDFGPU_H
#define FILTER_SDFGPU_H

#include <QObject>

#include <common/interfaces.h>

#include <gpuProgram.h>
#include <framebufferObject.h>
#include <texture2D.h>


enum ONPRIMITIVE{ON_VERTICES, ON_FACES};

class SdfGpuPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:

    enum{ SDF_SDF, SDF_CORRECTION_THIN_PARTS, SDF_OBSCURANCE };

    SdfGpuPlugin();

    QString filterName(FilterIDType filterId) const;

    QString filterInfo(FilterIDType filterId) const;

    int     getRequirements(QAction *action);

    virtual FilterClass getClass(){
      return MeshFilterInterface::Generic;
    }


    //main plugin function
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    //parameters init for user interface
    virtual void initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst);
    //draw the mesh
    void fillFrameBuffer(bool front,  MeshModel* mm);
    //mesh setup
    void setupMesh(MeshDocument& md, ONPRIMITIVE onprim );
    //init openGL context
    bool initGL(unsigned int numVertices);
    //openGL clean up
    void releaseGL();
    //setup camera orientation
    void setCamera(vcg::Point3f camDir, vcg::Box3f &meshBBox);
    //Do the actual sdf calculation
    void TraceRays(int peelingIteration, float tolerance, const vcg::Point3f& dir, MeshModel* mm );
    //enable depth peeling shader
    void useDepthPeelingShader(FramebufferObject* fbo);

    void vertexDataToTexture(MeshModel &m);

    void calculateSdfHW(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* fboPrevBack, const vcg::Point3f& cameraDir);

    void applySdfHW(MeshModel &m, float numberOfRays);

    void calculateObscurance(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* nextFront, const vcg::Point3f& cameraDir);

    void applyObscurance(MeshModel &m, float numberOfRays);

  protected:

    FilterIDType       mAction;
    unsigned int       mResTextureDim;
    FloatTexture2D*    mVertexCoordsTexture;
    FloatTexture2D*    mVertexNormalsTexture;
    //Fbo and texture for storing the result computation
    FramebufferObject* mFboResult;
    FloatTexture2D*    mResultTexture;
    //Fbos and textures for depth peeling
    FramebufferObject* mFboArray[4];
    FloatTexture2D*    mColorTextureArray[4];
    FloatTexture2D*    mDepthTextureArray[4];

    unsigned int       mPeelingTextureSize;
    float              mTolerance;
    float              mDepthTolerance;
    float              mMinCos;
    float              mMaxCos;
    //obscurance exponent
    float              mTau;
    //min dist between vertices to check too thin parts
    float              mMinDist;
    GPUProgram*        mDeepthPeelingProgram;
    GPUProgram*        mSDFProgram;
    GPUProgram*        mObscuranceProgram;
};

#endif // FILTER_SDFGPU_H
