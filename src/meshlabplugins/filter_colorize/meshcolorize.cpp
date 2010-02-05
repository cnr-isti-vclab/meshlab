/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2008                                           \/)\/    *
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
#include <QtGui>
#include <limits>
#include "meshcolorize.h"
#include "color_manifold.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/update/quality.h>


  class Frange
  {
  public:
    Frange(){}
    Frange(std::pair<float,float> minmax):minV(minmax.first),maxV(minmax.second){}
    Frange(float _min,float _max):minV(_min),maxV(_max){}

    float minV;
    float maxV;
  };

using namespace std;
using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
    typeList << 
    CP_MAP_QUALITY_INTO_COLOR <<
    CP_DISCRETE_CURVATURE <<
    CP_TRIANGLE_QUALITY <<
		CP_SELFINTERSECT_SELECT <<
		CP_SELFINTERSECT_COLOR <<
    CP_BORDER <<
		CP_TEXBORDER <<
    CP_COLOR_NON_MANIFOLD_FACE <<
    CP_COLOR_NON_MANIFOLD_VERTEX <<
    CP_VERTEX_SMOOTH <<
		CP_FACE_SMOOTH <<
		CP_VERTEX_TO_FACE <<
		CP_FACE_TO_VERTEX <<
		CP_TEXTURE_TO_VERTEX <<
    //CP_COLOR_NON_TOPO_COHERENT <<
		CP_RANDOM_FACE;
    
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);
}

QString ExtraMeshColorizePlugin::filterName(FilterIDType c) const{
  switch(c)
  {
    case CP_MAP_QUALITY_INTO_COLOR:   return QString("Colorize by Quality");
    case CP_DISCRETE_CURVATURE:                 return QString("Discrete Curvatures");
    case CP_TRIANGLE_QUALITY:         return QString("Triangle quality");
    case CP_SELFINTERSECT_COLOR:      return QString("Self Intersections");
    case CP_SELFINTERSECT_SELECT:     return QString("Self Intersecting Faces");
    case CP_BORDER:                   return QString("Border");
    case CP_TEXBORDER:                   return QString("Texture Border");
    case CP_COLOR_NON_MANIFOLD_FACE:  return QString("Color non Manifold Faces");
    case CP_COLOR_NON_MANIFOLD_VERTEX:return QString("Color non Manifold Vertices");
    case CP_COLOR_NON_TOPO_COHERENT:  return QString("Color edges topologically non coherent");
    case CP_VERTEX_SMOOTH:                   return QString("Laplacian Smooth Vertex Color");
    case CP_FACE_SMOOTH:                   return QString("Laplacian Smooth Face Color");
    case CP_VERTEX_TO_FACE:                   return QString("Vertex to Face color transfer");
    case CP_FACE_TO_VERTEX:                   return QString("Face to Vertex color transfer");
	case CP_TEXTURE_TO_VERTEX:                   return QString("Texture to Vertex color transfer");
		case CP_RANDOM_FACE:         return QString("Random Face Color");
			
    default: assert(0);
  }
  return QString("error!");
}
QString ExtraMeshColorizePlugin::filterInfo(FilterIDType filterId) const 
{
  switch(filterId)
  {
    case CP_MAP_QUALITY_INTO_COLOR : return tr("Colorize vertex and faces depending on quality field (manually equalized).");
    case CP_DISCRETE_CURVATURE :     return tr("Colorize according to various discrete curvature computed as described in:<br>"
																							 "'<i>Discrete Differential-Geometry Operators for Triangulated 2-Manifolds</i>' <br>"
																							 "M. Meyer, M. Desbrun, P. Schroder, A. H. Barr");
    case CP_TRIANGLE_QUALITY:        return tr("Colorize faces depending on triangle quality:<br/>1: minimum ratio height/edge among the edges<br/>2: ratio between radii of incenter and circumcenter<br/>3:  2*sqrt(a, b)/(a+b), a, b the eigenvalues of M^tM, M transform triangle into equilateral");
    case CP_SELFINTERSECT_SELECT:    return tr("Select only self intersecting faces.");
    case CP_SELFINTERSECT_COLOR:     return tr("Colorize only self intersecting faces.");
    case CP_BORDER :                 return tr("Colorize only border edges.");
    case CP_TEXBORDER :                 return tr("Colorize only border edges.");
    case CP_COLOR_NON_MANIFOLD_FACE: return tr("Colorize the non manifold edges, eg the edges where there are more than two incident faces");
    case CP_COLOR_NON_MANIFOLD_VERTEX:return tr("Colorize only non manifold edges eg. ");
    case CP_VERTEX_SMOOTH:                   return QString("Laplacian Smooth Vertex Color");
    case CP_FACE_SMOOTH:                   return QString("Laplacian Smooth Face Color");
    case CP_VERTEX_TO_FACE:                   return QString("Vertex to Face color transfer");
    case CP_FACE_TO_VERTEX:                   return QString("Face to Vertex color transfer");
    case CP_TEXTURE_TO_VERTEX:                   return QString("Texture to Vertex color transfer");
    case CP_COLOR_NON_TOPO_COHERENT :return tr("Color edges topologically non coherent.");
		case CP_RANDOM_FACE:         return QString("Colorize Faces randomly. If internal edges are present they are used");

    default: assert(0); 
  }
  return QString();
}

int ExtraMeshColorizePlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case CP_DISCRETE_CURVATURE:       return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_VERTCURV;
    case CP_TRIANGLE_QUALITY:         return MeshModel::MM_FACECOLOR;
    case CP_SELFINTERSECT_SELECT:
		case CP_SELFINTERSECT_COLOR:
		            return MeshModel::MM_FACEMARK | MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACECOLOR;
    case CP_BORDER:             return MeshModel::MM_FACEFLAGBORDER;
    case CP_TEXBORDER:                   return MeshModel::MM_FACEFACETOPO;
    case CP_COLOR_NON_MANIFOLD_FACE:       
    case CP_COLOR_NON_MANIFOLD_VERTEX:       return MeshModel::MM_FACEFACETOPO;
    case CP_RANDOM_FACE:       return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACECOLOR;
    case CP_MAP_QUALITY_INTO_COLOR:   return 0;
    case CP_VERTEX_SMOOTH:                   return 0;
    case CP_FACE_SMOOTH:                   return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACECOLOR ;
    case CP_VERTEX_TO_FACE:                   return MeshModel::MM_FACECOLOR;
    case CP_FACE_TO_VERTEX:                   return MeshModel::MM_VERTCOLOR;
	case CP_TEXTURE_TO_VERTEX:                   return MeshModel::MM_FACECOLOR;

    default: assert(0);
  }
  return 0;
}


