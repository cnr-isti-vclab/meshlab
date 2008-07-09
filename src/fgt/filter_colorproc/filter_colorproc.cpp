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
    case CP_FILLING : return "Fills the mesh with a color choosed by the user.";
    case CP_TRESHOLDING : return "Reduces the mesh to two colors according to a treshold.";
    case CP_BRIGHTNESS : return "Sets the brightness of the mesh.";
    case CP_CONTRAST : return "Sets the contrast of the mesh.";
    case CP_CONTR_BRIGHT : return "Sets brightness and contrast of the mesh.";
    case CP_GAMMA : return "Provides standard gamma correction.";
    case CP_INVERT : return "Inverts the colors of the mesh.";
    case CP_LEVELS : return "Sets colors levels.";
    case CP_COLOURISATION : return "Colors the mesh.";
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
  return MeshModel::MM_VERTCOLOR;
}

void FilterColorProc::initParameterSet(QAction *a, MeshModel &m, FilterParameterSet & par)
{
	switch(ID(a))
  {
    case CP_FILLING:
    {
			float r = 255.0f, g = 255.0f, b = 255.0f;
			par.addDynamicFloat("r", r, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR ,"Red:", "Sets the red component of the color.");
			par.addDynamicFloat("g", g, 0, 255, MeshModel::MM_VERTCOLOR ,"Green:", "Sets the green component of the color.");
			par.addDynamicFloat("b", b, 0, 255, MeshModel::MM_VERTCOLOR ,"Blue:", "Sets the blue component of the color.");
			break;
		}
    case CP_TRESHOLDING:
    {
      float treshold = 128.0f;
      QColor color1 = QColor(0,0,0), color2 = QColor(255,255,255);
      par.addColor("color1", color1, "Color 1:", "Sets the color to apply below the treshold.");
      par.addColor("color2", color2, "Color 2:", "Sets the color to apply above the treshold.");
      par.addDynamicFloat("treshold", treshold, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Treshold:", "Colors above the threshold becomes Color 2, others Color 1.");

      break;
    }
    case CP_BRIGHTNESS:
    {
      float brightness = 0.0f;
      par.addDynamicFloat("brightness", brightness, -255.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Brightness:", "Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;");
      break;
    }
    case CP_CONTRAST:
    {
      float factor = 1.0f;
      par.addDynamicFloat("factor", factor, 0.2f, 5.0f, MeshModel::MM_VERTCOLOR, "Contrast factor:", "Sets the amount of contrast of the mesh.");
      break;
    }
    case CP_CONTR_BRIGHT:
    {
      float brightness = 0.0f;
      float factor = 1.0f;
      par.addDynamicFloat("brightness", brightness, -255.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Brightness:", "Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;");
      par.addDynamicFloat("factor", factor, 0.2f, 5.0f, MeshModel::MM_VERTCOLOR, "Contrast factor:", "Sets the amount of contrast of the mesh.");
      break;
    }
    case CP_GAMMA :
    {
      float gamma = 1.0f;
      par.addDynamicFloat("gamma", gamma, 0.1f, 5.0f, MeshModel::MM_VERTCOLOR, "Gamma:", "Sets the values of the exponent gamma.");
      break;
    }
    case CP_LEVELS:
    {
			float in_min = 0, in_max = 255, out_min = 0, out_max = 255, gamma = 1;
			par.addDynamicFloat("in_min", in_min, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Black input level:", "");
			par.addDynamicFloat("in_max", in_max, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "White input level:", "");
			par.addDynamicFloat("out_min", out_min, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Black output level:", "");
			par.addDynamicFloat("out_max", out_max, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "White output level:", "");
			par.addDynamicFloat("gamma", gamma, 0.1f, 5.0f, MeshModel::MM_VERTCOLOR, "Gamma:", "");
			break;
		}
		case CP_COLOURISATION:
		{
		  float intensity = 0.5f;
      double hue, luminance, saturation;
			ColorSpace<unsigned char>::RGBtoHSL(1.0, 1.0, 1.0, hue, saturation, luminance);
			par.addDynamicFloat("hue", (float)hue, 0.0f, 1.0f, MeshModel::MM_VERTCOLOR, "Hue:", "Changes the hue of the mesh.");
			par.addDynamicFloat("saturation", (float)saturation, 0.0f, 1.0f, MeshModel::MM_VERTCOLOR, "Saturation:", "Changes the saturation of the mesh.");
			par.addDynamicFloat("luminance", (float)luminance, 0.0f, 1.0f, MeshModel::MM_VERTCOLOR, "Luminance:", "Changes the luminance of the mesh.");
			par.addDynamicFloat("intensity", intensity, 0.0f, 1.0f, MeshModel::MM_VERTCOLOR, "Intensity:", "Sets the intensity with which the color it's blended to the mesh.");
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
      int r = (int)par.getDynamicFloat("r");
      int g = (int)par.getDynamicFloat("g");
      int b = (int)par.getDynamicFloat("b");
      Color4b new_col = Color4b(r,g,b,1);

      vcg::tri::UpdateColor<CMeshO>::Filling(m.cm, new_col, false);
      return true;
    }
    case CP_TRESHOLDING:
    {
      float treshold = par.getDynamicFloat("treshold");
      QColor temp = par.getColor("color1");
      Color4b c1 = Color4b(temp.red(), temp.green(),temp.blue(), 1);
      temp = par.getColor("color2");
      Color4b c2 = Color4b(temp.red(), temp.green(),temp.blue(), 1);

      vcg::tri::UpdateColor<CMeshO>::Tresholding(m.cm, treshold, c1, c2, false);
      return true;
    }
    case CP_BRIGHTNESS:
    {
      int brightness = (int)par.getDynamicFloat("brightness");
			vcg::tri::UpdateColor<CMeshO>::Brighting(m.cm, brightness, false);
	    return true;
    }
    case CP_CONTRAST:
    {
      float factor = par.getDynamicFloat("factor");

      vcg::tri::UpdateColor<CMeshO>::Contrast(m.cm, factor, false);
      return true;
    }
    case CP_CONTR_BRIGHT:
    {
      int brightness = (int)par.getDynamicFloat("brightness");
      float factor = par.getDynamicFloat("factor");

      vcg::tri::UpdateColor<CMeshO>::ContrastBrightness(m.cm, factor, brightness, false);
      return true;
    }
    case CP_GAMMA :
    {
      float gamma = par.getDynamicFloat("gamma");

      FilterColorProc::gamma(m, gamma); //move to vcg and refactorize
      return true;
    }
    case CP_INVERT :
    {
      vcg::tri::UpdateColor<CMeshO>::Invert(m.cm, false);
      return true;
    }
    case CP_LEVELS:
    {
			float gamma = par.getDynamicFloat("gamma");
      int  in_min = (int)par.getDynamicFloat("in_min");
      int  in_max = (int)par.getDynamicFloat("in_max");
      int  out_min = (int)par.getDynamicFloat("out_min");
      int  out_max = (int)par.getDynamicFloat("out_max");

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
			float luminance = par.getDynamicFloat("luminance");
			float saturation = par.getDynamicFloat("saturation");
      float hue = par.getDynamicFloat("hue");
      float intensity = par.getDynamicFloat("intensity");

      double r, g, b;
      ColorSpace<unsigned char>::HSLtoRGB( (double)hue, (double)saturation, (double)luminance, r, g, b);
      Color4b color = Color4b((int)(r*255), (int)(g*255), (int)(b*255), 1);

      vcg::tri::UpdateColor<CMeshO>::Colourisation(m.cm, color, intensity, false);
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
