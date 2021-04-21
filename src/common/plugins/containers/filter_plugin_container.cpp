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

#include "filter_plugin_container.h"

FilterPluginContainer::FilterPluginContainer()
{
}

void FilterPluginContainer::clear()
{
	filterPlugins.clear();
	actionFilterMap.clear();
}

void FilterPluginContainer::pushFilterPlugin(
		FilterPlugin* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		filterAction->setData(QVariant(iFilter->pluginName()));
		actionFilterMap.insert(filterAction->text(), filterAction);
	}
	filterPlugins.push_back(iFilter);
}

void FilterPluginContainer::eraseFilterPlugin(
		FilterPlugin* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		actionFilterMap.remove(filterAction->text());
	}
	filterPlugins.erase(std::find(filterPlugins.begin(), filterPlugins.end(), iFilter));
}

QAction* FilterPluginContainer::filterAction(const QString& name)
{
	auto it = actionFilterMap.find(name);
	if (it != actionFilterMap.end())
		return it.value();
	else
		return nullptr;
}

FilterPluginContainer::FilterPluginRangeIterator FilterPluginContainer::filterPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return FilterPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

ConstPluginIterator<FilterPlugin> FilterPluginContainer::FilterPluginRangeIterator::begin()
{
	return ConstPluginIterator<FilterPlugin>(pm->filterPlugins, pm->filterPlugins.begin(), b);
}

ConstPluginIterator<FilterPlugin> FilterPluginContainer::FilterPluginRangeIterator::end()
{
	return ConstPluginIterator<FilterPlugin>(pm->filterPlugins, pm->filterPlugins.end(), b);
}

FilterPluginContainer::FilterPluginRangeIterator::FilterPluginRangeIterator(
		const FilterPluginContainer* pm,
		bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
