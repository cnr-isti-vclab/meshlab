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
    virtual bool parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);

private:
    MeshModel* m_scenery;
    ODEFacade m_engine;

    static int m_filterType;
};

#endif // GRAVITYSUBFILTER_H
