/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History
$Log$
Revision 1.39  2007/11/23 15:35:19  cignoni
disambiguated sqrt call

Revision 1.38  2007/11/20 18:26:52  ponchio
Added triangle quality with possibiliti to cohose metric.

Revision 1.37  2007/11/19 17:08:54  ponchio
Added triangle quality.

Revision 1.36  2007/10/02 08:13:52  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.35  2007/06/19 00:30:53  cignoni
getParameter should return false only in case of aborting a dialog...

Revision 1.34  2007/04/16 09:25:29  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing again...

Revision 1.33  2007/03/20 16:23:09  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.32  2007/02/08 23:46:17  pirosu
merged srcpar and par in the GetStdParameters() function

Revision 1.31  2006/12/11 23:49:32  cignoni
removed unimplemented filter non topo coherent

Revision 1.30  2006/12/05 15:37:27  cignoni
Added rough version of non manifold vertex coloring

Revision 1.29  2006/11/29 00:59:17  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.28  2006/11/07 09:10:32  cignoni
Removed shorthHelp and added topo_non_coherent (draft)

Revision 1.27  2006/06/18 21:26:56  cignoni
range color bug for quality/curvature

Revision 1.26  2006/06/16 01:26:42  cignoni
Debugged Color by quality and default Curvature

Revision 1.25  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.24  2006/02/04 09:41:44  vannini
Better handling of curvature computation for border vertex
Plugin info updated

Revision 1.23  2006/02/01 16:23:09  vannini
Added "smooth color" filter

Revision 1.22  2006/01/31 10:54:28  vannini
curvature<>color mapping now ignores border vertex

Revision 1.21  2006/01/30 17:19:22  vannini
Added manual values to control min and max Q (mapping curvature to color)

Revision 1.20  2006/01/27 18:27:53  vannini
code refactoring for curvature colorize
added colorize equalizer dialog and
"Colorize by Quality" filter
some small bugfixes
removed color_curvature.h in favour of curvature.h

Revision 1.19  2006/01/20 18:17:07  vannini
added Restore Color

Revision 1.18  2006/01/20 16:25:39  vannini
Added Absolute Curvature colorize

Revision 1.17  2006/01/20 14:46:44  vannini
Code refactoring
Added RMS Curvature colorize

Revision 1.16  2006/01/17 23:46:36  cignoni
Moved some include from meshmodel.h to here

Revision 1.15  2006/01/13 16:24:16  vannini
Moved gaussian and mean curvature functions into color_curvature.h

Revision 1.14  2006/01/13 15:22:04  vannini
colorize:
 -colorize nonmanifold now sets colorPerVertex mode
 -code rewritten to follow naming conventions (as in meshfilter)
 -added help and infos

Revision 1.13  2006/01/13 12:10:31  vannini
Added logging to mean and gaussian curvautres colorization

Revision 1.12  2006/01/10 15:38:53  vannini
Added mean curvature colorize

Revision 1.11  2006/01/06 11:15:26  giec
Added color non manifolt filter.

Revision 1.10  2006/01/04 13:27:53  alemochi
Added help in plugin dialog

Revision 1.9  2005/12/23 19:34:09  glvertex
Removed ColorModes (none,pervert,perface)

Revision 1.8  2005/12/19 16:49:14  cignoni
Added SelfIntersection and Border colorization methods

Revision 1.7  2005/12/12 22:48:42  cignoni
Added plugin info methods

Revision 1.6  2005/12/08 22:52:50  cignoni
Added safer min max search

Revision 1.5  2005/12/05 11:37:13  ggangemi
workaround: added rendermode to compute method

Revision 1.4  2005/12/03 23:23:37  ggangemi
new interface

Revision 1.3  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>
#include <limits>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/update/flag.h>
#include "meshcolorize.h"
#include "color_manifold.h"
#include "curvature.h"
#include "smoothcolor.h"
#include <vcg/space/triangle3.h> //for quality

