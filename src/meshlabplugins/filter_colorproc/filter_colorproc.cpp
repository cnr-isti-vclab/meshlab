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

#include <vcg/space/colorspace.h>
#include "filter_colorproc.h"

#include <vcg/math/random_generator.h>

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterColorProc::FilterColorProc()
{
  typeList << CP_FILLING
           << CP_INVERT
           << CP_THRESHOLDING
//           << CP_BRIGHTNESS
//           << CP_CONTRAST
           << CP_CONTR_BRIGHT
//           << CP_GAMMA
           << CP_LEVELS
           << CP_COLOURISATION
           << CP_DESATURATION
           << CP_EQUALIZE
           << CP_WHITE_BAL
           << CP_PERLIN_COLOR
           << CP_COLOR_NOISE
           << CP_SCATTER_PER_MESH;

  FilterIDType tt;
  foreach(tt , types())
    actionList << new QAction(filterName(tt), this);

}

FilterColorProc::~FilterColorProc()
{
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

 QString FilterColorProc::filterName(FilterIDType filter) const
{
  switch(filter)
  {
    case CP_FILLING : return "Vertex Color Filling";
    case CP_THRESHOLDING : return "Vertex Color Thresholding";
    case CP_BRIGHTNESS : return "Vertex Color Brightness";
    case CP_CONTRAST : return "Vertex Color Contrast";
    case CP_CONTR_BRIGHT : return "Vertex Color Brightness Contrast Gamma";
    case CP_GAMMA : return "Vertex Color Gamma Correction";
    case CP_INVERT : return "Vertex Color Invert";
    case CP_LEVELS : return "Vertex Color Levels Adjustement";
    case CP_COLOURISATION : return "Vertex Color Colourisation";
    case CP_DESATURATION : return "Vertex Color Desaturation";
    case CP_EQUALIZE : return "Equalize Vertex Color ";
    case CP_WHITE_BAL : return "Vertex Color White Balance";
    case CP_PERLIN_COLOR : return "Perlin color";
    case CP_COLOR_NOISE : return "Color noise";
    case CP_SCATTER_PER_MESH : return "PerMesh Color Scattering";
    default: assert(0);
  }
  return QString("error!");
}

 QString FilterColorProc::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
    case CP_FILLING : return "Fills the color of the vertexes of the mesh  with a color choosed by the user.";
    case CP_THRESHOLDING : return "Reduces the color the vertexes of the mesh to two colors according to a threshold.";
    case CP_BRIGHTNESS : return "Change the color the vertexes of the mesh adjusting the overall brightness.";
    case CP_CONTRAST : return "Change the color the vertexes of the mesh adjusting the contrast.";
    case CP_CONTR_BRIGHT : return "Change the color the vertexes of the mesh adjusting brightness, contrast and gamma.";
    case CP_GAMMA : return "Provides standard gamma correction for adjusting the color the vertexes of the mesh.";
    case CP_INVERT : return "Inverts the colors of the vertexes of the mesh.";
    case CP_LEVELS : return "The filter allows adjustment of color levels. It is a custom way to map an interval of color into another one. The user can set the input minimum and maximum levels, gamma and the output minimum and maximum levels (many tools call them respectively input black point, white point, gray point, output black point and white point).";
    case CP_COLOURISATION : return "Allows the application of a color to the mesh. In spite of the Fill operation, the color is blended with the mesh according to a given intensity. .";
    case CP_DESATURATION : return "The filter desaturates the colors of the mesh. This provides a simple way to convert a mesh in gray tones. The user can choose the desaturation method to apply; they are based on Lightness, Luminosity and Average.";
    case CP_EQUALIZE : return "The filter equalizes the colors histogram. It is a kind of automatic regulation of contrast; the colors histogram is expanded to fit all the range of colors.";
    case CP_WHITE_BAL : return "The filter provides a standard white balance transformation. It is done correcting the RGB channels with a factor such that, the brighter color in the mesh, that is supposed to be white, becomes really white.";    
    case CP_PERLIN_COLOR : return "Paints the mesh using PerlinColor function. The color assigned to verteces depends on their position in the space; it means that near verteces will be painted with similar colors.";
    case CP_COLOR_NOISE : return "Adds to the color the requested amount of bits of noise. Bits of noise are added independently for each RGB channel.";
    case CP_SCATTER_PER_MESH : return "Assigns a random color to each mesh in the document. Colors change every time the filter is executed, but are always chosen so that they differs as much as possible.";
    default: assert(0);
  }
  return QString("error!");
}

 int FilterColorProc::getRequirements(QAction *action)
{
    switch(ID(action))
    {
        case CP_SCATTER_PER_MESH : return MeshModel::MM_COLOR;
        default : return MeshModel::MM_VERTCOLOR;
    }
    assert(0);
}

