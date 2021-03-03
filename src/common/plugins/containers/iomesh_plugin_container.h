/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#ifndef MESHLAB_IOMESH_PLUGIN_CONTAINER_H
#define MESHLAB_IOMESH_PLUGIN_CONTAINER_H

#include "../interfaces/iomesh_plugin.h"
#include "generic_container_iterator.h"

/**
 * @brief The IOMeshPluginContainer class allows to organize
 * all the IOMesh plugins contained in the PluginManager.
 *
 * Note: plugins are not owned by this container, but by the PluginManager,
 * since each plugin can inherit from more than one PluginInterface.
 */
class IOMeshPluginContainer
{
public:
	class IOMeshPluginRangeIterator;
	IOMeshPluginContainer();

	size_t size() const;
	void clear();
	void pushIOMeshPlugin(IOMeshPlugin* iIOMesh);
	void eraseIOMeshPlugin(IOMeshPlugin* iIOMesh);

	bool isInputMeshFormatSupported(const QString& inputFormat) const;
	bool isOutputMeshFormatSupported(const QString& outputFormat) const;
	IOMeshPlugin* inputMeshPlugin(const QString& inputFormat) const;
	IOMeshPlugin* outputMeshPlugin(const QString& outputFormat) const;

	QStringList inputMeshFormatList() const;
	QStringList outputMeshFormatList() const;

	IOMeshPluginRangeIterator ioMeshPluginIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	std::vector<IOMeshPlugin*> ioMeshPlugins;
	QMap<QString,IOMeshPlugin*> inputMeshFormatToPluginMap;
	QMap<QString,IOMeshPlugin*> outputMeshFormatToPluginMap;
};

class IOMeshPluginContainer::IOMeshPluginRangeIterator
{
	friend class IOMeshPluginContainer;
public:
	ConstPluginIterator<IOMeshPlugin> begin();
	ConstPluginIterator<IOMeshPlugin> end();
private:
	IOMeshPluginRangeIterator(
			const IOMeshPluginContainer* pm,
			bool iterateAlsoDisabledPlugins = false);
	const IOMeshPluginContainer* pm;
	bool b;
};

#endif // MESHLAB_IOMESH_PLUGIN_CONTAINER_H
