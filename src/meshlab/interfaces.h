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
Revision 1.8  2005/11/25 00:10:08  glvertex
added Q_DECLARE_INTERFACE for MeshColorizeInterface

Revision 1.7  2005/11/24 10:54:37  cignoni
Yet another test to compile under linux: added a const before QString in applyfilter...

Revision 1.6  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.5  2005/11/23 00:25:06  glvertex
Reverted plugin interface to prev version

Revision 1.4  2005/11/22 17:10:53  glvertex
MeshFilter Plugin STRONGLY reviewed and changed

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
class QIcon;
class QString;
class QStringList;
class MeshModel;
class RenderMode;

class MeshIOInterface
{
public:
    virtual ~MeshIOInterface() {}

    virtual QStringList format() const = 0;
    
    virtual bool open(
      QString &filter, // "OBJ"
      MeshModel &m, 
      int mask,
      CallBackPos *cb=0,
      QWidget *parent=0);
    
  virtual bool save(
      QString &filter, // "OBJ"
      MeshModel &m, 
      int mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0) ; // prima istanza il dialogo di opzioni viene sempre.
};

class MeshFilterInterface
{
public:
    virtual ~MeshFilterInterface() {}
    virtual QIcon *getIcon(QString &filter, QWidget *parent) {return 0;};
    virtual bool applyFilter(const QString &filter, MeshModel &m, QWidget *parent) = 0;
    virtual QStringList filters() const = 0;
};
/*
Serve per customizzare totalmente il processo di rendering
Viene invocata al posto del rendering standard della mesh.
- Con che stato opengl gia settato per quanto riguarda:
    - Matrici proj e model
    - Lighting (dir e tipo luci) 
    - Bf cull ecc e tutto lo stato classico

*/

class MeshRenderInterface
{
public:
    virtual ~MeshRenderInterface() {}

    virtual void Init(    QString &mode, MeshModel &m, QWidget *parent){};
    virtual void Render(  QString &mode, MeshModel &m, RenderMode &rm, QWidget *parent) =0;
    virtual void Finalize(QString &mode, MeshModel &m, QWidget *parent){};

    virtual QStringList modes() const = 0;
};

class MeshColorizeInterface
{
public:
    virtual void Compute( QString &mode, MeshModel &m, QWidget *parent){};
    virtual void Show(QString &mode, bool show, MeshModel &m, QWidget *parent) {};
    virtual void Finalize(QString &mode, MeshModel &m, QWidget *parent){};
    virtual QStringList colorsFrom() const = 0;
};

Q_DECLARE_INTERFACE(MeshIOInterface,
                    "vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,
                    "vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,
                    "vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshColorizeInterface,
                    "vcg.meshlab.MeshColorizeInterface/1.0")

#endif
