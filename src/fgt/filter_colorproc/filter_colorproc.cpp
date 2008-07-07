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
#include <vcg/space/colorspace.h>

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterColorProc::FilterColorProc()
{
  typeList << CP_FILLING
           << CP_INVERT
           << CP_TRESHOLDING
           << CP_BRIGHTNESS
           << CP_CONTRAST
           << CP_CONTR_BRIGHT
           << CP_GAMMA
           << CP_LEVELS
           << CP_COLOURISATION;

  FilterIDType tt;
  foreach(tt , types())
    actionList << new QAction(filterName(tt), this);

}

FilterColorProc::~FilterColorProc()
{
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

const QString FilterColorProc::filterName(FilterIDType filter)
{
  switch(filter)
  {
    case CP_FILLING : return "Filling";
    case CP_TRESHOLDING : return "Tresholding";
    case CP_BRIGHTNESS : return "Brightness";
    case CP_CONTRAST : return "Contrast";
    case CP_CONTR_BRIGHT : return "Contrast and Brightness";
    case CP_GAMMA : return "Gamma";
    case CP_INVERT : return "Invert";
    case CP_LEVELS : return "Levels";
    case CP_COLOURISATION : return "Colourisation";
    default: assert(0);
  }
  return QString("error!");
}

const QString FilterColorProc::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
    case CP_FILLING : return "Fill the whole mesh with a color choosed by the user.";
    case CP_TRESHOLDING : return "All the vertex of the mesh above a certain threshold level becomes white, others black. Treshold = 255 -> all black; Treshold = 0 -> all white;";
    case CP_BRIGHTNESS : return "Adds or subtracts luminance to the mesh. Brightness = 255 -> all white; Brightness = -255 -> all black;";
    case CP_CONTRAST : return "Contrast";
    case CP_CONTR_BRIGHT : return "Contrast and Brightness";
    case CP_GAMMA : return "Gamma";
    case CP_INVERT : return "Invert the colors of the mesh.";
    case CP_LEVELS : return "Levels";
    case CP_COLOURISATION : return "Colourisation";
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
  //first of all make a copy of color per vertex. This is done to undo
  //the filters changes, but also to allow that filters always work on
  //the original colors, avoiding sum of sequential transformation
  m.storeVertexColor();

	switch(ID(a))
  {
    case CP_FILLING:
    {
			int r = 255, g = 255, b = 255;
			par.addInt("r", r, "Red:", "Set the <b><i>Red</b></i> component of the color.");
			par.addInt("g", g, "Green:", "Set the <b><i>Green</b></i> component of the color.");
			par.addInt("b", b, "Blue:", "Set the <b><i>Blue</b></i> component of the color.");
			break;
		}
    case CP_TRESHOLDING:
    {
      float treshold = 128.0f;
      par.addFloat("treshold", treshold, "Treshold:", "Set the treshold between shadows (black) and lights (white).");
      break;
    }
    case CP_BRIGHTNESS:
    {
      int brightness = 0;
      par.addInt("brightness", brightness, "Brightness:", "Set the amount of brightness (or darkness) that will be added to the color.");
      break;
    }
    case CP_CONTRAST:
    {
      float factor = 1.0f;
      par.addFloat("factor", factor, "Contrast factor:", "Set the amount of contrast of the mesh.");
      break;
    }
    case CP_CONTR_BRIGHT:
    {
      int brightness = 0;
      float factor = 1.0f;
      par.addInt("brightness", brightness, "Brightness:", "Set the amount of brightness (or darkness) that will be added to the color.");
      par.addFloat("factor", factor, "Contrast factor:", "Set the amount of contrast of the mesh.");
      break;
    }
    case CP_GAMMA :
    {
      float gamma = 1.0f;
      par.addFloat("gamma", gamma, "Gamma:", "Provides standard Gamma correction, according to the relation: display_intensity = pixel_value^gamma.");
      break;
    }
    case CP_LEVELS:
    {
			int in_min = 0, in_max = 255, out_min = 0, out_max = 255;
			float gamma = 1;
			par.addInt("in_min", in_min, "Input min:", "");
			par.addInt("in_max", in_max, "Input max:", "");
			par.addInt("out_min", out_min, "Output min:", "");
			par.addInt("out_max", out_max, "Output max:", "");
			par.addFloat("gamma", gamma, "Gamma:", "");
			break;
		}
		case CP_COLOURISATION:
		{
		  float intensity = 1.0f;
      double hue, luminance, saturation;
			ColorSpace<unsigned char>::RGBtoHSL(1.0, 1.0, 1.0, hue, saturation, luminance);
			par.addFloat("hue", (float)hue, "Hue:", "Change the <b><i>Hue</b></i> of the mesh.");
			par.addFloat("saturation", (float)saturation, "Saturation:", "Change the <b><i>Saturation</b></i> of the mesh.");
			par.addFloat("luminance", (float)luminance, "Luminance:", "Change the <b><i>Luminance</b></i> of the mesh.");
			par.addFloat("intensity", intensity, "Intensity:", "");
			break;
    }
    default: assert(0);
	}
}

