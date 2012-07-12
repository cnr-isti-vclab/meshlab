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
#include <GL/glew.h>

#include <QtCore>
#include <QApplication>
#include <QAction>
#include <QGLContext>
#include <QGLFormat>
#include <QMessageBox>
#include <QTabletEvent>
#include "filterparameter.h"
#include "GLLogStream.h"
#include "meshmodel.h"
#include "scriptinterface.h"
#include "xmlfilterinfo.h"


class QWidget;
class QGLWidget;
class QString;
class QVariant;
class QMouseEvent;
class QTreeWidgetItem;
class MeshModel;
class RenderMode;
class GLArea;
class GLAreaReg;
class QScriptEngine;


/** The MainWindowInterface class defines just the executeFilter() callback function
    that is invoked by the standard parameter input dialog.
    It is used as base class of the MainWindow.
    */
class MainWindowInterface
{
public:
  virtual void executeFilter(QAction *, RichParameterSet &, bool /* isPreview */){}
  virtual void executeFilter(MeshLabXMLFilterContainer*, EnvWrap& , bool /* isPreview */){}
};

/** \brief The MeshLabInterface class is the base of all the plugin interfaces.

  The main idea common to all the framework is that each plugin export a set of actions,
  internally each action is associated to a FilterIDType, and for each action a name and a formatted INFO is defined.

  For coding easyness ID are more practical (you can use them in switches).
  Using action on the other hand is practical because it simplify their management in menus/toolbars and it allows to define icons and other things in a automatic way.
  Moreover ID are UNSAFE (different plugin can have same id) so they should be used only INTERNALLY

    \todo There is inconsistency in the usage of ID and actions for retrieving particular filters. Remove.

*/
class MeshLabInterface
{
public:
  /** the type used to identify plugin actions; there is a one-to-one relation betweeen an ID and an Action.
    \todo To be renamed as ActionIDType
    */

	MeshLabInterface() :log(0) {}
	virtual ~MeshLabInterface() {}
private:
	GLLogStream *log;	
public:
	
	/// Standard stuff that usually should not be redefined. 
	void setLog(GLLogStream *log) { this->log = log ; }
	// This fucntion must be used to communicate useful information collected in the parsing/saving of the files. 
	// NEVER EVER use a msgbox to say something to the user.
  void Log(const char * f, ... );
  void Log(int Level, const char * f, ... ) ;
  void RealTimeLog(QString Id, const char * f, ... ) ;
};

class MeshCommonInterface : public MeshLabInterface
{
public:
	typedef int FilterIDType;
	MeshCommonInterface() {}
	virtual ~MeshCommonInterface() {}

  virtual QString pluginName(void) const { return ""; }

  /** \brief This function is called by the framework, for each plugin that has global parameters (e.g. \ref MeshDecorateInterface) at the start of the application.
   The rationale is to allow to each plugin to have a list of global persistent parameters that can be changed from the meshlab itself and whose value is persistent between different meshlab invocations.
   A typical example is the background color.

   For the global parameters the following rules apply:

   \li there is a <b>hardwired</b> default value: a safe consistent value that is directly coded into the plugin and to which the user can always revert if needed.
   \li there is a <b>saved</b> value: a value that is stored into a persistent location into the user space (registry/home/library) and it is presented as default value of the parameter at each MeshLab invocation.
   \li there is a <b>current</b> value: a value that is currently used, different for each document instance and that is not stored permanently.

   The plugin use the current value to draw its decoration.
   at startup the current value is always silently initialized to the saved value.
   User can revert current value to the saved values and to the hardwired values.
   In the dialog for each parameter some buttons should be present:

   \li apply: use the currently edited parameter value without saving it anywhere. After the closure of the document these values will be lost.
   \li load:  load from the saved values
   \li save:  save to a permanent location the current value (to the registry),
   \li reset:  revert to the hardwired values

   If your plugins/action has no GlobalParameter, do nothing.
   The RichParameterSet comes to the StartDecorate already intialized with the values stored on the permanent storage.
   At the start up the initGlobalParameterSet function is called with an empty RichParameterSet (to collect the default values)
   If a filter wants to save some permanent stuff should set the permanent default values.
*/
	virtual void initGlobalParameterSet(QAction * /*format*/, RichParameterSet & /*globalparam*/) {}
};
/** \brief The MeshIOInterface is the base class for all the single mesh loading plugins.
  */
