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

#include "filter_camera.h"

#include <vcg/complex/algorithms/clean.h>


using namespace std;
using namespace vcg;

// Constructor
FilterCameraPlugin::FilterCameraPlugin()
{
	typeList <<
  FP_SET_MESH_CAMERA <<
  FP_SET_RASTER_CAMERA <<
  FP_QUALITY_FROM_CAMERA <<
  FP_MESHCAMERA_SCALE<<
  FP_RASTERCAMERA_SCALE<<
  FP_CAMERA_EDIT;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
QString FilterCameraPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_SET_MESH_CAMERA :      return QString("Set Mesh Camera");
    case FP_SET_RASTER_CAMERA :    return QString("Set Raster Camera");
    case FP_QUALITY_FROM_CAMERA :  return QString("Vertex Quality from Camera");
    case FP_MESHCAMERA_SCALE :         return QString("Scale Mesh Camera");
    case FP_RASTERCAMERA_SCALE :         return QString("Scale Raster Camera");
    case FP_CAMERA_EDIT :         return QString("Edit Raster Camera");
    default : assert(0);
  }
}

// Info() return the longer string describing each filtering action
QString FilterCameraPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_SET_MESH_CAMERA :     return QString("This filter allow to set a shot for the current mesh");
    case FP_SET_RASTER_CAMERA :   return QString("This filter allow to set a shot for the current mesh");
    case FP_QUALITY_FROM_CAMERA : return QString("Compute vertex quality using the camera definition, according to viewing angle or distance");
    case FP_MESHCAMERA_SCALE :        return QString("Scale the mesh camera");
    case FP_RASTERCAMERA_SCALE :        return QString("Scale the raster camera, or all the raster cameras of the project");
    case FP_CAMERA_EDIT :        return QString("Allow to edit the current raster camera allowing to tweak intrinsics.");
    default : assert(0);
  }
}

// This function define the needed parameters for each filter.
void FilterCameraPlugin::initParameterSet(QAction *action, MeshDocument &/*m*/, RichParameterSet & parlst)
{
  Shotf defShot;
	 switch(ID(action))
	 {
   case FP_CAMERA_EDIT :
     parlst.addParam(new RichDynamicFloat("fov_scale", 0, -3,3,"Scaling exp", "Exponent of the scaling factor. 0 means no scaling, 1 means 10 times larger, -1 means 1/10."));
     break;
   case FP_MESHCAMERA_SCALE :
     parlst.addParam(new RichFloat("scale", 1.0, "Scale factor", "The scale factor that has to be applied to the camera"));
     break;
   case FP_RASTERCAMERA_SCALE :
     parlst.addParam(new RichFloat("scale", 1.0, "Scale factor", "The scale factor that has to be applied to the camera"));
	 parlst.addParam(new RichBool ("toall", false, "Apply to all Raster layers", "Apply the same scaling to all the Raster layers"));
     break;
   case FP_SET_RASTER_CAMERA :
     parlst.addParam(new RichShotf ("Shot", defShot, "New shot", "This filter allow to set a shot for the current raster."));
     break;
   case FP_SET_MESH_CAMERA :
     parlst.addParam(new RichShotf ("Shot", defShot, "New shot", "This filter allow to set a shot for the current mesh."));
                 break;
   case FP_QUALITY_FROM_CAMERA :
                 parlst.addParam(new RichBool ("Depth", true, "Depth", "Use depth as a factor."));
                 parlst.addParam(new RichBool ("Facing", false, "ViewAngle", "Use cosine of viewing angle as a factor."));
                 parlst.addParam(new RichBool ("Clip", false,  "Clipping", "clip values outside the viewport to zero."));
                 parlst.addParam(new RichBool("normalize",false,"normalize","if checked normalize all quality values in range [0..1]"));
                 parlst.addParam(new RichBool("map",false,"map into color", "if checked map quality generated values into per-vertex color"));
                 break;
   default: break; // do not add any parameter for the other filters
  }
}