void FilterColorProc::initParameterSet(QAction *a, MeshDocument& /*md*/, RichParameterSet & par)
{
    switch(ID(a))
  {
    case CP_FILLING:
    {
			float r = 255.0f, g = 255.0f, b = 255.0f;
			par.addParam(new RichDynamicFloat("r", r, 0.0f, 255.0f,"Red:", "Sets the red component of the color."));
			par.addParam(new RichDynamicFloat("g", g, 0, 255,"Green:", "Sets the green component of the color."));
			par.addParam(new RichDynamicFloat("b", b, 0, 255,"Blue:", "Sets the blue component of the color."));
			break;
		}
    case CP_THRESHOLDING:
    {
      float threshold = 128.0f;
      QColor color1 = QColor(0,0,0), color2 = QColor(255,255,255);
      par.addParam(new RichColor("color1", color1, "Color 1:", "Sets the color to apply below the threshold."));
			par.addParam(new RichColor("color2", color2, "Color 2:", "Sets the color to apply above the threshold."));
      par.addParam(new RichDynamicFloat("threshold", threshold, 0.0f, 255.0f,"Threshold:", "Colors above the threshold becomes Color 2, others Color 1."));
      break;
    }
    case CP_BRIGHTNESS:
    {
      float brightness = 0.0f;
			par.addParam(new RichDynamicFloat("brightness", brightness, -255.0f, 255.0f,"Brightness:","Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;"));
      break;
    }
    case CP_CONTRAST:
    {
      float factor = 1.0f;
      par.addParam(new RichDynamicFloat("factor", factor, 0.2f, 5.0f, "Contrast factor:", "Sets the amount of contrast of the mesh."));
      break;
    }
    case CP_CONTR_BRIGHT:
    {
      float brightness = 0.0f;
      float contrast = 0.0f;
      float gamma = 1.0f;
      par.addParam(new RichDynamicFloat("brightness", brightness, -255.0f, 255.0f, "Brightness:", "Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;"));
      par.addParam(new RichDynamicFloat("contrast", contrast, -255.0f, 255.0f, "Contrast factor:", "Sets the amount of contrast of the mesh."));
      par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f, "Gamma:", "Sets the values of the exponent gamma."));
      break;
    }
    case CP_GAMMA :
    {
      float gamma = 1.0f;
      par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f, "Gamma:", "Sets the values of the exponent gamma."));
      break;
    }
    case CP_LEVELS:
    {
			float in_min = 0, in_max = 255, out_min = 0, out_max = 255, gamma = 1;
			par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f,  "Gamma:", ""));
			par.addParam(new RichDynamicFloat("in_min", in_min, 0.0f, 255.0f,  "Min input level:", ""));
			par.addParam(new RichDynamicFloat("in_max", in_max, 0.0f, 255.0f,  "Max input level:", ""));
			par.addParam(new RichDynamicFloat("out_min", out_min, 0.0f, 255.0f, "Min output level:", ""));
			par.addParam(new RichDynamicFloat("out_max", out_max, 0.0f, 255.0f,"Max output level:", ""));
			par.addParam(new RichBool("rCh", true, "Red Channel:", ""));
			par.addParam(new RichBool("gCh", true, "Green Channel:", ""));
			par.addParam(new RichBool("bCh", true, "Blue Channel:", ""));
			break;
		}
		case CP_COLOURISATION:
		{
		  float intensity = 0.5f;
      double hue, luminance, saturation;
			ColorSpace<unsigned char>::RGBtoHSL(1.0, 0.0, 0.0, hue, saturation, luminance);
			par.addParam(new RichDynamicFloat("hue", (float)hue*360, 0.0f, 360.0f, "Hue:", "Changes the hue of the mesh."));
			par.addParam(new RichDynamicFloat("saturation", (float)saturation*100, 0.0f, 100.0f, "Saturation:", "Changes the saturation of the mesh."));
			par.addParam(new RichDynamicFloat("luminance", (float)luminance*100, 0.0f, 100.0f,"Luminance:", "Changes the luminance of the mesh."));
			par.addParam(new RichDynamicFloat("intensity", intensity*100, 0.0f, 100.0f, "Intensity:", "Sets the intensity with which the color it's blended to the mesh."));
			break;
    }
    case CP_DESATURATION:
    {
      QStringList l; l << "Lightness" << "Luminosity" << "Average";
      par.addParam(new RichEnum("method", 0, l,"Desaturation method:", "Lightness is computed as (Max(r,g,b)+Min(r,g,b))/2<br>Luminosity is computed as 0.212*r + 0.715*g + 0.072*b<br>Average is computed as (r+g+b)/3"));
      break;
    }
    case CP_EQUALIZE:
    {
      par.addParam(new RichBool("rCh", true, "Red Channel:", "Select the red channel."));
			par.addParam(new RichBool("gCh", true, "Green Channel:", "Select the green channel."));
			par.addParam(new RichBool("bCh", true, "Blue Channel:", "Select the blue channel.<br><br>If no channels are selected<br>filter works on Lightness."));
			break;
    }
    case CP_WHITE_BAL:
    {
      par.addParam(new RichBool("auto",true,"Automatic white balance","If checked, an automatic balancing is done, otherwise an unbalanced white color must be chosen"));
      par.addParam(new RichColor("color", QColor(255,255,255),"Unbalanced white: ","The color that is supposed to be white."));
      break;
    }
    case CP_PERLIN_COLOR:
    {
        par.addParam(new RichDynamicFloat("freq", 10.0f, 0.1f, 50.0f,"Frequency:","Frequency of the Perlin Noise function. High frequencies produces many small splashes of colours, while low frequencies produces few big splashes."));
        break;
    }
    case CP_COLOR_NOISE:
    {
        par.addParam(new RichInt("noiseBits", 1, "Noise bits:","Bits of noise added to each RGB channel. Example: 3 noise bits adds three random offsets in the [-4,+4] interval to each RGB channels."));
        break;
    }
    default: break; // do not add any parameter for the other filters
	}
}