class MeshIOInterface : public MeshCommonInterface
{
public:
	class Format
	{
	public:
		Format(QString description,QString ex): description(description){extensions << ex;}
		QString description;
		QStringList extensions;
	};

	MeshIOInterface(): MeshCommonInterface() {  }
  virtual ~MeshIOInterface() {}
	
	virtual QList<Format> importFormats() const = 0;
	virtual QList<Format> exportFormats() const = 0;

  // This function is called to initialize the list of additional parameters that a OPENING filter could require
	// it is called by the framework BEFORE the actual mesh loading to perform to determine how parse the input file
	// The instanced parameters are then passed to the open at the loading time.
	// Typical example of use to decide what subportion of a mesh you have to load.
	// If you do not need any additional processing simply do not override this and ignore the parameterSet in the open
	virtual void initPreOpenParameter(const QString &/*format*/, const QString &/*fileName*/, RichParameterSet & /*par*/) {}
	
	// This function is called to initialize the list of additional parameters that a OPENING filter could require 
	// it is called by the framework AFTER the mesh is already loaded to perform more or less standard processing on the mesh.
	// typical example: unifying vertices in stl models. 
	// If you do not need any additional processing do nothing.
	virtual void initOpenParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & /*par*/) {}

  // This is the corresponding function that is called after the mesh is loaded with the initialized parameters 
	virtual void applyOpenParameter(const QString &/*format*/, MeshModel &/*m*/, const RichParameterSet &/*par*/){}

	// This function is called to initialize the list of additional parameters that a SAVING filter could require 
	// it is called by the framework after the mesh is loaded to perform more or less standard processing on the mesh.
	// typical example: ascii or binary format for ply or stl 
	// If you do not need any additional parameter simply do nothing.
	virtual void initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & /*par*/) 	{}


	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const = 0;
    
  /// callback used to actually load a mesh from a file
  virtual bool open(
      const QString &format,					/// the extension of the format e.g. "PLY"
      const QString &fileName,				/// The name of the file to be opened
      MeshModel &m,										/// The mesh that is filled with the file content
      int &mask,											/// a bit mask that will be filled reporting what kind of data we have found in the file (per vertex color, texture coords etc)
      const RichParameterSet & par,	/// The parameters that have been set up in the initPreOpenParameter()
      vcg::CallBackPos *cb=0,					/// standard callback for reporting progress in the loading
      QWidget *parent=0)=0;						/// you should not use this...

  virtual bool save(
      const QString &format, // the extension of the format e.g. "PLY"
			const QString &fileName,
      MeshModel &m, 
      const int mask,       // a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
      const RichParameterSet & par,
			vcg::CallBackPos *cb=0,
      QWidget *parent= 0)=0 ; 

	/// This function is invoked by the framework when the import/export plugin fails to give some info to the user about the failure
	/// io plugins should avoid using QMessageBox for reporting errors. 
	/// Failure should put some meaningful information inside the errorMessage string.
	virtual QString &errorMsg() {return this->errorMessage;}
	void clearErrorString() {errorMessage.clear();}
	
	// this string is used to pass back to the framework error messages in case of failure of a filter apply.
	// NEVER EVER use a msgbox to say something to the user.
	QString errorMessage;

};


/**
  \brief The MeshFilterInterface class provide the interface of the filter plugins.

*/
class MeshFilterInterface : public MeshCommonInterface
{
public:
  /** The FilterClass enum represents the set of keywords that must be used to categorize a filter.
   Each filter can belong to one or more filtering class, or-ed togheter.
  */
	enum FilterClass 
	{ 
        Generic          =0x00000, /*!< Should be avoided if possible. */  //
        Selection        =0x00001, /*!<  select or de-select something, basic operation on selections (like deleting)*/
        Cleaning         =0x00002, /*!<  Filters that can be used to clean meshes (duplicated vertices etc)*/
        Remeshing        =0x00004, /*!<  Simplification, Refinement, Reconstruction and mesh optimization*/
        FaceColoring     =0x00008,
        VertexColoring   =0x00010,
        MeshCreation     =0x00020,
        Smoothing        =0x00040, /*!<  Stuff that does not change the topology, but just the vertex positions*/
        Quality          =0x00080,
        Layer            =0x00100, /*!<  Layers, attributes */
        RasterLayer      =0x20000, /*!<  Raster Layers, attributes */
        Normal           =0x00200, /*!<  Normal, Curvature, orientation (rotations and transformations fall here)*/
        Sampling         =0x00400,
        Texture          =0x00800,
        RangeMap         =0x01000, /*!<  filters specific for range map processing*/
        PointSet         =0x02000,
        Measure          =0x04000,  /*!<  Filters that compute measures and information on meshes.*/
        Polygonal        =0x08000,  /*!<  Filters that works on polygonal and quad meshes.*/
        Camera           =0x10000  /*!<  Filters that works on shot of mesh and raster.*/
    };
	
	
	
