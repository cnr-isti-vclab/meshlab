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

#include "meshcolorize.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/parametrization/distortion.h>



using namespace std;
using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
    typeList << 
    CP_CLAMP_QUALITY <<
    CP_SATURATE_QUALITY <<
    CP_MAP_VQUALITY_INTO_COLOR <<
    CP_MAP_FQUALITY_INTO_COLOR <<
    CP_DISCRETE_CURVATURE <<
    CP_TRIANGLE_QUALITY <<
    CP_VERTEX_SMOOTH <<
		CP_FACE_SMOOTH <<
		CP_VERTEX_TO_FACE <<
		CP_FACE_TO_VERTEX <<
		CP_TEXTURE_TO_VERTEX <<
    CP_RANDOM_FACE <<
    CP_RANDOM_CONNECTED_COMPONENT;
    
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);
}

QString ExtraMeshColorizePlugin::filterName(FilterIDType c) const{
  switch(c){
  case CP_CLAMP_QUALITY:             return QString("Clamp Vertex Quality");
  case CP_SATURATE_QUALITY:          return QString("Saturate Vertex Quality");
  case CP_MAP_VQUALITY_INTO_COLOR:   return QString("Colorize by vertex Quality");
  case CP_MAP_FQUALITY_INTO_COLOR:   return QString("Colorize by face Quality");
  case CP_DISCRETE_CURVATURE:        return QString("Discrete Curvatures");
  case CP_TRIANGLE_QUALITY:          return QString("Per Face Quality according to Triangle shape and aspect ratio");
  case CP_COLOR_NON_TOPO_COHERENT:   return QString("Color edges topologically non coherent");
  case CP_VERTEX_SMOOTH:             return QString("Smooth: Laplacian Vertex Color");
  case CP_FACE_SMOOTH:               return QString("Smooth: Laplacian Face Color");
  case CP_VERTEX_TO_FACE:            return QString("Transfer Color: Vertex to Face");
  case CP_FACE_TO_VERTEX:            return QString("Transfer Color: Face to Vertex");
  case CP_TEXTURE_TO_VERTEX:         return QString("Transfer Color: Texture to Vertex");
  case CP_RANDOM_FACE:               return QString("Random Face Color");
  case CP_RANDOM_CONNECTED_COMPONENT:return QString("Random Component Color ");
  default: assert(0);
  }
  return QString("error!");
}
QString ExtraMeshColorizePlugin::filterInfo(FilterIDType filterId) const {
  switch(filterId){
  case CP_CLAMP_QUALITY:             return QString("Clamp vertex quality values to a given range according to specific values or to percentiles");
  case CP_SATURATE_QUALITY:          return QString("Saturate vertex quality, so that for each vertex the gradient of the quality is lower than the given threshold value (in absolute value)\n"
                                                    "The saturation is done in a conservative way (quality is always decreased and never increased)");
  case CP_MAP_VQUALITY_INTO_COLOR :  return QString("Color vertices depending on their quality field (manually equalized).");
  case CP_MAP_FQUALITY_INTO_COLOR :  return QString("Color faces depending on their quality field (manually equalized).");
  case CP_DISCRETE_CURVATURE :       return QString("Colorize according to various discrete curvature computed as described in:<br>"
                                                   "'<i>Discrete Differential-Geometry Operators for Triangulated 2-Manifolds</i>' <br>"
                                                   "M. Meyer, M. Desbrun, P. Schroder, A. H. Barr");
  case CP_TRIANGLE_QUALITY:         return QString("Compute a quality and colorize faces depending on triangle quality:<br>"
                                                   "1: minimum ratio height/edge among the edges<br>"
                                                   "2: ratio between radii of incenter and circumcenter<br>"
                                                   "3: 2*sqrt(a, b)/(a+b), a, b the eigenvalues of M^tM, M transform triangle into equilateral");
  case CP_VERTEX_SMOOTH:            return QString("Laplacian Smooth Vertex Color");
  case CP_FACE_SMOOTH:              return QString("Laplacian Smooth Face Color");
  case CP_VERTEX_TO_FACE:           return QString("Vertex to Face color transfer");
  case CP_FACE_TO_VERTEX:           return QString("Face to Vertex color transfer");
  case CP_TEXTURE_TO_VERTEX:        return QString("Texture to Vertex color transfer");
  case CP_COLOR_NON_TOPO_COHERENT : return QString("Color edges topologically non coherent.");
  case CP_RANDOM_FACE:              return QString("Colorize Faces randomly. If internal edges are present they are used. Useful for quads.");
  case CP_RANDOM_CONNECTED_COMPONENT:  return QString("Colorize each connected component randomly.");
  default: assert(0); return QString("");
  }
}

