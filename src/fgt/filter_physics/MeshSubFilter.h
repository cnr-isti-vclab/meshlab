/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef MESHSUBFILTER_H
#define MESHSUBFILTER_H

#include <common/meshmodel.h>
#include <common/interfaces.h>

#include <vcg/math/matrix44.h>

#include <vector>
#include <string>

class MeshSubFilter{
public:
    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par) = 0;
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb) = 0;
};

#endif // MESHSUBFILTER_H
