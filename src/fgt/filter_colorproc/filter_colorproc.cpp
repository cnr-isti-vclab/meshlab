/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>
#include "filter_colorproc.h"

#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/color.h>

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterColorProc::FilterColorProc()

{
  typeList << CP_COLOR_FILLING<<CP_TRESHOLDING<<CP_BRIGHTNESS<<CP_CONTRAST<<CP_GAMMA;

  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterColorProc::~FilterColorProc() {
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

const QString FilterColorProc::filterName(FilterIDType filter)
{
 switch(filter)
  {
      case CP_COLOR_FILLING : return "Color Filling";
      case CP_TRESHOLDING : return "Tresholding";
      case CP_BRIGHTNESS : return "Brightness";
      case CP_CONTRAST : return "Contrast";
      case CP_GAMMA : return "Gamma";
   default: assert(0);
  }
  return QString("error!");
}

const QString FilterColorProc::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
    case CP_COLOR_FILLING : return "Color Filling";
      case CP_TRESHOLDING : return "Tresholding";
      case CP_BRIGHTNESS : return "Brightness";
      case CP_CONTRAST : return "Contrast";
      case CP_GAMMA : return "Gamma";
	default: assert(0);
  }
  return QString("error!");
}


const PluginInfo &FilterColorProc::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr( __DATE__ );
	 ai.Version = tr("0.1");
	 ai.Author = ("Francesco Tonarelli");
   return ai;
}

const int FilterColorProc::getRequirements(QAction *action)
{
    return 0;
}

void FilterColorProc::initParameterSet(QAction *a, MeshModel &m, FilterParameterSet & par)
{
	switch(ID(a))
  {
	case CP_COLOR_FILLING: {
			float r = 255, g = 255, b = 255;
			par.addFloat("r", r, "Red:", "Set the <b><i>Red</b></i> component of the color.");
			par.addFloat("g", g, "Green:", "Set the <b><i>Green</b></i> component of the color.");
			par.addFloat("b", b, "Blue:", "Set the <b><i>Blue</b></i> component of the color.");
			break;
		}
	default: assert(0);
	}
}

bool FilterColorProc::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb)
{
    switch(ID(filter))
      {
        case CP_COLOR_FILLING: {
                int r = (int)(math::Clamp(par.getFloat("r"), 0.0f, 255.0f));
                int g = (int)(math::Clamp(par.getFloat("g"), 0.0f, 255.0f));
                int b = (int)(math::Clamp(par.getFloat("b"), 0.0f, 255.0f));
                Color4b new_col = Color4b(r,g,b,1);
			CMeshO::VertexIterator vi;
                for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
                if(!(*vi).IsD())
			if((*vi).IsS()) {(*vi).C() = new_col; }
			else (*vi).C() = new_col;
                return true;
            }
        default: assert(0);
        }
	return false;
}

const MeshFilterInterface::FilterClass FilterColorProc::getClass(QAction *a)
{
  switch(ID(a))
  {
    case CP_COLOR_FILLING :
    case CP_TRESHOLDING :
    case CP_BRIGHTNESS :
    case CP_CONTRAST :
    case CP_GAMMA : return MeshFilterInterface::VertexColoring;
	default: assert(0);
  }
}
Q_EXPORT_PLUGIN(FilterColorProc)
