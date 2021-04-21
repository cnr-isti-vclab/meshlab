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

#ifndef MESHLAB_DECORATE_PLUGIN_CONTAINER_H
#define MESHLAB_DECORATE_PLUGIN_CONTAINER_H

#include "../interfaces/decorate_plugin.h"
#include "generic_container_iterator.h"

/**
 * @brief The DecoratePluginContainer class allows to organize
 * all the decorate plugins contained in the PluginManager.
 *
 * Note: plugins are not owned by this container, but by the PluginManager,
 * since each plugin can inherit from more than one PluginInterface.
 */
class DecoratePluginContainer
{
public:
	class DecoratePluginRangeIterator;
	DecoratePluginContainer();

	void clear();
	void pushDecoratePlugin(DecoratePlugin* iDecorate);
	void eraseDecoratePlugin(DecoratePlugin* iDecorate);

	DecoratePlugin* decoratePlugin(const QString& name);

	DecoratePluginRangeIterator decoratePluginIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	std::vector<DecoratePlugin*> decoratePlugins;
};

class DecoratePluginContainer::DecoratePluginRangeIterator
{
	friend class DecoratePluginContainer;
public:
	ConstPluginIterator<DecoratePlugin> begin() {return ConstPluginIterator<DecoratePlugin>(pm->decoratePlugins, pm->decoratePlugins.begin(), b);}
	ConstPluginIterator<DecoratePlugin> end()   {return ConstPluginIterator<DecoratePlugin>(pm->decoratePlugins, pm->decoratePlugins.end(), b);}
private:
	DecoratePluginRangeIterator(const DecoratePluginContainer* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const DecoratePluginContainer* pm;
	bool b;
};

#endif // MESHLAB_DECORATE_PLUGIN_CONTAINER_H
