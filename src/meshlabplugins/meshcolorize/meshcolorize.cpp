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
#include <vcg/complex/trimesh/update/flag.h>
#include "meshcolorize.h"
#include "color_manifold.h"
#include "color_curvature.h"

using namespace vcg;

ExtraMeshColorizePlugin::ExtraMeshColorizePlugin() {
	actionList << new QAction(ST(CP_GAUSSIAN), this);
  actionList << new QAction(ST(CP_MEAN), this);
  actionList << new QAction(ST(CP_RMS), this);
  actionList << new QAction(ST(CP_ABSOLUTE), this);
  actionList << new QAction(ST(CP_SELFINTERSECT), this);
  actionList << new QAction(ST(CP_BORDER), this);
  actionList << new QAction(ST(CP_COLORNM), this);
}
const QString ExtraMeshColorizePlugin::ST(ColorizeType c) {
  switch(c)
  {
    case CP_GAUSSIAN: 
      return QString("Gaussian Curvature");
    case CP_MEAN: 
      return QString("Mean Curvature");
    case CP_RMS: 
      return QString("Root mean square Curvature");
    case CP_ABSOLUTE: 
      return QString("Absolute Curvature");
    case CP_SELFINTERSECT: 
      return QString("Self Intersections");
    case CP_BORDER: 
      return QString("Border");
    case CP_COLORNM: 
      return QString("Color non Manifold");
    default: assert(0);
  }
  return QString("error!");
}
const ActionInfo &ExtraMeshColorizePlugin::Info(QAction *action) 
{
	static ActionInfo ai; 
  
	if( action->text() == ST(CP_GAUSSIAN) )
  {
    ai.Help = tr("Colorize vertex and faces depending on gaussian curvature.");
    ai.ShortHelp = tr("Colorize by gaussian curvature");
  }
  if( action->text() == ST(CP_MEAN) )
  {
    ai.Help = tr("Colorize vertex and faces depending on mean curvature.");
    ai.ShortHelp = tr("Colorize by mean curvature");
  }
  if( action->text() == ST(CP_RMS) )
  {
    ai.Help = tr("Colorize vertex and faces depending on root mean square curvature.");
    ai.ShortHelp = tr("Colorize by root mean square curvature");
  }
  if( action->text() == ST(CP_ABSOLUTE) )
  {
    ai.Help = tr("Colorize vertex and faces depending on absolute curvature.");
    ai.ShortHelp = tr("Colorize by absolute curvature");
  }
  if( action->text() == ST(CP_SELFINTERSECT) )
  {
    ai.Help = tr("Colorize only self intersecting faces.");
    ai.ShortHelp = tr("Colorize only self intersecting faces");
  }

  if( action->text() == ST(CP_BORDER) )
  {
    ai.Help = tr("Colorize only border edges.");
    ai.ShortHelp = tr("Colorize only border edges");
  }

  if( action->text() == ST(CP_COLORNM) )
  {
    ai.Help = tr("Colorize only non manifold edges.");
    ai.ShortHelp = tr("Colorize only non manifold edges");
  }

  return ai;
}
const PluginInfo &ExtraMeshColorizePlugin::Info() 
{
  static PluginInfo ai; 
  ai.Date=tr("__DATE__");
  ai.Version = tr("0.5");
  ai.Author = ("Francesco Vannini, Giorgio Gangemi, Andrea Venturi");
  return ai;
}

QList<QAction *> ExtraMeshColorizePlugin::actions() const {
	return actionList;
}
void ExtraMeshColorizePlugin::Compute(QAction * mode, MeshModel &m, RenderMode &rm, GLArea *parent){
	if(mode->text() == ST(CP_GAUSSIAN))
    {
      ColorGaussian<CMeshO>(m.cm, log);
      vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

	if(mode->text() == ST(CP_MEAN))
    {
      ColorMean<CMeshO>(m.cm, log);
      vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == ST(CP_RMS))
    {
      ColorRMS<CMeshO>(m.cm, log);
      vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == ST(CP_ABSOLUTE))
    {
      ColorAbsolute<CMeshO>(m.cm, log);
      vcg::tri::UpdateColor<CMeshO>::VertexQuality(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == ST(CP_SELFINTERSECT))
    {
      vector<CFaceO *> IntersFace;
      tri::Clean<CMeshO>::SelfIntersections(m.cm,IntersFace);
      
      vector<CFaceO *>::iterator fpi;
      for(fpi=IntersFace.begin();fpi!=IntersFace.end();++fpi)
        (*fpi)->C()=Color4b::Red;
      
      rm.colorMode = GLW::CMPerFace;
      return;
    }

  if(mode->text() == ST(CP_BORDER))
    {
      vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
      vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
      vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace (m.cm);
      vcg::tri::UpdateColor<CMeshO>::VertexBorderFlag(m.cm);
      rm.colorMode = GLW::CMPerVert;
      return;
    }

  if(mode->text() == ST(CP_COLORNM))
  {
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
    ColorManifold<CMeshO>(m.cm);
    rm.colorMode = GLW::CMPerVert;
  }
}
Q_EXPORT_PLUGIN(ExtraMeshColorizePlugin)
  
