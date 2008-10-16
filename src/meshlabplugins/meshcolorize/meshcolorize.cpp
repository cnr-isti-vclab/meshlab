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
#include "curvature.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/update/quality.h>


using namespace std;
using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
    typeList << 
    CP_MAP_QUALITY_INTO_COLOR <<
    CP_GAUSSIAN <<
    CP_MEAN <<
    CP_RMS <<
    CP_ABSOLUTE <<
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

const QString ExtraMeshColorizePlugin::filterName(FilterIDType c) {
  switch(c)
  {
    case CP_MAP_QUALITY_INTO_COLOR:   return QString("Colorize by Quality");
    case CP_GAUSSIAN:                 return QString("Gaussian Curvature (equalized)");
    case CP_MEAN:                     return QString("Mean Curvature (equalized)");
    case CP_RMS:                      return QString("Root mean square Curvature (equalized)");
    case CP_ABSOLUTE:                 return QString("Absolute Curvature (equalized)");
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
const QString ExtraMeshColorizePlugin::filterInfo(FilterIDType filterId) 
{
  switch(filterId)
  {
    case CP_MAP_QUALITY_INTO_COLOR : return tr("Colorize vertex and faces depending on quality field (manually equalized).");
    case CP_GAUSSIAN :               return tr("Colorize vertex and faces depending on equalized gaussian curvature.");
    case CP_MEAN :                   return tr("Colorize vertex and faces depending on equalized mean curvature.");
    case CP_RMS :                    return tr("Colorize vertex and faces depending on equalized root mean square curvature.");
    case CP_ABSOLUTE :               return tr("Colorize vertex and faces depending on equalize absolute curvature.");
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

const int ExtraMeshColorizePlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case CP_GAUSSIAN:                 
    case CP_MEAN:                     
    case CP_RMS:                      
    case CP_ABSOLUTE:                 return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_CURV;
    case CP_TRIANGLE_QUALITY:         return MeshModel::MM_FACECOLOR;
    case CP_SELFINTERSECT_SELECT:
		case CP_SELFINTERSECT_COLOR:
		            return MeshModel::MM_FACEMARK | MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR;
    case CP_BORDER:             return MeshModel::MM_BORDERFLAG;
    case CP_TEXBORDER:                   return MeshModel::MM_FACETOPO;
    case CP_COLOR_NON_MANIFOLD_FACE:       
    case CP_COLOR_NON_MANIFOLD_VERTEX:       return MeshModel::MM_FACETOPO;
    case CP_RANDOM_FACE:       return MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR;
    case CP_MAP_QUALITY_INTO_COLOR:   return 0;
    case CP_VERTEX_SMOOTH:                   return 0;
    case CP_FACE_SMOOTH:                   return MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR ;
    case CP_VERTEX_TO_FACE:                   return MeshModel::MM_FACECOLOR;
    case CP_FACE_TO_VERTEX:                   return MeshModel::MM_VERTCOLOR;
	case CP_TEXTURE_TO_VERTEX:                   return MeshModel::MM_FACECOLOR;

    default: assert(0);
  }
  return 0;
}

// this function is called to fill the parameter list 
// It is called only for filters that have a not empty list of parameters and 
// that do not use the autogenerated dialog, but want a personalized dialog.
bool ExtraMeshColorizePlugin::getCustomParameters(QAction *action, QWidget * parent, MeshModel &m, FilterParameterSet & par, MainWindowInterface *mw) 
{
	switch(ID(action))
  {
    case CP_MAP_QUALITY_INTO_COLOR :
		{
      Histogramf H;
      tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
			
      Frange mmmq(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm));
      eqSettings.meshMinQ = mmmq.minV;
      eqSettings.meshMaxQ = mmmq.maxV;
			
      eqSettings.histoMinQ = H.Percentile(eqSettings.percentile/100);
      eqSettings.histoMaxQ = H.Percentile(1.0f-eqSettings.percentile/100);
			
      EqualizerDialog eqdialog(parent);
      eqdialog.setValues(eqSettings);
			
      if (eqdialog.exec()!=QDialog::Accepted) 
        return false;
			
      Frange FinalRange;
      eqSettings=eqdialog.getValues();
      if (eqSettings.useManual) 
        FinalRange = Frange(eqSettings.manualMinQ,eqSettings.manualMaxQ);
			else
			{
        FinalRange.minV=H.Percentile(eqSettings.percentile/100.0);
        FinalRange.maxV=H.Percentile(1.0f-(eqSettings.percentile/100.0));
			}
			
      par.addFloat("RangeMin",FinalRange.minV);
      par.addFloat("RangeMax",FinalRange.maxV);      
			mw->executeFilter(action,par, false);

			return true;
		}
		
		default :assert(0);
	}
	return true;
}

void ExtraMeshColorizePlugin::initParameterSet(QAction *a,MeshModel &m, FilterParameterSet & par) {
	switch(ID(a))
  {
		case CP_FACE_SMOOTH: 
		case CP_VERTEX_SMOOTH: 
			par.addInt("iteration",1,tr("Iteration"),tr("the number ofiteration of the smoothing algorithm"));
								 break;
	case CP_TRIANGLE_QUALITY: {
			QStringList metrics;
			metrics.push_back("area/max side");
			metrics.push_back("inradius/circumradius");
			metrics.push_back("mean ratio");
			par.addEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality."));
/// transformation matrix into a regular simplex"));

//			par.addEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality."));
			break;
		}
	default: assert(0);
	}
}

bool ExtraMeshColorizePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
 switch(ID(filter)) {
  case CP_MAP_QUALITY_INTO_COLOR :
    {
      float RangeMin = par.getFloat("RangeMin");	
      float RangeMax = par.getFloat("RangeMax");		
      tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,RangeMin,RangeMax);
      break;
    }
  case CP_GAUSSIAN:
  case CP_MEAN:
  case CP_RMS:
  case CP_ABSOLUTE:
    {
			if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
				errorMessage = "Mesh has some not 2-manifold faces, Curvature computation requires manifoldness"; // text
				return false; // can't continue, mesh can't be processed
			}
			
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log(GLLogStream::Info, "Pre-Curvature Cleaning: Removed %d unreferenced vertices",delvert);
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			tri::UpdateCurvature<CMeshO>::MeanAndGaussian(m.cm);
      //Curvature<CMeshO> c(m.cm);
      switch (ID(filter)){
          case CP_GAUSSIAN: tri::UpdateQuality<CMeshO>::VertexFromGaussianCurvature(m.cm);    break;
          case CP_MEAN:     tri::UpdateQuality<CMeshO>::VertexFromMeanCurvature(m.cm);         break;
          case CP_RMS:      tri::UpdateQuality<CMeshO>::VertexFromRMSCurvature(m.cm);         break;
          case CP_ABSOLUTE: tri::UpdateQuality<CMeshO>::VertexFromAbsoluteCurvature(m.cm);         break;
      }      
      
      Histogramf H;
      tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
      tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,H.Percentile(0.1f),H.Percentile(0.9f));
      Log(GLLogStream::Info, "Curvature Range: %f %f (Used 90 percentile %f %f) ",H.MinV(),H.MaxV(),H.Percentile(0.1f),H.Percentile(0.9f));
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

const MeshFilterInterface::FilterClass ExtraMeshColorizePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case   CP_BORDER:
    case   CP_TEXBORDER:
    case   CP_COLOR_NON_MANIFOLD_VERTEX:
    case   CP_COLOR_NON_MANIFOLD_FACE:
    case   CP_MAP_QUALITY_INTO_COLOR:
    case   CP_GAUSSIAN:
    case   CP_MEAN:
    case   CP_RMS:
    case   CP_ABSOLUTE:
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
	switch(ID(a)) {
  case  CP_TRIANGLE_QUALITY: 
	 case   CP_VERTEX_SMOOTH:
   case  CP_FACE_SMOOTH:return true;
    
	default: return false;
  }
}

Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
