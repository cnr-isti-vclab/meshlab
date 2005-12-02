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

#ifndef DUMMYPLUGIN_H
#define DUMMYPLUGIN_H

#include <QObject>
#include <QAction>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class DummyPlugin : public QObject, public MeshRenderInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshRenderInterface)

	QList <QAction *> actionList;
  
public:
	DummyPlugin()
	{
		actionList << new QAction(QString("action 1"),this);
		actionList << new QAction(QString("action 2"),this);
		actionList << new QAction(QString("action 3"),this);
	}

	QList<QAction *> actions () const {return actionList;}

	void Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla);
};

#endif