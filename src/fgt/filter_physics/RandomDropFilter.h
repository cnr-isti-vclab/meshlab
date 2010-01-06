/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef RANDOMDROPFILTER_H
#define RANDOMDROPFILTER_H

#include "DynamicMeshSubFilter.h"
#include "ODEFacade.h"

class RandomDropFilter : public DynamicMeshSubFilter{
public:
    RandomDropFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);
    virtual void addRandomObject(MeshDocument& md);

private:
    int m_randomLayer;
    int m_dropRate;
    float m_distance;
    ODEFacade m_engine;
};

#endif // RANDOMDROPFILTER_H
