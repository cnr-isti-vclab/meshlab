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

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/parametrization/distortion.h>
#include <vcg/space/fitting3.h>
#include <vcg/math/random_generator.h>

#include <stdlib.h>
#include <time.h>

// ERROR CHECKING UTILITY
#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}

using namespace std;
using namespace vcg;

FilterColorProc::FilterColorProc()
{
  typeList << CP_FILLING
           << CP_INVERT
           << CP_THRESHOLDING
           << CP_CONTR_BRIGHT
           << CP_LEVELS
           << CP_COLOURISATION
           << CP_DESATURATION
           << CP_EQUALIZE
           << CP_WHITE_BAL
           << CP_PERLIN_COLOR
           << CP_COLOR_NOISE
           << CP_SCATTER_PER_MESH
		   << CP_CLAMP_QUALITY
		   << CP_SATURATE_QUALITY
		   << CP_MAP_VQUALITY_INTO_COLOR
		   << CP_MAP_FQUALITY_INTO_COLOR
		   << CP_DISCRETE_CURVATURE
		   << CP_TRIANGLE_QUALITY
		   << CP_VERTEX_SMOOTH
		   << CP_FACE_SMOOTH
		   << CP_FACE_TO_VERTEX
		   << CP_TEXTURE_TO_VERTEX
		   << CP_VERTEX_TO_FACE
		   << CP_MESH_TO_FACE
		   << CP_RANDOM_FACE
		   << CP_RANDOM_CONNECTED_COMPONENT ;

  FilterIDType tt;
  foreach(tt , types())
    actionList << new QAction(filterName(tt), this);

}

FilterColorProc::~FilterColorProc()
{
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

QString FilterColorProc::pluginName() const
{
    return "FilterColorProc";
}

 QString FilterColorProc::filterName(FilterIDType filter) const
{
  switch(filter)
  {
	case CP_FILLING:                   return QString("Vertex Color Filling");
	case CP_THRESHOLDING:              return QString("Vertex Color Thresholding");
	case CP_CONTR_BRIGHT:              return QString("Vertex Color Brightness Contrast Gamma");
	case CP_INVERT:                    return QString("Vertex Color Invert");
	case CP_LEVELS:                    return QString("Vertex Color Levels Adjustment");
	case CP_COLOURISATION:             return QString("Vertex Color Colourisation");
	case CP_DESATURATION:              return QString("Vertex Color Desaturation");
	case CP_EQUALIZE:                  return QString("Equalize Vertex Color");
	case CP_WHITE_BAL:                 return QString("Vertex Color White Balance");
	case CP_PERLIN_COLOR:              return QString("Perlin color");
	case CP_COLOR_NOISE:               return QString("Color noise");
	case CP_SCATTER_PER_MESH:          return QString("PerMesh Color Scattering");
	case CP_CLAMP_QUALITY:             return QString("Clamp Vertex Quality");
	case CP_SATURATE_QUALITY:          return QString("Saturate Vertex Quality");
	case CP_MAP_VQUALITY_INTO_COLOR:   return QString("Colorize by vertex Quality");
	case CP_MAP_FQUALITY_INTO_COLOR:   return QString("Colorize by face Quality");
	case CP_DISCRETE_CURVATURE:        return QString("Discrete Curvatures");
	case CP_TRIANGLE_QUALITY:          return QString("Per Face Quality according to Triangle shape and aspect ratio");
	case CP_VERTEX_SMOOTH:             return QString("Smooth: Laplacian Vertex Color");
	case CP_FACE_SMOOTH:               return QString("Smooth: Laplacian Face Color");
	case CP_VERTEX_TO_FACE:            return QString("Transfer Color: Vertex to Face");
	case CP_MESH_TO_FACE:              return QString("Transfer Color: Mesh to Face");
	case CP_FACE_TO_VERTEX:            return QString("Transfer Color: Face to Vertex");
	case CP_TEXTURE_TO_VERTEX:         return QString("Transfer Color: Texture to Vertex");
	case CP_RANDOM_FACE:               return QString("Random Face Color");
	case CP_RANDOM_CONNECTED_COMPONENT:return QString("Random Component Color");

    default: assert(0);
  }
  return QString("error!");
}

 QString FilterColorProc::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
	case CP_FILLING: return QString("Fills the color of the vertices of the mesh with a color chosen by the user.");
	case CP_THRESHOLDING: return QString("Colors the vertices of the mesh using two colors according to a lightness threshold (on the original color).");
	case CP_CONTR_BRIGHT: return QString("Change the color the vertices of the mesh adjusting brightness, contrast and gamma.");
	case CP_INVERT: return QString("Inverts the colors of the vertices of the mesh.");
	case CP_LEVELS: return QString("The filter allows adjustment of color levels. It is a custom way to map an interval of color into another one. The user can set the input minimum and maximum levels, gamma and the output minimum and maximum levels (many tools call them respectively input black point, white point, gray point, output black point and white point).");
	case CP_COLOURISATION: return QString("Allows the application of a color to the mesh. In spite of the Fill operation, the color is blended with the mesh according to a given intensity.");
	case CP_DESATURATION: return QString("The filter desaturates the colors of the mesh. This provides a simple way to convert a mesh in gray tones. The user can choose the desaturation method to apply; they are based on Lightness, Luminosity and Average.");
	case CP_EQUALIZE: return QString("The filter equalizes the colors histogram. It is a kind of automatic regulation of contrast; the colors histogram is expanded to fit all the range of colors.");
	case CP_WHITE_BAL: return QString("The filter provides a standard white balance transformation. It is done correcting the RGB channels with a factor such that, the brighter color in the mesh, that is supposed to be white, becomes really white.");
	case CP_PERLIN_COLOR: return QString("Paints the mesh using PerlinColor function. The color assigned to vertices depends on their position in the space; it means that near vertices will be painted with similar colors.");
	case CP_COLOR_NOISE: return QString("Adds to the color the requested amount of bits of noise. Bits of noise are added independently for each RGB channel.");
	case CP_SCATTER_PER_MESH: return QString("Assigns a random color to each visible mesh layer in the document. Colors change every time the filter is executed, but are always chosen so that they differ as much as possible.");
	case CP_CLAMP_QUALITY: return QString("Clamp vertex quality values to a given range according to specific values or to percentiles");
	case CP_SATURATE_QUALITY: return QString("Saturate vertex quality, so that for each vertex the gradient of the quality is lower than the given threshold value (in absolute value)\n"
		"The saturation is done in a conservative way (quality is always decreased and never increased)");
	case CP_MAP_VQUALITY_INTO_COLOR: return QString("Color vertices depending on their quality field (manually equalized).");
	case CP_MAP_FQUALITY_INTO_COLOR: return QString("Color faces depending on their quality field (manually equalized).");
	case CP_DISCRETE_CURVATURE: return QString("Colorize according to various discrete curvature computed as described in:<br>"
		"'<i>Discrete Differential-Geometry Operators for Triangulated 2-Manifolds</i>' <br>"
		"M. Meyer, M. Desbrun, P. Schroder, A. H. Barr");
	case CP_TRIANGLE_QUALITY: return QString("Compute a quality and colorize faces depending on triangle shape:<ol>"
		"<li>area/max side of triangle"
		"<li>ratio inradius/circumradius (radii of incircle and circumcircle)"
		"<li>Mean ratio of triangle = area/(a*a + b*b + c*c)"
		"<li>Area"
		"<li>Texture Angle Distortion. Difference between angle in 3D space and texture space"
		"<li>Texture Area Distortion. Difference between area in 3D space and texture space"
		"<li>Polygonal Planarity (max distance to support plane)"
		"<li>Polygonal Planarity (relative distance to support plane)</ol>");
	case CP_VERTEX_SMOOTH: return QString("Laplacian Smooth Vertex Color");
	case CP_FACE_SMOOTH: return QString("Laplacian Smooth Face Color");
	case CP_VERTEX_TO_FACE: return QString("Vertex to Face color transfer");
	case CP_MESH_TO_FACE: return QString("Mesh to Face color transfer");
	case CP_FACE_TO_VERTEX: return QString("Face to Vertex color transfer");
	case CP_TEXTURE_TO_VERTEX: return QString("Texture to Vertex color transfer");
	case CP_RANDOM_FACE: return QString("Colorize Faces randomly. If internal edges are present they are used. Useful for quads.");
	case CP_RANDOM_CONNECTED_COMPONENT:  return QString("Colorize each connected component randomly.");

    default: assert(0);
  }
  return QString("error!");
}

 int FilterColorProc::getRequirements(const QAction *action)
{
    switch(ID(action))
    {
        case CP_SCATTER_PER_MESH : return MeshModel::MM_COLOR;
        default : return MeshModel::MM_VERTCOLOR;
    }
    assert(0);
}