	MeshFilterInterface() : MeshCommonInterface() 
	{
	}
	virtual ~MeshFilterInterface() {}


  /** The very short string (a few words) describing each filtering action
  // This string is used also to define the menu entry
  */
  virtual QString filterName(FilterIDType ) const =0;

  /** The long, formatted string describing each filtering action.
	// This string is printed in the top of the parameter window 
  // so it should be at least one or two paragraphs long. The more the better.
  // you can use simple html formatting tags (like "<br>" "<b>" and "<i>") to improve readability.
  // This string is used in the 'About plugin' dialog and by meshlabserver to create the filter list wiki page and the doxygen documentation of the filters.
  // Here is the place where you should put you bibliographic references in a form like this:
  <br>
  See: <br />
  <i>Luiz Velho, Denis Zorin </i><br/>
  <b>"4-8 Subdivision"</b><br/>
  CAGD, volume 18, Issue 5, Pages 397-427.<br/>
  <br>
  e.g. italic for authors, bold for title (quoted) and plain for bib ref.
  */
	virtual QString filterInfo(FilterIDType filter) const =0;
	
  /** The FilterClass describes in which generic class of filters it fits.
	// This choice affect the submenu in which each filter will be placed 
	// For example filters that perform an action only on the selection will be placed in the Selection Class
  */
	virtual FilterClass getClass(QAction *) { return MeshFilterInterface::Generic; }
	
  /**
   The filters can have some additional requirements on the mesh capabiliteis.
	// For example if a filters requires Face-Face Adjacency you shoud re-implement 
	// this function making it returns MeshModel::MM_FACEFACETOPO. 
	// The framework will ensure that the mesh has the requirements satisfied before invoking the applyFilter function
  //
  // Furthermore, requirements are checked just before the invocation of a filter. If your filter
  // outputs a never used before mesh property (e.g. face colors), it will be allocated by a call
  // to MeshModel::updateDataMask(...)
  */
  virtual int getRequirements(QAction *){return MeshModel::MM_NONE;}
	
  /** The FilterPrecondition mask is used to explicitate what kind of data a filter really needs to be applied.
	// For example algorithms that compute per face quality have as precondition the existence of faces 
	// (but quality per face is not a precondition, because quality per face is created by these algorithms)
	// on the other hand an algorithm that deletes faces according to the stored quality has both FaceQuality
	// and Face as precondition.
  // These conditions do NOT include computed properties like borderFlags, manifoldness or watertightness.
  // They are also used to grayout menus un-appliable entries.
  */
  virtual int getPreConditions(QAction *) const {return MeshModel::MM_NONE;}

  /** Function used by the framework to get info about the mesh properties changed by the filter.
	// It is widely used by the meshlab's preview system.
	//TO BE REPLACED WITH = 0
  */
  virtual int postCondition( QAction* ) const {return MeshModel::MM_UNKNOWN;}

  /** \brief applies the selected filter with the already stabilished parameters
  * This function is called by the framework after getting values for the parameters specified in the \ref InitParameterSet
  * NO GUI interaction should be done here. No dialog asking, no messagebox errors.
  * Think that his function will also be called by the commandline framework.
  * If you want report errors, use the \ref errorMsg() string. It will displayed in case of filters returning false.
  * When implementing your applyFilter, you should use the cb function to report to the framework the current state of the processing.
  * During your (long) processing you should call from time to time cb(perc,descriptiveString), where perc is an int (0..100)
  * saying what you are doing and at what point of the computation you currently are.
  * \sa errorMsg
  * \sa initParameterSet
  */
    virtual bool applyFilter(QAction *   filter, MeshDocument &md,   RichParameterSet & par,       vcg::CallBackPos *cb) =0;