void ExtraMeshColorizePlugin::initParameterSet(QAction *a,MeshModel &m, RichParameterSet & par) {
	switch(ID(a))
  {
		case CP_FACE_SMOOTH: 
		case CP_VERTEX_SMOOTH: 
			//par.addInt("iteration",1,tr("Iteration"),tr("the number ofiteration of the smoothing algorithm"));
			par.addParam(new RichInt("iteration",1,tr("Iteration"),tr("the number ofiteration of the smoothing algorithm")));
								 break;
	case CP_TRIANGLE_QUALITY: {
			QStringList metrics;
			metrics.push_back("area/max side");
			metrics.push_back("inradius/circumradius");
			metrics.push_back("mean ratio");
			par.addParam(new RichEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality.")));
			break;
		}
	case CP_DISCRETE_CURVATURE: {
			QStringList curvNameList;
			curvNameList.push_back("Mean Curvature");
			curvNameList.push_back("Gaussian Curvature");
			curvNameList.push_back("RMS Curvature");
			curvNameList.push_back("ABS Curvature");
			par.addParam(new RichEnum("CurvatureType", 0, curvNameList, tr("Type:"), tr("Choose the curvatures. Mean and Gaussian curvature are computed according the technique described in the Desbrun et al. paper.<br>"
			"Absolute curvature is defined as |H|+|K| and RMS curvature as sqrt(4* H^2 - 2K) as explained in <br><i>Improved curvature estimation for watershed segmentation of 3-dimensional meshes </i> by S. Pulla, A. Razdan, G. Farin. ")));
		break;
		}
	case CP_MAP_QUALITY_INTO_COLOR :
		{
			pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
			
			par.addParam(new RichFloat("minVal",minmax.first,"Min","The value that will be mapped with the lower end of the scale (blue)"));
			par.addParam(new RichFloat("maxVal",minmax.second,"Max","The value that will be mapped with the upper end of the scale (red)"));
			par.addParam(new RichDynamicFloat("perc",0,0,100,"Percentile Crop [0..100]","If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to <i>P</i> the value <i>V</i> for which <i>P</i>% of the vertices have a quality <b>lower</b>(greater) than <i>V</i> is used as min (max) value.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
			par.addParam(new RichBool("zeroSym",false,"Zero Simmetric","If true the min max range will be enlarged to be symmertic (so that green is always Zero)"));
		} 
		break;
	
	default: assert(0);
	}
}

bool ExtraMeshColorizePlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
 MeshModel &m=*(md.mm());
 switch(ID(filter)) {
  case CP_MAP_QUALITY_INTO_COLOR :
    {
      float RangeMin = par.getFloat("minVal");	
      float RangeMax = par.getFloat("maxVal");		
			bool usePerc = par.getDynamicFloat("perc")>0;
			
			Histogramf H;
			tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);

			float PercLo = H.Percentile(par.getDynamicFloat("perc")/100.f);
			float PercHi = H.Percentile(1.0-par.getDynamicFloat("perc")/100.f);
			
			if(par.getBool("zeroSym"))
				{
					RangeMin = min(RangeMin, -math::Abs(RangeMax));
					RangeMax = max(math::Abs(RangeMin), RangeMax);
					PercLo = min(PercLo, -math::Abs(PercHi));
					PercHi = max(math::Abs(PercLo), PercHi);
				}
			
			if(usePerc) 
			{
				tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,PercLo,PercHi);
				Log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ",H.MinV(),H.MaxV(),PercLo,PercHi,par.getDynamicFloat("perc"),100-par.getDynamicFloat("perc"));
			}
			else
			{
				tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,RangeMin,RangeMax);
				Log("Quality Range: %f %f; Used (%f %f)",H.MinV(),H.MaxV(),RangeMin,RangeMax);				
			}
      break;
    }
  case CP_DISCRETE_CURVATURE:
    {
			if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
				errorMessage = "Mesh has some not 2-manifold faces, Curvature computation requires manifoldness"; // text
				return false; // can't continue, mesh can't be processed
			}
			
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log("Pre-Curvature Cleaning: Removed %d unreferenced vertices",delvert);
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			tri::UpdateCurvature<CMeshO>::MeanAndGaussian(m.cm);
      int curvType = par.getEnum("CurvatureType");
			
			switch(curvType){ 
          case 0: tri::UpdateQuality<CMeshO>::VertexFromMeanCurvature(m.cm);        Log(GLLogStream::FILTER, "Computed Mean Curvature");      break;
			    case 1: tri::UpdateQuality<CMeshO>::VertexFromGaussianCurvature(m.cm);    Log(GLLogStream::FILTER, "Computed Gaussian Curvature"); break;
          case 2: tri::UpdateQuality<CMeshO>::VertexFromRMSCurvature(m.cm);         Log(GLLogStream::FILTER, "Computed RMS Curvature"); break;
          case 3: tri::UpdateQuality<CMeshO>::VertexFromAbsoluteCurvature(m.cm);    Log(GLLogStream::FILTER, "Computed ABS Curvature"); break;
					default : assert(0);
      }      
      
      Histogramf H;
      tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
      tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,H.Percentile(0.1f),H.Percentile(0.9f));
      Log(GLLogStream::FILTER, "Curvature Range: %f %f (Used 90 percentile %f %f) ",H.MinV(),H.MaxV(),H.Percentile(0.1f),H.Percentile(0.9f));
			m.updateDataMask(MeshModel::MM_VERTQUALITY);
			m.updateDataMask(MeshModel::MM_VERTCOLOR);
    break;
    }  
  case CP_TRIANGLE_QUALITY:
    {
			CMeshO::FaceIterator fi;
			float min = 0;
			float max = 1.0;
			int metric = par.getEnum("Metric");
			switch(metric){ 
			case 0: { //area / max edge
				max = sqrt(3.0f)/2.0f;
				for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
					if(!(*fi).IsD()) 
						(*fi).C().ColorRamp(min, max, vcg::Quality((*fi).P(0), (*fi).P(1),(*fi).P(2)));
			} break;
			case 1: { //inradius / circumradius
				for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
					if(!(*fi).IsD()) 
						(*fi).C().ColorRamp(min, max, vcg::QualityRadii((*fi).P(0), (*fi).P(1), (*fi).P(2)));
			} break;
			case 2: { //mean ratio
				for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
					if(!(*fi).IsD()) 
						(*fi).C().ColorRamp(min, max, vcg::QualityMeanRatio((*fi).P(0), (*fi).P(1), (*fi).P(2)));
			} break;
			default: assert(0);
			} 
		break;
    }

  case CP_SELFINTERSECT_SELECT:
	case CP_SELFINTERSECT_COLOR:
    {
      vector<CFaceO *> IntersFace;
			vector<CFaceO *>::iterator fpi;
      tri::Clean<CMeshO>::SelfIntersections(m.cm,IntersFace);
			if(ID(filter)==CP_SELFINTERSECT_COLOR) 
				{
					tri::UpdateColor<CMeshO>::FaceConstant(m.cm,Color4b::White);
					for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
						(*fpi)->C()=Color4b::Red;
				}
			else
			{
				tri::UpdateSelection<CMeshO>::ClearFace(m.cm);   
				for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
					(*fpi)->SetS();
			}
    break;  
    }

  case CP_BORDER:
    vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
    break;
	case CP_RANDOM_FACE:
    vcg::tri::UpdateColor<CMeshO>::MultiFaceRandom(m.cm);
    break;
	case CP_TEXBORDER:
		vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);
    vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
		
		// Just to be sure restore standard topology and border flags 
		tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
    tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);
    break;
  case CP_COLOR_NON_MANIFOLD_FACE:
    ColorManifoldFace<CMeshO>(m.cm);
    break;
  case CP_COLOR_NON_MANIFOLD_VERTEX:
    ColorManifoldVertex<CMeshO>(m.cm);
    break;
  case CP_VERTEX_SMOOTH:
		{
		int iteration = par.getInt("iteration");
		tri::Smooth<CMeshO>::VertexColorLaplacian(m.cm,iteration,false,cb);
		}
		break;
  case CP_FACE_SMOOTH:
		{
		int iteration = par.getInt("iteration");
		tri::Smooth<CMeshO>::FaceColorLaplacian(m.cm,iteration,false,cb);
		}
		break;
  case CP_FACE_TO_VERTEX:
		 tri::UpdateColor<CMeshO>::VertexFromFace(m.cm);
		break;
	 case CP_VERTEX_TO_FACE:
		 tri::UpdateColor<CMeshO>::FaceFromVertex(m.cm);
		 break;
  case CP_TEXTURE_TO_VERTEX:
		{
			if(!HasPerWedgeTexCoord(m.cm)) break;
			CMeshO::FaceIterator fi; 
			QImage tex(m.cm.textures[0].c_str());
			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD()) 
			{
				for (int i=0; i<3; i++)
				{
					vcg::Point2f newcoord((*fi).WT(i).P().X()-(int)(*fi).WT(i).P().X(),(*fi).WT(i).P().Y()-(int)(*fi).WT(i).P().Y());
					QRgb val = tex.pixel(newcoord[0]*tex.width(),(1-newcoord[1])*tex.height()-1);
					(*fi).V(i)->C().SetRGB(qRed(val),qGreen(val),qBlue(val));
					
				}
			}
	    }
		
		break;
 }
	return true;
}

