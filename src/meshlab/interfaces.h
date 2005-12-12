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
Revision 1.25  2005/12/12 22:46:05  cignoni
Cleaned up and added info functions

Revision 1.24  2005/12/09 00:26:25  buzzelli
io importing mechanism adapted in order to be fully transparent towards the user

Revision 1.23  2005/12/07 08:01:09  fmazzant
exporter obj temporany

Revision 1.22  2005/12/05 18:08:21  ggangemi
added MeshRenderInterface::isSupported() method

Revision 1.21  2005/12/05 12:17:09  ggangemi
Added MeshDecorateInterface

Revision 1.20  2005/12/05 11:38:52  ggangemi
workaround: added RenderMode parameter to MeshColorizePlugin::compute

Revision 1.19  2005/12/03 23:50:15  cignoni
changed io interface to return a list instead of a vector

Revision 1.18  2005/12/03 16:05:18  glvertex
solved some compatilbility issues

Revision 1.17  2005/12/02 17:39:07  glvertex
modified plugin import code. old plugins have been disabled cause of new interface.

Revision 1.16  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.15  2005/11/28 15:36:41  mariolatronico
changed again interface on MeshIO (filename not const on save / open)

Revision 1.14  2005/11/28 15:21:07  mariolatronico
added const on various methods to comply C++ standard on reference variable

Revision 1.13  2005/11/27 18:36:58  buzzelli
changed applyImportExport method in order to handle correctly the case of no opened subwindows

Revision 1.12  2005/11/26 23:57:04  cignoni
made io filters to appear into file menu.

Revision 1.11  2005/11/26 23:29:08  cignoni
Commented out names of unused parameters to remove boring warnings

Revision 1.10  2005/11/25 21:57:51  mariolatronico
changed signature of MeshColorizeInterface::Compute to allow gcc compilation

Revision 1.9  2005/11/25 19:29:01  buzzelli
small changes to signature of MeshIOInterface methods

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

class QWidget;
class QIcon;
class QString;
//class QList;
class QAction;
class MeshModel;
class RenderMode;
class GLArea;

class MeshIOInterface
{
public:
    virtual ~MeshIOInterface() {}
    virtual QStringList formats(QString &description) const = 0;
    
 virtual bool open(
      const QString &format, // "OBJ"
			QString &fileName,
      MeshModel &m, 
      int &mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent=0)=0;
    
  virtual bool save(
      const QString &format, // "OBJ"
			QString &fileName,
      MeshModel &m, 
      int &mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; // prima istanza il dialogo di opzioni viene sempre.
};

class ActionInfo
{
public:
  QString Help;
  QString ShortHelp;
};

class PluginInfo
{
public:
  QString Date;
  QString Version;
  QString Author;
};


class MeshFilterInterface
{
public:
    virtual ~MeshFilterInterface() {}
		virtual QList<QAction *> actions() const = 0;
    virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;
    
		virtual bool applyFilter(QAction * /*filter*/, MeshModel &/*m*/, QWidget * /*parent*/, vcg::CallBackPos * /*cb*/) = 0;
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

    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Render(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, GLArea * /*parent*/) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual bool isSupported() = 0;
		virtual QList<QAction *> actions() const = 0;
};

class MeshColorizeInterface
{
public:
    virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;
    
		virtual void Compute(QAction * /*mode*/, MeshModel &/*m*/,  RenderMode &/*rm*/, GLArea * /*parent*/){};    
		virtual void Show(QAction * /*mode*/, bool /*show*/, MeshModel &/*m*/, GLArea * /*parent*/) {};    
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual QList<QAction *> actions() const = 0;
};



class MeshDecorateInterface
{
public:
    virtual ~MeshDecorateInterface() {}

    virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, GLArea * /*parent*/) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual QList<QAction *> actions() const = 0;
};

Q_DECLARE_INTERFACE(MeshIOInterface,       "vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,   "vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,   "vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshColorizeInterface, "vcg.meshlab.MeshColorizeInterface/1.0")
Q_DECLARE_INTERFACE(MeshDecorateInterface, "vcg.meshlab.MeshDecorateInterface/1.0")

#endif
