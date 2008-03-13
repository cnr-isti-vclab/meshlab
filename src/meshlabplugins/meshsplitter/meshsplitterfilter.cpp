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

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include<vcg/complex/trimesh/allocate.h>

#include <vcg/complex/trimesh/update/selection.h>
#include<vcg/complex/trimesh/append.h>

#include "meshsplitterfilter.h"

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

MeshSplitterFilterPlugin::MeshSplitterFilterPlugin() 
{ 
	typeList << FP_SPLITSELECT;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString MeshSplitterFilterPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_SPLITSELECT :  return QString("Move selection on another layer"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString MeshSplitterFilterPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_SPLITSELECT :  return QString("Selected faces are moved (or duplicated) in a new layer"); 
		default : assert(0); 
	}
}

const PluginInfo &MeshSplitterFilterPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Marco Callieri");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void MeshSplitterFilterPlugin::initParameterSet(QAction *action,MeshDocument & /*m*/, FilterParameterSet & parlst) 
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		case FP_SPLITSELECT :  
 		  parlst.addBool ("DeleteOriginal",
											true,
											"Delete original selection",
											"Deletes the origianl selected faces, thus splitting the mesh among layers. \n\n"
											"if false, the selected faces are duplicated in the new layer");
											break;
											
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool MeshSplitterFilterPlugin::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	CMeshO::FaceIterator fi;
	int numFacesSel,numVertSel;

	// creating the new layer
	// that is the back one
	MeshModel *mm= new MeshModel();	
	md.meshList.push_back(mm);	

	MeshModel *destMesh     = md.meshList.back();	// destination = last
	MeshModel *currentMesh  = md.mm();				// source = current

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

		currentMesh->clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);

		Log(0,"Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
	}
	else								// keep original faces
	{
		Log(0,"Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
	}

	// init new layer
	destMesh->fileName = "newlayer.ply";								// mesh name
	tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
	for(fi=destMesh->cm.face.begin();fi!=destMesh->cm.face.end();++fi)	// face normals
		face::ComputeNormalizedNormal(*fi);	
	tri::UpdateNormals<CMeshO>::PerVertex(destMesh->cm);				// vertex normals
	destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
	destMesh->busy=false;												// ready to be used

	return true;
}

Q_EXPORT_PLUGIN(MeshSplitterFilterPlugin)
