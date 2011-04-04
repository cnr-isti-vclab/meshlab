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
#include <QFileDialog>
#include "filter_bnpts.h"

using namespace std;
using namespace vcg;

// Constructor
FilterBnptsPlugin::FilterBnptsPlugin()
{
	typeList <<
	FP_BNPTSGEN;

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
QString FilterBnptsPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_BNPTSGEN	:	return QString("generate a BNPTS file for out-of-core poisson");
		default : assert(0);
	}
}

// Info() return the longer string describing each filtering action
 QString FilterBnptsPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_BNPTSGEN	:	return QString("generate a BNPTS file using data from all layers. This file contains points+normals and it is used with the external tool for out-of-core poisson merging");
    default : assert(0);
	}
}

// This function define the needed parameters for each filter.
void FilterBnptsPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst)
{
	MeshModel *mm=md.mm();
	
	switch(ID(action))
	{
		case FP_BNPTSGEN :
		{
			parlst.addParam(new RichBool ("onlyvisible",
				true,
				"Use only visible layers",
				"If true, only visible layers are used for generating the BNPTS"));
			parlst.addParam(new RichBool ("append",
				false,
				"append to existing file",
				"If true and the output files already exist, appends data to \n\n"
				"the existing BNPTS"));				
		}
		break;
   
   
		default: break; // do not add any parameter for the other filters
	}
}

// Core Function doing the actual mesh processing.
bool FilterBnptsPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	CMeshO::FaceIterator fi;

	switch(ID(filter))
	{

		case FP_BNPTSGEN :
		{
      bool onlyvisiblelayers = par.getBool("onlyvisible");
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

		} break;
	}
	
	return true;
}

FilterBnptsPlugin::FilterClass FilterBnptsPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_BNPTSGEN :
      return MeshFilterInterface::Layer;
    default :  assert(0);
      return MeshFilterInterface::Generic;
}
}

Q_EXPORT_PLUGIN(FilterBnptsPlugin)
