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

#ifndef MESHLAB_INTERFACES_H
#define MESHLAB_INTERFACES_H
#include <QtCore>
#include <QApplication>
#include <QAction>
#include <QMessageBox>
#include <QTabletEvent>
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
	
	virtual QList<Format> importFormats() const = 0;
	virtual QList<Format> exportFormats() const = 0;

	
	// The longer string describing each filtering action. 
	// This string is printed in the top of the parameter window 
	// so it should be at least one or two paragraphs long.
	// you can use simple html formatting tags (like <br> <b> and <i>) to improve readability.
	// This string is used in the About plugin dialog and by meshlabserver to create the filter list page.
	//virtual const QString filterInfo(FilterIDType filter) {return QString("Puppa 1");};

	// The very short string (a few words) describing each filtering action 
	// This string is used also to define the menu entry
	//virtual const QString filterName(FilterIDType filter){return QString("Puppa 2");};
	
		// This function is called by the framework, for each action at the loading of the plugins.
		// it allows to add a list of global persistent parameters that can be changed from the meshlab itself.
		// If your plugins/action has no GlobalParameter, do nothing.
	virtual void initGlobalParameterSet(QString /*format*/, FilterParameterSet & /*globalparam*/) {}
	
	// This function is called to initialize the list of additional parameters that a OPENING filter could require 
	// it is called by the framework BEFORE the actual mesh loading to perform to determine how parse the input file
	// The instanced parameters are then passed to the open at the loading time.
	// Typical example of use to decide what subportion of a mesh you have to load.
	// If you do not need any additional processing simply do not override this and ignore the parameterSet in the open
	virtual void initPreOpenParameter(const QString &/*format*/, const QString &/*fileName*/, FilterParameterSet & /*par*/) {}
	
	// This function is called to initialize the list of additional parameters that a OPENING filter could require 
	// it is called by the framework AFTER the mesh is already loaded to perform more or less standard processing on the mesh.
	// typical example: unifying vertices in stl models. 
	// If you do not need any additional processing do nothing.
	virtual void initOpenParameter(const QString &/*format*/, MeshModel &/*m*/, FilterParameterSet & /*par*/) {}

  // This is the corresponding function that is called after the mesh is loaded with the initialized parameters 
	virtual void applyOpenParameter(const QString &/*format*/, MeshModel &/*m*/, const FilterParameterSet &/*par*/){}

	// This function is called to initialize the list of additional parameters that a SAVING filter could require 
	// it is called by the framework after the mesh is loaded to perform more or less standard processing on the mesh.
	// typical example: ascii or binary format for ply or stl 
	// If you do not need any additional parameter simply do nothing.
	virtual void initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, FilterParameterSet & /*par*/) 	{}


	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const = 0;
    
  virtual bool open(
      const QString &format,					// the extension of the format e.g. "PLY"
			const QString &fileName,				// The name of the file to be opened
      MeshModel &m,										// The mesh that is filled with the file content 
      int &mask,											// a bit mask that will be filled reporting what kind of data we have found in the file (per vertex color, texture coords etc)
			const FilterParameterSet & par,	// The parameters that have been set up in the initPreOpenParameter()
      vcg::CallBackPos *cb=0,					// standard callback for reporting progress in the loading
      QWidget *parent=0)=0;						// you should not use this...
    
  virtual bool save(
      const QString &format, // the extension of the format e.g. "PLY"
			const QString &fileName,
      MeshModel &m, 
      const int mask,       // a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
      const FilterParameterSet & par,
			vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; 

	/// This function is invoked by the framework when the import/export plugin fails to give some info to the user about the failure
	/// io plugins should avoid using QMessageBox for reporting errors. 
	/// Failure should put some meaningful information inside the errorMessage string.
	virtual const QString &errorMsg() {return this->errorMessage;}
	
	// this string is used to pass back to the framework error messages in case of failure of a filter apply.
	// NEVER EVER use a msgbox to say something to the user.
	QString errorMessage;
	
	
	GLLogStream *log;	
		

	/// Standard stuff that usually should not be redefined. 
	void setLog(GLLogStream *log) { this->log = log ; }
	// This fucntion must be used to communicate useful information collected in the parsing/saving of the files. 
	// NEVER EVER use a msgbox to say something to the user.
	void Log(const char * f, ... ) 
		{
			if(log)
			{	
				char buf[4096];
				va_list marker;
				va_start( marker, f );     
				vsprintf(buf,f,marker);
				va_end( marker );              
				log->Log(GLLogStream::FILTER,buf);
			}
		}


};

