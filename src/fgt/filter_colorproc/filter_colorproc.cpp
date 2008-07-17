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
           << CP_COLOURISATION
           << CP_DESATURATION
           << CP_EQUALIZE;

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
    case CP_DESATURATION : return "Desaturation";
    case CP_EQUALIZE : return "Equalize";
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
    case CP_LEVELS : return "Adjusts colors levels.";
    case CP_COLOURISATION : return "Colors the mesh.";
    case CP_DESATURATION : return "Desaturates colors according to the selected method.";
    case CP_EQUALIZE : return "Equalize colors values.";
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
			par.addDynamicFloat("in_min", in_min, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Min input level:", "");
			par.addDynamicFloat("gamma", gamma, 0.1f, 5.0f, MeshModel::MM_VERTCOLOR, "Gamma:", "");
			par.addDynamicFloat("in_max", in_max, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Max input level:", "");
			par.addDynamicFloat("out_min", out_min, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Min output level:", "");
			par.addDynamicFloat("out_max", out_max, 0.0f, 255.0f, MeshModel::MM_VERTCOLOR, "Max output level:", "");
			par.addBool("rCh", true, "Red Channel:", "");
			par.addBool("gCh", true, "Green Channel:", "");
			par.addBool("bCh", true, "Blue Channel:", "");
			break;
		}
		case CP_COLOURISATION:
		{
		  float intensity = 0.5f;
      double hue, luminance, saturation;
			ColorSpace<unsigned char>::RGBtoHSL(1.0, 0.0, 0.0, hue, saturation, luminance);
			par.addDynamicFloat("hue", (float)hue*360, 0.0f, 360.0f, MeshModel::MM_VERTCOLOR, "Hue:", "Changes the hue of the mesh.");
			par.addDynamicFloat("saturation", (float)saturation*100, 0.0f, 100.0f, MeshModel::MM_VERTCOLOR, "Saturation:", "Changes the saturation of the mesh.");
			par.addDynamicFloat("luminance", (float)luminance*100, 0.0f, 100.0f, MeshModel::MM_VERTCOLOR, "Luminance:", "Changes the luminance of the mesh.");
			par.addDynamicFloat("intensity", intensity*100, 0.0f, 100.0f, MeshModel::MM_VERTCOLOR, "Intensity:", "Sets the intensity with which the color it's blended to the mesh.");
			break;
    }
    case CP_DESATURATION:
    {
      QStringList l; l << "Lightness" << "Luminance" << "Mean";
      par.addEnum("method", 0, l,"Desaturation method:", "Lightness value is computed as (Max(r,g,b)+Min(r,g,b))/2<br>Luminance value is computed as 0.212*r+0.715*g+0.072*b<br>Mean value is computed as (r+g+b)/3");
      break;
    }
    case CP_EQUALIZE:
    {
      par.addBool("rCh", true, "Red Channel:", "Select the red channel.");
			par.addBool("gCh", true, "Green Channel:", "Select the green channel.");
			par.addBool("bCh", true, "Blue Channel:", "Select the blue channel.<br><br>If no channels are selected<br>filter works on Lightness.");
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
      int r = math::Clamp((int)par.getDynamicFloat("r"), 0, 255);
      int g = math::Clamp((int)par.getDynamicFloat("g"), 0, 255);
      int b = math::Clamp((int)par.getDynamicFloat("b"), 0, 255);
      Color4b new_col = Color4b(r,g,b,255);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Filling(m.cm, new_col, selected);
      return true;
    }
    case CP_TRESHOLDING:
    {
      float treshold = math::Clamp<float>(par.getDynamicFloat("treshold"), 0.0f, 255.0f);
      QColor temp = par.getColor("color1");
      Color4b c1 = Color4b(temp.red(), temp.green(),temp.blue(), 255);
      temp = par.getColor("color2");
      Color4b c2 = Color4b(temp.red(), temp.green(),temp.blue(), 255);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Tresholding(m.cm, treshold, c1, c2, selected);
      return true;
    }
    case CP_BRIGHTNESS:
    {
      float brightness = math::Clamp<float>(par.getDynamicFloat("brightness"), -255.0f, 255.0f);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

			vcg::tri::UpdateColor<CMeshO>::Brighting(m.cm, brightness, selected);
	    return true;
    }
    case CP_CONTRAST:
    {
      float factor = math::Clamp<float>(par.getDynamicFloat("factor"), 0.2f, 5.0f);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Contrast(m.cm, factor, selected);
      return true;
    }
    case CP_CONTR_BRIGHT:
    {
      float brightness = math::Clamp<float>(par.getDynamicFloat("brightness"), -255.0f, 255.0f);
      float factor = math::Clamp<float>(par.getDynamicFloat("factor"), 0.2f, 5.0f);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::ContrastBrightness(m.cm, factor, brightness, selected);
      return true;
    }
    case CP_GAMMA :
    {
      float gamma = math::Clamp(par.getDynamicFloat("gamma"), 0.1f, 5.0f);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Gamma(m.cm, gamma, selected);
      return true;
    }
    case CP_INVERT :
    {
      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Invert(m.cm, selected);
      return true;
    }
    case CP_LEVELS:
    {
			float gamma = par.getDynamicFloat("gamma");
      float  in_min = par.getDynamicFloat("in_min")/255;
      float  in_max = par.getDynamicFloat("in_max")/255;
      float  out_min = par.getDynamicFloat("out_min")/255;
      float  out_max = par.getDynamicFloat("out_max")/255;

      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;
      if(rgbMask == vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS) rgbMask = vcg::tri::UpdateColor<CMeshO>::ALL_CHANNELS;

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Levels(m.cm, gamma, in_min, in_max, out_min, out_max, rgbMask, selected);
      return true;
		}
		case CP_COLOURISATION:
		{
			float luminance = math::Clamp(par.getDynamicFloat("luminance")/100, 0.0f, 1.0f);
			float saturation = math::Clamp(par.getDynamicFloat("saturation")/100, 0.0f, 1.0f);
      float hue = math::Clamp(par.getDynamicFloat("hue")/360, 0.0f, 1.0f);
      float intensity = math::Clamp(par.getDynamicFloat("intensity")/100, 0.0f, 1.0f);

      double r, g, b;
      ColorSpace<unsigned char>::HSLtoRGB( (double)hue, (double)saturation, (double)luminance, r, g, b);
      Color4b color = Color4b((int)(r*255), (int)(g*255), (int)(b*255), 255);

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Colourisation(m.cm, color, intensity, selected);
      return true;
    }
    case CP_DESATURATION:
    {
      int method = par.getEnum("method");
      bool selected = false;
      if(m.cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Desaturation(m.cm, method, selected);
      return true;
    }
    case CP_EQUALIZE:
    {
      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;

      bool selected = false;
      if(m.cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Equalize(m.cm, rgbMask, selected);
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
    case CP_EQUALIZE :
    case CP_DESATURATION :
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
