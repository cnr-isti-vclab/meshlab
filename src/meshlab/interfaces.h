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

#ifndef MESHLAB_INTERFACES_H
#define MESHLAB_INTERFACES_H

class QImage;
class QPainter;
class QPainterPath;
class QPoint;
class QRect;
class QString;
class QStringList;

class MeshIOInterface
{
public:
    virtual ~MeshIOInterface() {}

    virtual QStringList format() const = 0;
};

class MeshFilterInterface
{
public:
    virtual ~MeshFilterInterface() {}

    virtual QStringList filter() const = 0;
};

class MeshRenderInterface
{
public:
    virtual ~MeshRenderInterface() {}

    virtual QStringList mode() const = 0;
};

Q_DECLARE_INTERFACE(MeshIOInterface,
                    "com.trolltech.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,
                    "com.trolltech.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,
                    "com.trolltech.meshlab.MeshRenderInterface/1.0")

#endif
