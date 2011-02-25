#ifndef FILTER_SDFGPU_H
#define FILTER_SDFGPU_H


#include <filterinterface.h>
#include "Vscan.h"

enum ONPRIMITIVE{ON_VERTICES, ON_FACES};

class SdfGpuPlugin : public SingleMeshFilterInterface{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:
    SdfGpuPlugin();
    bool applyFilter(MeshDocument&, RichParameterSet&, vcg::CallBackPos*);
    virtual void initParameterSet(MeshDocument&, RichParameterSet &);

    //draw the mesh
    void fillFrameBuffer(bool front,  MeshModel* mm);
    //mesh setup
    void setupMesh(MeshDocument& md, ONPRIMITIVE onprim );
    //init openGL context
    bool initGL(unsigned int numVertices);
    //openGL clean up
    void releaseGL();
    //set the fragment shader for depth peeling
    void useDepthPeelingShader();
    //set tolerance uniform paramter
    void setDepthPeelingTolerance(float t);
    //set the FBO size uniform paramter
    void setDepthPeelingSize(const Vscan & scan);
    //
    void useScreenAsDest();
    //
    void useDefaultShader();
    //
    void setCamera(vcg::Point3f camDir, vcg::Box3f &meshBBox);
    //Do the actual calculation
    void TraceRays(int peelingIteration, float tolerance, const vcg::Point3f& dir, MeshModel* mm );

    void drawVertexMarkers();

    bool checkFramebuffer();

protected:

    unsigned int maxTexSize;
    unsigned int numTexPages;
    Vscan*       vsA;
    Vscan*       vsB;
    int          shaderProgram;
    int          fboSize;
    GLuint*      resultBufferTex;
    GLenum*      resultBufferMRT;
    GLuint       fboResult;
    GLenum       colorFormat;
    GLenum       dataTypeFP;

};

#endif // FILTER_SDFGPU_H
