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
Revision 1.16  2006/01/27 18:27:53  vannini
code refactoring for curvature colorize
added colorize equalizer dialog and
"Colorize by Quality" filter
some small bugfixes
removed color_curvature.h in favour of curvature.h

Revision 1.15  2006/01/20 18:17:07  vannini
added Restore Color

Revision 1.14  2006/01/20 16:25:39  vannini
Added Absolute Curvature colorize

Revision 1.13  2006/01/20 14:46:44  vannini
Code refactoring
Added RMS Curvature colorize

Revision 1.12  2006/01/13 16:24:16  vannini
Moved gaussian and mean curvature functions into color_curvature.h


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
#include "equalizerdialog.h"

class ExtraMeshColorizePlugin : public QObject, public MeshColorizeInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshColorizeInterface)

public:

    enum ColorizeType {CP_EQUALIZE,CP_GAUSSIAN,CP_MEAN,CP_RMS,CP_ABSOLUTE,CP_SELFINTERSECT,CP_BORDER,CP_COLORNM,CP_RESTORE_ORIGINAL};
    const QString ST(ColorizeType c);

    ExtraMeshColorizePlugin();
    
    virtual const ActionInfo &Info(QAction *);
    virtual const PluginInfo &Info();
    virtual QList<QAction *> actions() const;
		
		void Compute(QAction * mode, MeshModel &m, RenderMode &rm, GLArea *parent);
    void setLog(GLLogStream *log) { this->log = log ; }
		  
protected:
	GLLogStream *log;
  QList <QAction *> actionList;
  EqualizerSettings eqSettings;

};

#endif