void ExtraMeshColorizePlugin::initParameterSet(QAction *a, MeshModel &m, RichParameterSet & par){
  // variables cannot be defined within switch statement
  QStringList metrics;
  QStringList curvNameList;
  pair<float,float> minmax;

  switch(ID(a)){
  case CP_FACE_SMOOTH:
  case CP_VERTEX_SMOOTH:
    par.addParam(new RichInt("iteration",1,QString("Iteration"),QString("the number ofiteration of the smoothing algorithm")));
    break;
  case CP_TRIANGLE_QUALITY:
            metrics.push_back("area/max side");
			metrics.push_back("inradius/circumradius");
			metrics.push_back("mean ratio");
			metrics.push_back("Area");
			metrics.push_back("Texture Angle Distortion");
			metrics.push_back("Texture Area Distortion");
			par.addParam(new RichEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality.")));
			break;
  case CP_DISCRETE_CURVATURE:
			curvNameList.push_back("Mean Curvature");
			curvNameList.push_back("Gaussian Curvature");
			curvNameList.push_back("RMS Curvature");
			curvNameList.push_back("ABS Curvature");
      par.addParam(new RichEnum("CurvatureType", 0, curvNameList, tr("Type:"),
                                QString("Choose the curvatures. Mean and Gaussian curvature are computed according the technique described in the Desbrun et al. paper.<br>"
                                        "Absolute curvature is defined as |H|+|K| and RMS curvature as sqrt(4* H^2 - 2K) as explained in <br><i>Improved curvature estimation"
                                        "for watershed segmentation of 3-dimensional meshes </i> by S. Pulla, A. Razdan, G. Farin. ")));
      break;
  case CP_SATURATE_QUALITY:
      par.addParam(new RichFloat("gradientThr",1,"Gradient Threshold","The maximum value admitted for the quality gradient (in absolute valu)"));
      par.addParam(new RichBool("updateColor",false,"Update ColorMap","if true the color ramp is computed again"));

      break;

  case CP_CLAMP_QUALITY:
  case CP_MAP_VQUALITY_INTO_COLOR:
      minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
			par.addParam(new RichFloat("minVal",minmax.first,"Min","The value that will be mapped with the lower end of the scale (blue)"));
			par.addParam(new RichFloat("maxVal",minmax.second,"Max","The value that will be mapped with the upper end of the scale (red)"));
			par.addParam(new RichDynamicFloat("perc",0,0,100,"Percentile Crop [0..100]","If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to <i>P</i> the value <i>V</i> for which <i>P</i>% of the vertices have a quality <b>lower</b>(greater) than <i>V</i> is used as min (max) value.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
			par.addParam(new RichBool("zeroSym",false,"Zero Simmetric","If true the min max range will be enlarged to be symmertic (so that green is always Zero)"));
      break;
  case CP_MAP_FQUALITY_INTO_COLOR:
      minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m.cm);
      par.addParam(new RichFloat("minVal",minmax.first,"Min","The value that will be mapped with the lower end of the scale (blue)"));
      par.addParam(new RichFloat("maxVal",minmax.second,"Max","The value that will be mapped with the upper end of the scale (red)"));
      par.addParam(new RichDynamicFloat("perc",0,0,100,"Percentile Crop [0..100]","If not zero this value will be used for a percentile cropping of the quality values.<br> If this parameter is set to <i>P</i> the value <i>V</i> for which <i>P</i>% of the vertices have a quality <b>lower</b>(greater) than <i>V</i> is used as min (max) value.<br><br> The automated percentile cropping is very useful for automatically discarding outliers."));
      par.addParam(new RichBool("zeroSym",false,"Zero Simmetric","If true the min max range will be enlarged to be symmertic (so that green is always Zero)"));
      break;
  }
}

bool ExtraMeshColorizePlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb){
 MeshModel &m=*(md.mm());
 switch(ID(filter)) {
  case CP_SATURATE_QUALITY:{
     m.updateDataMask(MeshModel::MM_VERTFACETOPO);
     tri::UpdateQuality<CMeshO>::VertexSaturate(m.cm, par.getFloat("gradientThr"));
     if(par.getBool("updateColor"))
     {
       Histogramf H;
       tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
       m.updateDataMask(MeshModel::MM_VERTCOLOR);
       tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,H.Percentile(0.1f),H.Percentile(0.9f));
     }
     Log("Saturated ");
   }
break;
  case CP_MAP_VQUALITY_INTO_COLOR:
     m.updateDataMask(MeshModel::MM_VERTCOLOR);
  case CP_CLAMP_QUALITY:
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
        if(ID(filter)==CP_CLAMP_QUALITY) tri::UpdateQuality<CMeshO>::VertexClamp(m.cm,PercLo,PercHi);
                                    else tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,PercLo,PercHi);
        Log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ",H.MinV(),H.MaxV(),PercLo,PercHi,par.getDynamicFloat("perc"),100-par.getDynamicFloat("perc"));
      } else {
        if(ID(filter)==CP_CLAMP_QUALITY) tri::UpdateQuality<CMeshO>::VertexClamp(m.cm,RangeMin,RangeMax);
                                    else tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,RangeMin,RangeMax);
        Log("Quality Range: %f %f; Used (%f %f)",H.MinV(),H.MaxV(),RangeMin,RangeMax);
      }
      break;
    }
   case CP_MAP_FQUALITY_INTO_COLOR: {
       m.updateDataMask(MeshModel::MM_FACECOLOR);
       float RangeMin = par.getFloat("minVal");
       float RangeMax = par.getFloat("maxVal");
       float perc = par.getDynamicFloat("perc");

       Histogramf H;
       tri::Stat<CMeshO>::ComputePerFaceQualityHistogram(m.cm,H);
       float PercLo = H.Percentile(perc/100.f);
       float PercHi = H.Percentile(1.0-perc/100.f);

       // Make the range and percentile symmetric w.r.t. zero, so that
       // the value zero is always colored in yellow
       if(par.getBool("zeroSym")){
         RangeMin = min(RangeMin, -math::Abs(RangeMax));
         RangeMax = max(math::Abs(RangeMin), RangeMax);
         PercLo = min(PercLo, -math::Abs(PercHi));
         PercHi = max(math::Abs(PercLo), PercHi);
       }

       tri::UpdateColor<CMeshO>::FaceQualityRamp(m.cm,PercLo,PercHi);
       Log("Quality Range: %f %f; Used (%f %f) percentile (%f %f) ",
           H.MinV(), H.MaxV(), PercLo, PercHi, perc, 100-perc);
       break;
   }

  case CP_DISCRETE_CURVATURE:
    {
      m.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCURV);
      m.updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
      tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);

      if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0) {
				errorMessage = "Mesh has some not 2-manifold faces, Curvature computation requires manifoldness"; // text
				return false; // can't continue, mesh can't be processed
			}
			
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log("Pre-Curvature Cleaning: Removed %d unreferenced vertices",delvert);
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			tri::UpdateCurvature<CMeshO>::MeanAndGaussian(m.cm);
      int curvType = par.getEnum("CurvatureType");
			
			switch(curvType){ 
          case 0: tri::UpdateQuality<CMeshO>::VertexFromMeanCurvature(m.cm);        Log( "Computed Mean Curvature");      break;
			    case 1: tri::UpdateQuality<CMeshO>::VertexFromGaussianCurvature(m.cm);    Log( "Computed Gaussian Curvature"); break;
          case 2: tri::UpdateQuality<CMeshO>::VertexFromRMSCurvature(m.cm);         Log( "Computed RMS Curvature"); break;
          case 3: tri::UpdateQuality<CMeshO>::VertexFromAbsoluteCurvature(m.cm);    Log( "Computed ABS Curvature"); break;
					default : assert(0);
      }      
      
      Histogramf H;
      tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
      tri::UpdateColor<CMeshO>::VertexQualityRamp(m.cm,H.Percentile(0.1f),H.Percentile(0.9f));
      Log( "Curvature Range: %f %f (Used 90 percentile %f %f) ",H.MinV(),H.MaxV(),H.Percentile(0.1f),H.Percentile(0.9f));
    break;
    }  
   case CP_TRIANGLE_QUALITY:
   {
     m.updateDataMask(MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY);
     CMeshO::FaceIterator fi;
     Distribution<float> distrib;
     float minV = 0;
     float maxV = 1.0;
     int metric = par.getEnum("Metric");
     if(metric ==4 || metric ==5 )
     {
       if(!m.hasDataMask(MeshModel::MM_VERTTEXCOORD) && !m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
       {
         this->errorMessage = "This metric need Texture Coordinate";
         return false;
       }
     }
     switch(metric){

       case 0: { //area / max edge
         minV = 0;
         maxV = sqrt(3.0f)/2.0f;
         for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = vcg::Quality((*fi).P(0), (*fi).P(1),(*fi).P(2));
       } break;

       case 1: { //inradius / circumradius
         for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = vcg::QualityRadii((*fi).P(0), (*fi).P(1), (*fi).P(2));
       } break;

       case 2: { //mean ratio
         for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = vcg::QualityMeanRatio((*fi).P(0), (*fi).P(1), (*fi).P(2));
       } break;

       case 3: { // AREA
         for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
           (*fi).Q() = vcg::DoubleArea((*fi))*0.5f;
         tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m.cm,minV,maxV);
       } break;

       case 4: { //TEXTURE Angle Distortion
         if(m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
         {
           for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = Distortion<CMeshO,true>::AngleDistortion(&*fi);
         } else {
           for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = Distortion<CMeshO,false>::AngleDistortion(&*fi);
         }
         tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m.cm,distrib);
         minV = distrib.Percentile(0.05);
         maxV = distrib.Percentile(0.95);
       } break;

       case 5: { //TEXTURE Area Distortion
         float areaScaleVal, edgeScaleVal;
         if(m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
         {
           Distortion<CMeshO,true>::MeshScalingFactor(m.cm, areaScaleVal,edgeScaleVal);
           for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = Distortion<CMeshO,true>::AreaDistortion(&*fi,areaScaleVal);
         } else {
           Distortion<CMeshO,false>::MeshScalingFactor(m.cm, areaScaleVal,edgeScaleVal);
           for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
             (*fi).Q() = Distortion<CMeshO,false>::AreaDistortion(&*fi,areaScaleVal);

         }
         tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m.cm,distrib);
         minV = distrib.Percentile(0.05);
         maxV = distrib.Percentile(0.95);
       } break;

       default: assert(0);
     }
     tri::UpdateColor<CMeshO>::FaceQualityRamp(m.cm,minV,maxV,false);
     break;
   }


  case CP_RANDOM_CONNECTED_COMPONENT:
   m.updateDataMask(MeshModel::MM_FACEFACETOPO);
   m.updateDataMask(MeshModel::MM_FACEMARK | MeshModel::MM_FACECOLOR);
   vcg::tri::UpdateColor<CMeshO>::FaceRandomConnectedComponent(m.cm);
   break;

 case CP_RANDOM_FACE:
     m.updateDataMask(MeshModel::MM_FACEFACETOPO);
     m.updateDataMask(MeshModel::MM_FACEMARK | MeshModel::MM_FACECOLOR);
    vcg::tri::UpdateColor<CMeshO>::MultiFaceRandom(m.cm);
    break;

  case CP_VERTEX_SMOOTH:
		{
		int iteration = par.getInt("iteration");
		tri::Smooth<CMeshO>::VertexColorLaplacian(m.cm,iteration,false,cb);
		}
		break;
  case CP_FACE_SMOOTH:
		{
    m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		int iteration = par.getInt("iteration");
		tri::Smooth<CMeshO>::FaceColorLaplacian(m.cm,iteration,false,cb);
		}
		break;
  case CP_FACE_TO_VERTEX:
     m.updateDataMask(MeshModel::MM_VERTCOLOR);
     tri::UpdateColor<CMeshO>::VertexFromFace(m.cm);
		break;
	 case CP_VERTEX_TO_FACE:
     m.updateDataMask(MeshModel::MM_FACECOLOR);
     tri::UpdateColor<CMeshO>::FaceFromVertex(m.cm);
		 break;
  case CP_TEXTURE_TO_VERTEX:
		{
      m.updateDataMask(MeshModel::MM_VERTCOLOR);
      if(!HasPerWedgeTexCoord(m.cm)) break;
			CMeshO::FaceIterator fi; 
			QImage tex(m.cm.textures[0].c_str());
			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD()) 
			{
				for (int i=0; i<3; i++)
				{
          // note the trick for getting only the fractional part of the uv with the correct wrapping (e.g. 1.5 -> 0.5 and -0.3 -> 0.7)
          vcg::Point2f newcoord((*fi).WT(i).P().X()-floor((*fi).WT(i).P().X()),(*fi).WT(i).P().Y()-floor((*fi).WT(i).P().Y()));
          QRgb val = tex.pixel(newcoord[0]*tex.width(),(1-newcoord[1])*tex.height()-1);
					(*fi).V(i)->C().SetRGB(qRed(val),qGreen(val),qBlue(val));
				}
			}
	    }
		
		break;
 }
	return true;
}