// Core Function doing the actual mesh processing.
bool FilterCameraPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
  CMeshO &cm=md.mm()->cm;
  RasterModel *rm = md.rm();
	switch(ID(filter))
  {		
  case FP_MESHCAMERA_SCALE :
    {
		float scale = par.getFloat("scale");
		cm.shot.RescalingWorld(scale);
    }
  case FP_RASTERCAMERA_SCALE :
    {
		float scale = par.getFloat("scale");
		if (!par.getBool("toall"))
			rm->shot.RescalingWorld(scale);
		else
		{
			for (int i=0; i<md.rasterList.size(); i++)
				md.rasterList[i]->shot.RescalingWorld(scale);
		}
    }
  break;
  case FP_SET_RASTER_CAMERA :
	  {
			vcg::Shotf shotGot=par.getShotf("Shot");
			rm->shot = shotGot;
			float ratio=(float)rm->currentPlane->image.height()/(float)shotGot.Intrinsics.ViewportPx[1];
			rm->shot.Intrinsics.ViewportPx[0]=rm->currentPlane->image.width();
			rm->shot.Intrinsics.ViewportPx[1]=rm->currentPlane->image.height();
			rm->shot.Intrinsics.PixelSizeMm[1]/=ratio;
			rm->shot.Intrinsics.PixelSizeMm[0]/=ratio;
			rm->shot.Intrinsics.CenterPx[0]=(int)((float)rm->shot.Intrinsics.ViewportPx[0]/2.0);
			rm->shot.Intrinsics.CenterPx[1]=(int)((float)rm->shot.Intrinsics.ViewportPx[1]/2.0);
	  }
  break;
    case FP_SET_MESH_CAMERA :
      cm.shot = par.getShotf("Shot");
    break;
    case FP_QUALITY_FROM_CAMERA :
      {
        if(!cm.shot.IsValid())
        {
          this->errorMessage="Mesh has not a valid camera";
          return false;
        }
        md.mm()->updateDataMask(MeshModel::MM_VERTQUALITY + MeshModel::MM_VERTCOLOR);
        bool clipFlag = par.getBool("Clip");
        bool depthFlag = par.getBool("Depth");
        bool facingFlag = par.getBool("Facing");
        CMeshO::VertexIterator vi;
        float deltaN = cm.bbox.Diag()/100.0f;
        for(vi=cm.vert.begin();vi!=cm.vert.end();++vi)
          {
            Point2f pp = cm.shot.Project( (*vi).P());
            float depth = cm.shot.Depth((*vi).P());
            Point3f pc = cm.shot.ConvertWorldToCameraCoordinates((*vi).P());
            Point3f pn = cm.shot.ConvertWorldToCameraCoordinates((*vi).P()+(*vi).N()*deltaN);
            float q=1.0;

            if(depthFlag) q*=depth;
            if(facingFlag) q*=pn[2]-pc[2];
            if(clipFlag)
            {
              if(pp[0]<0 || pp[1]<0 ||
                 pp[0]>cm.shot.Intrinsics.ViewportPx[0] || pp[1]>cm.shot.Intrinsics.ViewportPx[1])
                  q=0;
            }
            (*vi).Q() = q;
           }
        if(par.getBool("normalize")) tri::UpdateQuality<CMeshO>::VertexNormalize(cm);
        if(par.getBool("map")) tri::UpdateColor<CMeshO>::VertexQualityRamp(cm);

      }
    break;
  }

	return true;
}

int FilterCameraPlugin::postCondition(QAction * filter) const
{
  switch (ID(filter))
  {
    case FP_SET_MESH_CAMERA :
    case FP_MESHCAMERA_SCALE                  : return MeshModel::MM_CAMERA;
	case FP_RASTERCAMERA_SCALE                  : return MeshModel::MM_CAMERA;
    case FP_QUALITY_FROM_CAMERA           : return MeshModel::MM_VERTQUALITY + MeshModel::MM_VERTCOLOR;
    default                  : return MeshModel::MM_UNKNOWN;
  }
}

 FilterCameraPlugin::FilterClass FilterCameraPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
  case FP_MESHCAMERA_SCALE :
  case FP_RASTERCAMERA_SCALE :
  case FP_CAMERA_EDIT :
  case FP_SET_MESH_CAMERA :
  case FP_SET_RASTER_CAMERA :
  case FP_QUALITY_FROM_CAMERA :
      return MeshFilterInterface::Camera;
  }
  assert(0);
}

Q_EXPORT_PLUGIN(FilterCameraPlugin)
