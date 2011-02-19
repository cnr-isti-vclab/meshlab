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

    void fillFrameBuffer(bool front,  MeshModel* mm);

    void setupMesh(MeshDocument& md, ONPRIMITIVE onprim );

    void initGL();

    void releaseGL();

    bool checkAndLogGL();

    void useDepthPeelingShader();

    void setDepthPeelingTolerance(float t);

    void setDepthPeelingSize(const Vscan & scan);

    void useScreenAsDest();

    void useDefaultShader();

private:

    Vscan* vsA;
    Vscan* vsB;
    int    shaderProgram;

};

#endif // FILTER_SDFGPU_H
