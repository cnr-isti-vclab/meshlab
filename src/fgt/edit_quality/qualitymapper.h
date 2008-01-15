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
Log: qualityMapper.h
Revision 1.0 2008/01/03 MAL
Beginning

****************************************************************************/

#ifndef _QUALITY_MAPPER_PLUGIN_H_
#define _QUALITY_MAPPER_PLUGIN_H_

#include <QObject>
#include <QAction>
//#include <QActionGroup>
//#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

//#include <vcg/math/base.h>
#include <vcg/math/histogram.h>
//#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/color.h> //<-- contains VertexQuality method


class QualityMapperPlugin : public QObject, public MeshEditInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)

	QList <QAction *> actionList;

private:
	Histogramf _histogram;

public:
    QualityMapperPlugin();
    ~QualityMapperPlugin(){};

	virtual const QString Info(QAction *);
	// Generic Info about the plug in version and author.
    const PluginInfo &Info();
	QList<QAction *> actions() const ;

	// Called when the user press the first time the button 
    void StartEdit			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	// Called when the user press the second time the button 
    void EndEdit			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    void Decorate			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	QPoint cur;
	bool haveToPick;
	
	/*
	// The longer string describing each filtering action 
	// (this string is used in the About plug in dialog)
	const QString filterInfo(FilterIDType filter)
	{	return QString("This plug in colorizes mesh vertexes by Quality noting quality distribution and applying a custom color band");	}

	// The very short string describing each filtering action 
	// (this string is used also to define the menu entry)
	const QString filterName(FilterIDType filter)
	{	return QString("Colorize mesh vertexes by Quality following some rules");	}
	*/

/*
		// This function is called by the framework, for each action at the loading of the plugins.
		// it allows to add a list of global persistent parameters that can be changed from the meshlab itself.
		// If your plug ins/action has no GlobalParameter, do nothing.
		virtual void initGlobalParameterSet(QAction *, FilterParameterSet & / *globalparam* /) {}

	//TO BE IMPLEMENTED IF NECESSARY!

	// The FilterClass describes in which generic class of filters it fits. 
	// This choice affect the submenu in which each filter will be placed 
	// For example filters that perform an action only on the selection will be placed in the Selection Class Ê
	const FilterClass getClass(QAction *) { return MeshEditInterface::VertexColoring; } //modified by MAL


		// The filters can have some additional requirements on the mesh capabilities. 
		// For example if a filters requires Face-Face Adjacency you should re-implement 
		// this function making it returns MeshModel::MM_FACETOPO. 
		// The framework will ensure that the mesh has the requirements satisfied before invoking the applyFilter function
		virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}*/
	//TO BE IMPLEMENTED IF NECESSARY!
	
/*
	// The main function that applies the selected filter with the already stabilished parameters
	// This function is called by the framework after getting the user params 
	bool applyFilter(QAction * , MeshModel &, FilterParameterSet &, vcg::CallBackPos * );
	//TO BE IMPLEMENTED!


		//  this function returns true if the filter has parameters that must be filled with an automatically build dialog.
	bool autoDialog(QAction *) {return false;}
	//TO BE IMPLEMENTED IF NECESSARY!
	*/

	// This function is called to initialized the list of parameters. 
	// it is called by the auto dialog framework to know the list of parameters.
	void initParameterSet(QAction *action,MeshModel &meshModel, FilterParameterSet &parent);


/*
		//  this function returns true if the filter has a personally customized dialog..
		virtual bool customDialog(QAction *) {return false;}*/
	//TO BE IMPLEMENTED IF NECESSARY!


	// This function invokes the custom dialog of a filter and when has the params 
	// it notifies it to the main window with the collected parameters
	bool getParameters(QAction *action, QWidget * parent, MeshModel & meshModel, FilterParameterSet & params, MainWindowInterface *mw);
// 		{
// 			assert(mw);
// 			mw->executeFilter(action,params);
// 			return true;
// 		}
//		TO DELETE!?!?


/*
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
	virtual QList<FilterIDType> &types() { return typeList;}*/
	//TO BE IMPLEMENTED IF NECESSARY!
};

#endif