/* this is used to callback the executeFilter() function
	when the apply button of the standard plugin window
	is clicked
*/
class MainWindowInterface
{
public:
	//isPreview tells whether this execution is being used to produce or preview a result
	virtual void executeFilter(QAction *, FilterParameterSet &, bool /* isPreview */){};
	virtual ~MainWindowInterface(){};
	
	// This function is to find the dir where all the deployed stuff reside. 
	// We mean the stuff used by default by MeshLab and that should not interest the standard user
	// like for example the plugins directory, the textures used by the shaders, the cubemaps, the shaders etc.
	// During development this stuff is in the meshlab/src/meshlab dir
	// once deployed it depends on the OS
	// - on windows is in the installation dir
	// - on macs it is in inside the application bundle (to allow easy application moving)
	
	static QString getBaseDirPath()
	{
		QDir baseDir(qApp->applicationDirPath());
		
		#if defined(Q_OS_WIN)
		// Windows: 
		// during development with visual studio binary could be in the debug/release subdir.
		// once deployed plugins dir is in the application directory, so 
				if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")		baseDir.cdUp();
		#endif 

		#if defined(Q_OS_MAC)
		// Mac: during developmentwith xcode  and well deployed the binary is well buried.
				for(int i=0;i<6;++i){
						if(baseDir.exists("plugins")) break;
						baseDir.cdUp();
					}
		#endif
		return baseDir.absolutePath();
	}
	
	static QString getPluginDirPath()
	{
		QDir pluginsDir(getBaseDirPath());
		if(!pluginsDir.exists("plugins"))
				QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
		
		pluginsDir.cd("plugins");
		return pluginsDir.absolutePath();
	}
};



class MeshFilterInterface
{
public:
  typedef int FilterIDType;
	
	// The FilterClass enum represents the set of keywords that must be used to categorize a filter.
	// Each filter can belong to one or more filtering class, or-ed togheter. 
	// The filter class 
	
	enum FilterClass 
	{ 
			Generic          =0x00000, // Should be avoided if possible
			Selection        =0x00001, // select or de-select something, basic operation on selections (like deleting)
			Cleaning         =0x00002, // Filters that can be used to clean meshes (duplicated vertices etc)
			Remeshing        =0x00004, // Simplification, Refinement, Reconstruction and mesh optimization
			FaceColoring     =0x00008, 
			VertexColoring   =0x00010, 
			MeshCreation     =0x00020, 
			Smoothing        =0x00040, // Stuff that does not change the topology, but just the vertex positions
			Quality          =0x00080, 
			Layer            =0x00100, // Layers, attributes
			Normal           =0x00200, // Normal, Curvature, orientation (rotations and transformations fall here)
		  Sampling         =0x00400,
		  Texture          =0x00800,
		  RangeMap         =0x01000, // filters specific for range map processing
      PointSet         =0x02000,
      Measure          =0x04000 // Filters that compute measures and information on meshes.
	};
	
	
	// The FilterPrecondition enum is used to build the prerequisite bitmask that each filter reports. 
	// This mask is used to explicitate what data a filter really needs in order to start. 
	// For example algorithms that compute per face quality have as precondition the existence of faces 
	// (but quality per face is not a precondition, because quality per face is created by these algorithms)
	// on the other hand an algorithm that deletes faces according to the stored quality has both FaceQuality
	// and Face as precondition.
	// These conditions do NOT include computed properties like borderFlags, manifoldness or watertightness. 
	// They are also used to grayout menus un-appliable entries.
	
	enum FilterPrecondition
	{
			FP_Generic          =0x00000, // Should be avoided if possible
			FP_Face             =0x00001, //  
			FP_VertexColor      =0x00002, //  
			FP_VertexQuality    =0x00004, //  
			FP_VertexRadius     =0x00008, //  
			FP_WedgeTexCoord    =0x00010  //
	};


	virtual ~MeshFilterInterface() {}

	// The longer string describing each filtering action. 
	// This string is printed in the top of the parameter window 
	// so it should be at least one or two paragraphs long.
	// you can use simple html formatting tags (like <br> <b> and <i>) to improve readability.
	// This string is used in the About plugin dialog and by meshlabserver to create the filter list page.
	virtual const QString filterInfo(FilterIDType filter) const =0;
	
	// The very short string (a few words) describing each filtering action 
	// This string is used also to define the menu entry
	virtual const QString filterName(FilterIDType filter) const =0;
	
	// This function is called by the framework, for each action at the loading of the plugins.
	// it allows to add a list of global persistent parameters that can be changed from the meshlab itself.
	// If your plugins/action has no GlobalParameter, do nothing.
	virtual void initGlobalParameterSet(QAction *, FilterParameterSet & /*globalparam*/) {}

