/* A filter for morphing
 *   
 * @author Oscar Barney
 */


#ifndef FILTERMORPHPLUGIN_H
#define FILTERMORPHPLUGIN_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "filter_morph_tools.h"

class FilterMorph : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_LINEAR_MORPH } ;
	
	/* default values for standard parameters' values of the plugin actions */
	FilterMorph();
	~FilterMorph();

	virtual const QString filterName(FilterIDType filter);
	virtual const QString filterInfo(FilterIDType filter);

	virtual const FilterClass getClass(QAction *);
	virtual const int getRequirements(QAction *);

	virtual bool autoDialog(QAction *) {return true;}
	virtual void initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst);
	virtual bool applyFilter(QAction *filter, MeshModel &mm, FilterParameterSet &parent, vcg::CallBackPos * cb) ;

private:
	MorphTools tools;
	
	//const paramter names
	static const QString MeshToMorph;
	static const QString TargetMesh;
	static const QString PercentMorph;
	
};


#endif