  /** \brief tests if a filter is applicable to a mesh.
  This function is a handy wrapper used by the framework for the \a getPreConditions callback;
  For istance a colorize by quality filter cannot be applied to a mesh without per-vertex-quality.
  On failure (returning false) the function fills the MissingItems list with strings describing the missing items.
  */
  bool isFilterApplicable(QAction *act, const MeshModel& m, QStringList &MissingItems) const;

	// This function is called to initialized the list of parameters. 
  // it is always called. If a filter does not need parameter it leave it empty and the framework
  // will not create a dialog (unless for previewing)
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*par*/) {}
	virtual void initParameterSet(QAction *filter,MeshDocument &md, RichParameterSet &par) 
	{initParameterSet(filter,*(md.mm()),par);}
		
  /** \brief is invoked by the framework when the applyFilter fails to give some info to the user about the fiter failure
    * Filters \b must never use QMessageBox for reporting errors.
    * Failing filters should put some meaningful information inside the errorMessage string and return false with the \ref applyFilter
    */
	const QString &errorMsg() {return this->errorMessage;}
  virtual QString filterInfo(QAction *a) const {return this->filterInfo(ID(a));}
  virtual QString filterName(QAction *a) const {return this->filterName(ID(a));}
  virtual QString filterScriptFunctionName(FilterIDType /*filterID*/) {return "";}



  /**
  Builds a QTreeWidgetItem that visually describes a given Tag.
  This is a virtual function, so every filter/edit that generates Tag should implement it.

  In each update, the layerDialog visits the meshlist of the document, and for each mesh asks to the meshDocument the correspondent list of taggings.
  For each tag it invokes this callback and inserts the generated QtreeWidgetItem) into the treeWidget

  In order to generate different TreeWidgetItem according to the position of the tag,
  MeshModel* mm and RasterModel* rm pointers are needed.
  In fact, a tag is displayed below all the meshes/rasters it refers and in the global list of tags,
  so, it makes sense displaying different info in each position.
  */
  virtual QTreeWidgetItem *tagDump(TagBase * /*tag*/, MeshDocument &/*md*/, MeshModel */*mm*/) {assert (0); return 0;}

  virtual FilterIDType ID(QAction *a) const
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
      return AC(idName);
    }

    virtual QAction *AC(QString idName)
    {
      foreach( QAction *tt, actionList)
        if( idName  == tt->text() ) return tt;

      qDebug("unable to find the action corresponding to action  '%s'",qPrintable(idName));
      assert(0);
      return 0;
    }
	 
    virtual QList<QAction *> actions() const { return actionList;}
	  virtual QList<FilterIDType> types() const { return typeList;}

	  /** Generate the mask of attributes would be created IF the MeshFilterInterface filt would has been called on MeshModel mm
	      BE CAREFUL! this function does NOT change in anyway the state of the MeshModel!!!! **/
	  int previewOnCreatedAttributes(QAction* act,const MeshModel& mm);
	QString generatedScriptCode;

	/** If you need to init your QGLContext in order to use GPU redefine this function. */
	virtual bool initGLContext() {return true;}

	QGLContext* glContext;
protected:
    // Each plugins exposes a set of filtering possibilities. 
		// Each filtering procedure corresponds to a single QAction with a corresponding FilterIDType id. 
		// 
		
    // The list of actions exported by the plugin. Each actions strictly corresponds to 
		QList <QAction *> actionList;
    
		QList <FilterIDType> typeList;
    
		// this string is used to pass back to the framework error messages in case of failure of a filter apply.
		QString errorMessage;
};


/**
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

class MeshRenderInterface : public MeshCommonInterface
{
public:
	MeshRenderInterface() :MeshCommonInterface() {}
    virtual ~MeshRenderInterface() {}
		
  virtual void Init(QAction * /*mode*/, MeshDocument &/*m*/, RenderMode &/*rm*/, QGLWidget * /*parent*/){}
	virtual void Render(QAction * /*mode*/, MeshDocument &/*md*/, RenderMode &/*rm*/, QGLWidget * /*parent*/) = 0;
  virtual void Finalize(QAction * /*mode*/, MeshDocument */*m*/, GLArea * /*parent*/){}
	virtual bool isSupported() = 0;
	virtual QList<QAction *> actions() = 0;
};
/**
  MeshDecorateInterface is the base class of all <b> decorators </b>
  Decorators are 'read-only' visualization aids that helps to show some data about a document.

  There are two classes of Decorations
  - PerMesh
  - PerDocument

  Some example of PerDocument Decorations
    - backgrounds
    - trackball icon
    - axis
    - shadows
    - screen space Ambient occlusion (think it as a generic 'darkner')

  Some example of PerMesh Decorations
    - coloring of selected vertex/face
    - displaying of normals/curvature directions
    - display of specific tagging
  */

