/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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
Revision 1.64  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.63  2007/11/26 07:35:26  cignoni
Yet another small cosmetic change to the interface of the io filters.

Revision 1.62  2007/11/25 09:48:39  cignoni
Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

Revision 1.61  2007/10/09 13:02:08  fuscof
Initial implementation of multipass rendering.
Please note that MeshRenderInterface has been modified to get the number of rendering passes.

Revision 1.60  2007/10/09 12:07:39  corsini
fix missing return value

Revision 1.59  2007/10/02 07:59:37  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.58  2007/07/10 06:46:26  cignoni
better comments

Revision 1.57  2007/05/16 15:03:21  cignoni
better comments

Revision 1.56  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

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


// The basic interface for opening and saving a mesh.
// The plugins for opening and saving different mesh types must be derived from this class.
//

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
	
	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const = 0;
    
  virtual bool open(
      const QString &format, // the extension of the format e.g. "PLY"
			const QString &fileName,     // The name of the file to be opened
      MeshModel &m,          // The mesh that is filled with the file content 
      int &mask,             // a bit mask reporting what kind of data we have found in the file (per vertex color, texture coords etc)
      vcg::CallBackPos *cb=0,
      QWidget *parent=0)=0;
    
  virtual bool save(
      const QString &format, // the extension of the format e.g. "PLY"
			const QString &fileName,
      MeshModel &m, 
      const int mask,       // a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; 
};

/* this is used to callback the executeFilter() function
	when the apply button of the standard plugin window
	is clicked
*/
class MainWindowInterface
{
public:
	virtual void executeFilter(QAction *, FilterParameterSet &){};
	virtual ~MainWindowInterface(){};
};



class MeshFilterInterface
{
public:
  typedef int FilterIDType;
	enum FilterClass { Generic, Selection, Cleaning, Remeshing, FaceColoring, VertexColoring, MeshCreation} ;
	virtual ~MeshFilterInterface() {}

	// The longer string describing each filtering action 
	// (this string is used in the About plugin dialog)
	virtual const QString filterInfo(FilterIDType filter)=0;
	
	// The very short string describing each filtering action 
	// (this string is used also to define the menu entry)
	virtual const QString filterName(FilterIDType filter)=0;

	// Generic Info about the plugin version and author.
	virtual const PluginInfo &pluginInfo()=0;

	// The FilterClass describes in which generic class of filters it fits. 
	// This choice affect the submenu in which each filter will be placed 
	// For example filters that perform an action only on the selection will be placed in the Selection Class Ê
	virtual const FilterClass getClass(QAction *) { return MeshFilterInterface::Generic; }
	
	// The filters can have some additional requirements on the mesh capabiliteis. 
	// For example if a filters requires Face-Face Adjacency you shoud re-implement 
	// this function making it returns MeshModel::MM_FACETOPO. 
	// The framework will ensure that the mesh has the requirements satisfied before invoking the applyFilter function
	virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}

	// The main function that applies the selected filter with the already stabilished parameters
	// This function is called by the framework after getting the user params 
	virtual bool applyFilter(QAction * /*filter*/, MeshModel &/*m*/, FilterParameterSet & /*parent*/, vcg::CallBackPos * /*cb*/) = 0;
	
  
	//  this function returns true if the filter has parameters that must be filled with an automatically build dialog.
	virtual bool autoDialog(QAction *) {return false;}
	
	// This function is called to initialized the list of parameters. 
	// it is called by the auto dialog framework to know the list of parameters.
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/) {}

	//  this function returns true if the filter has a personally customized dialog..
	virtual bool customDialog(QAction *) {return false;}
	
	// This function is invokes the custom dialog of a filter and when has the params 
	// it notify it to the mainwindow with the collected parameters
	virtual bool getParameters(QAction *action, QWidget * /*parent*/, MeshModel &/*m*/, FilterParameterSet & params, MainWindowInterface *mw) 
	{
		assert(mw);
		mw->executeFilter(action,params);
		return true;
	}	

  /// Standard stuff that usually should not be redefined. 
	  void setLog(GLLogStream *log) { this->log = log ; }

    virtual const FilterIDType ID(QAction *a)
  	{
      foreach( FilterIDType tt, types())
        if( a->text() == this->filterName(tt) ) return tt;
          assert(0);
      return 0;
    }
	 
	 virtual const QString filterInfo(QAction *a){return filterInfo(ID(a));};
	 virtual const QString filterName(QAction *a){return filterName(ID(a));};
	 
    virtual QList<QAction *> actions() const { return actionList;}
	  virtual QList<FilterIDType> &types() { return typeList;}

protected:
    // Each plugins exposes a set of filtering possibilities. 
		// Each filtering procedure corresponds to a single QAction with a corresponding FilterIDType id. 
		// 
		
    // The list of actions exported by the plugin. Each actions strictly corresponds to 
		QList <QAction *> actionList;
    
		QList <FilterIDType> typeList;
    
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
    virtual int passNum() = 0;
};


class MeshDecorateInterface
{
public:
  typedef int FilterIDType;

    virtual ~MeshDecorateInterface() {}

    virtual const QString Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    virtual void Init(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/,QFont qf) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
        
    virtual const QString ST(FilterIDType filter) const=0;
    virtual const FilterIDType ID(QAction *a)
  	{
      foreach( FilterIDType tt, types())
        if( a->text() == this->ST(tt) ) return tt;
          assert(0);
      return 0;
    }

    virtual QList<QAction *> actions() const { return actionList;}
    virtual QList<FilterIDType> &types() { return typeList;}
protected:
    QList <QAction *> actionList;
    QList <FilterIDType> typeList;
    GLLogStream *log;	
};


class MeshEditInterface
{
public:
    virtual ~MeshEditInterface() {}
		virtual QList<QAction *> actions() const = 0;
    virtual const QString Info(QAction *)=0;
    virtual const PluginInfo &Info()=0;

    // Called when the user press the first time the button 
		virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		
		// Called when the user press the second time the button 
		virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) = 0;
    virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	  virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	  virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
//	  virtual void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );

};

Q_DECLARE_INTERFACE(MeshIOInterface,       "vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,   "vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,   "vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshDecorateInterface, "vcg.meshlab.MeshDecorateInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterface,     "vcg.meshlab.MeshEditInterface/1.0")

#endif
