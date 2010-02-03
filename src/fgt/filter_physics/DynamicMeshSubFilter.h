#ifndef DYNAMICMESHSUBFILTER_H
#define DYNAMICMESHSUBFILTER_H

#include "MeshSubFilter.h"

#include <vcg/math/matrix44.h>

#include <vector>
#include <string>

class DynamicMeshSubFilter : public MeshSubFilter{
public:
    DynamicMeshSubFilter();
    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);
    virtual bool parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par);

protected:
    typedef std::vector<vcg::Matrix44f> LayerTransformations;
    typedef std::vector<LayerTransformations> LayersTransformations;

    virtual void initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);

    int m_fps;
    int m_iterations;
    int m_contacts;
    int m_steps;
    int m_seconds;
    float m_bounciness;
    float m_gravity;
    float m_friction;

    LayersTransformations m_layersTrans;
    std::vector<std::string> m_files;
    std::vector<MeshModel*> m_state;
};

#endif // DYNAMICMESHSUBFILTER_H
