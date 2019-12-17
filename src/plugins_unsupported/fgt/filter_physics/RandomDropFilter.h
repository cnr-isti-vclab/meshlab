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
    virtual bool parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb);
    virtual bool configurationHasChanged(MeshDocument& md, RichParameterSet& par);
    virtual void addRandomObject(MeshDocument& md, int meshID);

private:
    MeshModel* m_randomMesh;
    float m_dropRate;
    float m_distance;
    ODEFacade m_engine;

    static int m_filterType;
};

#endif // RANDOMDROPFILTER_H
