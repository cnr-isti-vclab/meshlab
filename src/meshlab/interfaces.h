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
Revision 1.55  2007/03/27 12:20:16  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.54  2007/02/28 00:02:12  cignoni
Added missing virtual destructors

Revision 1.53  2007/02/08 23:45:26  pirosu
merged srcpar and par in the GetStdParameters() function

Revision 1.52  2007/01/11 19:51:46  pirosu
fixed bug for QT 4.1.0/dotnet2003
removed the request of the window title to the plugin. The action description is used instead.

Revision 1.51  2006/12/27 21:41:41  pirosu
Added improvements for the standard plugin window:
split of the apply button in two buttons:ok and apply
added support for parameters with absolute and percentage values

Revision 1.50  2006/12/13 17:37:02  pirosu
Added standard plugin window support

Revision 1.49  2006/11/29 00:55:36  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.48  2006/11/07 14:56:23  zifnab1974
Changes for compilation with gcc 3.4.6 on linux AMD64

Revision 1.47  2006/11/07 09:03:17  cignoni
Removed short help
Slightly changed the Decorate interface

Revision 1.46  2006/06/27 08:07:42  cignoni
Restructured plugins interface for simplifying the server

Revision 1.45  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

****************************************************************************/

#ifndef MESHLAB_INTERFACES_H
#define MESHLAB_INTERFACES_H
#include <QtCore>
#include <QAction>
#include "filterparameter.h"
#include "GLLogStream.h"



class QWidget;
class QGLWidget;
class QString;
class QVariant;
class QMouseEvent;
class MeshModel;
class RenderMode;
class GLArea;

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
  virtual const PluginInfo &Info()=0;
	
	virtual QList<Format> importFormats() const = 0;
	virtual QList<Format> exportFormats() const = 0;
	virtual int GetExportMaskCapability(QString &format) const = 0;
    
  virtual bool open(
      const QString &format, // the extension of the format e.g. "PLY"
			QString &fileName,
      MeshModel &m, 
      int &mask,             // what kind of data are found in the file (per vertex color, texture coords etc)
      vcg::CallBackPos *cb=0,
      QWidget *parent=0)=0;
    
  virtual bool save(
      const QString &format, // the extension of the format e.g. "PLY"
			QString &fileName,
      MeshModel &m, 
      const int &mask,       // what kind of data are saved
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; // prima istanza il dialogo di opzioni viene sempre.
};

/* this is used to callback the executeFilter() function
	when the apply button of the standard plugin window
	is clicked
*/
class MainWindowInterface
{
public:
	virtual void executeFilter(QAction *,FilterParameter *){};
	virtual ~MainWindowInterface(){};
};


class MeshFilterInterface
{
public:
  typedef int FilterType;
  	enum FilterClass { Generic, Selection, Cleaning, Remeshing, FaceColoring, VertexColoring} ;
    virtual ~MeshFilterInterface() {}
		virtual const QString Info(QAction *)=0;

    // The filterclass describe in which submenu each filter should be placed 
    virtual const FilterClass getClass(QAction *) {return MeshFilterInterface::Generic;};
    
    // This function invokes a dialog and get back the parameters
    virtual bool getParameters(QAction *, QWidget * /*parent*/, MeshModel &/*m*/, FilterParameter & /*par*/) {return true;};
    
    // The filters can require some additional
    virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}

    // The main function that applies the selected filter 
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

	  /* Returns an array of standard parameters descriptors for the standard plugin window .NULL is returned by default if the plugin doesn't implement this */
	virtual bool getStdFields(QAction *, MeshModel &, StdParList &){return false;}
    
	/* Overloading of the function getParameters that supports the standard plugin window. If the plugin doesn't implement this, the classic function is called */
	virtual bool getStdParameters(QAction *qa, QWidget *qw /*parent*/, MeshModel &mm/*m*/, FilterParameter &fp /*par*/) {return getParameters(qa,qw,mm,fp);};


protected:
    QList <QAction *> actionList;
    QList <FilterType> typeList;
    void Log(int Level, const char * f, ... ) 
		{
		if(log)
			{	
				char buf[4096];
				va_list marker;
				va_start( marker, f );     
				vsprintf(buf,f,marker);
				va_end( marker );              
				log->Log(Level,buf);
			}
		}

private:
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
		virtual QList<QAction *> actions() = 0;
};


class MeshDecorateInterface
{
public:
  typedef int FilterType;

    virtual ~MeshDecorateInterface() {}

    virtual const QString Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/,QFont qf) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
//		virtual QList<QAction *> actions() const = 0;
        
    virtual const QString ST(FilterType filter) const=0;
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


class MeshEditInterface
{
public:
    virtual ~MeshEditInterface() {}

    virtual const QString Info(QAction *)=0;
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
