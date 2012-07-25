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

#include "filter_layer.h"

#include<vcg/complex/append.h>



using namespace std;
using namespace vcg;

// Constructor
FilterLayerPlugin::FilterLayerPlugin()
{
  typeList <<
              FP_FLATTEN <<
              FP_DELETE_MESH <<
              FP_DELETE_RASTER <<
              FP_SPLITSELECT <<
              FP_SPLITCONNECTED <<
              FP_RENAME_MESH <<
			  FP_RENAME_RASTER <<
              FP_DUPLICATE <<
              FP_SELECTCURRENT;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
 QString FilterLayerPlugin::filterName(FilterIDType filterId) const
{
   switch(filterId) {
   case FP_SPLITSELECT :  return QString("Move selection on another layer");
   case FP_SPLITCONNECTED :  return QString("Split in Connected Components");
   case FP_DUPLICATE :  return QString("Duplicate Current layer");
   case FP_DELETE_MESH :  return QString("Delete Current Mesh");
   case FP_DELETE_RASTER :  return QString("Delete Current Raster");
   case FP_FLATTEN :  return QString("Flatten Visible Layers");
   case FP_RENAME_MESH :  return QString("Rename Current Mesh");
   case FP_RENAME_RASTER :  return QString("Rename Current Raster");
   case FP_SELECTCURRENT :  return QString("Change the current layer");
   default : assert(0);
   }
 }

// Info() return the longer string describing each filtering action
 QString FilterLayerPlugin::filterInfo(FilterIDType filterId) const
{
   switch(filterId) {
   case FP_SPLITSELECT :  return QString("Selected faces are moved (or duplicated) in a new layer");
   case FP_DELETE_MESH :  return QString("The current mesh layer is deleted");
   case FP_DELETE_RASTER :  return QString("The current raster layer is deleted");
   case FP_SPLITCONNECTED:  return QString("Split current Layer into many layers, one for each connected components");
   case FP_DUPLICATE :  return QString("Create a new layer containing the same model as the current one");
   case FP_FLATTEN :  return QString("Flatten all or only the visible layers into a single new mesh. <br> Transformations are preserved. Existing layers can be optionally deleted");
   case FP_RENAME_MESH :  return QString("Explicitly change the label shown for a given mesh");
   case FP_RENAME_RASTER :  return QString("Explicitly change the label shown for a given raster");
   case FP_SELECTCURRENT :  return QString("Change the current layer from its name");
   default : assert(0);
   }
 }

// This function define the needed parameters for each filter.
void FilterLayerPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst)
{
  MeshModel *mm=md.mm();
  RasterModel *rm=md.rm();
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
   case FP_FLATTEN :
       parlst.addParam(new RichBool ("MergeVisible", true, "Merge Only Visible Layers",
                       "Merge the vertices that are duplicated among different layers. <br>"
                       "Very useful when the layers are spliced portions of a single big mesh."));
       parlst.addParam(new RichBool ("DeleteLayer", true, "Delete Layers ",
                       "Delete all the merged layers. <br>If all layers are visible only a single layer will remain after the invocation of this filter"));
       parlst.addParam(new RichBool ("MergeVertices", true,  "Merge duplicate vertices",
                       "Merge the vertices that are duplicated among different layers. \n\n"
                       "Very useful when the layers are spliced portions of a single big mesh."));
       parlst.addParam(new RichBool ("AlsoUnreferenced", false, "Keep unreferenced vertices",
                       "Do not discard unreferenced vertices from source layers\n\n"
                       "Necessary for point-only layers"));
     break;
   case FP_RENAME_MESH :
       parlst.addParam(new RichString ("newName",
                       mm->label(),
                       "New Label",
                       "New Label for the mesh"));
       break;
   case FP_RENAME_RASTER :
       parlst.addParam(new RichString ("newName",
                       rm->label(),
                       "New Label",
                       "New Label for the raster"));
       break;
   case FP_SELECTCURRENT :
			parlst.addParam(new RichMesh ("mesh",md.mm(),&md, "Mesh",
                                          "The name of the current mesh"));
       break;
	 default: break; // do not add any parameter for the other filters
  }
}