void FilterColorProc::initParameterList(const QAction *a, MeshDocument& md, RichParameterList & par)
{
	switch(ID(a))
	{
		case CP_FILLING:
		{
			QColor color1 = QColor(0, 0, 0, 255);
			par.addParam(RichColor("color1", color1, "Color:", "Sets the color to apply to vertices."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_THRESHOLDING:
		{
			float threshold = 128.0f;
			QColor color1 = QColor(0, 0, 0, 255), color2 = QColor(255, 255, 255, 255);;
			par.addParam(RichColor("color1", color1, "Color 1:", "Sets the color to apply below the threshold."));
			par.addParam(RichColor("color2", color2, "Color 2:", "Sets the color to apply above the threshold."));
			par.addParam(RichDynamicFloat("threshold", threshold, 0.0f, 255.0f,"Threshold:", "Vertices with color above the lightness threshold becomes Color 2, the others Color 1."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_CONTR_BRIGHT:
		{
			float brightness = 0.0f;
			float contrast = 0.0f;
			float gamma = 1.0f;
			par.addParam(RichDynamicFloat("brightness", brightness, -255.0f, 255.0f, "Brightness:", "Sets the amount of brightness that will be added/subtracted to the colors.<br>Brightness = 255  ->  all white;<br>Brightness = -255  ->  all black;"));
			par.addParam(RichDynamicFloat("contrast", contrast, -255.0f, 255.0f, "Contrast factor:", "Sets the amount of contrast of the mesh."));
			par.addParam(RichDynamicFloat("gamma", gamma, 0.1f, 5.0f, "Gamma:", "Sets the values of the exponent gamma."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_INVERT:
		{
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}			
		case CP_LEVELS:
		{
			float in_min = 0, in_max = 255, out_min = 0, out_max = 255, gamma = 1;
			par.addParam(RichDynamicFloat("gamma", gamma, 0.1f, 5.0f,       "Gamma:", ""));
			par.addParam(RichDynamicFloat("in_min", in_min, 0.0f, 255.0f,   "Min input level:", ""));
			par.addParam(RichDynamicFloat("in_max", in_max, 0.0f, 255.0f,   "Max input level:", ""));
			par.addParam(RichDynamicFloat("out_min", out_min, 0.0f, 255.0f, "Min output level:", ""));
			par.addParam(RichDynamicFloat("out_max", out_max, 0.0f, 255.0f, "Max output level:", ""));
			par.addParam(RichBool("rCh", true, "Red Channel:",   ""));
			par.addParam(RichBool("gCh", true, "Green Channel:", ""));
			par.addParam(RichBool("bCh", true, "Blue Channel:",  ""));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			par.addParam(RichBool("apply_to_all", false, "All visible layers", "if true, apply to all visible layers"));
			break;
		}
		case CP_COLOURISATION:
		{
			float intensity = 0.5f;
			double hue, luminance, saturation;
			ColorSpace<unsigned char>::RGBtoHSL(1.0, 0.0, 0.0, hue, saturation, luminance);
			par.addParam(RichDynamicFloat("hue", (float)hue*360, 0.0f, 360.0f, "Hue:", "Changes the hue of the mesh."));
			par.addParam(RichDynamicFloat("saturation", (float)saturation*100, 0.0f, 100.0f, "Saturation:", "Changes the saturation of the mesh."));
			par.addParam(RichDynamicFloat("luminance", (float)luminance*100, 0.0f, 100.0f,"Luminance:", "Changes the luminance of the mesh."));
			par.addParam(RichDynamicFloat("intensity", intensity*100, 0.0f, 100.0f, "Blending:", "Sets the blending factor used in adding the new color to the existing one."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_DESATURATION:
		{
			QStringList l; l << "Lightness" << "Luminosity" << "Average";
			par.addParam(RichEnum("method", 0, l,"Desaturation method:", "Lightness is computed as (Max(r,g,b)+Min(r,g,b))/2<br>Luminosity is computed as 0.212*r + 0.715*g + 0.072*b<br>Average is computed as (r+g+b)/3"));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_EQUALIZE:
		{
			par.addParam(RichBool("rCh", true, "Red Channel:",   "Select the red channel."));
			par.addParam(RichBool("gCh", true, "Green Channel:", "Select the green channel."));
			par.addParam(RichBool("bCh", true, "Blue Channel:",  "Select the blue channel.<br><br>If no channel is selected<br>filter works on Lightness."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_WHITE_BAL:
		{
			par.addParam(RichColor("color", QColor(255,255,255),"Unbalanced white: ","The color that is supposed to be white."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_PERLIN_COLOR:
		{
			QColor color1 = QColor(0, 0, 0, 255), color2 = QColor(255, 255, 255, 255);
			par.addParam(RichColor("color1", color1, "Color 1:", "Sets the first color to mix with Perlin Noise function."));
			par.addParam(RichColor("color2", color2, "Color 2:", "Sets the second color to mix with Perlin Noise function."));
			par.addParam(RichDynamicFloat("freq", 10.0f, 0.1f, 100.0f,"Frequency:","Frequency of the Perlin Noise function, expressed as multiples of mesh bbox (frequency 10 means a noise period of bbox diagonal / 10). High frequencies produces many small splashes of colours, while low frequencies produces few big splashes."));
			par.addParam(RichPoint3f("offset", Point3f(0.0f, 0.0f, 0.0f), "Offset",	"This values is the XYZ frequency offset of the Noise function (offset 1 means 1 period shift)."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_COLOR_NOISE:
		{
			par.addParam(RichInt("noiseBits", 1, "Noise bits:","Bits of noise added to each RGB channel. Example: 3 noise bits adds three random offsets in the [-4,+4] interval to each RGB channels."));
			par.addParam(RichBool("onSelected", false, "Only on selection", "If checked, only affects selected vertices"));
			break;
		}
		case CP_SCATTER_PER_MESH:
		{
			par.addParam(RichInt("seed", 0, "Seed","Random seed used to generate scattered colors. Zero means totally random (each time the filter is started it generates a different result)"));
			break;
		}
		case CP_FACE_SMOOTH:
		case CP_VERTEX_SMOOTH:
		{
			par.addParam(RichInt("iteration", 1, QString("Iteration"), QString("the number of iteration of the smoothing algorithm")));
			break;
		}
		case CP_TRIANGLE_QUALITY:
		{
			QStringList metrics;
			metrics.push_back("area/max side");
			metrics.push_back("inradius/circumradius");
			metrics.push_back("Mean ratio");
			metrics.push_back("Area");
			metrics.push_back("Texture Angle Distortion");
			metrics.push_back("Texture Area Distortion");
			metrics.push_back("Polygonal planarity (max)");
			metrics.push_back("Polygonal planarity (relative)");

			par.addParam(RichEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality.")));
			break;
		}
		case CP_DISCRETE_CURVATURE:
		{
			QStringList curvNameList;
			curvNameList.push_back("Mean Curvature");
			curvNameList.push_back("Gaussian Curvature");
			curvNameList.push_back("RMS Curvature");
			curvNameList.push_back("ABS Curvature");
			par.addParam(RichEnum("CurvatureType", 0, curvNameList, tr("Type:"),
				QString("Choose the curvature value that you want transferred onto the scalar Quality."
				"Mean (H) and Gaussian (K) curvature are computed according the technique described in the Desbrun et al. paper.<br>"
				"Absolute curvature is defined as |H|+|K| and RMS curvature as sqrt(4* H^2 - 2K) as explained in <br><i>Improved curvature estimation"
				"for watershed segmentation of 3-dimensional meshes </i> by S. Pulla, A. Razdan, G. Farin. ")));
			break;
		}
		case CP_SATURATE_QUALITY:
		{
			par.addParam(RichFloat("gradientThr", 1, "Gradient Threshold", "The maximum value admitted for the quality gradient (in absolute value)"));
			par.addParam(RichBool("updateColor", false, "Update ColorMap", "if true the color ramp is computed again"));

			break;
		}
		case CP_MESH_TO_FACE:
		{
			par.addParam(RichBool("allVisibleMesh", false, "Apply to all Meshes", "If true the color mapping is applied to all the meshes."));
			break;
		}
		case CP_CLAMP_QUALITY:
		{
			pair<float, float> minmax;
			minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(md.mm()->cm);
			par.addParam(RichFloat("minVal", minmax.first, "Min", "The value that will be mapped with the lower end of the scale (blue)"));
			par.addParam(RichFloat("maxVal", minmax.second, "Max", "The value that will be mapped with the upper end of the scale (red)"));
			par.addParam(RichDynamicFloat("perc", 0, 0, 100, "Percentile Crop [0..100]", "If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to a value <i>P</i> then the two values <i>V_min,V_max</i> for which <i>P</i>% of the vertices have a quality <b>lower or greater</b> than <i>V_min,V_max</i> are used as min/max values for clamping.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
			par.addParam(RichBool("zeroSym", false, "Zero Symmetric", "If true the min max range will be enlarged to be symmetric (so that green is always Zero)"));
			break;
		}
		case CP_MAP_VQUALITY_INTO_COLOR:
		{
			pair<float, float> minmax;
			minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(md.mm()->cm);
			par.addParam(RichFloat("minVal", minmax.first, "Min", "The value that will be mapped with the lower end of the scale (blue)"));
			par.addParam(RichFloat("maxVal", minmax.second, "Max", "The value that will be mapped with the upper end of the scale (red)"));
			par.addParam(RichDynamicFloat("perc", 0, 0, 100, "Percentile Crop [0..100]", "If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to a value <i>P</i> then the two values <i>V_min,V_max</i> for which <i>P</i>% of the vertices have a quality <b>lower or greater</b> than <i>V_min,V_max</i> are used as min/max values for clamping.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
			par.addParam(RichBool("zeroSym", false, "Zero Symmetric", "If true the min max range will be enlarged to be symmetric (so that green is always Zero)"));
			break;
		}
		case CP_MAP_FQUALITY_INTO_COLOR:
		{
			pair<float, float> minmax;
			minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(md.mm()->cm);
			par.addParam(RichFloat("minVal", minmax.first, "Min", "The value that will be mapped with the lower end of the scale (blue)"));
			par.addParam(RichFloat("maxVal", minmax.second, "Max", "The value that will be mapped with the upper end of the scale (red)"));
			par.addParam(RichDynamicFloat("perc", 0, 0, 100, "Percentile Crop [0..100]", "If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to a value <i>P</i> then the two values <i>V_min,V_max</i> for which <i>P</i>% of the faces have a quality <b>lower or greater</b> than <i>V_min,V_max</i> are used as min/max values for clamping.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
			par.addParam(RichBool("zeroSym", false, "Zero Symmetric", "If true the min max range will be enlarged to be symmetric (so that green is always Zero)"));
			break;
		}

		default: break; // do not add any parameter for the other filters
	}
}

bool FilterColorProc::applyFilter(const QAction *filter, MeshDocument &md, unsigned int& /*postConditionMask*/, const RichParameterList &par, vcg::CallBackPos *cb)
{
	MeshModel *m = md.mm();  //get current mesh from document

	switch(ID(filter))
	{
		case CP_FILLING:
		{
			QColor temp = par.getColor("color1");
			Color4b new_col = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());

			bool selected = par.getBool("onSelected");

			vcg::tri::UpdateColor<CMeshO>::PerVertexConstant(m->cm, new_col, selected);
			return true;
		}

		case CP_THRESHOLDING:
		{
			float threshold = math::Clamp<float>(par.getDynamicFloat("threshold"), 0.0f, 255.0f);
			QColor temp = par.getColor("color1");
			Color4b c1 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());
			temp = par.getColor("color2");
			Color4b c2 = Color4b(temp.red(), temp.green(), temp.blue(), temp.alpha());
			bool selected = par.getBool("onSelected");

			vcg::tri::UpdateColor<CMeshO>::PerVertexThresholding(m->cm, threshold, c1, c2, selected);
			return true;
		}

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

		case CP_INVERT :
		{
			bool selected = par.getBool("onSelected");

			vcg::tri::UpdateColor<CMeshO>::PerVertexInvert(m->cm, selected);
			return true;
		}

		case CP_LEVELS:
		{
			float gamma = par.getDynamicFloat("gamma");
			float  in_min = par.getDynamicFloat("in_min")/255;
			float  in_max = par.getDynamicFloat("in_max")/255;
			float  out_min = par.getDynamicFloat("out_min")/255;
			float  out_max = par.getDynamicFloat("out_max")/255;
			bool all_levels = par.getBool("apply_to_all");
			bool selected = par.getBool("onSelected");

			//builds incrementally a bitmask that indicates on which channels the filter works...
			unsigned char rgbMask = vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS;
			if(par.getBool("rCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::RED_CHANNEL;
			if(par.getBool("gCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::GREEN_CHANNEL;
			if(par.getBool("bCh")) rgbMask = rgbMask | vcg::tri::UpdateColor<CMeshO>::BLUE_CHANNEL;
			//if no channels are checked, we intend to work on all rgb channels, so...
			if(rgbMask == vcg::tri::UpdateColor<CMeshO>::NO_CHANNELS) rgbMask = vcg::tri::UpdateColor<CMeshO>::ALL_CHANNELS;

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
			bool selected = par.getBool("onSelected");

			double r, g, b;   //converts color from HSL to RGB....
			ColorSpace<unsigned char>::HSLtoRGB( (double)hue, (double)saturation, (double)luminance, r, g, b);
			Color4b color = Color4b((int)(r*255), (int)(g*255), (int)(b*255), 255);

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
			int noiseBits = par.getInt("noiseBits");
			bool selected = par.getBool("onSelected");

			tri::UpdateColor<CMeshO>::PerVertexAddNoise(m->cm, noiseBits, selected);
			return true;
		}

		case CP_SATURATE_QUALITY:
		{
			m->updateDataMask(MeshModel::MM_VERTFACETOPO);
			tri::UpdateQuality<CMeshO>::VertexSaturate(m->cm, par.getFloat("gradientThr"));
			if (par.getBool("updateColor"))
			{
				Histogramf H;
				tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m->cm, H);
				m->updateDataMask(MeshModel::MM_VERTCOLOR);
				tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m->cm, H.Percentile(0.1f), H.Percentile(0.9f));
			}
			log("Saturated Vertex Quality");
			return true;
		}

		case CP_MAP_VQUALITY_INTO_COLOR:
		{
			m->updateDataMask(MeshModel::MM_VERTCOLOR);

			float RangeMin = par.getFloat("minVal");
			float RangeMax = par.getFloat("maxVal");
			bool usePerc = par.getDynamicFloat("perc")>0;

			Histogramf H;
			tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m->cm, H);

			float PercLo = H.Percentile(par.getDynamicFloat("perc") / 100.f);
			float PercHi = H.Percentile(1.0 - par.getDynamicFloat("perc") / 100.f);

			if (par.getBool("zeroSym"))
			{
				RangeMin = min(RangeMin, -math::Abs(RangeMax));
				RangeMax = max(math::Abs(RangeMin), RangeMax);
				PercLo = min(PercLo, -math::Abs(PercHi));
				PercHi = max(math::Abs(PercLo), PercHi);
			}

			if (usePerc)
			{
				tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m->cm, PercLo, PercHi);
				log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ", H.MinV(), H.MaxV(), PercLo, PercHi, par.getDynamicFloat("perc"), 100 - par.getDynamicFloat("perc"));
			}
			else {
				tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m->cm, RangeMin, RangeMax);
				log("Quality Range: %f %f; Used (%f %f)", H.MinV(), H.MaxV(), RangeMin, RangeMax);
			}
			return true;
		}

		case CP_CLAMP_QUALITY:
		{
			float RangeMin = par.getFloat("minVal");
			float RangeMax = par.getFloat("maxVal");
			bool usePerc = par.getDynamicFloat("perc")>0;

			Histogramf H;
			tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m->cm, H);

			float PercLo = H.Percentile(par.getDynamicFloat("perc") / 100.f);
			float PercHi = H.Percentile(1.0 - par.getDynamicFloat("perc") / 100.f);

			if (par.getBool("zeroSym"))
			{
				RangeMin = min(RangeMin, -math::Abs(RangeMax));
				RangeMax = max(math::Abs(RangeMin), RangeMax);
				PercLo = min(PercLo, -math::Abs(PercHi));
				PercHi = max(math::Abs(PercLo), PercHi);
			}

			if (usePerc)
			{
				tri::UpdateQuality<CMeshO>::VertexClamp(m->cm, PercLo, PercHi);
				log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ", H.MinV(), H.MaxV(), PercLo, PercHi, par.getDynamicFloat("perc"), 100 - par.getDynamicFloat("perc"));
			}
			else {
				tri::UpdateQuality<CMeshO>::VertexClamp(m->cm, RangeMin, RangeMax);
				log("Quality Range: %f %f; Used (%f %f)", H.MinV(), H.MaxV(), RangeMin, RangeMax);
			}
			return true;
		}

		case CP_MAP_FQUALITY_INTO_COLOR:
		{
			m->updateDataMask(MeshModel::MM_FACECOLOR);
			float RangeMin = par.getFloat("minVal");
			float RangeMax = par.getFloat("maxVal");
			float perc = par.getDynamicFloat("perc");
			bool usePerc = perc>0;

			Histogramf H;
			tri::Stat<CMeshO>::ComputePerFaceQualityHistogram(m->cm, H);
			float PercLo = H.Percentile(perc / 100.f);
			float PercHi = H.Percentile(1.0 - perc / 100.f);

			// Make the range and percentile symmetric w.r.t. zero, so that
			// the value zero is always colored in yellow
			if (par.getBool("zeroSym")){
				RangeMin = min(RangeMin, -math::Abs(RangeMax));
				RangeMax = max(math::Abs(RangeMin), RangeMax);
				PercLo = min(PercLo, -math::Abs(PercHi));
				PercHi = max(math::Abs(PercLo), PercHi);
			}

			if (usePerc){
				tri::UpdateColor<CMeshO>::PerFaceQualityRamp(m->cm, PercLo, PercHi);
				log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ",
					H.MinV(), H.MaxV(), PercLo, PercHi, perc, 100 - perc);
			}
			else {
				tri::UpdateColor<CMeshO>::PerFaceQualityRamp(m->cm, RangeMin, RangeMax);
				log("Quality Range: %f %f; Used (%f %f)", H.MinV(), H.MaxV(), RangeMin, RangeMax);
			}
			return true;
		}

		case CP_DISCRETE_CURVATURE:
		{
			m->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCURV);
			m->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
			tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m->cm);

			if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m->cm) > 0) {
				errorMessage = "Mesh has some not 2-manifold faces, Curvature computation requires manifoldness"; // text
				return false; // can't continue, mesh can't be processed
			}

			int delvert = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
			if (delvert) log("Pre-Curvature Cleaning: Removed %d unreferenced vertices", delvert);
			tri::Allocator<CMeshO>::CompactVertexVector(m->cm);
			tri::UpdateCurvature<CMeshO>::MeanAndGaussian(m->cm);
			int curvType = par.getEnum("CurvatureType");

			switch (curvType)
			{
				case 0: tri::UpdateQuality<CMeshO>::VertexFromMeanCurvatureHG(m->cm);        log("Computed Mean Curvature");      break;
				case 1: tri::UpdateQuality<CMeshO>::VertexFromGaussianCurvatureHG(m->cm);    log("Computed Gaussian Curvature"); break;
				case 2: tri::UpdateQuality<CMeshO>::VertexFromRMSCurvature(m->cm);         log("Computed RMS Curvature"); break;
				case 3: tri::UpdateQuality<CMeshO>::VertexFromAbsoluteCurvature(m->cm);    log("Computed ABS Curvature"); break;
				default: assert(0);
			}

			Histogramf H;
			tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m->cm, H);
			tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m->cm, H.Percentile(0.1f), H.Percentile(0.9f));
			log("Curvature Range: %f %f (Used 90 percentile %f %f) ", H.MinV(), H.MaxV(), H.Percentile(0.1f), H.Percentile(0.9f));
			return true;
		}

		case CP_TRIANGLE_QUALITY:
		{
			m->updateDataMask(MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY);
			CMeshO::FaceIterator fi;
			Distribution<float> distrib;
			float minV = 0;
			float maxV = 1.0;
			int metric = par.getEnum("Metric");
			if (metric == 4 || metric == 5)
			{
				if (!m->hasDataMask(MeshModel::MM_VERTTEXCOORD) && !m->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
				{
					this->errorMessage = "This metric need Texture Coordinate";
					return false;
				}
			}
			if ((metric == 6 || metric == 7) && !m->hasDataMask(MeshModel::MM_POLYGONAL))
			{
				this->errorMessage = "This metric is meaningless for triangle only meshes (all faces are planar by definition)";
				return false;
			}
			switch (metric){

			case 0: { //area / max edge
						minV = 0;
						maxV = sqrt(3.0f) / 2.0f;
						for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
							(*fi).Q() = vcg::Quality((*fi).P(0), (*fi).P(1), (*fi).P(2));
			} break;

			case 1: { //inradius / circumradius
						for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
							(*fi).Q() = vcg::QualityRadii((*fi).P(0), (*fi).P(1), (*fi).P(2));
			} break;

			case 2: { //Mean Ratio = area/(a*a + b*b + c*c). See vcg::QualityMeanRatio.
						for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
							(*fi).Q() = vcg::QualityMeanRatio((*fi).P(0), (*fi).P(1), (*fi).P(2));
			} break;

			case 3: { // Area of triangle
						for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
							(*fi).Q() = vcg::DoubleArea((*fi))*0.5f;
						tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm, minV, maxV);
			} break;

			case 4: { //TEXTURE Angle Distortion
						if (m->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
						{
							for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
								(*fi).Q() = Distortion<CMeshO, true>::AngleDistortion(&*fi);
						}
						else {
							for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
								(*fi).Q() = Distortion<CMeshO, false>::AngleDistortion(&*fi);
						}
						tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m->cm, distrib);
						minV = distrib.Percentile(CMeshO::ScalarType(0.05));
						maxV = distrib.Percentile(CMeshO::ScalarType(0.95));
			} break;

			case 5: { //TEXTURE Area Distortion
						CMeshO::ScalarType areaScaleVal, edgeScaleVal;
						if (m->hasDataMask(MeshModel::MM_WEDGTEXCOORD))
						{
							Distortion<CMeshO, true>::MeshScalingFactor(m->cm, areaScaleVal, edgeScaleVal);
							for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
								(*fi).Q() = Distortion<CMeshO, true>::AreaDistortion(&*fi, areaScaleVal);
						}
						else {
							Distortion<CMeshO, false>::MeshScalingFactor(m->cm, areaScaleVal, edgeScaleVal);
							for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
								(*fi).Q() = Distortion<CMeshO, false>::AreaDistortion(&*fi, areaScaleVal);

						}
						tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m->cm, distrib);
						minV = distrib.Percentile(CMeshO::ScalarType(0.05));
						maxV = distrib.Percentile(CMeshO::ScalarType(0.95));
			} break;
			case 6:
			case 7: { // polygonal planarity
						tri::UpdateFlags<CMeshO>::FaceClearV(m->cm);
						std::vector<CMeshO::VertexPointer> vertVec;
						std::vector<CMeshO::FacePointer> faceVec;
						for (size_t i = 0; i<m->cm.face.size(); ++i)
						if (!m->cm.face[i].IsV())
						{
							tri::PolygonSupport<CMeshO, CMeshO>::ExtractPolygon(&(m->cm.face[i]), vertVec, faceVec);
							//assert(faceVec.size()==vertVec.size()-2);

							std::vector<CMeshO::CoordType> pointVec;
							for (size_t j = 0; j<vertVec.size(); ++j)
								pointVec.push_back(vertVec[j]->P());

							Plane3m pl;
							vcg::FitPlaneToPointSet(pointVec, pl);
							float maxDist = 0, sumDist = 0, halfPerim = 0;
							for (size_t j = 0; j<vertVec.size(); ++j)
							{
								float d = fabs(SignedDistancePlanePoint(pl, pointVec[j]));
								sumDist += d;
								maxDist = max(maxDist, d);
								halfPerim += Distance(pointVec[j], pointVec[(j + 1) % vertVec.size()]);
							}

							float avgDist = sumDist / float(vertVec.size());
							for (size_t j = 0; j<faceVec.size(); ++j)
							if (metric == 6)
								faceVec[j]->Q() = maxDist;
							else
								faceVec[j]->Q() = avgDist / halfPerim;

						}
						tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m->cm, distrib);
						minV = distrib.Percentile(CMeshO::ScalarType(0.05));
						maxV = distrib.Percentile(CMeshO::ScalarType(0.95));
			} break;

			default: assert(0);
			}
			tri::UpdateColor<CMeshO>::PerFaceQualityRamp(m->cm, minV, maxV, false);
			return true;
		}


		case CP_RANDOM_CONNECTED_COMPONENT:
		{
			m->updateDataMask(MeshModel::MM_FACEFACETOPO);
			m->updateDataMask(MeshModel::MM_FACEMARK | MeshModel::MM_FACECOLOR);
			vcg::tri::UpdateColor<CMeshO>::PerFaceRandomConnectedComponent(m->cm);
			return true;
		}

		case CP_RANDOM_FACE:
		{
			m->updateDataMask(MeshModel::MM_FACEFACETOPO);
			m->updateDataMask(MeshModel::MM_FACEMARK | MeshModel::MM_FACECOLOR);
			vcg::tri::UpdateColor<CMeshO>::PerFaceRandom(m->cm);
			return true;
		}

		case CP_VERTEX_SMOOTH:
		{
			int iteration = par.getInt("iteration");
			tri::Smooth<CMeshO>::VertexColorLaplacian(m->cm, iteration, false, cb);
			return true;
		}

		case CP_FACE_SMOOTH:
		{
			m->updateDataMask(MeshModel::MM_FACEFACETOPO);
			int iteration = par.getInt("iteration");
			tri::Smooth<CMeshO>::FaceColorLaplacian(m->cm, iteration, false, cb);
			return true;
		}

		case CP_FACE_TO_VERTEX:
		{
			m->updateDataMask(MeshModel::MM_VERTCOLOR);
			tri::UpdateColor<CMeshO>::PerVertexFromFace(m->cm);
			return true;
		}

		case CP_MESH_TO_FACE:
		{
			QList<MeshModel *> meshList;
			foreach(MeshModel *mmi, md.meshList)
			{
				if (mmi->visible)
				{
					mmi->updateDataMask(MeshModel::MM_FACECOLOR);
					tri::UpdateColor<CMeshO>::PerFaceConstant(mmi->cm, mmi->cm.C());
				}
			}
			return true;
		}

		case CP_VERTEX_TO_FACE:
		{
			m->updateDataMask(MeshModel::MM_FACECOLOR);
			tri::UpdateColor<CMeshO>::PerFaceFromVertex(m->cm);
			return true;
		}

		case CP_TEXTURE_TO_VERTEX:
		{
			m->updateDataMask(MeshModel::MM_VERTCOLOR);
			if (!HasPerWedgeTexCoord(m->cm)) break;
			CMeshO::FaceIterator fi;

			vector <QImage> srcImgs;
			srcImgs.resize(m->cm.textures.size());
			QString path;

			for (size_t textInd = 0; textInd < m->cm.textures.size(); textInd++)
			{
				path = m->fullName();
				path = path.left(std::max<int>(path.lastIndexOf('\\'), path.lastIndexOf('/')) + 1).append(m->cm.textures[textInd].c_str());
				CheckError(!QFile(path).exists(), QString("Source texture \"").append(path).append("\" doesn't exists"));
				CheckError(!srcImgs[textInd].load(path), QString("Source texture \"").append(path).append("\" cannot be opened"));
			}

			for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi) if (!(*fi).IsD())
			{
				for (int i = 0; i<3; i++)
				{
					// note the trick for getting only the fractional part of the uv with the correct wrapping (e.g. 1.5 -> 0.5 and -0.3 -> 0.7)
					vcg::Point2f newcoord((*fi).WT(i).P().X() - floor((*fi).WT(i).P().X()), (*fi).WT(i).P().Y() - floor((*fi).WT(i).P().Y()));
					int textIndex = (*fi).WT(i).N();
					if ((textIndex >= 0) && (textIndex < (int)m->cm.textures.size()))
					{
						QRgb val = srcImgs[textIndex].pixel(newcoord[0] * srcImgs[textIndex].width(), (1 - newcoord[1])*srcImgs[textIndex].height() - 1);
						(*fi).V(i)->C() = Color4b(qRed(val), qGreen(val), qBlue(val), 255);
					}
					else
					{
						(*fi).V(i)->C() = Color4b(255, 255, 255, 255);
					}
				}
			}
			return true;
		}

		default: assert(0);
	}
	return false;
}

 FilterPluginInterface::FilterClass FilterColorProc::getClass(const QAction *a) const
{
	switch(ID(a))
	{
		case CP_FILLING:
		case CP_THRESHOLDING:
		case CP_CONTR_BRIGHT:
		case CP_INVERT:
		case CP_COLOURISATION:
		case CP_EQUALIZE:
		case CP_DESATURATION:
		case CP_WHITE_BAL:
		case CP_LEVELS:
		case CP_PERLIN_COLOR:
		case CP_COLOR_NOISE:
		case CP_MAP_VQUALITY_INTO_COLOR:
		case CP_VERTEX_SMOOTH:
		case CP_FACE_TO_VERTEX:
		case CP_TEXTURE_TO_VERTEX:          return FilterPluginInterface::VertexColoring;
		case CP_SCATTER_PER_MESH:           return FilterPluginInterface::MeshColoring;
		case CP_SATURATE_QUALITY:
		case CP_CLAMP_QUALITY:              return FilterPluginInterface::Quality;
		case CP_DISCRETE_CURVATURE:         return FilterClass(Normal + VertexColoring);
		case CP_TRIANGLE_QUALITY:           return FilterClass(Quality + FaceColoring);
		case CP_RANDOM_FACE:
		case CP_RANDOM_CONNECTED_COMPONENT:
		case CP_FACE_SMOOTH:
		case CP_VERTEX_TO_FACE:
		case CP_MESH_TO_FACE:
		case CP_MAP_FQUALITY_INTO_COLOR:    return FilterPluginInterface::FaceColoring;
		default: assert(0);
	}
	return FilterPluginInterface::Generic;
}

int FilterColorProc::postCondition( const QAction* filter ) const
{
	switch(ID(filter))
	{
		case CP_FILLING:
		case CP_THRESHOLDING:
		case CP_CONTR_BRIGHT:
		case CP_INVERT:
		case CP_COLOURISATION:
		case CP_EQUALIZE:
		case CP_DESATURATION:
		case CP_WHITE_BAL:
		case CP_LEVELS:
		case CP_PERLIN_COLOR:
		case CP_COLOR_NOISE:
		case CP_MAP_VQUALITY_INTO_COLOR:
		case CP_VERTEX_SMOOTH:
		case CP_FACE_TO_VERTEX:
		case CP_TEXTURE_TO_VERTEX:          return MeshModel::MM_VERTCOLOR;
		case CP_SATURATE_QUALITY:
		case CP_CLAMP_QUALITY:              return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;
		case CP_DISCRETE_CURVATURE:         return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTNUMBER;
		case CP_RANDOM_FACE:
		case CP_RANDOM_CONNECTED_COMPONENT:
		case CP_FACE_SMOOTH:
		case CP_VERTEX_TO_FACE:
		case CP_MESH_TO_FACE:
		case CP_MAP_FQUALITY_INTO_COLOR:    return MeshModel::MM_FACECOLOR;
		case CP_TRIANGLE_QUALITY:           return MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY;
		case CP_SCATTER_PER_MESH:           return MeshModel::MM_COLOR;

		default: assert(0);
    }
	return MeshModel::MM_NONE;
}

int FilterColorProc::getPreConditions(const QAction* filter ) const
{
	switch(ID(filter))
	{
		case CP_FILLING:
		case CP_COLOURISATION:
		case CP_SCATTER_PER_MESH:
		case CP_PERLIN_COLOR:
		case CP_COLOR_NOISE:                
		case CP_MESH_TO_FACE:               return MeshModel::MM_NONE;
		case CP_THRESHOLDING:
		case CP_CONTR_BRIGHT:
		case CP_INVERT:
		case CP_EQUALIZE:
		case CP_DESATURATION:
		case CP_WHITE_BAL:
		case CP_LEVELS:                     
		case CP_VERTEX_SMOOTH:
		case CP_VERTEX_TO_FACE:             return MeshModel::MM_VERTCOLOR;
		case CP_TRIANGLE_QUALITY:
		case CP_RANDOM_FACE:
		case CP_RANDOM_CONNECTED_COMPONENT:
		case CP_DISCRETE_CURVATURE:         return MeshModel::MM_FACENUMBER;
		case CP_SATURATE_QUALITY:
		case CP_CLAMP_QUALITY:
		case CP_MAP_VQUALITY_INTO_COLOR:    return MeshModel::MM_VERTQUALITY;
		case CP_MAP_FQUALITY_INTO_COLOR:    return MeshModel::MM_FACEQUALITY;
		case CP_FACE_TO_VERTEX:
		case CP_FACE_SMOOTH:                return MeshModel::MM_FACECOLOR;
		case CP_TEXTURE_TO_VERTEX:          return MeshModel::MM_NONE;

		default: assert(0);
	}
	return MeshModel::MM_NONE;
}

FilterPluginInterface::FILTER_ARITY FilterColorProc::filterArity(const QAction* act ) const
{
    switch(ID(act))
    {
		case CP_FILLING:
		case CP_COLOURISATION:
		case CP_PERLIN_COLOR:
		case CP_COLOR_NOISE:
		case CP_MESH_TO_FACE:
		case CP_THRESHOLDING:
		case CP_CONTR_BRIGHT:
		case CP_INVERT:
		case CP_EQUALIZE:
		case CP_DESATURATION:
		case CP_WHITE_BAL:
		case CP_LEVELS:
		case CP_VERTEX_SMOOTH:
		case CP_VERTEX_TO_FACE:
		case CP_TRIANGLE_QUALITY:
		case CP_RANDOM_FACE:
		case CP_RANDOM_CONNECTED_COMPONENT:
		case CP_DISCRETE_CURVATURE:
		case CP_SATURATE_QUALITY:
		case CP_CLAMP_QUALITY:
		case CP_MAP_VQUALITY_INTO_COLOR:
		case CP_MAP_FQUALITY_INTO_COLOR:
		case CP_FACE_TO_VERTEX:
		case CP_FACE_SMOOTH:
		case CP_TEXTURE_TO_VERTEX:          return FilterPluginInterface::SINGLE_MESH;
		case CP_SCATTER_PER_MESH:           return FilterPluginInterface::VARIABLE;

		default: assert(0);
    }
	return FilterPluginInterface::SINGLE_MESH;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterColorProc)
