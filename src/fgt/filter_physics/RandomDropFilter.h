/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef RANDOMDROPFILTER_H
#define RANDOMDROPFILTER_H

#include "MeshSubFilter.h"
#include "ODEFacade.h"

class RandomDropFilter : public MeshSubFilter{
public:
    RandomDropFilter();

    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&);
    virtual bool configurationHasChanged(RichParameterSet& par);

private:
    ODEFacade m_engine;
};

#endif // RANDOMDROPFILTER_H