// Core Function doing the actual mesh processing.
bool FilterLayerPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
  CMeshO::FaceIterator fi;
  int numFacesSel,numVertSel;

  switch(ID(filter))
  {
  case  FP_RENAME_MESH:          md.mm()->setLabel(par.getString("newName"));  break;
  case  FP_RENAME_RASTER:          md.rm()->setLabel(par.getString("newName"));  break;
  case  FP_SELECTCURRENT:   md.setCurrent(par.getMesh("mesh"));           break;
  case  FP_DELETE_MESH :    
	  if(md.mm()) 
		  md.delMesh(md.mm());    
	  break;
  case  FP_DELETE_RASTER :  if(md.rm()) md.delRaster(md.rm());            break;

  case FP_SPLITSELECT :
  {
    // creating the new layer
    // that is the back one
    MeshModel *currentMesh  = md.mm();				// source = current
    MeshModel *destMesh= md.addNewMesh("","SelectedSubset"); // After Adding a mesh to a MeshDocument the new mesh is the current one
    // select all points involved
    tri::UpdateSelection<CMeshO>::VertexClear(currentMesh->cm);
    tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currentMesh->cm);

    tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, currentMesh->cm, true);

    numFacesSel = tri::UpdateSelection<CMeshO>::FaceCount(currentMesh->cm);
    numVertSel  = tri::UpdateSelection<CMeshO>::VertexCount(currentMesh->cm);

	destMesh->updateDataMask(currentMesh);
    if(par.getBool("DeleteOriginal"))	// delete original faces
    {
      CMeshO::VertexIterator vi;
      CMeshO::FaceIterator   fi;
      tri::UpdateSelection<CMeshO>::VertexClear(currentMesh->cm);
      tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(currentMesh->cm);
      for(fi=currentMesh->cm.face.begin();fi!=currentMesh->cm.face.end();++fi)
        if(!(*fi).IsD() && (*fi).IsS() )
          tri::Allocator<CMeshO>::DeleteFace(currentMesh->cm,*fi);
      for(vi=currentMesh->cm.vert.begin();vi!=currentMesh->cm.vert.end();++vi)
        if(!(*vi).IsD() && (*vi).IsS() )
          tri::Allocator<CMeshO>::DeleteVertex(currentMesh->cm,*vi);

      tri::UpdateSelection<CMeshO>::VertexClear(currentMesh->cm);
      tri::UpdateSelection<CMeshO>::FaceClear(currentMesh->cm);

      currentMesh->clearDataMask(MeshModel::MM_FACEFACETOPO );

      Log("Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
    }
    else								// keep original faces
    {
      Log("Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
    }
    vcg::tri::UpdateFlags<CMeshO>::VertexClear(destMesh->cm,CMeshO::VertexType::SELECTED);
    vcg::tri::UpdateFlags<CMeshO>::FaceClear(destMesh->cm,CMeshO::FaceType::SELECTED);

    // init new layer
    tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
    for(fi=destMesh->cm.face.begin();fi!=destMesh->cm.face.end();++fi)	// face normals
      face::ComputeNormalizedNormal(*fi);
    tri::UpdateNormals<CMeshO>::PerVertex(destMesh->cm);				// vertex normals
    destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
  } break;

  case FP_DUPLICATE :
  {
    // creating the new layer
    // that is the back one
    MeshModel *currentMesh  = md.mm();				// source = current
    QString newName = currentMesh->label() + "_copy";
    MeshModel *destMesh= md.addNewMesh("",newName); // After Adding a mesh to a MeshDocument the new mesh is the current one
    destMesh->updateDataMask(currentMesh);
    tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, currentMesh->cm);

    Log("Duplicated current model to layer %i", md.meshList.size());

    // init new layer
    tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
    for(fi=destMesh->cm.face.begin();fi!=destMesh->cm.face.end();++fi)	// face normals
      face::ComputeNormalizedNormal(*fi);
    tri::UpdateNormals<CMeshO>::PerVertex(destMesh->cm);				// vertex normals
    destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
  } break;

  case FP_FLATTEN :
  {
    // to access to the parameters of the filter dialog simply use the getXXXX function of the FilterParameter Class
    bool deleteLayer = par.getBool("DeleteLayer");
    bool mergeVisible = par.getBool("MergeVisible");
    bool mergeVertices = par.getBool("MergeVertices");
    bool alsoUnreferenced = par.getBool("AlsoUnreferenced");

    MeshModel *destMesh= md.addNewMesh("","Merged Mesh");
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
          if(!alsoUnreferenced)
            vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmp->cm);
          tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm,mmp->cm);
          tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,Inverse(mmp->cm.Tr),true);
          destMesh->updateDataMask(mmp);
        }
      }
    }

    if( deleteLayer )	{
      Log( "Deleted %d merged layers", toBeDeletedList.size());
      foreach(MeshModel *mmp,toBeDeletedList) {
        md.delMesh(mmp);
      }
    }

    if( mergeVertices )
    {
      int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(destMesh->cm);
      Log( "Removed %d duplicated vertices", delvert);
    }

    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(destMesh->cm);
    tri::UpdateBounding<CMeshO>::Box(destMesh->cm);
    Log("Merged all the layers to single mesh of %i vertices",md.mm()->cm.vn);
  } break;

  case FP_SPLITCONNECTED :
  {
    CMeshO &cm = md.mm()->cm;
    md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
    std::vector< std::pair<int,CMeshO::FacePointer> > connectedCompVec;
    int numCC = tri::Clean<CMeshO>::ConnectedComponents(cm,  connectedCompVec);
    Log("Found %i Connected Components",numCC);
    for(size_t i=0; i<connectedCompVec.size();++i)
    {
      tri::UpdateSelection<CMeshO>::FaceClear(cm);
      connectedCompVec[i].second->SetS();
      tri::UpdateSelection<CMeshO>::FaceConnectedFF(cm);
      tri::UpdateSelection<CMeshO>::VertexClear(cm);
      tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(cm);

      MeshModel *destMesh= md.addNewMesh("",QString("CC %1").arg(i));
      tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, cm, true);
      tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
      tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(destMesh->cm);				// vertex normals
      destMesh->cm.Tr = cm.Tr;								// copy transformation
    }
  } break;
  }

    return true;
  }

FilterLayerPlugin::FilterClass FilterLayerPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_RENAME_MESH :
    case FP_SPLITSELECT :
    case FP_DUPLICATE :
    case FP_FLATTEN :
    case FP_SELECTCURRENT :
    case FP_SPLITCONNECTED :
    case FP_DELETE_MESH :
      return MeshFilterInterface::Layer;
	case FP_RENAME_RASTER :
    case FP_DELETE_RASTER :
      return MeshFilterInterface::RasterLayer;

    default :  assert(0);
      return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(FilterLayerPlugin)
