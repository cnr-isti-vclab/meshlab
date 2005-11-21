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
Revision 1.3  2005/11/21 12:07:56  cignoni
Added copyright info


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
class MeshModel;
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
    virtual bool applyFilter(const QString &filter, MeshModel &m, QWidget *parent) = 0;
    virtual QStringList filters() const = 0;
};

class MeshRenderInterface
{
public:
    virtual ~MeshRenderInterface() {}

    virtual QStringList mode() const = 0;
};

Q_DECLARE_INTERFACE(MeshIOInterface,
                    "vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,
                    "vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,
                    "vcg.meshlab.MeshRenderInterface/1.0")

#endif