	// The FilterClass describes in which generic class of filters it fits. 
	// This choice affect the submenu in which each filter will be placed 
	// For example filters that perform an action only on the selection will be placed in the Selection Class
	virtual const FilterClass getClass(QAction *) { return MeshFilterInterface::Generic; }
	
	// The filters can have some additional requirements on the mesh capabiliteis. 
	// For example if a filters requires Face-Face Adjacency you shoud re-implement 
	// this function making it returns MeshModel::MM_FACEFACETOPO. 
	// The framework will ensure that the mesh has the requirements satisfied before invoking the applyFilter function
	virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}
	
	// The FilterPrecondition mask is used to explicitate what a filter really needs to be applied. 
	// For example algorithms that compute per face quality have as precondition the existence of faces 
	// (but quality per face is not a precondition, because quality per face is created by these algorithms)
	// on the other hand an algorithm that deletes faces according to the stored quality has both FaceQuality
	// and Face as precondition.
	virtual int getPreConditions(QAction *) const {return FP_Generic;}


	// Function used by framework to get info about the mesh properties changed by the filter.
	// It is widely used by the meshlab's preview system.
	//TO BE REPLACED WITH = 0
	virtual int postCondition( QAction* ) const {return MeshModel::MM_UNKNOWN;};

	// The main function that applies the selected filter with the already stabilished parameters
	// This function is called by the framework after getting the user params 
	// NO GUI interaction should be done here. No dialog asking, no messagebox errors. 
	// this function will also be called by the commandline framework.
	// If you want report errors, use the errorMsg() string. It will displayed in case of filters returning false.
	
	virtual bool applyFilter(QAction * /*filter*/, MeshModel &/*m*/, FilterParameterSet & /* par */, vcg::CallBackPos * /*cb*/) = 0;
 	virtual bool applyFilter(QAction *   filter, MeshDocument &md,   FilterParameterSet & par,       vcg::CallBackPos *cb) 
	{ 
		return applyFilter(filter,*(md.mm()),par,cb); 
	}

 
	//  this function returns true if the filter has parameters that must be filled with an automatically build dialog.
	virtual bool autoDialog(QAction *) {return false;}
	
	// This function is called to initialized the list of parameters. 
	// it is called by the auto dialog framework to know the list of parameters.
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*par*/) {}
	virtual void initParameterSet(QAction *filter,MeshDocument &md, FilterParameterSet &par) 
	{initParameterSet(filter,*(md.mm()),par);}

	//  this function returns true if the filter has a personally customized dialog..
	virtual bool customDialog(QAction *) {return false;}
	
	// This function is invoked for filters with a custom dialog of a filter and when has the params 
	// it notify it to the mainwindow with the collected parameters
	virtual bool getCustomParameters(QAction *action, QWidget * /*parent*/, MeshModel &/*m*/, FilterParameterSet & params, MainWindowInterface *mw) 
	{
		assert(mw);
		mw->executeFilter(action, params, false);
		return true;
	}	

  /// Standard stuff that usually should not be redefined. 
	void setLog(GLLogStream *log) { this->log = log ; }
		
	/// This function is invoked by the framework when the apply filter fails to give some info to the user about the fiter failure
	/// Filters should avoid using QMessageBox for reporting errors. 
	/// Failing filters should put some meaningful information inside the errorMessage string.
	const QString &errorMsg() {return this->errorMessage;}

    virtual const FilterIDType ID(QAction *a) const
  	{
      foreach( FilterIDType tt, types())
        if( a->text() == this->filterName(tt) ) return tt;
			
			
			qDebug("unable to find the id corresponding to action  '%s'",qPrintable(a->text()));
			assert(0);
      return -1;
    }
		
    virtual QAction *AC(FilterIDType filterID)
  	{
			QString idName=this->filterName(filterID);
      foreach( QAction *tt, actionList)
        if( idName  == tt->text() ) return tt;
			
			qDebug("unable to find the action corresponding to action  '%i'",filterID);
			assert(0);
      return 0;
    }
	 
	 virtual const QString filterInfo(QAction *a){return this->filterInfo(ID(a));};
	 virtual const QString filterName(QAction *a) const {return this->filterName(ID(a));};
	 
    virtual QList<QAction *> actions() const { return actionList;}
	  virtual const QList<FilterIDType> &types() const { return typeList;}

protected:
    // Each plugins exposes a set of filtering possibilities. 
		// Each filtering procedure corresponds to a single QAction with a corresponding FilterIDType id. 
		// 
		
    // The list of actions exported by the plugin. Each actions strictly corresponds to 
		QList <QAction *> actionList;
    
		QList <FilterIDType> typeList;
    
		void Log(const char * f, ... ) 
		{
			if(log)
			{	
				char buf[4096];
				va_list marker;
				va_start( marker, f );     
				vsprintf(buf,f,marker);
				va_end( marker );              
				log->Log(GLLogStream::FILTER,buf);
			}
		}

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

    GLLogStream *log;	
		
		// this string is used to pass back to the framework error messages in case of failure of a filter apply.
		QString errorMessage;
};


