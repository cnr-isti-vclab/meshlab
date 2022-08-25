/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef FILTERPHYSICS_PLUGIN_H
#define FILTERPHYSICS_PLUGIN_H

#include "ODEFacade.h"
#include "GravitySubFilter.h"
#include "RandomDropFilter.h"
#include "RandomFillFilter.h"

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/meshmodel.h>
#include <common/interfaces.h>

class FilterPhysics : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
    enum {FP_PHYSICS_GRAVITY, FP_PHYSICS_RNDDROP, FP_PHYSICS_RNDFILL} ;

    FilterPhysics();
    ~FilterPhysics();

    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual int getRequirements(QAction*){return MeshModel::MM_FACEVERT | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACEFACETOPO; }
    virtual int postCondition( QAction* ) const{return MeshModel::MM_FACENORMAL; /*| MeshModel::MM_TRANSFMATRIX;*/}

    virtual bool autoDialog(QAction*) {return true;}
    virtual void initParameterSet(QAction*, MeshDocument&, RichParameterSet&);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);
    virtual FilterClass getClass(QAction *);

    private:
    GravitySubFilter m_gravityFilter;
    RandomDropFilter m_rndDropFilter;
    RandomFillFilter m_rndFillFilter;
};

#endif