using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
    typeList << 
    CP_MAP_QUALITY_INTO_COLOR <<
    CP_GAUSSIAN <<
    CP_MEAN <<
    CP_RMS <<
    CP_ABSOLUTE <<
    CP_TRIANGLE_QUALITY <<
    CP_SELFINTERSECT <<
    CP_BORDER <<
    CP_COLOR_NON_MANIFOLD_FACE <<
    CP_COLOR_NON_MANIFOLD_VERTEX <<
    CP_SMOOTH <<
    //CP_COLOR_NON_TOPO_COHERENT <<
    CP_RESTORE_ORIGINAL;
    
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
    case CP_SELFINTERSECT:            return QString("Self Intersections");
    case CP_BORDER:                   return QString("Border");
    case CP_COLOR_NON_MANIFOLD_FACE:  return QString("Color non Manifold Faces");
    case CP_COLOR_NON_MANIFOLD_VERTEX:return QString("Color non Manifold Vertices");
    case CP_COLOR_NON_TOPO_COHERENT:  return QString("Color edges topologically non coherent");
    case CP_SMOOTH:                   return QString("Smooth Color");
    case CP_RESTORE_ORIGINAL:         return QString("Restore Color");
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
    case CP_TRIANGLE_QUALITY:        return tr("Colorize faces depending on triangle quality.");
    case CP_SELFINTERSECT:           return tr("Colorize only self intersecting faces.");
    case CP_BORDER :                 return tr("Colorize only border edges.");
    case CP_COLOR_NON_MANIFOLD_FACE: return tr("Colorize the non manifold edges, eg the edges where there are more than two incident faces");
    case CP_COLOR_NON_MANIFOLD_VERTEX:return tr("Colorize only non manifold edges eg. ");
    case CP_SMOOTH :                 return tr("Apply laplacian smooth for colors.");
    case CP_RESTORE_ORIGINAL :       return tr("Restore original per vertex color.");
    case CP_COLOR_NON_TOPO_COHERENT :return tr("Color edges topologically non coherent.");
    default: assert(0); 
  }
  return QString();
}
const PluginInfo &ExtraMeshColorizePlugin::pluginInfo() 
{
  static PluginInfo ai; 
  ai.Date=tr("Oct 2006");
  ai.Version = tr("1.1");
  ai.Author = ("Paolo Cignoni, Francesco Vannini, Giorgio Gangemi, Andrea Venturi");
  return ai;
}


const int ExtraMeshColorizePlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case CP_GAUSSIAN:                 
    case CP_MEAN:                     
    case CP_RMS:                      
    case CP_ABSOLUTE:                 return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG;
    case CP_TRIANGLE_QUALITY:         return MeshModel::MM_FACECOLOR;
    case CP_SELFINTERSECT:            return MeshModel::MM_FACEMARK | MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR;
    case CP_BORDER:                   return MeshModel::MM_BORDERFLAG;
    case CP_COLOR_NON_MANIFOLD_FACE:       
    case CP_COLOR_NON_MANIFOLD_VERTEX:       return MeshModel::MM_FACETOPO;
    case CP_SMOOTH:                   
    case CP_RESTORE_ORIGINAL:         
    case CP_MAP_QUALITY_INTO_COLOR:   return 0;
    default: assert(0);
  }
  return 0;
}

// this function is called to fill the parameter list 
// It is called only for filters that have a not empty list of parameters and 
// that do not use the autogenerated dialog, but want a personalized dialog.
bool ExtraMeshColorizePlugin::getParameters(QAction *action, QWidget * parent, MeshModel &m, FilterParameterSet & par, MainWindowInterface *mw) 
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
			mw->executeFilter(action,par);

			return true;
		}
		
		default :assert(0);
	}
	return true;
}

