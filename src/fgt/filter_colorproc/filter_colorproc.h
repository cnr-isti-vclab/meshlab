/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTERCOLORPROCPLUGIN_H
#define FILTERCOLORPROCPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <math.h>
#include <vcg/complex/trimesh/update/color.h>

class FilterColorProc : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	public:
		enum { CP_FILLING,
           CP_TRESHOLDING,
           CP_BRIGHTNESS,
           CP_CONTRAST,
           CP_CONTR_BRIGHT,
           CP_GAMMA,
           CP_LEVELS,
           CP_INVERT,
           CP_COLOURISATION };

		FilterColorProc();
		~FilterColorProc();

    virtual const FilterClass getClass(QAction *);
		virtual const QString filterName(FilterIDType filter);
		virtual const QString filterInfo(FilterIDType filter);
		virtual const PluginInfo &pluginInfo();

		virtual const int getRequirements(QAction *);

		virtual bool autoDialog(QAction *);
		virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/);
		virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb);





    static int levels(MeshModel &m, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      int counter=0;

      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_levels((*vi).C(),gamma, in_min, in_max, out_min, out_max);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_levels((*vi).C(),gamma, in_min, in_max, out_min, out_max);
            ++counter;
          }
        }
      }
      return counter;
    }

    static vcg::Color4b color_levels(vcg::Color4b c, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      return vcg::Color4b( value_levels(c[0]/255, gamma, in_min/255, in_max/255, out_min/255, out_max/255),
                           value_levels(c[1]/255, gamma, in_min/255, in_max/255, out_min/255, out_max/255),
                           value_levels(c[2]/255, gamma, in_min/255, in_max/255, out_min/255, out_max/255), 1);
    }

    static int value_levels(int value, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      // normalize
      if(in_max == in_min) value = 0;
      else value = (value-in_min) / (in_max - in_min);
      // transform gamma
      value = (int)(pow(value,gamma));
      // rescale range
      value = value * (out_max - out_min) + out_min;
      //back in interval [0,255] and clamp
      return vcg::math::Clamp<int>(value*255, 0, 255);
    }
};
#endif
