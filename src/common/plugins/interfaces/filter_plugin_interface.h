/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_FILTER_PLUGIN_INTERFACE_H
#define MESHLAB_FILTER_PLUGIN_INTERFACE_H

#include "plugin_interface.h"
#include "../../ml_document/mesh_document.h"

//declaring types to be used as QVariants

Q_DECLARE_METATYPE(Point2m)
Q_DECLARE_METATYPE(Point3m)
Q_DECLARE_METATYPE(Box3m)
Q_DECLARE_METATYPE(Matrix33m)
Q_DECLARE_METATYPE(Matrix44m)
Q_DECLARE_METATYPE(Eigen::VectorXd)

/**
 *\brief The FilterPluginInterface class provide the interface of the filter plugins.
 *
 */
class FilterPluginInterface : virtual public PluginInterface
{
public:
	/** 
	 * @brief The FilterClass enum represents the set of keywords that must be used to categorize a filter.
	 * Each filter can belong to one or more filtering class, or-ed together.
	 */
	enum FilterClass
	{
		Generic        = 0x00000, /*!< Should be avoided if possible. */  //
		Selection      = 0x00001, /*!<  select or de-select something, basic operation on selections (like deleting)*/
		Cleaning       = 0x00002, /*!<  Filters that can be used to clean meshes (duplicated vertices etc)*/
		Remeshing      = 0x00004, /*!<  Simplification, Refinement, Reconstruction and mesh optimization*/
		FaceColoring   = 0x00008,
		VertexColoring = 0x00010,
		MeshColoring   = 0x00020,
		MeshCreation   = 0x00040,
		Smoothing      = 0x00080, /*!<  Stuff that does not change the topology, but just the vertex positions*/
		Quality        = 0x00100,
		Layer          = 0x00200, /*!<  Layers, attributes */
		RasterLayer    = 0x00400, /*!<  Raster Layers, attributes */
		Normal         = 0x00800, /*!<  Normal, Curvature, orientation (rotations and transformations fall here)*/
		Sampling       = 0x01000,
		Texture        = 0x02000,
		RangeMap       = 0x04000, /*!<  filters specific for range map processing*/
		PointSet       = 0x08000,
		Measure        = 0x10000, /*!<  Filters that compute measures and information on meshes.*/
		Polygonal      = 0x20000, /*!<  Filters that works on polygonal and quad meshes.*/
		Camera         = 0x40000  /*!<  Filters that works on shot of mesh and raster.*/
	};



	FilterPluginInterface() : PluginInterface(), glContext(nullptr)
	{
	}
	virtual ~FilterPluginInterface() {}


	/** 
	 * @brief The very short string (a few words) describing each filtering action
	 * This string is used also to define the menu entry
	 */
	virtual QString filterName(FilterIDType) const = 0;

	/** 
	 * @brief The long, formatted string describing each filtering action.
	 * This string is printed in the top of the parameter window
	 * so it should be at least one or two paragraphs long. The more the better.
	 * you can use simple html formatting tags (like "<br>" "<b>" and "<i>") to improve readability.
	 * This string is used in the 'About plugin' dialog and by meshlabserver to create the filter list wiki page and the doxygen documentation of the filters.
	 * Here is the place where you should put you bibliographic references in a form like this:
	 * <br>
	 * See: <br />
	 * <i>Luiz Velho, Denis Zorin </i><br/>
	 * <b>"4-8 Subdivision"</b><br/>
	 * CAGD, volume 18, Issue 5, Pages 397-427.<br/>
	 * <br>
	 * e.g. italic for authors, bold for title (quoted) and plain for bib ref.
	 */
	virtual QString filterInfo(FilterIDType filter) const = 0;

	/** 
	 * @brief The FilterClass describes in which generic class of filters it fits.
	 * This choice affect the submenu in which each filter will be placed
	 * For example filters that perform an action only on the selection will be placed in the Selection Class
	 */
	virtual FilterClass getClass(const QAction*) const { return FilterPluginInterface::Generic; }

	/**
	 * @brief The filters can have some additional requirements on the mesh capabiliteis.
	 * For example if a filters requires Face-Face Adjacency you should re-implement
	 * this function making it returns MeshModel::MM_FACEFACETOPO.
	 * The framework will ensure that the mesh has the requirements satisfied before invoking the applyFilter function
	 *
	 * Furthermore, requirements are checked just before the invocation of a filter. If your filter
	 * outputs a never used before mesh property (e.g. face colors), it will be allocated by a call
	 * to MeshModel::updateDataMask(...)
	 */
	virtual int getRequirements(const QAction*) { return MeshModel::MM_NONE; }
	
	/**
	 * @brief This function should require true if the glContext is used by the
	 * filter. Without this, the glContext will remain set to nullptr on non-GUI
	 * softwares that will use the filter (E.G. PyMeshLab).
	 * Note: every filter that uses the glContext should first check if
	 * glContext != nullptr. 
	 */
	virtual bool requiresGLContext(const QAction*) const {return false;}