bool FilterColorProc::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb)
{
  switch(ID(filter))
  {
    case CP_FILLING:
    {
      int r = math::Clamp<int>(par.getInt("r"), 0, 255);
      int g = math::Clamp<int>(par.getInt("g"), 0, 255);
      int b = math::Clamp<int>(par.getInt("b"), 0, 255);
      Color4b new_col = Color4b(r,g,b,1);

      int  v_num = FilterColorProc::filling(m, new_col);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_TRESHOLDING:
    {
      float treshold = math::Clamp(par.getFloat("treshold"), 0.0f, 255.0f);

      int  v_num = FilterColorProc::tresholding(m, treshold);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_BRIGHTNESS:
    {
      int brightness = math::Clamp<int>(par.getInt("brightness"), -255, 255);

      int  v_num = FilterColorProc::brighting(m, brightness);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_CONTRAST:
    {
      float factor = math::Clamp<float>(par.getFloat("factor"), 1.0f/6.0f, 6.0f);

      int  v_num = FilterColorProc::contrast(m, factor);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_CONTR_BRIGHT:
    {
      int brightness = math::Clamp<int>(par.getInt("brightness"), -255, 255);
      float factor = math::Clamp<float>(par.getFloat("factor"), 1.0f/6.0f, 6.0f);

      int  v_num = FilterColorProc::contrastBrightness(m, factor, brightness);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_GAMMA :
    {
      float gamma = math::Clamp<float>(par.getFloat("gamma"), 0.5f, 1.5f);  //valori sensati???

      int  v_num = FilterColorProc::gamma(m, gamma);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_INVERT :
    {
      int  v_num = FilterColorProc::invert(m);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
    }
    case CP_LEVELS:
    {
			float gamma = math::Clamp<float>(par.getFloat("gamma"), 0.5f, 1.5f);  //valori sensati???
      int  in_min = par.getInt("in_min");
      int  in_max = par.getInt("in_max");
      int  out_min = par.getInt("out_min");
      int  out_max = par.getInt("out_max");

      int  v_num = FilterColorProc::levels(m, gamma, in_min, in_max, out_min, out_max);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

      return true;
		}
		case CP_COLOURISATION:
		{
			float luminance = math::Clamp<float>(par.getFloat("luminance"), 0.0f, 1.0f);
			float saturation = math::Clamp<float>(par.getFloat("saturation"), 0.0f, 1.0f);
      float hue = math::Clamp<float>(par.getFloat("hue"), 0.0f, 1.0f);
      float intensity = math::Clamp<float>(par.getFloat("intensity"), 0.0f, 1.0f);

      double r, g, b;

      ColorSpace<unsigned char>::HSLtoRGB( (double)hue, (double)saturation, (double)luminance, r, g, b);
      Color4b color = Color4b((int)(r*255), (int)(g*255), (int)(b*255), 1);
      int  v_num = FilterColorProc::colourisation(m, color, intensity);

      if(v_num==0)
      {
        errorMessage = "The mesh doesn't contains any vertex.";
        return false;
      }

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
    case CP_FILLING :
    case CP_TRESHOLDING :
    case CP_BRIGHTNESS :
    case CP_CONTRAST :
    case CP_CONTR_BRIGHT :
    case CP_GAMMA :
    case CP_INVERT :
    case CP_COLOURISATION :
    case CP_LEVELS : return MeshFilterInterface::VertexColoring;
    default: assert(0);
  }
}

bool FilterColorProc::autoDialog(QAction *a)
{
  switch(ID(a))
  {
    case CP_INVERT : return false;
    default : return true;
  }
  assert(0);
}

Q_EXPORT_PLUGIN(FilterColorProc)
