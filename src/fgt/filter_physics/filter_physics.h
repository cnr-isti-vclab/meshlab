/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef FILTERPHYSICS_PLUGIN_H
#define FILTERPHYSICS_PLUGIN_H

#include "ODEFacade.h"
#include "GravitySubFilter.h"

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class FilterPhysics : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
    enum {FP_PHYSICS_GRAVITY} ;

    FilterPhysics();
    ~FilterPhysics();

    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual const int getRequirements(QAction*){return MeshModel::MM_FACEVERT;}
    virtual int postCondition( QAction* ) const{return MeshModel::MM_TRANSFMATRIX;}

    virtual bool autoDialog(QAction*) {return true;}
    virtual void initParameterSet(QAction*, MeshDocument&, RichParameterSet&);
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb);
    virtual bool applyFilter(QAction*, MeshModel&, RichParameterSet&, vcg::CallBackPos*) { assert(0); return false;}
    virtual const FilterClass getClass(QAction *);

    private:
    GravitySubFilter m_gravityFilter;
};

#endif
