/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef MESHSUBFILTER_H
#define MESHSUBFILTER_H

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class MeshSubFilter{
public:
    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par) = 0;
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb) = 0;

protected:
    virtual void initialize(MeshDocument&, RichParameterSet&) = 0;
};

#endif // MESHSUBFILTER_H
