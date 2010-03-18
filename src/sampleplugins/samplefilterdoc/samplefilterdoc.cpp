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
/****************************************************************************
  History
$Log: samplefilter.cpp,v $
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "samplefilterdoc.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include<vcg/complex/trimesh/append.h>


using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SampleFilterDocPlugin::SampleFilterDocPlugin() 
{ 
	typeList << FP_FLATTEN;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString SampleFilterDocPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_FLATTEN :  return QString("Flatten visible layers"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString SampleFilterDocPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_FLATTEN :  return QString("Flatten all or only the visible layers into a single new mesh. <br> Transformations are preserved. Existing layers can be optionally deleted"); 
		default : assert(0); 
	}
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a long string describing the meaning of that parameter (shown as a popup help in the dialog)
void SampleFilterDocPlugin::initParameterSet(QAction *action,MeshDocument & /*m*/, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_FLATTEN :  
		  parlst.addParam(new RichBool ("MergeVisible",
											true,
											"Merge Only Visible Layers",
											"Merge the vertices that are duplicated among different layers. <br>"
											"Very useful when the layers are spliced portions of a single big mesh."));
		  parlst.addParam(new RichBool ("DeleteLayer",
											true,
											"Delete Layers ",
											"Delete all the merged layers. <br>If all layers are visible only a single layer will remain after the invocation of this filter"));
                  parlst.addParam(new RichBool ("MergeVertices",
											true,
											"Merge duplicate vertices",
											"Merge the vertices that are duplicated among different layers. \n\n"
											"Very useful when the layers are spliced portions of a single big mesh."));
                  parlst.addParam(new RichBool ("AlsoUnreferenced",
                                                                                        false,
                                                                                        "Keep unreferenced vertices",
                                                                                        "Do not discard unreferenced vertices from source layers\n\n"
                                                                                        "Necessary for point-only layers"));
                  break;

   default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool SampleFilterDocPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	
	switch(ID(filter)) {
		case FP_FLATTEN :
		{
			// to access to the parameters of the filter dialog simply use the getXXXX function of the FilterParameter Class
			bool deleteLayer = par.getBool("DeleteLayer");
			bool mergeVisible = par.getBool("MergeVisible");
			bool mergeVertices = par.getBool("MergeVertices");
                        bool alsounreferenced = par.getBool("AlsoUnreferenced");

			MeshModel *destMesh= md.addNewMesh("Merged Mesh");
			md.meshList.front();
			QList<MeshModel *> toBeDeletedList;
			
			int cnt=0;
			foreach(MeshModel *mmp, md.meshList)
			{ ++cnt;
				if(mmp->visible || !mergeVisible)
				{
					if(mmp!=destMesh)	
					{
							cb(cnt*100/md.meshList.size(), "Merging layers...");
							tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,mmp->cm.Tr,true);
							toBeDeletedList.push_back(mmp);
                                                        tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm,mmp->cm,false,alsounreferenced);
							tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,Inverse(mmp->cm.Tr),true);
                                                        destMesh->updateDataMask(mmp);
					}
				}		
			}
			
			if( deleteLayer )	{
				Log(GLLogStream::FILTER, "Deleted %d merged layers", toBeDeletedList.size());
				foreach(MeshModel *mmp,toBeDeletedList) {
						md.delMesh(mmp); 
					}
			}
			
			if( mergeVertices )	
			{
				int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(destMesh->cm);
				Log(GLLogStream::FILTER, "Removed %d duplicated vertices", delvert);
			}
			
			tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(destMesh->cm);
			tri::UpdateBounding<CMeshO>::Box(destMesh->cm);


			// Log function dump textual info in the lower part of the MeshLab screen. 
			Log(GLLogStream::FILTER,"Merged all the layers to single mesh of %i vertices",md.mm()->cm.vn);
				
		} break;
		default: assert (0);
	}
	return true;
}

 SampleFilterDocPlugin::FilterClass SampleFilterDocPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_FLATTEN :
      return MeshFilterInterface::Layer;     
    default : 
			return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(SampleFilterDocPlugin)
