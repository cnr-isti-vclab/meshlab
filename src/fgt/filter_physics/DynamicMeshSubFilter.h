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

protected:
    typedef std::vector<vcg::Matrix44f> LayerTransformations;
    typedef std::vector<LayerTransformations> LayersTransformations;

    virtual void initialize(MeshDocument&, RichParameterSet&);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);

    int m_steps;
    int m_seconds;
    LayersTransformations m_layersTrans;

private:
    std::vector<std::string> m_files;
};

#endif // DYNAMICMESHSUBFILTER_H
