#ifndef RANDOMFILLFILTER_H
#define RANDOMFILLFILTER_H

#include "MeshSubFilter.h"
#include "ODEFacade.h"

#include <vcg/space/point3.h>

class RandomFillFilter : public MeshSubFilter{
public:
    RandomFillFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);
    virtual bool parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par);

protected:
    virtual void addRandomObject(MeshDocument& md, MeshModel* filler, const vcg::Point3<float>& origin, int meshID);

private:
    vcg::Point3<float> getRandomOrigin(RichParameterSet& par);

    int m_dropRate;
    ODEFacade m_engine;

    static int m_filterType;
};

#endif // RANDOMFILLFILTER_H
