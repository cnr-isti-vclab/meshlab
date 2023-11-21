/*****************************************************************************
 * MeshLab - Alpha Wrap plugin                                       o o     *
 * by PTC, based on CGAL                                           o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2023                                                \/)\/    *
 * PTC                                                             /\/|      *
 * Lex van der Sluijs                                                 |      *
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

#ifndef MESHLAB_FILTER_MESH_ALPHAWRAP_H
#define MESHLAB_FILTER_MESH_ALPHAWRAP_H

// from meshlab common, include the abstract class file of filter plugins
#include <common/plugins/interfaces/filter_plugin.h>

/**
 * @brief The FilterMeshAlphaWrap class
 * This Meshlab plugin implements the Alpha Wrap algorithm from CGAL.
 *
 * It is very useful for making scaled-down CAD models 3D printable.
 * It is also able to robustly make a watertight mesh based on a mesh with degenerate geometries.
 *
 */
class FilterMeshAlphaWrap : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	// enum used to give an ID to every filter implemented in the plugin
	enum FilterIds { MESH_ALPHA_WRAP };

	FilterMeshAlphaWrap();

	QString pluginName() const;
	QString vendor() const;

	QString filterName(ActionIDType filter) const;
	QString pythonFilterName(ActionIDType f) const;

	QString     filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	// int getPreConditions(const QAction *) const;
	// int postCondition(const QAction* ) const;
	RichParameterList               initParameterList(const QAction*, const MeshDocument& /*m*/);
	std::map<std::string, QVariant> applyFilter(
		const QAction*           action,
		const RichParameterList& params,
		MeshDocument&            md,
		unsigned int&            postConditionMask,
		vcg::CallBackPos*        cb);

private:

};

#endif // MESHLAB_FILTER_MESH_ALPHAWRAP_H
