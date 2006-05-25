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
Revision 1.43  2006/05/25 09:46:37  cignoni
missing std and and all the other gcc detected syntax errors

Revision 1.42  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.41  2006/04/18 06:57:34  zifnab1974
syntax errors for gcc 3.4.5 resolved

Revision 1.40  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.39  2006/02/21 17:25:57  ggangemi
RenderMode is now passed to MeshRenderInterface::Init()

Revision 1.38  2006/02/21 15:59:06  ggangemi
added info() method in meshrender plugin

Revision 1.37  2006/02/16 19:29:14  fmazzant
transfer of Export_3ds.h, Export_obj.h, Io_3ds_obj_material.h from Meshlab to vcg

Revision 1.36  2006/02/15 23:09:06  fmazzant
added the part of MeshIO credits

Revision 1.35  2006/01/19 17:07:51  fmazzant
changed struct Format to class Format(QString, QString).
updated importFormats() and exportFormats() to the new class.

Revision 1.34  2006/01/19 15:58:59  fmazzant
moved savemaskexporter to mainwindows

Revision 1.33  2006/01/17 13:47:45  fmazzant
update interface meshio : formats -> importFormats() & exportFormts

Revision 1.32  2006/01/17 10:04:20  cignoni
Slightly change MeshEditInterface

Revision 1.31  2006/01/13 12:10:30  vannini
Added logging to mean and gaussian curvautres colorization

Revision 1.30  2006/01/04 15:27:30  alemochi
Renamed property of Format struct, and changed plugin dialog

Revision 1.29  2005/12/30 10:51:10  mariolatronico
- added GLLogStream forward declaration
- added method setLog to MeshFilterInterface to remove GLArea dependency on meshfilter plugin

Revision 1.28  2005/12/24 04:16:12  ggangemi
removed "const" from RenderInterface Actions() method

Revision 1.27  2005/12/15 01:15:07  buzzelli
minor changes into MeshIOInterface

Revision 1.26  2005/12/14 22:24:14  cignoni
Added preliminary supprot for editing/selection plugins.

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
#include <QTCore>
#include <QMap>
#include <QPair>
#include <QAction>

class QWidget;
class QGLWidget;
class QIcon;
class QString;
class QVariant;
class QMouseEvent;
class MeshModel;
class RenderMode;
class GLArea;
class GLLogStream;

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

class MeshIOInterface
{
public:
	class Format
	{
	public:
		Format(QString description,QString ex): description(description){extensions << ex;}
		QString description;
		QStringList extensions;
	};

    virtual ~MeshIOInterface() {}
	virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;
	
	virtual QList<Format> importFormats() const = 0;
	virtual QList<Format> exportFormats() const = 0;
	virtual int GetExportMaskCapability(QString &format) const = 0;
    
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
      const int &mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; // prima istanza il dialogo di opzioni viene sempre.
};



class FilterParameter
{
public:

  FilterParameter(){}

  inline bool getBool(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    assert(ii!=paramMap.end());
    assert(ii.value().type()==QVariant::Bool);
    return ii.value().toBool();
  }
  
  inline int getInt(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::Int);
    return ii.value().toInt();
  }

  inline float getFloat(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::Double);
    return float(ii.value().toDouble());
  }
  
  inline Matrix44f getMatrix44(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::List);
    Matrix44f matrix;
    QList<QVariant> matrixVals = ii.value().toList();
    assert(matrixVals.size()==16);
    for(int i=0;i<16;++i)
      matrix.V()[i]=matrixVals[i].toDouble();
    
    return matrix;
  }

  inline void addFloat(QString name,float val){ paramMap.insert(name, QVariant( double(val)) ); }
  inline void addInt  (QString name,float val){ paramMap.insert(name, QVariant(    int(val)) ); }
  inline void addBool (QString name,bool val) { paramMap.insert(name, QVariant(        val )  );  }
  
  inline void addMatrix44(QString name,Matrix44f val) { 
    QList<QVariant> matrixVals;
    for(int i=0;i<16;++i)
        matrixVals.append(val.V()[i]);
    paramMap.insert(name, QVariant(matrixVals)  );  
  }

  inline void clear() { paramMap.clear(); }
private:
  // The data is just a list of Parameters
  QMap<QString,QVariant> paramMap;  
};

class MeshFilterInterface
{
public:
  typedef int FilterType;
  	enum FilterClass { Generic, Selection, Cleaning, Remeshing, FaceColoring, VertexColoring} ;
    virtual ~MeshFilterInterface() {}
		virtual const ActionInfo &Info(QAction *)=0;

    // The filterclass describe in which submenu each filter should be placed 
    virtual const FilterClass getClass(QAction *) {return MeshFilterInterface::Generic;};
    
    // This function invokes a dialog and get back the parameters
    virtual bool getParameters(QAction *, QWidget * /*parent*/, MeshModel &/*m*/, FilterParameter & /*par*/) {return true;};
    
    // The filters can require some additional
    virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}

    // The main function that apply the selected filter 
    virtual bool applyFilter(QAction * /*filter*/, MeshModel &/*m*/, FilterParameter & /*parent*/, vcg::CallBackPos * /*cb*/) = 0;

    virtual const PluginInfo &Info()=0;
	  void setLog(GLLogStream *log) { this->log = log ; }

    virtual const QString ST(FilterType filter)=0;

    virtual const FilterType ID(QAction *a)
  	{
      foreach( FilterType tt, types())
        if( a->text() == this->ST(tt) ) return tt;
          assert(0);
      return 0;
    }
    virtual QList<QAction *> actions() const { return actionList;}
	  virtual QList<FilterType> &types() { return typeList;}

protected:
    QList <QAction *> actionList;
    QList <FilterType> typeList;
    GLLogStream *log;	
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
		
    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/){};
		virtual void Render(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual bool isSupported() = 0;
		virtual const PluginInfo &Info()=0;
		virtual QList<QAction *> actions() const = 0;
};


class MeshDecorateInterface
{
public:
    virtual ~MeshDecorateInterface() {}

    virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/,QFont qf) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual QList<QAction *> actions() const = 0;
};


class MeshEditInterface
{
public:
    virtual ~MeshEditInterface() {}

    virtual const ActionInfo &Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		//virtual void Edit(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, GLArea * /*parent*/) = 0;
		virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) = 0;
    virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	  virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	  virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
//	  virtual void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );
		virtual QList<QAction *> actions() const = 0;
};

Q_DECLARE_INTERFACE(MeshIOInterface,       "vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,   "vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,   "vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshDecorateInterface, "vcg.meshlab.MeshDecorateInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterface,     "vcg.meshlab.MeshEditInterface/1.0")

#endif
