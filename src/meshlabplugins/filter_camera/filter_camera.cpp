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
  FP_CAMERA_ROTATE<<
  FP_CAMERA_SCALE<<
  FP_CAMERA_TRANSLATE<<
  FP_CAMERA_TRANSFORM <<
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
	case FP_CAMERA_ROTATE :         return QString("Transform: Rotate Camera or set of cameras");
	case FP_CAMERA_SCALE :         return QString("Transform: Scale Camera or set of cameras");
	case FP_CAMERA_TRANSLATE :         return QString("Transform: Translate Camera or set of cameras");
 	case FP_CAMERA_TRANSFORM :        return QString("Transform the camera extrinsics, or all the cameras of the project.");
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
    case FP_CAMERA_ROTATE :       return QString("Rotate the camera, or all the cameras of the project. The selected raster is the reference if viewpoint rotation is selected.");
	case FP_CAMERA_SCALE :        return QString("Scale the camera, or all the cameras of the project. The selected raster is the reference if viewpoint scaling is selected.");
	case FP_CAMERA_TRANSLATE :        return QString("Translate the camera, or all the cameras of the project.");
	case FP_CAMERA_TRANSFORM :        return QString("Transform the camera extrinsics, or all the cameras of the project.");
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
   case FP_CAMERA_ROTATE :
	  {
	 QStringList shotType;
      shotType.push_back("Raster Camera");
      shotType.push_back("Mesh Camera");
	  parlst.addParam(new RichEnum("camera", 0, shotType, tr("Camera type"), tr("Choose the camera to scale")));
	  QStringList rotMethod;
      rotMethod.push_back("X axis");
      rotMethod.push_back("Y axis");
      rotMethod.push_back("Z axis");
      rotMethod.push_back("custom axis");
      parlst.addParam(new RichEnum("rotAxis", 0, rotMethod, tr("Rotation on:"), tr("Choose a method")));
      QStringList rotCenter;
      rotCenter.push_back("origin");
      rotCenter.push_back("camera viewpoint");
      rotCenter.push_back("custom point");
      parlst.addParam(new RichEnum("rotCenter", 0, rotCenter, tr("Center of rotation:"), tr("Choose a method")));
      parlst.addParam(new RichDynamicFloat("angle",0,-360,360,"Rotation Angle","Angle of rotation (in <b>degree</b>). If snapping is enable this vaule is rounded according to the snap value"));
      parlst.addParam(new RichPoint3f("customAxis",Point3f(0,0,0),"Custom axis","This rotation axis is used only if the 'custom axis' option is chosen."));
      parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This rotation center is used only if the 'custom point' option is chosen."));
	 parlst.addParam(new RichBool ("toallRaster", false, "Apply to all Raster layers", "Apply the same scaling to all the Raster layers: it is taken into account only if 'Raster Camera' is selected"));
	 parlst.addParam(new RichBool ("toall", false, "Apply to all Raster and Mesh layers", "Apply the same scaling to all the layers, including any 3D layer"));
	   }
     break;
  case FP_CAMERA_SCALE :
	  {
	 QStringList shotType;
      shotType.push_back("Raster Camera");
      shotType.push_back("Mesh Camera");
	  parlst.addParam(new RichEnum("camera", 0, shotType, tr("Camera type"), tr("Choose the camera to scale")));
		QStringList scaleCenter;
		scaleCenter.push_back("origin");
		scaleCenter.push_back("camera viewpoint");
		scaleCenter.push_back("custom point");
		parlst.addParam(new RichEnum("scaleCenter", 0, scaleCenter, tr("Center of scaling:"), tr("Choose a method")));
		parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This scaling center is used only if the 'custom point' option is chosen."));
     parlst.addParam(new RichFloat("scale", 1.0, "Scale factor", "The scale factor that has to be applied to the camera"));
	 parlst.addParam(new RichBool ("toallRaster", false, "Apply to all Raster layers", "Apply the same scaling to all the Raster layers: it is taken into account only if 'Raster Camera' is selected"));
	 parlst.addParam(new RichBool ("toall", false, "Apply to all Raster and Mesh layers", "Apply the same scaling to all the layers, including any 3D layer"));
	   }
     break;
   case FP_CAMERA_TRANSLATE :
	  {
		QStringList shotType;
        shotType.push_back("Raster Camera");
        shotType.push_back("Mesh Camera");
	    parlst.addParam(new RichEnum("camera", 0, shotType, tr("Camera type"), tr("Choose the camera to scale")));
	    parlst.addParam(new RichDynamicFloat("axisX",0,-1000,1000,"X Axis","Absolute translation amount along the X axis"));
        parlst.addParam(new RichDynamicFloat("axisY",0,-1000,1000,"Y Axis","Absolute translation amount along the Y axis"));
        parlst.addParam(new RichDynamicFloat("axisZ",0,-1000,1000,"Z Axis","Absolute translation amount along the Z axis"));
		parlst.addParam(new RichBool("centerFlag",false,"translate viewpoint position to the origin","If selected, the camera viewpoint is translated to the origin"));
		parlst.addParam(new RichBool ("toallRaster", false, "Apply to all Raster layers", "Apply the same scaling to all the Raster layers: it is taken into account only if 'Raster Camera' is selected"));
	    parlst.addParam(new RichBool ("toall", false, "Apply to all Raster and Mesh layers", "Apply the same scaling to all the layers, including any 3D layer"));
	   }
     break;
	
   case FP_CAMERA_TRANSFORM :
	  {
		QStringList shotType;
        shotType.push_back("Raster Camera");
        shotType.push_back("Mesh Camera");
		QStringList behaviour;
        behaviour.push_back("The matrix is the transformation to apply to the extrinsics");
        behaviour.push_back("The matrix represent the new extrinsics");

		vcg::Matrix44f mat; mat.SetIdentity();
	    parlst.addParam(new RichMatrix44f("TransformMatrix",mat,""));
	    parlst.addParam(new RichEnum("camera", 0, shotType, tr("Camera type"), tr("Choose the camera to scale")));
	    parlst.addParam(new RichEnum("behaviour", 0, behaviour, tr("Matrix semantic"), tr("What the matrix is used for")));
		parlst.addParam(new RichBool ("toallRaster", false, "Apply to all Raster layers", "Apply the same scaling to all the Raster layers: it is taken into account only if 'Raster Camera' is selected"));
	    parlst.addParam(new RichBool ("toall", false, "Apply to all Raster and Mesh layers", "Apply the same scaling to all the layers, including any 3D layer"));
	
	   }
     break;  case FP_SET_RASTER_CAMERA :
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
  case FP_CAMERA_ROTATE :
    {
		Matrix44f trRot; trRot.SetIdentity();
        Point3f axis, tranVec;
		Matrix44f trTran,trTranInv;

		switch(par.getEnum("rotAxis"))
		{
			case 0: axis=Point3f(1,0,0); break;
			case 1: axis=Point3f(0,1,0);break;
			case 2: axis=Point3f(0,0,1);break;
			case 3: axis=par.getPoint3f("customAxis");break;
		}
		switch(par.getEnum("rotCenter"))
		{
			case 0: tranVec=Point3f(0,0,0); break;
			case 1: {
					switch(par.getEnum("camera"))
					{
						case 0:     tranVec=rm->shot.Extrinsics.Tra();
							break;
						case 1: 	tranVec=cm.shot.Extrinsics.Tra();
					}
				}
				break;
			case 2: tranVec=par.getPoint3f("customCenter");break;
		}

		float angleDeg= par.getDynamicFloat("angle");
		
		trRot.SetRotateDeg(angleDeg,axis);
		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);
		//cm.Tr=trTran*trRot*trTranInv;
		Matrix44f transf=trTran*trRot*trTranInv;
		if (par.getBool("toall"))
		{
			for (int i=0; i<md.meshList.size(); i++)
			{
				md.meshList[i]->cm.Tr=transf;
				tri::UpdatePosition<CMeshO>::Matrix(md.meshList[i]->cm, md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerVertexMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerFaceMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateBounding<CMeshO>::Box(md.meshList[i]->cm);
				md.meshList[i]->cm.Tr.SetIdentity();
				md.meshList[i]->cm.shot.ApplyRigidTransformation(transf);
				
			}
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(transf);
				
			}
		}
		else if (par.getBool("toallRaster") && (par.getEnum("camera")==0))
		{
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(transf);

			}
		}
		else switch(par.getEnum("camera"))
		{	
			case 0: 
				{
				rm->shot.ApplyRigidTransformation(transf);
				}
		   case 1: 
				{		
				cm.shot.ApplyRigidTransformation(transf);
				}
		}
    }
  break;
  case FP_CAMERA_SCALE :
    {
		Matrix44f trScale; trScale.SetIdentity();
		Point3f tranVec;
		Matrix44f trTran,trTranInv;

		float Scale= par.getFloat("scale");
		trScale.SetScale(Scale,Scale,Scale);

		switch(par.getEnum("scaleCenter"))
		{
			case 0: tranVec=Point3f(0,0,0); break;
			case 1: 
				{
					switch(par.getEnum("camera"))
					{
						case 0:     tranVec=rm->shot.Extrinsics.Tra();
							break;
						case 1: 	tranVec=cm.shot.Extrinsics.Tra();
					}
				}
				break;
			case 2: tranVec=par.getPoint3f("customCenter");break;
		}

		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);
		if (par.getBool("toall"))
		{
			for (int i=0; i<md.meshList.size(); i++)
			{
				md.meshList[i]->cm.Tr=trTran*trScale*trTranInv;
				tri::UpdatePosition<CMeshO>::Matrix(md.meshList[i]->cm, md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerVertexMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerFaceMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateBounding<CMeshO>::Box(md.meshList[i]->cm);
				md.meshList[i]->cm.Tr.SetIdentity();
				md.meshList[i]->cm.shot.ApplyRigidTransformation(trTran);
				md.meshList[i]->cm.shot.RescalingWorld(trScale[0][0], false);
				md.meshList[i]->cm.shot.ApplyRigidTransformation(trTranInv);

			}
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(trTran);
				md.rasterList[i]->shot.RescalingWorld(trScale[0][0], false);
				md.rasterList[i]->shot.ApplyRigidTransformation(trTranInv);
			}
		}
		else if (par.getBool("toallRaster") && (par.getEnum("camera")==0))
		{
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(trTran);
				md.rasterList[i]->shot.RescalingWorld(trScale[0][0], false);
				md.rasterList[i]->shot.ApplyRigidTransformation(trTranInv);
			}
		}
		else switch(par.getEnum("camera"))
		{	
			case 0: 
				{
				rm->shot.ApplyRigidTransformation(trTran);
				rm->shot.RescalingWorld(Scale, false);
				rm->shot.ApplyRigidTransformation(trTranInv);
				}
		   case 1: 
				{		
				cm.shot.ApplyRigidTransformation(trTran);
				cm.shot.RescalingWorld(Scale, false);
				cm.shot.ApplyRigidTransformation(trTranInv);
				}
		}
    }
  break;
  case FP_CAMERA_TRANSLATE :
    {
		Matrix44f trTran; trTran.SetIdentity();

		float xScale= par.getDynamicFloat("axisX");
		float yScale= par.getDynamicFloat("axisY");
		float zScale= par.getDynamicFloat("axisZ");

		trTran.SetTranslate(xScale,yScale,zScale);
		if(par.getBool("centerFlag"))
			switch(par.getEnum("camera"))
					{
						case 0:     trTran.SetTranslate(-rm->shot.Extrinsics.Tra());
							break;
						case 1: 	trTran.SetTranslate(-cm.shot.Extrinsics.Tra());
					}

		if (par.getBool("toall"))
		{
			for (int i=0; i<md.meshList.size(); i++)
			{
				md.meshList[i]->cm.Tr=trTran;
				tri::UpdatePosition<CMeshO>::Matrix(md.meshList[i]->cm, md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerVertexMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerFaceMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateBounding<CMeshO>::Box(md.meshList[i]->cm);
				md.meshList[i]->cm.Tr.SetIdentity();
				md.meshList[i]->cm.shot.ApplyRigidTransformation(trTran);
				
			}
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(trTran);
				
			}
		}
		else if (par.getBool("toallRaster") && (par.getEnum("camera")==0))
		{
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplyRigidTransformation(trTran);

			}
		}
		else switch(par.getEnum("camera"))
		{	
			case 0: 
				{
				rm->shot.ApplyRigidTransformation(trTran);
				}
		   case 1: 
				{		
				cm.shot.ApplyRigidTransformation(trTran);
				}
		}
    }
  break;
  case FP_CAMERA_TRANSFORM :
	  {
		vcg::Matrix44f mat,inv; 
		inv.SetIdentity();
		vcg::Point3f tra;
	   	mat = par.getMatrix44("TransformMatrix"); 
		if(par.getEnum("behaviour") == 1){
			inv = rm->shot.Extrinsics.Rot();
			tra =  inv * rm->shot.Extrinsics.Tra();
			inv[0][3] = -tra[0];
			inv[1][3] = -tra[1];
			inv[2][3] = -tra[2];
			mat = mat * inv;
			mat = inv;
		}

	  if (par.getBool("toall"))
	  {
			for (int i=0; i<md.meshList.size(); i++)
			{
				md.meshList[i]->cm.Tr = mat;
				tri::UpdatePosition<CMeshO>::Matrix(md.meshList[i]->cm, md.meshList[i]->cm.Tr);
				tri::UpdateNormals<CMeshO>::PerFaceMatrix(md.meshList[i]->cm,md.meshList[i]->cm.Tr);
				tri::UpdateBounding<CMeshO>::Box(md.meshList[i]->cm);
				md.meshList[i]->cm.Tr.SetIdentity();
				md.meshList[i]->cm.shot.ApplySimilarity(mat);
				
			}
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplySimilarity(mat);
				
			}
		}
		else if (par.getBool("toallRaster") && (par.getEnum("camera")==0))
		{
			for (int i=0; i<md.rasterList.size(); i++)
			{
				md.rasterList[i]->shot.ApplySimilarity(mat);

			}
		}
		else switch(par.getEnum("camera"))
		{	
			case 0: 
				{
				rm->shot.ApplyRigidTransformation(mat);
				}
		   case 1: 
				{		
				cm.shot.ApplyRigidTransformation(mat);
				}
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
	case FP_CAMERA_ROTATE   : 	
	case FP_CAMERA_TRANSLATE   :               
	case FP_CAMERA_TRANSFORM:
	case FP_CAMERA_SCALE                  : return MeshModel::MM_CAMERA;
    case FP_QUALITY_FROM_CAMERA           : return MeshModel::MM_VERTQUALITY + MeshModel::MM_VERTCOLOR;
    default                  : return MeshModel::MM_UNKNOWN;
  }
}

 FilterCameraPlugin::FilterClass FilterCameraPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
  case FP_CAMERA_ROTATE :
  case FP_CAMERA_SCALE :
  case FP_CAMERA_TRANSLATE :
  case FP_CAMERA_EDIT :
  case FP_CAMERA_TRANSFORM:
  case FP_SET_MESH_CAMERA :
  case FP_QUALITY_FROM_CAMERA :
      return MeshFilterInterface::Camera;
  case FP_SET_RASTER_CAMERA :
	  return FilterClass (MeshFilterInterface::Camera + MeshFilterInterface::RasterLayer) ;



  }
  assert(0);
}

Q_EXPORT_PLUGIN(FilterCameraPlugin)
