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

#ifndef MESHLAB_RENDER_PLUGIN_CONTAINER_H
#define MESHLAB_RENDER_PLUGIN_CONTAINER_H

#include "../interfaces/render_plugin_interface.h"
#include "generic_container_iterator.h"

/**
 * @brief The RenderPluginContainer class allows to organize
 * all the render plugins contained in the PluginManager.
 *
 * Note: plugins are not owned by this container, but by the PluginManager,
 * since each plugin can inherit from more than one PluginInterface.
 */
class RenderPluginContainer
{
public:
	class RenderPluginRangeIterator;
	RenderPluginContainer();

	void clear();
	void pushRenderPlugin(RenderPluginInterface* iRender);
	void eraseRenderPlugin(RenderPluginInterface* iRender);

	RenderPluginRangeIterator renderPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
private:
	std::vector<RenderPluginInterface*> renderPlugins;
};

class RenderPluginContainer::RenderPluginRangeIterator
{
	friend class RenderPluginContainer;
public:
	ConstPluginIterator<RenderPluginInterface> begin();
	ConstPluginIterator<RenderPluginInterface> end();
private:
	RenderPluginRangeIterator(
			const RenderPluginContainer* pm,
			bool iterateAlsoDisabledPlugins = false);
	const RenderPluginContainer* pm;
	bool b;
};

#endif // MESHLAB_RENDER_PLUGIN_CONTAINER_H
