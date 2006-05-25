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

using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
    typeList << 
    CP_MAP_QUALITY_INTO_COLOR <<
    CP_GAUSSIAN <<
    CP_MEAN <<
    CP_RMS <<
    CP_ABSOLUTE <<
    CP_SELFINTERSECT <<
    CP_BORDER <<
    CP_COLOR_NON_MANIFOLD <<
    CP_SMOOTH <<
    CP_RESTORE_ORIGINAL;
    
  FilterType tt;
  foreach(tt , types())
	    actionList << new QAction(ST(tt), this);
}

const QString ExtraMeshColorizePlugin::ST(FilterType c) {
  switch(c)
  {
    case CP_MAP_QUALITY_INTO_COLOR:   return QString("Colorize by Quality");
    case CP_GAUSSIAN:                 return QString("Gaussian Curvature (equalized)");
    case CP_MEAN:                     return QString("Mean Curvature (equalized)");
    case CP_RMS:                      return QString("Root mean square Curvature (equalized)");
    case CP_ABSOLUTE:                 return QString("Absolute Curvature (equalized)");
    case CP_SELFINTERSECT:            return QString("Self Intersections");
    case CP_BORDER:                   return QString("Border");
    case CP_COLOR_NON_MANIFOLD:       return QString("Color non Manifold");
    case CP_SMOOTH:                   return QString("Smooth Color");
    case CP_RESTORE_ORIGINAL:         return QString("Restore Color");
    default: assert(0);
  }
  return QString("error!");
}
const ActionInfo &ExtraMeshColorizePlugin::Info(QAction *action) 
{
	static ActionInfo ai; 
  switch(ID(action))
  {
    case CP_MAP_QUALITY_INTO_COLOR :  ai.Help = tr("Colorize vertex and faces depending on quality field (manually equalized).");
                                 ai.ShortHelp = tr("Colorize by quality");break;
    case CP_GAUSSIAN :                ai.Help = tr("Colorize vertex and faces depending on equalized gaussian curvature.");
                                 ai.ShortHelp = tr("Colorize by gaussian curvature");break;
    case CP_MEAN :                    ai.Help = tr("Colorize vertex and faces depending on equalized mean curvature.");
                                 ai.ShortHelp = tr("Colorize by mean curvature");break;
    case CP_RMS :                     ai.Help = tr("Colorize vertex and faces depending on equalized root mean square curvature.");
                                 ai.ShortHelp = tr("Colorize by root mean square curvature");break;
    case CP_ABSOLUTE :                ai.Help = tr("Colorize vertex and faces depending on equalize absolute curvature.");
                                 ai.ShortHelp = tr("Colorize by absolute curvature");break;
    case CP_SELFINTERSECT:            ai.Help = tr("Colorize only self intersecting faces.");
                                 ai.ShortHelp = tr("Colorize only self intersecting faces");break;
    case CP_BORDER :                  ai.Help = tr("Colorize only border edges.");
                                 ai.ShortHelp = tr("Colorize only border edges");break;
    case CP_COLOR_NON_MANIFOLD:       ai.Help = tr("Colorize only non manifold edges.");
                                 ai.ShortHelp = tr("Colorize only non manifold edges");break;
    case CP_SMOOTH :                  ai.Help = tr("Apply laplacian smooth for colors.");
                                 ai.ShortHelp = tr("Laplacian smooth for colors");break;
    case CP_RESTORE_ORIGINAL :        ai.Help = tr("Restore original per vertex color.");
                                 ai.ShortHelp = tr("Restore original per vertex color");  break;
  }
  return ai;
}
const PluginInfo &ExtraMeshColorizePlugin::Info() 
{
  static PluginInfo ai; 
  ai.Date=tr("January 2006");
  ai.Version = tr("1.0");
  ai.Author = ("Francesco Vannini, Giorgio Gangemi, Andrea Venturi");
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
    case CP_SELFINTERSECT:            return MeshModel::MM_FACEMARK | MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR;
    case CP_BORDER:                   return MeshModel::MM_BORDERFLAG;
    case CP_COLOR_NON_MANIFOLD:       return MeshModel::MM_FACETOPO;
    case CP_SMOOTH:                   
    case CP_RESTORE_ORIGINAL:         
    case CP_MAP_QUALITY_INTO_COLOR:   return 0;
    default: assert(0);
  }
  return 0;
}


bool ExtraMeshColorizePlugin::getParameters(QAction *action, QWidget *parent, MeshModel &m,FilterParameter &par)
{
 par.clear();
 switch(ID(action))
  {
    case CP_MAP_QUALITY_INTO_COLOR :
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
        FinalRange.minV=H.Percentile(eqSettings.percentile);
        FinalRange.maxV=H.Percentile(1.0f-eqSettings.percentile);
       }

      par.addFloat("RangeMin",FinalRange.minV);
      par.addFloat("RangeMax",FinalRange.maxV);      
  }
}
bool ExtraMeshColorizePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos *cb)
{
 switch(ID(filter)) {
  case CP_MAP_QUALITY_INTO_COLOR :
    {
//      Curvature<CMeshO> c(m.cm);
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
      
      tri::UpdateColor<CMeshO>::VertexQuality(m.cm,-.1,.1);
    break;
    }
  case CP_SELFINTERSECT:
    {
      vector<CFaceO *> IntersFace;
      tri::Clean<CMeshO>::SelfIntersections(m.cm,IntersFace);
      
      vector<CFaceO *>::iterator fpi;
      for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
        (*fpi)->C()=Color4b::Red;
      
    break;
    }

  case CP_BORDER:
    vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
    break;
  case CP_COLOR_NON_MANIFOLD:
    ColorManifold<CMeshO>(m.cm);
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
    case   CP_COLOR_NON_MANIFOLD:
    case   CP_SMOOTH:
    case   CP_RESTORE_ORIGINAL:
    case   CP_MAP_QUALITY_INTO_COLOR:
    case   CP_GAUSSIAN:
    case   CP_MEAN:
    case   CP_RMS:
    case   CP_ABSOLUTE:
               return MeshFilterInterface::VertexColoring; 
    case   CP_SELFINTERSECT:
               return MeshFilterInterface::FaceColoring; 
    default: assert(0);
              return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