MeshFilterInterface::FilterClass ExtraMeshColorizePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case   CP_BORDER:
    case   CP_TEXBORDER:
    case   CP_COLOR_NON_MANIFOLD_VERTEX:
    case   CP_COLOR_NON_MANIFOLD_FACE:
    case   CP_MAP_QUALITY_INTO_COLOR:
    case   CP_DISCRETE_CURVATURE:
    case   CP_COLOR_NON_TOPO_COHERENT:
    case   CP_VERTEX_SMOOTH:
    case   CP_FACE_TO_VERTEX:
        return MeshFilterInterface::VertexColoring; 	
	  case   CP_TEXTURE_TO_VERTEX:
				return MeshFilterInterface::VertexColoring; 
	  case   CP_SELFINTERSECT_SELECT: return MeshFilterInterface::Selection;
    case   CP_SELFINTERSECT_COLOR:
    case   CP_TRIANGLE_QUALITY:
		case   CP_RANDOM_FACE:	
    case   CP_FACE_SMOOTH:
    case   CP_VERTEX_TO_FACE:
               return MeshFilterInterface::FaceColoring; 
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}

bool ExtraMeshColorizePlugin::autoDialog(QAction *a) {
	switch(ID(a)) 
	{
		case CP_DISCRETE_CURVATURE:
		case CP_TRIANGLE_QUALITY: 
		case CP_VERTEX_SMOOTH:
		case CP_FACE_SMOOTH:
		case CP_MAP_QUALITY_INTO_COLOR:
				return true;
    
		default: return false;
  }
}

