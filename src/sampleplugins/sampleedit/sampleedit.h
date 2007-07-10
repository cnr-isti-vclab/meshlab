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

#ifndef SAMPLEEDITPLUGIN_H
#define SAMPLEEDITPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class SampleEditPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
  QList <QAction *> actionList;
		
public:
    SampleEditPlugin();
    virtual ~SampleEditPlugin() {}

    virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();

    virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    virtual void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    virtual void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		virtual QList<QAction *> actions() const ;

    QPoint cur;
		QFont qFont;
    bool haveToPick;
		CMeshO::FacePointer curFacePtr;

};

#endif
