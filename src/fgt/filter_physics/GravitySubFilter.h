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
    virtual bool configurationHasChanged(RichParameterSet& par);

private:
    int m_currentSceneryLayer;
    ODEFacade m_engine;
};

#endif // GRAVITYSUBFILTER_H
