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
Revision 1.22  2006/12/05 15:37:27  cignoni
Added rough version of non manifold vertex coloring

Revision 1.21  2006/11/29 00:59:17  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.20  2006/11/07 09:10:32  cignoni
Removed shorthHelp and added topo_non_coherent (draft)

Revision 1.19  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.18  2006/02/01 16:23:09  vannini
Added "smooth color" filter

Revision 1.17  2006/01/29 18:59:12  mariolatronico
changed #include "equalizerdialog.h" -> equalizerDialog.h

Revision 1.16  2006/01/27 18:27:53  vannini
code refactoring for curvature colorize
added colorize equalizer dialog and
"Colorize by Quality" filter
some small bugfixes
removed color_curvature.h in favour of curvature.h
****************************************************************************/

#ifndef EXTRACOLORIZEPLUGIN_H
#define EXTRACOLORIZEPLUGIN_H

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include <vcg/math/base.h>
#include <vcg/math/histogram.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/color.h>
#include "equalizerDialog.h"

class ExtraMeshColorizePlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:

    enum  {
      CP_MAP_QUALITY_INTO_COLOR,
      CP_GAUSSIAN,
      CP_MEAN,
      CP_RMS,
      CP_ABSOLUTE,
      CP_SELFINTERSECT,
      CP_BORDER,
      CP_COLOR_NON_MANIFOLD_FACE,
      CP_COLOR_NON_MANIFOLD_VERTEX,
      CP_SMOOTH,
      CP_COLOR_NON_TOPO_COHERENT,
      CP_RESTORE_ORIGINAL
    };

    

    ExtraMeshColorizePlugin();
    ~ExtraMeshColorizePlugin(){};
  
  virtual const QString ST(FilterType filter);
  virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();
  virtual const FilterClass getClass(QAction *);
  virtual bool getParameters(QAction *, QWidget *, MeshModel &m, FilterParameter &par);
  virtual const int getRequirements(QAction *);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameter & /*parent*/, vcg::CallBackPos * cb) ;

protected:
  EqualizerSettings eqSettings;
};

#endif