class MeshDecorateInterface : public MeshCommonInterface
{
public:

  /** The DecorationClass enum represents the set of keywords that must be used to categorize a filter.
   Each filter can belong to one or more filtering class, or-ed togheter.
  */
  enum DecorationClass
  {
        Generic          =0x00000, /*!< Should be avoided if possible. */  //
        PerMesh          =0x00001, /*!<  Decoration that are applied on a single mesh */
        PerDocument      =0x00002, /*!<  Decoration that are applied on a single mesh */
        PreRendering     =0x00004, /*!<  Decoration that are applied <i>before</i> the rendering of the document/mesh */
        PostRendering    =0x00008, /*!<  Decoration that are applied <i>after</i> the rendering of the document/mesh */
  };

  MeshDecorateInterface(): MeshCommonInterface() {}
  virtual ~MeshDecorateInterface() {}
  /** The very short string (a few words) describing each filtering action
  // This string is used also to define the menu entry
  */
  virtual QString decorationName(FilterIDType ) const =0;
  virtual QString decorationInfo(FilterIDType ) const =0;
  virtual QString decorationInfo(QAction *a) const {return decorationInfo(ID(a));}


  virtual bool startDecorate(QAction * /*mode*/, MeshDocument &/*m*/, RichParameterSet * /*param*/, GLArea * /*parent*/) =0;
  virtual void decorate(QAction * /*mode*/,  MeshDocument &/*m*/, RichParameterSet *, GLArea * /*parent*/, QPainter */*p*/) = 0;
  virtual void endDecorate(QAction * /*mode*/,   MeshDocument &/*m*/, RichParameterSet *, GLArea * /*parent*/){}

  /** \brief tests if a decoration is applicable to a mesh.
  For istance curvature cannot be shown on a mesh without curvature.
  On failure (returning false) the function fills the MissingItems list with strings describing the missing items.
  It is invoked only for decoration of \i PerMesh class;
  */
  virtual bool isDecorationApplicable(QAction */*action*/, const MeshModel& /*m*/, QString&/*MissingItems*/) const {return true;}

  virtual int getDecorationClass(QAction */*action*/) const {return Generic;}

  virtual QList<QAction *> actions() const { return actionList;}
  virtual QList<FilterIDType> types() const { return typeList;}
protected:
  QList <QAction *> actionList;
  QList <FilterIDType> typeList;
	virtual FilterIDType ID(QAction *a) const
	{
		foreach( FilterIDType tt, types())
      if( a->text() == this->decorationName(tt) ) return tt;
		qDebug("unable to find the id corresponding to action  '%s'",qPrintable(a->text()));
		assert(0);
		return -1;
	}
};


/*
Editing Interface
Used to provide tools that needs some kind of interaction with the mesh.
Editing tools are exclusive (only one at a time) and can grab the mouse events and customize the rendering process.
*/

class MeshEditInterface : public MeshCommonInterface
{
public:
	MeshEditInterface() : MeshCommonInterface() {}
	virtual ~MeshEditInterface() {}
	
	//should return a sentence describing what the editing tool does
	static const QString Info();

