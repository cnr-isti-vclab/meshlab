#ifndef FILTER_DECOMPOSE_H
#define FILTER_DECOMPOSE_H

#include <QObject>

#include <common/interfaces.h>


template<class MeshType>
class FunctorType{
   typedef typename MeshType::FaceType FaceType;
public:
    double operator() (vcg::face::Pos<FaceType>);
};

class QScriptEngine;

class ExtraSamplePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
	  FP_MOVE_VERTEX,
      FP_TOPOLOGICAL_CUT,
      FP_DECOMPOSER
		 } ;

	ExtraSamplePlugin();

	virtual QString pluginName(void) const { return "ExtraSamplePlugin"; }
    int getPreConditions(QAction *) const {return MeshModel::MM_FACEQUALITY | MeshModel::MM_FACENORMAL;}
    QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
    int postCondition( QAction* ) const {return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACECOLOR;}
    FilterClass getClass(QAction *a);
	QString filterScriptFunctionName(FilterIDType filterID);

};


#endif
