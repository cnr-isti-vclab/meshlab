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
    //case CP_BRIGHTNESS : return "Vertex Color Brightness";
    //case CP_CONTRAST : return "Vertex Color Contrast";
    case CP_CONTR_BRIGHT : return "Vertex Color Brightness Contrast Gamma";
    //case CP_GAMMA : return "Vertex Color Gamma Correction";
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
    case CP_FILLING : return "Fills the color of the vertices of the mesh  with a color choosed by the user.";
    case CP_THRESHOLDING : return "Colors the vertices of the mesh using two colors according to a lightness threshold (on the original color).";
    //case CP_BRIGHTNESS : return "Change the color the vertices of the mesh adjusting the overall brightness.";
    //case CP_CONTRAST : return "Change the color the vertices of the mesh adjusting the contrast.";
    case CP_CONTR_BRIGHT : return "Change the color the vertices of the mesh adjusting brightness, contrast and gamma.";
    //case CP_GAMMA : return "Provides standard gamma correction for adjusting the color the vertices of the mesh.";
    case CP_INVERT : return "Inverts the colors of the vertices of the mesh.";
    case CP_LEVELS : return "The filter allows adjustment of color levels. It is a custom way to map an interval of color into another one. The user can set the input minimum and maximum levels, gamma and the output minimum and maximum levels (many tools call them respectively input black point, white point, gray point, output black point and white point).";
    case CP_COLOURISATION : return "Allows the application of a color to the mesh. In spite of the Fill operation, the color is blended with the mesh according to a given intensity. .";
    case CP_DESATURATION : return "The filter desaturates the colors of the mesh. This provides a simple way to convert a mesh in gray tones. The user can choose the desaturation method to apply; they are based on Lightness, Luminosity and Average.";
    case CP_EQUALIZE : return "The filter equalizes the colors histogram. It is a kind of automatic regulation of contrast; the colors histogram is expanded to fit all the range of colors.";
    case CP_WHITE_BAL : return "The filter provides a standard white balance transformation. It is done correcting the RGB channels with a factor such that, the brighter color in the mesh, that is supposed to be white, becomes really white.";
    case CP_PERLIN_COLOR : return "Paints the mesh using PerlinColor function. The color assigned to vertices depends on their position in the space; it means that near vertices will be painted with similar colors.";
    case CP_COLOR_NOISE : return "Adds to the color the requested amount of bits of noise. Bits of noise are added independently for each RGB channel.";
    case CP_SCATTER_PER_MESH : return "Assigns a random color to each visible mesh layer in the document. Colors change every time the filter is executed, but are always chosen so that they differ as much as possible.";
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
			QColor color1 = QColor(0, 0, 0, 255);
			par.addParam(new RichColor("color1", color1, "Color:", "Sets the color to apply to vertices."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_THRESHOLDING:
		{
			float threshold = 128.0f;
			QColor color1 = QColor(0, 0, 0, 255), color2 = QColor(255, 255, 255, 255);;
			par.addParam(new RichColor("color1", color1, "Color 1:", "Sets the color to apply below the threshold."));
			par.addParam(new RichColor("color2", color2, "Color 2:", "Sets the color to apply above the threshold."));
			par.addParam(new RichDynamicFloat("threshold", threshold, 0.0f, 255.0f,"Threshold:", "Vertices with color above the lightness threshold becomes Color 2, the others Color 1."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		/*case CP_BRIGHTNESS:
		{
			float brightness = 0.0f;
			par.addParam(new RichDynamicFloat("brightness", brightness, -255.0f, 255.0f,"Brightness:","Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;"));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}*/
		/*case CP_CONTRAST:
		{
			float factor = 1.0f;
			par.addParam(new RichDynamicFloat("factor", factor, 0.2f, 5.0f, "Contrast factor:", "Sets the amount of contrast of the mesh."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}*/
		case CP_CONTR_BRIGHT:
		{
			float brightness = 0.0f;
			float contrast = 0.0f;
			float gamma = 1.0f;
			par.addParam(new RichDynamicFloat("brightness", brightness, -255.0f, 255.0f, "Brightness:", "Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;"));
			par.addParam(new RichDynamicFloat("contrast", contrast, -255.0f, 255.0f, "Contrast factor:", "Sets the amount of contrast of the mesh."));
			par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f, "Gamma:", "Sets the values of the exponent gamma."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		/*case CP_GAMMA :
		{
			float gamma = 1.0f;
			par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f, "Gamma:", "Sets the values of the exponent gamma."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}*/
		case CP_INVERT:
		{
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}			
		case CP_LEVELS:
		{
			float in_min = 0, in_max = 255, out_min = 0, out_max = 255, gamma = 1;
			par.addParam(new RichDynamicFloat("gamma", gamma, 0.1f, 5.0f,       "Gamma:", ""));
			par.addParam(new RichDynamicFloat("in_min", in_min, 0.0f, 255.0f,   "Min input level:", ""));
			par.addParam(new RichDynamicFloat("in_max", in_max, 0.0f, 255.0f,   "Max input level:", ""));
			par.addParam(new RichDynamicFloat("out_min", out_min, 0.0f, 255.0f, "Min output level:", ""));
			par.addParam(new RichDynamicFloat("out_max", out_max, 0.0f, 255.0f, "Max output level:", ""));
			par.addParam(new RichBool("rCh", true, "Red Channel:",   ""));
			par.addParam(new RichBool("gCh", true, "Green Channel:", ""));
			par.addParam(new RichBool("bCh", true, "Blue Channel:",  ""));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			par.addParam(new RichBool("apply_to_all", false, "All visible layers", "if true, apply to all visible layers"));
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
			par.addParam(new RichDynamicFloat("intensity", intensity*100, 0.0f, 100.0f, "Blending:", "Sets the blending factor used in adding the new color to the existing one."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_DESATURATION:
		{
			QStringList l; l << "Lightness" << "Luminosity" << "Average";
			par.addParam(new RichEnum("method", 0, l,"Desaturation method:", "Lightness is computed as (Max(r,g,b)+Min(r,g,b))/2<br>Luminosity is computed as 0.212*r + 0.715*g + 0.072*b<br>Average is computed as (r+g+b)/3"));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_EQUALIZE:
		{
			par.addParam(new RichBool("rCh", true, "Red Channel:",   "Select the red channel."));
			par.addParam(new RichBool("gCh", true, "Green Channel:", "Select the green channel."));
			par.addParam(new RichBool("bCh", true, "Blue Channel:",  "Select the blue channel.<br><br>If no channel is selected<br>filter works on Lightness."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_WHITE_BAL:
		{
			par.addParam(new RichColor("color", QColor(255,255,255),"Unbalanced white: ","The color that is supposed to be white."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_PERLIN_COLOR:
		{
			QColor color1 = QColor(0, 0, 0, 255), color2 = QColor(255, 255, 255, 255);
			par.addParam(new RichColor("color1", color1, "Color 1:", "Sets the first color to mix with Perlin Noise function."));
			par.addParam(new RichColor("color2", color2, "Color 2:", "Sets the second color to mix with Perlin Noise function."));
			par.addParam(new RichDynamicFloat("freq", 10.0f, 0.1f, 100.0f,"Frequency:","Frequency of the Perlin Noise function, expressed as multiples of mesh bbox (frequency 10 means a noise period of bbox diagonal / 10). High frequencies produces many small splashes of colours, while low frequencies produces few big splashes."));
			par.addParam(new RichPoint3f("offset", Point3f(0.0f, 0.0f, 0.0f), "Offset",	"This values is the XYZ frequency offset of the Noise function (offset 1 means 1 period shift)."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_COLOR_NOISE:
		{
			par.addParam(new RichInt("noiseBits", 1, "Noise bits:","Bits of noise added to each RGB channel. Example: 3 noise bits adds three random offsets in the [-4,+4] interval to each RGB channels."));
			par.addParam(new RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_SCATTER_PER_MESH:
		{
			par.addParam(new RichInt("seed", 0, "Seed","Random seed used to generate scattered colors. Zero means totally random (each time the filter is started it generates a different result)"));
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
		QColor temp = par.getColor("color1");
		Color4b new_col = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());

		bool selected = par.getBool("onSelected");

		vcg::tri::UpdateColor<CMeshO>::PerVertexConstant(m->cm, new_col, selected);  //calls the function that does the real job
		return true;
    }
    case CP_THRESHOLDING:
    {
      //reads threshold, and colors to pass as parameters to the filter...
      float threshold = math::Clamp<float>(par.getDynamicFloat("threshold"), 0.0f, 255.0f);
      QColor temp = par.getColor("color1");
      Color4b c1 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());
      temp = par.getColor("color2");
	  Color4b c2 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexThresholding(m->cm, threshold, c1, c2, selected); //calls the function that does the real job
      return true;
    }
   /* case CP_BRIGHTNESS:
    {
      float brightness = math::Clamp<float>(par.getDynamicFloat("brightness"), -255.0f, 255.0f);

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexBrightness(m->cm, brightness, selected);
      return true;
    }*/
    /*case CP_CONTRAST:
    {
      float factor = math::Clamp<float>(par.getDynamicFloat("factor"), 0.2f, 5.0f);

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexContrast(m->cm, factor, selected);
      return true;
    }*/
    case CP_CONTR_BRIGHT:
    {
      float brightness = par.getDynamicFloat("brightness");
      float contrast = par.getDynamicFloat("contrast");
      float gamma = math::Clamp(par.getDynamicFloat("gamma"), 0.1f, 5.0f);

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexGamma(m->cm, gamma, selected);
      vcg::tri::UpdateColor<CMeshO>::PerVertexBrightnessContrast(m->cm, brightness/256.0f,contrast/256.0f , selected);
      return true;
    }
   /* case CP_GAMMA :
    {
      float gamma = math::Clamp(par.getDynamicFloat("gamma"), 0.1f, 5.0f);

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexGamma(m->cm, gamma, selected);
      return true;
    }*/
    case CP_INVERT :
    {
	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexInvert(m->cm, selected);
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

	  bool all_levels = par.getBool("apply_to_all");

      //builds incrementally a bitmask that indicates on which channels the filter works...
      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;
      //if no channels are checked, we intend to work on all rgb channels, so...
      if(rgbMask == vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS) rgbMask = vcg::tri::UpdateColor<CMeshO>::ALL_CHANNELS;

	  bool selected = par.getBool("onSelected");

	  if (all_levels)
      {
        foreach(MeshModel *mm, md.meshList)
		  if (mm->isVisible())
		    vcg::tri::UpdateColor<CMeshO>::PerVertexLevels(mm->cm, gamma, in_min, in_max, out_min, out_max, rgbMask, selected);
      }
	  else
      {
        vcg::tri::UpdateColor<CMeshO>::PerVertexLevels(m->cm, gamma, in_min, in_max, out_min, out_max, rgbMask, selected);
      }

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

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexColourisation(m->cm, color, intensity, selected);
      return true;
    }
    case CP_DESATURATION:
    {
      int method = par.getEnum("method");

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexDesaturation(m->cm, method, selected);
      return true;
    }
    case CP_EQUALIZE:
    {
      //builds incrementally a bitmask that indicates on which channels the filter works...
      unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
      if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
      if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
      if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexEqualize(m->cm, rgbMask, selected);
      return true;
    }
    case CP_WHITE_BAL:
    {
      QColor tempColor = par.getColor("color");
      Color4b color = Color4b(tempColor.red(),tempColor.green(),tempColor.blue(), 255);

	  bool selected = par.getBool("onSelected");

      vcg::tri::UpdateColor<CMeshO>::PerVertexWhiteBalance(m->cm, color, selected);
      return true;
    }
    case CP_SCATTER_PER_MESH:
    {
      int seed=par.getInt("seed");
      if(seed==0) seed = time(NULL);
      math::MarsenneTwisterRNG myrnd(seed);
      int numOfMeshes = md.meshList.size();
	  int id = myrnd.generate(numOfMeshes);
      foreach(MeshModel *mm, md.meshList)
      {
		if (mm->isVisible())
          mm->cm.C()=Color4b::Scatter(numOfMeshes,id);
        id=(id+1)%numOfMeshes;
      }
      return true;
    }
    case CP_PERLIN_COLOR:
    {
        //read parameters
		QColor temp = par.getColor("color1");
		Color4b c1 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());
		temp = par.getColor("color2");
		Color4b c2 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());

        float freq = par.getDynamicFloat("freq");//default frequency; grant to be the same for all mesh in the document
        float period = md.bbox().Diag() / freq;

		Point3m offset = par.getPoint3m("offset");
		bool selected = par.getBool("onSelected");

		tri::UpdateColor<CMeshO>::PerVertexPerlinColoring(m->cm, period, offset, c1, c2, selected);
        return true;
    }
    case CP_COLOR_NOISE:
    {
        //read parameters
        int noiseBits = par.getInt("noiseBits");
		
		bool selected = par.getBool("onSelected");

		tri::UpdateColor<CMeshO>::PerVertexAddNoise(m->cm, noiseBits, selected);
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
    //case CP_BRIGHTNESS :
    //case CP_CONTRAST :
    case CP_CONTR_BRIGHT :
    //case CP_GAMMA :
    case CP_INVERT :
    case CP_COLOURISATION :
    case CP_EQUALIZE :
    case CP_DESATURATION :
    case CP_WHITE_BAL :
    case CP_LEVELS :
    case CP_PERLIN_COLOR :
    case CP_COLOR_NOISE : return MeshFilterInterface::VertexColoring;
    case CP_SCATTER_PER_MESH : return MeshFilterInterface::MeshColoring;

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
        //case CP_BRIGHTNESS :
        //case CP_CONTRAST :
        case CP_CONTR_BRIGHT :
        //case CP_GAMMA :
        case CP_INVERT :
        case CP_EQUALIZE :
        case CP_DESATURATION :
        case CP_WHITE_BAL :
        case CP_LEVELS :
      return MeshModel::MM_VERTCOLOR;

    default: assert(0); return MeshModel::MM_NONE;
    }
}

MeshFilterInterface::FILTER_ARITY FilterColorProc::filterArity( QAction *act ) const
{
    switch(ID(act))
    {
    case CP_FILLING :
    case CP_COLOURISATION :
    case CP_PERLIN_COLOR :
    case CP_COLOR_NOISE :
    case CP_THRESHOLDING :
    //case CP_BRIGHTNESS :
    //case CP_CONTRAST :
    case CP_CONTR_BRIGHT :
    //case CP_GAMMA :
    case CP_INVERT :
    case CP_EQUALIZE :
    case CP_DESATURATION :
    case CP_WHITE_BAL :
    case CP_LEVELS :
        return MeshFilterInterface::SINGLE_MESH;
    case CP_SCATTER_PER_MESH:
        return MeshFilterInterface::VARIABLE;
    }
    return MeshFilterInterface::NONE;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterColorProc)
