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

#ifndef MESHLAB_IORASTER_PLUGIN_CONTAINER_H
#define MESHLAB_IORASTER_PLUGIN_CONTAINER_H

#include "../interfaces/ioraster_plugin.h"
#include "generic_container_iterator.h"

/**
 * @brief The IORasterPluginContainer class allows to organize
 * all the IORaster plugins contained in the PluginManager.
 *
 * Note: plugins are not owned by this container, but by the PluginManager,
 * since each plugin can inherit from more than one PluginInterface.
 */
class IORasterPluginContainer
{
public:
	class IORasterPluginRangeIterator;
	IORasterPluginContainer();

	size_t size() const;
	void clear();
	void pushIORasterPlugin(IORasterPlugin* iIORaster);
	void eraseIORasterPlugin(IORasterPlugin* iIORaster);

	IORasterPlugin* inputRasterPlugin(const QString inputFormat) const;
	bool isInputRasterFormatSupported(const QString inputFormat) const;

	QStringList inputRasterFormatList() const;

	IORasterPluginRangeIterator ioRasterPluginIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	std::vector<IORasterPlugin*> ioRasterPlugins;
	QMap<QString, IORasterPlugin*> inputRasterFormatToPluginMap;
};

class IORasterPluginContainer::IORasterPluginRangeIterator
{
	friend class IORasterPluginContainer;
public:
	ConstPluginIterator<IORasterPlugin> begin();
	ConstPluginIterator<IORasterPlugin> end();
private:
	IORasterPluginRangeIterator(
			const IORasterPluginContainer* pm,
			bool iterateAlsoDisabledPlugins = false);
	const IORasterPluginContainer* pm;
	bool b;
};

#endif // MESHLAB_IORASTER_PLUGIN_CONTAINER_H
