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
		case FP_QUALITY_MAPPER :  return QString("Quality Mapper applier"); 
		default : assert(0); 
	}
  return QString("");
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString QualityMapperFilter::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_QUALITY_MAPPER :  return QString("Edits color of mesh vertexes using Quality Mapper edit functionalities"); 
		default : assert(0); 
	}
  return QString("");
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
	 switch(ID(action))	 {
		case FP_QUALITY_MAPPER :
			{
				_meshMinMaxQuality = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);

				parlst.addString("csvFileName", "", "CSV input File" );
				parlst.addFloat("minQualityVal", _meshMinMaxQuality.minV, "Minimum mesh quality" );
				parlst.addFloat("maxQualityVal", _meshMinMaxQuality.maxV, "Maximum mesh quality" );
				parlst.addFloat("midHandlePos", _meshMinMaxQuality.minV + ((_meshMinMaxQuality.maxV-_meshMinMaxQuality.minV)/2.0f), "Middle quality percentage position", "defines the percentage position of middle quality value" );
				parlst.addFloat("brightness", 1.0f, "Mesh brightness", "must be between 0 and 2. 0 represents a completely dark mesh, 1 represents a mesh colorized with pure colors, 2 represents a completely bright mesh");
			}
			break;
											
		default : assert(0); break;
	}
}

// The Real Core Function doing the actual mesh processing.
// Apply color to mesh vertexes
bool QualityMapperFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	Q_UNUSED(filter);
	Q_UNUSED(cb);
	QString csvFileName = par.getString("csvFileName");

	EQUALIZER_INFO eqData;
	eqData.minQualityVal = par.getFloat("minQualityVal");
	eqData.midQualityPercentage = par.getFloat("midHandlePos");
	eqData.maxQualityVal = par.getFloat("maxQualityVal");
	eqData.brightness = 1.0f;

	if ( csvFileName != "" )
	{
		//setting equalizer values
		if ( loadEqualizerInfo(csvFileName, &eqData) > 0 )
		{
			par.setFloat("minQualityVal", eqData.minQualityVal );
			par.setFloat("maxQualityVal", eqData.maxQualityVal );
			par.setFloat("midHandlePos", _meshMinMaxQuality.minV + ((_meshMinMaxQuality.maxV-_meshMinMaxQuality.minV)/eqData.midQualityPercentage));
			par.setFloat("brightness", eqData.brightness);

			//building new TF object from external file
			TransferFunction transferFunction( par.getString("csvFileName") );
			// Applying colors
			applyColorByVertexQuality(m, &transferFunction, par.getFloat("minQualityVal"), par.getFloat("maxQualityVal"), par.getFloat("midHandlePos"), par.getFloat("brightness"));
		}
		else
		{
			QErrorMessage fileNotFoundError(0);
			fileNotFoundError.showMessage("Something went wrong while trying to open the file specified");
			return false;
		}

		return true;
	}
	else
		return false;
}


Q_EXPORT_PLUGIN(QualityMapperFilter)
