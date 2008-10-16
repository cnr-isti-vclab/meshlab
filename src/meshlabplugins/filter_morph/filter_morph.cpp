/* A filter for morphing
 *   
 * @author Oscar Barney
 */


#include <QtGui>
#include "filter_morph.h"
#include <vcg/complex/trimesh/allocate.h>

using namespace vcg;


const QString FilterMorph::MeshToMorph = "MeshToMorph";
		
const QString FilterMorph::TargetMesh = "TargetMesh";

const QString FilterMorph::PercentMorph = "PercentMorph";

FilterMorph::FilterMorph() 
{
	typeList << FP_LINEAR_MORPH;

	FilterIDType tt;
	foreach(tt , types())
		actionList << new QAction(filterName(tt), this);
}

FilterMorph::~FilterMorph() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString FilterMorph::filterName(FilterIDType filter) 
{
	switch(filter)
	{
		case FP_LINEAR_MORPH :	return QString("Morph Mesh");
		default: assert(0);
	}
	return QString("error!");
}

const QString FilterMorph::filterInfo(FilterIDType filterId)
{
	switch(filterId)
	{
		case FP_LINEAR_MORPH :
			return tr("Morph a mesh in the direction of a target with the same number of vertices.");
		default: assert(0);
	}
	return QString("error!");
}

const FilterMorph::FilterClass FilterMorph::getClass(QAction *a)
{
	switch(ID(a))
	{
		case FP_LINEAR_MORPH :
		default : 
			return MeshFilterInterface::Generic;
	}
}

const int FilterMorph::getRequirements(QAction *action)
{
	switch(ID(action))
	{
		//TODO maybe in the future there needs to be a MM_PICKED_POINTS flag???
		case FP_LINEAR_MORPH :
			return MeshModel::MM_NONE;
			break;
		default: assert(0);
	}
	return 0;
}

void FilterMorph::initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst) 
{ 
	switch(ID(action))
	{
		case FP_LINEAR_MORPH :
			parlst.addMesh (TargetMesh, 1, "Target Mesh",
					"The mesh that is the morph target.");
			
			parlst.addDynamicFloat(PercentMorph, 0.0, -150, 250,
					(MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL), "% Morph",
					"The percent you want to morph toward (or away from) the target.");
			break;
		default: assert(0);
	}
}



bool FilterMorph::applyFilter(QAction *filter, MeshModel &mm, FilterParameterSet &par, vcg::CallBackPos *cb) 
{

	if(filter->text() == filterName(FP_LINEAR_MORPH) )
	{
		MeshModel *targetModel = par.getMesh(TargetMesh);
				
		//if the numbers of vertices dont match up
		if(mm.cm.vn != targetModel->cm.vn)
		{
			Log(0,"Number of vertices are not the same so you cant morph between them.");
			return false;
		}
		
		//compact the vectors if they dont match up
		if(mm.cm.vn != mm.cm.vert.size() )
			vcg::tri::Allocator<CMeshO>::CompactVertexVector(mm.cm); 
		if(mm.cm.fn != mm.cm.face.size() )
			vcg::tri::Allocator<CMeshO>::CompactFaceVector(mm.cm); 
		if(targetModel->cm.vn != targetModel->cm.vert.size() )
			vcg::tri::Allocator<CMeshO>::CompactVertexVector(targetModel->cm); 
		if(targetModel->cm.fn != targetModel->cm.face.size() )
			vcg::tri::Allocator<CMeshO>::CompactFaceVector(targetModel->cm);

		float percentage = par.getDynamicFloat(PercentMorph);
		
		tools.calculateMorph(percentage, &mm, targetModel);
	
	} else assert(0);
	
	return true;
}


Q_EXPORT_PLUGIN(FilterMorph)
