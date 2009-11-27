#ifndef GRAVITYSUBFILTER_H
#define GRAVITYSUBFILTER_H

#include "MeshSubFilter.h"
#include "ODEFacade.h"

class GravitySubFilter : public MeshSubFilter{
public:
    GravitySubFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&);

private:
    bool m_initialized;
    unsigned int m_prevTimeSlice;
    unsigned int m_currentSceneryLayer;
    ODEFacade m_engine;
};

#endif // GRAVITYSUBFILTER_H
