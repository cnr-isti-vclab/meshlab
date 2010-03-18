/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "filter_splitter.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include<vcg/complex/trimesh/allocate.h>

#include <vcg/complex/trimesh/update/selection.h>
#include<vcg/complex/trimesh/append.h>



using namespace std;
using namespace vcg;

// Constructor
FilterSplitterPlugin::FilterSplitterPlugin()
{
	typeList <<
	FP_SPLITSELECT <<
	FP_DUPLICATE;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
 QString FilterSplitterPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_SPLITSELECT :  return QString("Move selection on another layer");
		case FP_DUPLICATE :  return QString("Duplicate current layer");
		default : assert(0);
	}
}

// Info() return the longer string describing each filtering action
 QString FilterSplitterPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_SPLITSELECT :  return QString("Selected faces are moved (or duplicated) in a new layer");
		case FP_DUPLICATE :  return QString("Create a new layer containing the same model as the current one");
		default : assert(0);
	}
}

// This function define the needed parameters for each filter.
void FilterSplitterPlugin::initParameterSet(QAction *action, MeshDocument &/*m*/, RichParameterSet & parlst)
{
	 switch(ID(action))
	 {
		case FP_SPLITSELECT :
			{
	 		  parlst.addParam(new RichBool ("DeleteOriginal",
												true,
												"Delete original selection",
												"Deletes the original selected faces, thus splitting the mesh among layers. \n\n"
												"if false, the selected faces are duplicated in the new layer"));
			}
			break;

   default: break; // do not add any parameter for the other filters
  }
}

// Core Function doing the actual mesh processing.
bool FilterSplitterPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *)
{
	CMeshO::FaceIterator fi;
	int numFacesSel,numVertSel;

	switch(ID(filter))
  {
		case FP_SPLITSELECT :
		{
			// creating the new layer
			// that is the back one
            MeshModel *currentMesh  = md.mm();				// source = current
            MeshModel *destMesh= md.addNewMesh("SelectedSubset"); // After Adding a mesh to a MeshDocument the new mesh is the current one

			// select all points involved
			tri::UpdateSelection<CMeshO>::ClearVertex(currentMesh->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currentMesh->cm);

			tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, currentMesh->cm, true);

			numFacesSel = tri::UpdateSelection<CMeshO>::CountFace(currentMesh->cm);
			numVertSel  = tri::UpdateSelection<CMeshO>::CountVertex(currentMesh->cm);

			if(par.getBool("DeleteOriginal"))	// delete original faces
			{
				CMeshO::VertexIterator vi;
				CMeshO::FaceIterator   fi;
				tri::UpdateSelection<CMeshO>::ClearVertex(currentMesh->cm);
				tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(currentMesh->cm);
				for(fi=currentMesh->cm.face.begin();fi!=currentMesh->cm.face.end();++fi)
					if(!(*fi).IsD() && (*fi).IsS() )
						tri::Allocator<CMeshO>::DeleteFace(currentMesh->cm,*fi);
				for(vi=currentMesh->cm.vert.begin();vi!=currentMesh->cm.vert.end();++vi)
					if(!(*vi).IsD() && (*vi).IsS() )
						tri::Allocator<CMeshO>::DeleteVertex(currentMesh->cm,*vi);

				tri::UpdateSelection<CMeshO>::ClearVertex(currentMesh->cm);
				tri::UpdateSelection<CMeshO>::ClearFace(currentMesh->cm);

				currentMesh->clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);

				Log(GLLogStream::FILTER,"Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
			}
			else								// keep original faces
			{
				Log(GLLogStream::FILTER,"Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
			}

			// init new layer
            tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
			for(fi=destMesh->cm.face.begin();fi!=destMesh->cm.face.end();++fi)	// face normals
				face::ComputeNormalizedNormal(*fi);
			tri::UpdateNormals<CMeshO>::PerVertex(destMesh->cm);				// vertex normals
			destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
		}
		break;

		case FP_DUPLICATE :
		{
			// creating the new layer
			// that is the back one
            MeshModel *currentMesh  = md.mm();				// source = current
            MeshModel *destMesh= md.addNewMesh("duplicated"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, currentMesh->cm, false, true); // the last true means "copy all vertices"

			Log(GLLogStream::FILTER,"Duplicated current model to layer %i", md.meshList.size());

			// init new layer
			tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
			for(fi=destMesh->cm.face.begin();fi!=destMesh->cm.face.end();++fi)	// face normals
				face::ComputeNormalizedNormal(*fi);
			tri::UpdateNormals<CMeshO>::PerVertex(destMesh->cm);				// vertex normals
			destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
		}
	}

	return true;
}

 FilterSplitterPlugin::FilterClass FilterSplitterPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_SPLITSELECT :
    case FP_DUPLICATE :
      return MeshFilterInterface::Layer;
		default :  assert(0);
			return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(FilterSplitterPlugin)
