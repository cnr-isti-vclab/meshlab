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
#include <QFileDialog>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "filter_color_projection.h"


using namespace std;
using namespace vcg;

// Constructor
FilterColorProjectionPlugin::FilterColorProjectionPlugin()
{
	typeList <<
	FP_SINGLEIMAGEPROJ;

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
QString FilterColorProjectionPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_SINGLEIMAGEPROJ	:	return QString("Project current raster color to current mesh");
		default : assert(0);
	}
}

// Info() return the longer string describing each filtering action
 QString FilterColorProjectionPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_SINGLEIMAGEPROJ	:	return QString("Color information from the current raster is perspective-projected on the current mesh");
    default : assert(0);
	}
}

// What "new" properties the plugin requires
int FilterColorProjectionPlugin::getRequirements(QAction *action){
  switch(ID(action)){
    case FP_SINGLEIMAGEPROJ:  return MeshModel::MM_VERTCOLOR;
    default: assert(0); return 0;
  }
  return 0;
}


// This function define the needed parameters for each filter.
void FilterColorProjectionPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst)
{
	MeshModel *mm=md.mm();
	
	switch(ID(action))
	{
		case FP_SINGLEIMAGEPROJ :
		{
			parlst.addParam(new RichBool ("usedepth",
				true,
				"Use depth for projection",
				"If true, depth is used to restrict projection on visible faces"));
		}
		break;
      
		default: break; // do not add any parameter for the other filters
	}
}

// Core Function doing the actual mesh processing.
bool FilterColorProjectionPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	CMeshO::FaceIterator fi;
  CMeshO::VertexIterator vi;

	switch(ID(filter))
	{

		case FP_SINGLEIMAGEPROJ :
		{
      bool use_depth = par.getBool("usedepth");

      // get current raster
      RasterModel *raster   = md.rm();
      MeshModel   *model    = md.mm();

      // no drawing if camera not valid
      if(!raster->shot.IsValid())  
        return false;

      // the mesh has to be correctly transformed before mapping
      tri::UpdatePosition<CMeshO>::Matrix(model->cm,model->cm.Tr,true);

      for(vi=model->cm.vert.begin();vi!=model->cm.vert.end();++vi)
      {
        if(!(*vi).IsD())
        {
          Point2f pp = raster->shot.Project((*vi).P());
          
          //float depth = raster->shot.Depth((*vi).P());

          //if inside image
          if(pp[0]>0 && pp[1]>0 && pp[0]<raster->shot.Intrinsics.ViewportPx[0] && pp[1]<raster->shot.Intrinsics.ViewportPx[1])
          {
            QRgb pcolor = raster->currentPlane->image.pixel(pp[0],raster->shot.Intrinsics.ViewportPx[1] - pp[1]);

            (*vi).C() = vcg::Color4b(qRed(pcolor), qGreen(pcolor), qBlue(pcolor), 255);          
          }
          else
          {
            //(*vi).C() = vcg::Color4b(128, 100, 150, 255);
          }
          

        }
      }

      // the mesh has to return to its original position
      tri::UpdatePosition<CMeshO>::Matrix(model->cm,Inverse(model->cm.Tr),true);



/*
      bool appendexisting = par.getBool("append");
      CMeshO::VertexIterator vi;

      // destination file
      QString filename = QFileDialog::getSaveFileName((QWidget*)NULL,tr("Export BNPTS File"), QDir::currentPath(), tr("BNPTS file (*.Bnpts )"));
      FILE* outfile=NULL;

      if(appendexisting)
        outfile  = fopen(qPrintable(filename), "wba");
      else
        outfile  = fopen(qPrintable(filename), "wb");

      if(outfile==NULL)
        return false;

      int cnt=0;
      foreach(MeshModel *mmp, md.meshList)
      { 
        ++cnt;

        // visible ?
        if(mmp->visible || !onlyvisiblelayers)
        {
          float buff[6];
          
          // the mesh has to be correctly transformed before exporting
          tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,mmp->cm.Tr,true);

          for(vi=mmp->cm.vert.begin();vi!=mmp->cm.vert.end();++vi)
					  if(!(*vi).IsD())
            {
              buff[0]=vi->P().X();
              buff[1]=vi->P().Y();
              buff[2]=vi->P().Z();
              buff[3]=vi->N().X();
              buff[4]=vi->N().Y();
              buff[5]=vi->N().Z();

              fwrite(buff,sizeof(float),6,outfile);
            }

          // the mesh has to return to its original position
          tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,Inverse(mmp->cm.Tr),true);
        }     
      }

      fclose(outfile);
*/
		} break;
	}
	
	return true;
}

FilterColorProjectionPlugin::FilterClass FilterColorProjectionPlugin::getClass(QAction *a)
{
  switch(ID(a)) {
    case FP_SINGLEIMAGEPROJ :
      return FilterClass(Camera + VertexColoring);
    default :  assert(0);
      return MeshFilterInterface::Generic;
  }
}

int FilterColorProjectionPlugin::postCondition( QAction* a ) const{
  switch(ID(a)) {
    case FP_SINGLEIMAGEPROJ:
      return MeshModel::MM_VERTCOLOR;
    default: assert(0);
      return MeshModel::MM_NONE;
	}
}

Q_EXPORT_PLUGIN(FilterColorProjectionPlugin)
