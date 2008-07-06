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

    //Apply the color filling filter, with the given color, to the mesh.
    //This don't need to work on the original per vertex color.
		static int Filling(MeshModel &m, vcg::Color4b c)
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
              (*vi).C() = c;
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = c;
            ++counter;
          }
        }
      }
      return counter;
    }

    //Apply the tresholding filter, with the given treshold, to the mesh.
    static int Tresholding(MeshModel &m, float treshold)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              float value = compute_lightness((*ci));

              if(value<=treshold) (*vi).C() = vcg::Color4b(0,0,0,1);
              else (*vi).C() = vcg::Color4b(255,255,255,1);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            float value = compute_lightness((*ci));

            if(value<=treshold) (*vi).C() = vcg::Color4b(0,0,0,1);
            else (*vi).C() = vcg::Color4b(255,255,255,1);
            ++counter;
          }
        }
      }
      return counter;
    }

    //Apply the brightness filter, with the given amount, to the mesh.
    static int Brighting(MeshModel &m, int amount)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_add((*ci),amount);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_add((*ci),amount);
            ++counter;
          }
        }
      }
      return counter;
    }

    static int Contrast(MeshModel &m, float factor)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_mul((*ci),factor);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_mul((*ci),factor);
            ++counter;
          }
        }
      }
      return counter;
    }

    static int ContrastBrightness(MeshModel &m, float factor, int amount)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_mul_add((*ci),factor,amount);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_mul_add((*ci),factor,amount);
            ++counter;
          }
        }
      }
      return counter;
    }

    //Apply the gamma correction filter, with the given gamma exponet, to the mesh.
    static int Gamma(MeshModel &m, float gamma)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_gamma((*ci),gamma);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_gamma((*ci),gamma);
            ++counter;
          }
        }
      }
      return counter;
    }

    //Invert the rgb components of the color.
    //This don't need to work on the original per vertex color.
    static int Invert(MeshModel &m)
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
              (*vi).C() = color_invert((*vi).C());
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_invert((*vi).C());
            ++counter;
          }
        }
      }
      return counter;
    }


    static int Levels(MeshModel &m, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_levels((*ci),gamma, in_min, in_max, out_min, out_max);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_levels((*ci),gamma, in_min, in_max, out_min, out_max);
            ++counter;
          }
        }
      }
      return counter;
    }

    static int Colourisation(MeshModel &m, vcg::Color4b c,float intensity)
    {
      int counter=0;

      //security check: arrays of differents sizes means that something wrong copying
      //original colors. we can't work out the filter...
      assert(m.cm.vert.size()==m.originalVertexColor.size());

      std::vector<vcg::Color4b>::iterator ci;
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin(),ci=m.originalVertexColor.begin();vi!=m.cm.vert.end();++vi,++ci) //scan all the vertex...
      {
        if(!(*vi).IsD()) //if it has not been deleted...
        {
          if(m.cm.sfn!=0) //mesh has a selected region, work just on it
          {
            if((*vi).IsS()) //if this vertex has been selected, do transormation
            {
              (*vi).C() = color_apply_diff((*ci), c, intensity);
              ++counter;
            }
          }
          else //mesh has not a selected region, transorm all vertex
          {
            (*vi).C() = color_apply_diff((*ci), c,intensity);
            ++counter;
          }
        }
      }
      return counter;
    }
    //Computes the luminance value for a specified color.
    static float compute_lightness(vcg::Color4b c)
    {
      float min_rgb = vcg::math::Min((float)c[0],(float)c[1]);
      min_rgb = vcg::math::Min(min_rgb,(float)c[2]);
      float max_rgb = vcg::math::Max((float)c[0],(float)c[1]);
      max_rgb = vcg::math::Max(max_rgb,(float)c[2]);
      return (max_rgb + min_rgb)/2;
    }

    //Adds an integer amount to rgb components of the color.
    static vcg::Color4b color_add(vcg::Color4b c, int amount)
    {
      return vcg::Color4b( vcg::math::Clamp<int>(c[0]+amount,0,255),
                           vcg::math::Clamp<int>(c[1]+amount,0,255),
                           vcg::math::Clamp<int>(c[2]+amount,0,255), 1 );
    }

    //Subtracts a middle value, multiplies the rgb components of the color for a factor,
    //and adds the middle value back.This is used for Contrast operation.
    static vcg::Color4b color_mul(vcg::Color4b c, float factor)
    {
      return vcg::Color4b( (int)vcg::math::Clamp<float>((c[0] - 128)*factor + 128,0.0f,255.0f),
                           (int)vcg::math::Clamp<float>((c[1] - 128)*factor + 128,0.0f,255.0f),
                           (int)vcg::math::Clamp<float>((c[2] - 128)*factor + 128,0.0f,255.0f), 1 );
    }

    //This is a composition of color_mul() and color_add(), for Contrast&Brightness filter.
    //The result is clamped just one time after all computations; this get a more accurate
    //result respect to composition of color_mul() and color_add().
    static vcg::Color4b color_mul_add(vcg::Color4b c, float factor, int amount)
    {
      return vcg::Color4b( (int)vcg::math::Clamp<float>((c[0] - 128)*factor + 128 + amount,0.0f,255.0f),
                           (int)vcg::math::Clamp<float>((c[1] - 128)*factor + 128 + amount,0.0f,255.0f),
                           (int)vcg::math::Clamp<float>((c[2] - 128)*factor + 128 + amount,0.0f,255.0f), 1 );
    }

    //computes the gamma transformation on a given color, according to new_val = old_val^gamma
    static vcg::Color4b color_gamma(vcg::Color4b c, float exponent)
    {
      return vcg::Color4b( (int)vcg::math::Clamp<float>(pow((double)c[0], exponent),0.0f,255.0f),
                           (int)vcg::math::Clamp<float>(pow((double)c[1], exponent),0.0f,255.0f),
                           (int)vcg::math::Clamp<float>(pow((double)c[2], exponent),0.0f,255.0f), 1 );
    }

    //invert the given color
    static vcg::Color4b color_invert(vcg::Color4b c)
    {
      return vcg::Color4b( 255 - c[0], 255 - c[1], 255 - c[2], 1);
    }

    static vcg::Color4b color_levels(vcg::Color4b c, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      return vcg::Color4b( levels_value(c[0], gamma, in_min, in_max, out_min, out_max),
                      levels_value(c[1], gamma, in_min, in_max, out_min, out_max),
                      levels_value(c[2], gamma, in_min, in_max, out_min, out_max), 1);
    }

    static int levels_value(int value, float gamma, int in_min, int in_max, int out_min, int out_max)
    {
      // normalize
      value = (value-in_min) / (in_max - in_min);
      // transform gamma
      value = (int)(pow(value,gamma));
      // rescale range and clamp
      return vcg::math::Clamp<int>(value * (out_max - out_min) + out_min, 0, 255);
    }

    static vcg::Color4b color_apply_diff(vcg::Color4b old_color, vcg::Color4b new_color, float intensity)
    {
      return vcg::Color4b( value_apply_diff(old_color[0],new_color[0],intensity), value_apply_diff(old_color[1],new_color[1],intensity), value_apply_diff(old_color[2], new_color[2],intensity), 1);
    }

    static int value_apply_diff(int old_value, int new_value, float intensity)
    {
      return  vcg::math::Clamp<int>((int)(old_value + intensity * (new_value - old_value)), 0, 255);
    }
};
#endif
