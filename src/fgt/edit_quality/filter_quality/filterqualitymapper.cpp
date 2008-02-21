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
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include "filterqualitymapper.h"


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

QualityMapperFilter::QualityMapperFilter() 
{ 
	typeList << FP_QUALITY_MAPPER;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString QualityMapperFilter::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_QUALITY_MAPPER :  return QString("Quality Mapper edit applier"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString QualityMapperFilter::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_QUALITY_MAPPER :  return QString("Edits color of mesh vertexes using Quality Mapper edit functionalities"); 
		default : assert(0); 
	}
}

const PluginInfo &QualityMapperFilter::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Federico Bellucci, Alessandro Maione");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void QualityMapperFilter::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
//void QualityMapperFilter::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	QString csvFileName = 0;
	 switch(ID(action))	 {
		case FP_QUALITY_MAPPER :
			//user chooses the file to load
			csvFileName = QFileDialog::getOpenFileName(0, "Open Input CSV File", QDir::currentPath(), "CSV File (*.csv)");

			EQUALIZER_INFO eqData;
			eqData.minQualityVal = 0.0f;
			eqData.midQualityPercentage = 0.5f;
			eqData.maxQualityVal = 100.0f;
			eqData.brightness = 1.0f;

			if (!csvFileName.isNull())
			{
				//setting equalizer values
				loadEqualizerInfo(csvFileName, &eqData);
			}

			parlst.addString("csvFileName", csvFileName, "CSV input File" );
			parlst.addFloat("minQualityVal", eqData.minQualityVal, "Minimum mesh quality" );
			parlst.addFloat("maxQualityVal", eqData.maxQualityVal, "Maximum mesh quality" );
			parlst.addAbsPerc("midHandlePos", eqData.midQualityPercentage, 0.0f, 1.0f, "Middle quality percentage position", "defines the percentage position of middle quality value");
			parlst.addFloat("brightness", eqData.brightness, "mesh brightness" );
			break;
											
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool QualityMapperFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	QString csvFileName = par.getString("csvFileName");
	if (!csvFileName.isNull())
	{
		//building new TF object from external file
		TransferFunction transferFunction( csvFileName );
		applyColorByVertexQuality(m, &transferFunction, par.getFloat("minQualityVal"), par.getFloat("maxQualityVal"), par.getAbsPerc("midHandlePos"), par.getFloat("brightness"));

		// Log function dump textual info in the lower part of the MeshLab screen. 
		//Log(0,"Successfully displaced %i vertices",m.cm.vn);

		return true;
	}
	else
		return false;
}


Q_EXPORT_PLUGIN(QualityMapperFilter)
