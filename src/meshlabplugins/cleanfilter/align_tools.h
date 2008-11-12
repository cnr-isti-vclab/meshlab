/*
 * This class will will hold useful things for doing alignment of meshes
 *
 * @author Oscar Barney
 *
 */

#ifndef ALIGN_TOOLS_H
#define ALIGN_TOOLS_H

#include <QObject>

#include <meshlab/filterparameter.h>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>

#include <meshlabplugins/edit_pickpoints/pickedPoints.h>
#include <meshlabplugins/editalign/align/AlignPair.h>

class AlignTools : public QObject
{
	Q_OBJECT

public:
	static const QString FilterName;
	static const QString UseMarkers;
	static const QString AllowScaling;
	static const QString UseICP;
	static const QString StuckMesh;
	static const QString MeshToMove;
	
	//make the default parameter set for this filter
	static void buildParameterSet(FilterParameterSet & parlst);

	//setup all the parameters and then call align
	static bool setupThenAlign(MeshModel &mm, FilterParameterSet & par);

	/*
	 * stuckModel - the mesh one that stays put
	 * modelToMove - the mesh to be moved into place
	 * modelToMoveGLArea - so we can update the position of the model
	 * parentWidget - the widget that should be the parent of the confirm dialog window
	 * confirm - whether to ask the user if they want the alignment to stay put
	 *
	 * return value - true if align was accepted or successful
	*/
	static bool align(MeshModel *stuckModel, PickedPoints *stuckPickedPoints,
				MeshModel *modelToMove, PickedPoints *modelToMovePickedPoints,
				GLArea *modelToMoveGLArea,
				FilterParameterSet &parameters,
				QWidget *parentWidget = 0, bool confirm = false);

	//returns the key applied if this transform is stored to perMeshAttribute
	static const std::string getKey() { return "TransformAppliedKey"; }
	
protected:
	AlignTools();

};

#endif