MeshFilterInterface::FilterClass ExtraMeshColorizePlugin::getClass(QAction *a){
  switch(ID(a)){
  case   CP_SATURATE_QUALITY:
  case   CP_CLAMP_QUALITY:
    return MeshFilterInterface::Quality;

  case   CP_MAP_VQUALITY_INTO_COLOR:
  case   CP_DISCRETE_CURVATURE:
  case   CP_COLOR_NON_TOPO_COHERENT:
  case   CP_VERTEX_SMOOTH:
  case   CP_FACE_TO_VERTEX:
  case   CP_TEXTURE_TO_VERTEX:
    return MeshFilterInterface::VertexColoring;

  case   CP_TRIANGLE_QUALITY:
    return FilterClass(Quality + FaceColoring);

  case   CP_RANDOM_FACE:
  case   CP_RANDOM_CONNECTED_COMPONENT:
  case   CP_FACE_SMOOTH:
  case   CP_VERTEX_TO_FACE:
  case   CP_MAP_FQUALITY_INTO_COLOR:
    return MeshFilterInterface::FaceColoring;

  default:
    assert(0);
	return MeshFilterInterface::Generic;
  }
}

int ExtraMeshColorizePlugin::getPreConditions(QAction *a) const{
  switch(ID(a)){
  case CP_TRIANGLE_QUALITY:
  case CP_RANDOM_FACE:
  case CP_RANDOM_CONNECTED_COMPONENT:
  case CP_DISCRETE_CURVATURE:
  case CP_COLOR_NON_TOPO_COHERENT:
    return MeshModel::MM_FACENUMBER;
  case CP_SATURATE_QUALITY:
  case CP_CLAMP_QUALITY:
  case CP_MAP_VQUALITY_INTO_COLOR:
    return MeshModel::MM_VERTQUALITY;
  case CP_MAP_FQUALITY_INTO_COLOR:
    return MeshModel::MM_FACEQUALITY;
  case CP_FACE_TO_VERTEX:
  case CP_FACE_SMOOTH:
    return MeshModel::MM_FACECOLOR;
  case   CP_VERTEX_SMOOTH:
  case   CP_VERTEX_TO_FACE:
    return MeshModel::MM_VERTCOLOR;
  case   CP_TEXTURE_TO_VERTEX:
    return MeshModel::MM_NONE; // TODO: wrong? compare with original
  default: assert(0);
	  return MeshModel::MM_NONE;
  }
}

int ExtraMeshColorizePlugin::postCondition( QAction* a ) const{
  switch(ID(a)){
  case CP_TRIANGLE_QUALITY:
    return MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY;
  case CP_RANDOM_FACE:
  case CP_RANDOM_CONNECTED_COMPONENT:
  case CP_COLOR_NON_TOPO_COHERENT:
  case CP_FACE_SMOOTH:
  case CP_VERTEX_TO_FACE:
  case CP_MAP_FQUALITY_INTO_COLOR:
    return MeshModel::MM_FACECOLOR;
  case CP_MAP_VQUALITY_INTO_COLOR:
  case CP_FACE_TO_VERTEX:
  case CP_VERTEX_SMOOTH:
  case CP_TEXTURE_TO_VERTEX:
    return MeshModel::MM_VERTCOLOR;
  case CP_DISCRETE_CURVATURE:
    return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTNUMBER;
  case CP_CLAMP_QUALITY:
  case CP_SATURATE_QUALITY:
    return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;
  default: assert(0);
     return MeshModel::MM_NONE;
	}
}

Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
