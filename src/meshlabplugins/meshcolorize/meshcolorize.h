/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef EXTRACURVATUREPLUGIN_H
#define EXTRACURVATUREPLUGIN_H

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QList>

#include <math.h>
//#include <stdlib.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include <vcg/math/base.h>
#include <vcg/math/histogram.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/color.h>

class MeshColorCurvaturePlugin : public QObject, public MeshColorizeInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshColorizeInterface)

		QList <QAction *> actionList;

public:
    virtual const ActionInfo &Info(QAction *);
    virtual const PluginInfo &Info();
  
		MeshColorCurvaturePlugin()
		{
			QAction *qa; 
			QActionGroup * ag; 


			actionList << new QAction(QString("Gaussian Curvature"),this);
			actionList << new QAction(QString("Self Intersections"),this);
			actionList << new QAction(QString("Border"),this);
			actionList << new QAction(QString("Color non Manifold"),this);
      
		}
    
		void Compute(QAction * mode, MeshModel &m, RenderMode &rm, GLArea *parent);
    
		QList<QAction *> actions() const { return actionList; }

};

#endif
