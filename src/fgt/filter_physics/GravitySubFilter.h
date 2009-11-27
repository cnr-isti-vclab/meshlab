/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

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
    const float m_stepSize;
    unsigned int m_steps;
    unsigned int m_currentSceneryLayer;
    int m_prevStep;
    ODEFacade m_engine;
};

#endif // GRAVITYSUBFILTER_H