int ExtraMeshColorizePlugin::getPreConditions(QAction *a) const 
{
	switch(ID(a)) 
	{
    case   CP_BORDER:
    case   CP_COLOR_NON_MANIFOLD_VERTEX:
    case   CP_COLOR_NON_MANIFOLD_FACE:
	  case   CP_SELFINTERSECT_SELECT: 
		case   CP_SELFINTERSECT_COLOR:
    case   CP_TRIANGLE_QUALITY:
		case   CP_RANDOM_FACE:	
    case   CP_DISCRETE_CURVATURE:
    case   CP_COLOR_NON_TOPO_COHERENT:
				return MeshFilterInterface::FP_Face;

    case   CP_MAP_QUALITY_INTO_COLOR:
				return MeshFilterInterface::FP_VertexQuality;

    case   CP_FACE_TO_VERTEX:
    case   CP_FACE_SMOOTH:
				return MeshFilterInterface::FP_FaceColor;

    case   CP_VERTEX_SMOOTH:
    case   CP_VERTEX_TO_FACE:
				return MeshFilterInterface::FP_VertexColor;

	  case   CP_TEXTURE_TO_VERTEX:
    case   CP_TEXBORDER: 
				return MeshFilterInterface::FP_WedgeTexCoord;

    default: assert(0);
	}
	return MeshFilterInterface::FP_Generic;
}

int ExtraMeshColorizePlugin::postCondition( QAction* a ) const 
{
	switch(ID(a)) 
	{
	  case   CP_SELFINTERSECT_SELECT: 
		case   CP_SELFINTERSECT_COLOR:
    case   CP_TRIANGLE_QUALITY:
		case   CP_RANDOM_FACE:	
    case   CP_COLOR_NON_TOPO_COHERENT:
    case   CP_FACE_SMOOTH:
    case   CP_VERTEX_TO_FACE:
				return MeshModel::MM_FACECOLOR;
    case   CP_BORDER:
    case   CP_COLOR_NON_MANIFOLD_VERTEX:
    case   CP_COLOR_NON_MANIFOLD_FACE:
    case   CP_MAP_QUALITY_INTO_COLOR:
    case   CP_FACE_TO_VERTEX:
    case   CP_VERTEX_SMOOTH:
	  case   CP_TEXTURE_TO_VERTEX:
    case   CP_TEXBORDER: 
				return MeshModel::MM_VERTCOLOR;

		case   CP_DISCRETE_CURVATURE:
			return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTNUMBER;

    default: assert(0);
	}
	return MeshModel::MM_UNKNOWN;
}


Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