bool FilterColorProc::applyFilter(QAction *filter, MeshDocument& md, RichParameterSet & par, vcg::CallBackPos * /* cb */)
{
  MeshModel* m = md.mm();  //get current mesh from document

  switch(ID(filter))
  {
    case CP_FILLING:
    {
      //reads the color components and build the color
      int r = math::Clamp((int)par.getDynamicFloat("r"), 0, 255);
      int g = math::Clamp((int)par.getDynamicFloat("g"), 0, 255);
      int b = math::Clamp((int)par.getDynamicFloat("b"), 0, 255);
      Color4b new_col = Color4b(r,g,b,255);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;  //if a region of he mesh is selected, filter works on that.

      vcg::tri::UpdateColor<CMeshO>::Filling(m->cm, new_col, selected);  //calls the function that does the real job
      return true;
    }
    case CP_THRESHOLDING:
    {
      //reads threshold, and colors to pass as parameters to the filter...
      float threshold = math::Clamp<float>(par.getDynamicFloat("threshold"), 0.0f, 255.0f);
      QColor temp = par.getColor("color1");
      Color4b c1 = Color4b(temp.red(), temp.green(),temp.blue(), 255);
      temp = par.getColor("color2");
      Color4b c2 = Color4b(temp.red(), temp.green(),temp.blue(), 255);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true; //if a region of he mesh is selected, filter works on that.

      vcg::tri::UpdateColor<CMeshO>::Thresholding(m->cm, threshold, c1, c2, selected); //calls the function that does the real job
      return true;
    }
    case CP_BRIGHTNESS:
    {
      float brightness = math::Clamp<float>(par.getDynamicFloat("brightness"), -255.0f, 255.0f);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

                        vcg::tri::UpdateColor<CMeshO>::Brighting(m->cm, brightness, selected);
	    return true;
    }
    case CP_CONTRAST:
    {
      float factor = math::Clamp<float>(par.getDynamicFloat("factor"), 0.2f, 5.0f);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Contrast(m->cm, factor, selected);
      return true;
    }
    case CP_CONTR_BRIGHT:
    {
      float brightness = par.getDynamicFloat("brightness");
      float contrast = par.getDynamicFloat("contrast");
      float gamma = math::Clamp(par.getDynamicFloat("gamma"), 0.1f, 5.0f);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Gamma(m->cm, gamma, selected);
      vcg::tri::UpdateColor<CMeshO>::BrightnessContrast(m->cm, brightness/256.0f,contrast/256.0f , selected);
      return true;
    }
    case CP_GAMMA :
    {
      float gamma = math::Clamp(par.getDynamicFloat("gamma"), 0.1f, 5.0f);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Gamma(m->cm, gamma, selected);
      return true;
    }
    case CP_INVERT :
    {
      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Invert(m->cm, selected);
      return true;
    }
    case CP_LEVELS:
    {
      //reads the parameters...
			float gamma = par.getDynamicFloat("gamma");
      float  in_min = par.getDynamicFloat("in_min")/255;
      float  in_max = par.getDynamicFloat("in_max")/255;
      float  out_min = par.getDynamicFloat("out_min")/255;
      float  out_max = par.getDynamicFloat("out_max")/255;

      //builds incrementally a bitmask that indicates on which channels the filter works...
      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;
      //if no channels are checked, we intend to work on all rgb channels, so...
      if(rgbMask == vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS) rgbMask = vcg::tri::UpdateColor<CMeshO>::ALL_CHANNELS;

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Levels(m->cm, gamma, in_min, in_max, out_min, out_max, rgbMask, selected);
      return true;
		}
		case CP_COLOURISATION:
		{
		  //reads parameters and normalizes their values in [0,1]
			float luminance = math::Clamp(par.getDynamicFloat("luminance")/100, 0.0f, 1.0f);
			float saturation = math::Clamp(par.getDynamicFloat("saturation")/100, 0.0f, 1.0f);
      float hue = math::Clamp(par.getDynamicFloat("hue")/360, 0.0f, 1.0f);
      float intensity = math::Clamp(par.getDynamicFloat("intensity")/100, 0.0f, 1.0f);

      double r, g, b;   //converts color from HSL to RGB....
      ColorSpace<unsigned char>::HSLtoRGB( (double)hue, (double)saturation, (double)luminance, r, g, b);
      Color4b color = Color4b((int)(r*255), (int)(g*255), (int)(b*255), 255);

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;

      vcg::tri::UpdateColor<CMeshO>::Colourisation(m->cm, color, intensity, selected);
      return true;
    }
    case CP_DESATURATION:
    {
      int method = par.getEnum("method");
      bool selected = false;
      if(m->cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Desaturation(m->cm, method, selected);
      return true;
    }
    case CP_EQUALIZE:
    {
      //builds incrementally a bitmask that indicates on which channels the filter works...
      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;

      bool selected = false;
      if(m->cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::Equalize(m->cm, rgbMask, selected);
      return true;
    }
    case CP_WHITE_BAL:
    {
      bool automatic =  par.getBool("auto");
      QColor tempColor = par.getColor("color");
      Color4b color = Color4b(tempColor.red(),tempColor.green(),tempColor.blue(), 255);
      bool selected = false;
      if(m->cm.sfn!=0) selected = true;
      vcg::tri::UpdateColor<CMeshO>::WhiteBalance(m->cm, automatic, color, selected);
      return true;
    }
    case CP_SCATTER_PER_MESH:
    {
      math::SubtractiveRingRNG myrnd(time(NULL));
      int numOfMeshes = md.meshList.size();
      int id = myrnd.generate(numOfMeshes);
      foreach(MeshModel *mm, md.meshList)
      {
        mm->cm.C()=Color4b::Scatter(numOfMeshes+1,id);
        id=(id+1)%numOfMeshes;
      }
      return true;
    }
    case CP_PERLIN_COLOR:
    {
        //read parameters
        float freq = par.getDynamicFloat("freq");//default frequency; grant to be the same for all mesh in the document
        tri::UpdateColor<CMeshO>::PerlinColor(m->cm, md.bbox(), freq, Point3i(0,64,128));
        return true;
    }
    case CP_COLOR_NOISE:
    {
        //read parameters
        int noiseBits = par.getInt("noiseBits");
        tri::UpdateColor<CMeshO>::ColorNoise(m->cm, noiseBits);
        return true;
    }
    default: assert(0);
  }
	return false;
}

 MeshFilterInterface::FilterClass FilterColorProc::getClass(QAction *a)
{
  switch(ID(a))
  {
    case CP_FILLING :
    case CP_THRESHOLDING :
    case CP_BRIGHTNESS :
    case CP_CONTRAST :
    case CP_CONTR_BRIGHT :
    case CP_GAMMA :
    case CP_INVERT :
    case CP_COLOURISATION :
    case CP_EQUALIZE :
    case CP_DESATURATION :
    case CP_WHITE_BAL :
    case CP_LEVELS :
    case CP_SCATTER_PER_MESH :
    case CP_PERLIN_COLOR :
    case CP_COLOR_NOISE : return MeshFilterInterface::VertexColoring;
    default: assert(0);
  }
}

int FilterColorProc::postCondition( QAction* filter ) const
{
    switch(ID(filter))
    {
        case CP_SCATTER_PER_MESH : return MeshModel::MM_COLOR;
        default : return MeshModel::MM_VERTCOLOR;
    }
}

int FilterColorProc::getPreConditions( QAction * filter ) const
{
	switch(ID(filter))
	{
		case CP_FILLING :
		case CP_COLOURISATION :
		case CP_SCATTER_PER_MESH :
		case CP_PERLIN_COLOR :
		case CP_COLOR_NOISE :
      return MeshModel::MM_NONE;

    case CP_THRESHOLDING :
		case CP_BRIGHTNESS :
		case CP_CONTRAST :
		case CP_CONTR_BRIGHT :
		case CP_GAMMA :
		case CP_INVERT :
		case CP_EQUALIZE :
		case CP_DESATURATION :
		case CP_WHITE_BAL :
		case CP_LEVELS :
      return MeshModel::MM_VERTCOLOR;

    default: assert(0); return MeshModel::MM_NONE;
	}
}

Q_EXPORT_PLUGIN(FilterColorProc)
