/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef GRAVITYSUBFILTER_H
#define GRAVITYSUBFILTER_H

#include "DynamicMeshSubFilter.h"
#include "ODEFacade.h"

class GravitySubFilter : public DynamicMeshSubFilter{
public:
    GravitySubFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);

private:
    MeshModel* m_scenery;
    ODEFacade m_engine;
};

#endif // GRAVITYSUBFILTER_H
