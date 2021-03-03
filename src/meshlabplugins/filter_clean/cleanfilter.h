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

#ifndef __CLEAN_FILTER_H__
#define __CLEAN_FILTER_H__

#include <QObject>
#include <common/plugins/interfaces/filter_plugin.h>

class CleanFilter : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
    /* naming convention :
    - FP -> Filter Plugin
    - name of the plugin separated by _
    */
    enum { 
        FP_BALL_PIVOTING, 
        FP_REMOVE_ISOLATED_COMPLEXITY, 
        FP_REMOVE_ISOLATED_DIAMETER, 
        FP_REMOVE_WRT_Q,
        FP_REMOVE_TVERTEX_FLIP,
        FP_SNAP_MISMATCHED_BORDER,
        FP_REMOVE_TVERTEX_COLLAPSE,
        FP_REMOVE_FOLD_FACE,
        FP_REMOVE_DUPLICATE_FACE,
        FP_REMOVE_NON_MANIF_EDGE,
		FP_REMOVE_NON_MANIF_EDGE_SPLIT,
        FP_REMOVE_NON_MANIF_VERT,
		FP_REMOVE_UNREFERENCED_VERTEX,
		FP_REMOVE_DUPLICATED_VERTEX,
		FP_REMOVE_FACE_ZERO_AREA,
        FP_MERGE_CLOSE_VERTEX,
        FP_MERGE_WEDGE_TEX,
        FP_COMPACT_VERT,
        FP_COMPACT_FACE
    } ;

    CleanFilter();
    ~CleanFilter();

    QString pluginName() const;
    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual FilterClass getClass(const QAction*) const;
    virtual int getRequirements(const QAction*);
    int postCondition(const QAction* ) const;
    int getPreConditions(const QAction *) const {    return MeshModel::MM_NONE; }
    virtual void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
    virtual bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
    FILTER_ARITY filterArity(const QAction *) const {return SINGLE_MESH;}
};






#endif