/*
Used to customized the rendering process. 
Rendering plugins are now responsible of the rendering of the whole MeshDocument and not only of a single MeshModel. 

The Render function is called in with the ModelView and Projection Matrices already set up, screen cleared and background drawn. 
After the Render call the MeshLab frawework draw on the opengl context other decorations and the trackball, so it there is the 
requirement for a rendering plugin is that it should leave the z-buffer in a coherent state.

The typical rendering loop of a Render plugin is something like, :

<your own opengl setup>

		foreach(MeshModel * mp, meshDoc.meshList)
				{
					if(mp->visible) mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
				}

*/

class MeshRenderInterface
{
public:
    virtual ~MeshRenderInterface() {}
		
    virtual void Init(QAction * /*mode*/, MeshDocument &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/){};
		virtual void Render(QAction * /*mode*/, MeshDocument &/*md*/, RenderMode &/*rm*/, QGLWidget * /*parent*/) = 0;
		virtual void Finalize(QAction * /*mode*/, MeshDocument &/*m*/, GLArea * /*parent*/){};
		virtual bool isSupported() = 0;
		virtual QList<QAction *> actions() = 0;
};


class MeshDecorateInterface
{
public:
  typedef int FilterIDType;

    virtual ~MeshDecorateInterface() {}

    virtual const QString Info(QAction *)=0;

		// This function is called by the framework, for each action at the loading of the plugins.
		// it allows to add a list of global persistent parameters that can be changed from the meshlab itself.
		// If your plugins/action has no GlobalParameter, do nothing.
		// The FilterParameterSet comes here already intialized with the values stored on the permanent storage.
		// If a filter wants to save some permanent stuff should check its esistence here.
	
		virtual void initGlobalParameterSet(QAction *, FilterParameterSet * /*globalparam*/) {}		
		
    virtual void StartDecorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
		virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, FilterParameterSet * /*param*/, GLArea * /*parent*/,QFont qf) = 0;
		virtual void EndDecorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
        
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


/*
Editing Interface
Used to provide tools that needs some kind of interaction with the mesh.
Editing tools are exclusive (only one at a time) and can grab the mouse events and customize the rendering process.
*/

class MeshEditInterface
{
public:
	virtual ~MeshEditInterface() {}
	
	//should return a sentence describing what the editing tool does
	static const QString Info();

	// Called when the user press the first time the button 
	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/){return true;};
	virtual bool StartEdit(MeshDocument &md, GLArea *parent)
	{
		assert(NULL != md.mm());
		return (StartEdit(*(md.mm()), parent));	
	}
		
	// Called when the user press the second time the button 
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/){};
    
	// Called when the user changes the selected layer
	//by default it calls end edit with the layer that was selected and start with the new layer that is
	//selected.  This ensures that plugins who dont support layers do not get sent pointers to meshes
	//they are not expecting
	virtual void LayerChanged(MeshDocument &md, MeshModel &oldMeshModel, GLArea *parent)
	{
		EndEdit(oldMeshModel, parent);
		StartEdit(md, parent);
	}
		
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/) = 0;
	virtual void mousePressEvent  (QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	virtual void mouseMoveEvent   (QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	//virtual void wheelEvent     (QWheelEvent*e, MeshModel &/*m*/, GLArea * );
	virtual void keyReleaseEvent  (QKeyEvent *, MeshModel &/*m*/, GLArea *){};
	virtual void keyPressEvent    (QKeyEvent *, MeshModel &/*m*/, GLArea *){};
	virtual void tabletEvent(QTabletEvent * e, MeshModel &/*m*/, GLArea *){e->ignore();};
};


/* MeshEditInterfaceFactory
 * 
 */
class MeshEditInterfaceFactory
{
public:
	virtual ~MeshEditInterfaceFactory() {}
	
	//gets a list of actions available from this plugin
	virtual QList<QAction *> actions() const = 0;
	
	//get the edit tool for the given action
	virtual MeshEditInterface* getMeshEditInterface(QAction *) = 0;
	
	//get the description for the given action
	virtual const QString getEditToolDescription(QAction *)=0;

};


Q_DECLARE_INTERFACE(MeshIOInterface,						"vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,				"vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,				"vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshDecorateInterface,			"vcg.meshlab.MeshDecorateInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterface,					"vcg.meshlab.MeshEditInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterfaceFactory,			"vcg.meshlab.MeshEditInterfaceFactory/1.0")

#endif