	/** 
	 * @brief The FilterPrecondition mask is used to explicitate what kind of data a filter really needs to be applied.
	 * For example algorithms that compute per face quality have as precondition the existence of faces
	 * (but quality per face is not a precondition, because quality per face is created by these algorithms)
	 * on the other hand an algorithm that deletes faces according to the stored quality has both FaceQuality
	 * and Face as precondition.
	 * These conditions do NOT include computed properties like borderFlags, manifoldness or watertightness.
	 * They are also used to grayout menus un-appliable entries.
	 */
	virtual int getPreConditions(const QAction*) const { return MeshModel::MM_NONE; }

	/** 
	 * @brief Function used by the framework to get info about the mesh properties changed by the filter.
	 * It is widely used by the meshlab's preview system.
	 * TO BE REPLACED WITH = 0
	 */
	virtual int postCondition(const QAction*) const { return MeshModel::MM_ALL; }

	/** 
	 * @brief applies the selected filter with the already stabilished parameters
	 * This function is called by the framework after getting values for the parameters specified in the \ref InitParameterSet
	 * NO GUI interaction should be done here. No dialog asking, no messagebox errors.
	 * Think that his function will also be called by the commandline framework.
	 * If you want report errors, use the \ref errorMsg() string. It will displayed in case of filters returning false.
	 * When implementing your applyFilter, you should use the cb function to report to the framework the current state of the processing.
	 * During your (long) processing you should call from time to time cb(perc,descriptiveString), where perc is an int (0..100)
	 * saying what you are doing and at what point of the computation you currently are.
	 * @sa errorMsg
	 * @sa initParameterSet
	 */
	virtual bool applyFilter(const QAction* filter, MeshDocument& md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList& par, vcg::CallBackPos* cb) = 0;

	/** 
	 * \brief tests if a filter is applicable to a mesh.
	 * This function is a handy wrapper used by the framework for the \a getPreConditions callback;
	 * For instance a colorize by quality filter cannot be applied to a mesh without per-vertex-quality.
	 * On failure (returning false) the function fills the MissingItems list with strings describing the missing items.
	 */
	bool isFilterApplicable(const QAction* act, const MeshModel& m, QStringList &MissingItems) const;


	enum FILTER_ARITY { NONE = 0, SINGLE_MESH = 1, FIXED = 2, VARIABLE = 3, UNKNOWN_ARITY = 4 };

	/** 
	 * @brief this function informs the MeshLab core on how many meshes the filter will work on.
	 * Valid value:
	 * - SINGLE_MESH: the filter works just on the current mesh
	 * - FIXED: the number (and the names) of the meshes involved in the filter computation is determined by the parameters selected in the filter's parameters form
	 * - VARIABLE: the filter works on a not predetermined number of meshes. The meshes involved are typically selected by the user checking on the correspondent layer on the layer dialog
	 */
	virtual FILTER_ARITY filterArity(const QAction *act) const = 0;

	/**
	 * @brief This function is called to initialized the list of parameters.
	 * it is always called. If a filter does not need parameter it leave it empty and the framework
	 * will not create a dialog (unless for previewing)
	 */
	virtual void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*par*/) {}
	virtual void initParameterList(const QAction* filter, MeshDocument &md, RichParameterList &par)
	{
		initParameterList(filter, *(md.mm()), par);
	}

	/** 
	 * @brief is invoked by the framework when the applyFilter fails to give some info to the user about the filter failure
	 * Filters \b must never use QMessageBox for reporting errors.
	 * Failing filters should put some meaningful information inside the errorMessage string and return false with the \ref applyFilter
	 */
	const QString& errorMsg() const { return this->errorMessage; }
	virtual QString filterInfo(const QAction* a) const { return this->filterInfo(ID(a)); }
	virtual QString filterName(const QAction* a) const { return this->filterName(ID(a)); }
	virtual QString filterScriptFunctionName(FilterIDType /*filterID*/) { return ""; }

	virtual FilterIDType ID(const QAction *a) const;

	virtual QAction* getFilterAction(FilterIDType filterID);
	virtual QAction* getFilterAction(const QString& idName);

	virtual QList<QAction*> actions() const { return actionList; }
	virtual QList<FilterIDType> types() const { return typeList; }

	/** 
	 * Generate the mask of attributes would be created IF the MeshFilterInterface filt would has been called on MeshModel mm
	 * BE CAREFUL! this function does NOT change in anyway the state of the MeshModel!!!! 
	 */
	int previewOnCreatedAttributes(const QAction* act, const MeshModel& mm) const;
	QString generatedScriptCode;

	MLPluginGLContext* glContext;
protected:
	// Each plugins exposes a set of filtering possibilities.
	// Each filtering procedure corresponds to a single QAction with a corresponding FilterIDType id.
	//

	// The list of actions exported by the plugin. Each actions strictly corresponds to
	QList <QAction*> actionList;

	QList <FilterIDType> typeList;

	// this string is used to pass back to the framework error messages in case of failure of a filter apply.
	QString errorMessage;
};

#define FILTER_PLUGIN_INTERFACE_IID  "vcg.meshlab.FilterPluginInterface/1.0"
Q_DECLARE_INTERFACE(FilterPluginInterface, FILTER_PLUGIN_INTERFACE_IID)

#endif // MESHLAB_FILTER_PLUGIN_INTERFACE_H
