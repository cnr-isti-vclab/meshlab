#ifndef FILTER_SDFGPU_H
#define FILTER_SDFGPU_H

#include <filterinterface.h>

#include <gpuProgram.h>
#include <framebufferObject.h>
#include <texture2D.h>


enum ONPRIMITIVE{ON_VERTICES, ON_FACES};

class SdfGpuPlugin : public SingleMeshFilterInterface{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:
    SdfGpuPlugin();
    //main plugin function
    bool applyFilter(MeshDocument&, RichParameterSet&, vcg::CallBackPos*);
    //parameters init for user interface
    virtual void initParameterSet(MeshDocument&, RichParameterSet &);
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

    void calculateSdfHW(FramebufferObject& fboFront, FramebufferObject& fboBack, const vcg::Point3f& cameraDir);

    void applySdfHW(MeshModel &m, float numberOfRays);

protected:

    unsigned int       mResTextureDim;
    FloatTexture2D*    mResultTexture;
    FloatTexture2D*    mVertexCoordsTexture;
    FloatTexture2D*    mVertexNormalsTexture;
    FramebufferObject* mFboA;
    FramebufferObject* mFboB;
    FramebufferObject* mFboResult;
    FloatTexture2D*    mColorTextureA;
    FloatTexture2D*    mDepthTextureA;
    FloatTexture2D*    mColorTextureB;
    FloatTexture2D*    mDepthTextureB;
    unsigned int       mPeelingTextureSize;
    float              mTolerance;
    GPUProgram*        mDeepthPeelingProgram;
    GPUProgram*        mSDFProgram;
};

#endif // FILTER_SDFGPU_H