void ExtraMeshColorizePlugin::initParameterSet(QAction *a,MeshModel &m, FilterParameterSet & par) {
	switch(ID(a))
  {
	case CP_TRIANGLE_QUALITY: {
			QStringList metrics;
			metrics.push_back("area/max side");
			metrics.push_back("inradius/circumradius");
			par.addEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality."));
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
      tri::UpdateColor<CMeshO>::VertexQuality(m.cm,RangeMin,RangeMax);
      break;
    }
  case CP_GAUSSIAN:
  case CP_MEAN:
  case CP_RMS:
  case CP_ABSOLUTE:
    {
      Curvature<CMeshO> c(m.cm);
      switch (ID(filter)){
          case CP_GAUSSIAN: c.MapGaussianCurvatureIntoQuality();    break;
          case CP_MEAN:     c.MapMeanCurvatureIntoQuality();        break;
          case CP_RMS:      c.MapRMSCurvatureIntoQuality();         break;
          case CP_ABSOLUTE: c.MapAbsoluteCurvatureIntoQuality();    break;
      }      
      
      Histogramf H;
      tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
      tri::UpdateColor<CMeshO>::VertexQuality(m.cm,H.Percentile(0.1),H.Percentile(0.9));
      
    break;
    }  
  case CP_TRIANGLE_QUALITY:
    {
			int metric = par.getEnum("Metric");
			if(metric == 0) { //area / max edge
				float min = 0;
				float max = sqrt(3.0)/2.0;
				for(unsigned int i = 0; i < m.cm.face.size(); i++) {
					CFaceO &f = m.cm.face[i];
					f.C().ColorRamp(min, max, Quality(f.P(0), f.P(1), f.P(2)));
				}
			} else { //inradius / circumradius
				float min = 0;
				float max = 0.5;
				for(unsigned int i = 0; i < m.cm.face.size(); i++) {
					CFaceO &f = m.cm.face[i];
					f.C().ColorRamp(min, max, QualityRadii(f.P(0), f.P(1), f.P(2)));
				}
			} 
		break;
    }

  case CP_SELFINTERSECT:
    {
      vector<CFaceO *> IntersFace;
      tri::Clean<CMeshO>::SelfIntersections(m.cm,IntersFace);
      tri::UpdateColor<CMeshO>::FaceConstant(m.cm,Color4b::White);
      vector<CFaceO *>::iterator fpi;
      for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
        (*fpi)->C()=Color4b::Red;
      
    break;  
    }

  case CP_BORDER:
    vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
    break;
  case CP_COLOR_NON_MANIFOLD_FACE:
    ColorManifoldFace<CMeshO>(m.cm);
    break;
  case CP_COLOR_NON_MANIFOLD_VERTEX:
    ColorManifoldVertex<CMeshO>(m.cm);
    break;
  case CP_RESTORE_ORIGINAL:
     m.restoreVertexColor();
     break;
  case CP_SMOOTH:
     LaplacianSmoothColor(m.cm,1);
     break;
 }
	return true;
}

const MeshFilterInterface::FilterClass ExtraMeshColorizePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case   CP_BORDER:
    case   CP_COLOR_NON_MANIFOLD_VERTEX:
    case   CP_COLOR_NON_MANIFOLD_FACE:
    case   CP_SMOOTH:
    case   CP_RESTORE_ORIGINAL:
    case   CP_MAP_QUALITY_INTO_COLOR:
    case   CP_GAUSSIAN:
    case   CP_MEAN:
    case   CP_RMS:
    case   CP_ABSOLUTE:
    case   CP_COLOR_NON_TOPO_COHERENT:
        return MeshFilterInterface::VertexColoring; 
    case   CP_SELFINTERSECT:
    case   CP_TRIANGLE_QUALITY:
               return MeshFilterInterface::FaceColoring; 
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}

bool ExtraMeshColorizePlugin::autoDialog(QAction *a) {
	switch(ID(a)) {
  case  CP_TRIANGLE_QUALITY: return true;
	default: return false;
  }
}

Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
