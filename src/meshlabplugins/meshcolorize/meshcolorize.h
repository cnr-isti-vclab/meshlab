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

#ifndef EXTRACOLORIZEPLUGIN_H
#define EXTRACOLORIZEPLUGIN_H

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QList>

#include <math.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include <vcg/math/base.h>
#include <vcg/math/histogram.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/color.h>

class ExtraMeshColorizePlugin : public QObject, public MeshColorizeInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshColorizeInterface)

public:

    enum ColorizeType {CP_GAUSSIAN,CP_MEAN,CP_SELFINTERSECT,CP_BORDER,CP_COLORNM};
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

};

#endif