  // Called when the user press the first time the button
    virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/){return true;}
	virtual bool StartEdit(MeshDocument &md, GLArea *parent)
	{
		//assert(NULL != md.mm());
		if ( md.mm() != NULL)
			return (StartEdit(*(md.mm()), parent));	
		else return false;
	}
	// Called when the user press the second time the button 
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/){}
    

	// There are two classes of editing tools, the one that works on a single layer at a time
	// and the ones that works on all layers and have to manage in a correct way the action of changing the current layer.
	// For the edit tools that works ona single layer changing the layer means the restart of the edit tool.
	virtual bool isSingleMeshEdit() const { return true; }

	// Called when the user changes the selected layer
	//by default it calls end edit with the layer that was selected and start with the new layer that is
	//selected.  This ensures that plugins who dont support layers do not get sent pointers to meshes
	//they are not expecting.
	// If your editing plugins is not singleMesh you MUST reimplement this to correctly handle the change of layer.
	virtual void LayerChanged(MeshDocument &md, MeshModel &oldMeshModel, GLArea *parent)
	{
		assert(this->isSingleMeshEdit());
		EndEdit(oldMeshModel, parent);
		StartEdit(md, parent);
	}
		
  virtual void Decorate(MeshModel &m, GLArea *parent, QPainter * /*p*/) { Decorate(m,parent); }
  virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/){}

	virtual void mousePressEvent  (QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	virtual void mouseMoveEvent   (QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * )=0;
	  virtual void keyReleaseEvent  (QKeyEvent *, MeshModel &/*m*/, GLArea *){}
  virtual void keyPressEvent    (QKeyEvent *, MeshModel &/*m*/, GLArea *){}
  virtual void wheelEvent(QWheelEvent*, MeshModel &/*m*/, GLArea * ){}
  virtual void tabletEvent(QTabletEvent * e, MeshModel &/*m*/, GLArea *){e->ignore();}
};


/** MeshEditInterfaceFactory
    \short The MeshEditInterfaceFactory class is a <i>factory</i> is used to generate a object for each starting of an editing filter.

    This is needed because editing filters have a internal state, so if you want to have an editing tool for two different documents you have to instance two objects.
    This class is used by the framework to generate an independent MeshEditInterface for each document.
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
	virtual QString getEditToolDescription(QAction *)=0;

};



/**************************************************************************************************************************************************************/
/*The new class of filter defined through XML file*/

typedef bool SignalCallBack();

class MeshLabFilterInterface : public QObject, public MeshLabInterface
{
	Q_OBJECT
public:
	MeshLabFilterInterface();
	virtual ~MeshLabFilterInterface() {}

	QGLContext* glContext;

	static void initConvertingMap(QMap<QString,MeshModel::MeshElement>& convertingMap);
	static void initConvertingCategoryMap(QMap<QString,MeshFilterInterface::FilterClass>& convertingMap);
	static bool arePreCondsValid(const int filterPreConds,const MeshModel& m, QStringList &MissingItems);
	static int convertStringListToMeshElementEnum(const QStringList& stringListEnum);
	static int convertStringListToCategoryEnum(const QStringList& stringListEnum);
	virtual bool applyFilter(const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos* cb) =0;
	const QString &errorMsg() {return this->errorMessage;}
public slots:
	inline void setInterrupt(const bool& inter) {intteruptreq = inter;};

protected:
	////This function has two different aims: 
	////1) should be invoked by filters in order to request a redraw of a subset of meshes and/or rasters inside the MeshDocument. 
	////2) like a synchronization point where the filter can safely stop is execution.
	////if filter has not a pending interrupt request a render state update request will be sent to the framework.
	////return value: true if the request has been sent, false otherwise (filter has an interrupt request).

	//bool sendUpdateRequest(const MeshDocument& md,);

	//QList<int> meshestobeupdated;
	//int meshmaskattributestobeupdated;
	//QList<int> rasterstobeupdated;
	//int rastermaskattributestobeupdated;

	// this string is used to pass back to the framework error messages in case of failure of a filter apply.
	QString errorMessage;
	bool intteruptreq;
};

Q_DECLARE_INTERFACE(MeshIOInterface,						"vcg.meshlab.MeshIOInterface/1.0")
Q_DECLARE_INTERFACE(MeshFilterInterface,				"vcg.meshlab.MeshFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshLabFilterInterface,				"vcg.meshlab.MeshLabFilterInterface/1.0")
Q_DECLARE_INTERFACE(MeshRenderInterface,				"vcg.meshlab.MeshRenderInterface/1.0")
Q_DECLARE_INTERFACE(MeshDecorateInterface,			"vcg.meshlab.MeshDecorateInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterface,					"vcg.meshlab.MeshEditInterface/1.0")
Q_DECLARE_INTERFACE(MeshEditInterfaceFactory,			"vcg.meshlab.MeshEditInterfaceFactory/1.0")

#endif
