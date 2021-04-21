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

#include "render_plugin_container.h"

RenderPluginContainer::RenderPluginContainer()
{
}

void RenderPluginContainer::clear()
{
	renderPlugins.clear();
}

void RenderPluginContainer::pushRenderPlugin(RenderPlugin* iRender)
{
	renderPlugins.push_back(iRender);
}

void RenderPluginContainer::eraseRenderPlugin(RenderPlugin* iRender)
{
	renderPlugins.erase(std::find(renderPlugins.begin(), renderPlugins.end(), iRender));
}

RenderPluginContainer::RenderPluginRangeIterator RenderPluginContainer::renderPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return RenderPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

ConstPluginIterator<RenderPlugin> RenderPluginContainer::RenderPluginRangeIterator::begin()
{
	return ConstPluginIterator<RenderPlugin>(pm->renderPlugins, pm->renderPlugins.begin(), b);
}

ConstPluginIterator<RenderPlugin> RenderPluginContainer::RenderPluginRangeIterator::end()
{
	return ConstPluginIterator<RenderPlugin>(pm->renderPlugins, pm->renderPlugins.end(), b);
}

RenderPluginContainer::RenderPluginRangeIterator::RenderPluginRangeIterator(
		const RenderPluginContainer* pm,
		bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
