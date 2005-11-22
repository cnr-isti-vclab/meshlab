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

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QImage>

#include <meshlab/interfaces.h>
#include <meshlab/meshmodel.h>

class MFPlugin_UnifyVertex : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

		QString name;	// Filter Descriptor String
//		int id;				// Filter ID (current position in the Filters Menu)

public:
		MFPlugin_UnifyVertex() : name("Unify Vertexes") {}
		int filterID() const;
    QString filterName() const;
    bool applyFilter(MeshModel &m, QWidget *parent);
};



class MFPlugin_RemoveUnref : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

		QString name;	// Filter Descriptor String
//		int id;							// Filter ID (current position in the Filters Menu)

public:
		MFPlugin_RemoveUnref() : name("Remove Unreferenced Vertexes") {}
		int filterID()const;
    QString filterName() const;
    bool applyFilter(MeshModel &m, QWidget *parent);
};

#endif
