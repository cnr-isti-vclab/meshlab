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

#include "sample_xmlfilter.h"
#include <QtScript>

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SampleXMLFilterPlugin::SampleXMLFilterPlugin() 
{ 
}

bool SampleXMLFilterPlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * cb )
{
	if (filterName == "Random vertex displacement")
	{
		MeshModel &m=*md.mm();
		srand(time(NULL)); 
		const float max_displacement =env.evalFloat("Displacement");

		for(unsigned int i = 0; i< m.cm.vert.size(); i++)
		{
			// Typical usage of the callback for showing a nice progress bar in the bottom. 
		//	// First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
			cb(100*i/m.cm.vert.size(), "Randomly Displacing...");

			float rndax = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
			float rnday = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
			float rndaz = (float(2.0f*rand())/RAND_MAX - 1.0f ) *max_displacement;
			m.cm.vert[i].P() += vcg::Point3f(rndax,rnday,rndaz);		
		}

		//// Log function dump textual info in the lower part of the MeshLab screen. 
		Log("Successfully displaced %i vertices",m.cm.vn);

		//// to access to the parameters of the filter dialog simply use the getXXXX function of the FilterParameter Class
		if(env.evalBool("UpdateNormals"))	
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		
 		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);

		return true;
	}
	return false;
}

Q_EXPORT_PLUGIN(SampleXMLFilterPlugin)
