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

    enum rgbChMask {ALL_CHANNELS = 7, RED_CHANNEL = 4, GREEN_CHANNEL = 2, BLUE_CHANNEL = 1, NO_CHANNELS = 0 };


    static int levels(MeshModel &m, float gamma, float in_min, float in_max, float out_min, float out_max, unsigned char rgbMask)
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
              (*vi).C() = color_levels((*vi).C(),gamma, in_min, in_max, out_min, out_max, rgbMask);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            if(counter==0)
            (*vi).C() = color_levels((*vi).C(),gamma, in_min, in_max, out_min, out_max, rgbMask);
            ++counter;
          }
        }
      }
      return counter;
    }

    static vcg::Color4b color_levels(vcg::Color4b c, float gamma, float in_min, float in_max, float out_min, float out_max, unsigned char rgbMask)
    {
      unsigned char r = c[0], g = c[1], b = c[2];
      if(rgbMask & RED_CHANNEL) r = value_levels(c[0], gamma, in_min, in_max, out_min, out_max);
      if(rgbMask & GREEN_CHANNEL) g = value_levels(c[1], gamma, in_min, in_max, out_min, out_max);
      if(rgbMask & BLUE_CHANNEL) b = value_levels(c[2], gamma, in_min, in_max, out_min, out_max);
      return vcg::Color4b(r, g, b, 255);
    }

    static int value_levels(int value, float gamma, float in_min, float in_max, float out_min, float out_max)
    {
      float fvalue = value/255.0f;
      // normalize
      if(in_max == in_min) fvalue = 0;
      else fvalue = vcg::math::Clamp<float>(fvalue - in_min, 0.0f, 1.0f) / vcg::math::Clamp<float>(in_max - in_min, 0.0f, 1.0f);
      // transform gamma
      fvalue = (pow(fvalue,1/gamma));
      // rescale range
      fvalue = fvalue * vcg::math::Clamp<float>(out_max - out_min,0.0f,1.0f) + out_min;
      //back in interval [0,255] and clamp
      return vcg::math::Clamp<int>((int)(fvalue * 255), 0, 255);
    }
};
#endif
