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

#include "edit_plugin_container.h"

EditPluginContainer::EditPluginContainer()
{
}

void EditPluginContainer::clear()
{
	editPlugins.clear();
}

void EditPluginContainer::pushEditPlugin(EditPlugin* iEditFactory)
{
	editPlugins.push_back(iEditFactory);
}

void EditPluginContainer::eraseEditPlugin(EditPlugin* iEditFactory)
{
	editPlugins.erase(std::find(editPlugins.begin(), editPlugins.end(), iEditFactory));
}

EditPluginContainer::EditPluginFactoryRangeIterator EditPluginContainer::editPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return EditPluginFactoryRangeIterator(this, iterateAlsoDisabledPlugins);
}

ConstPluginIterator<EditPlugin> EditPluginContainer::EditPluginFactoryRangeIterator::begin()
{
	return ConstPluginIterator<EditPlugin>(pm->editPlugins, pm->editPlugins.begin(), b);
}

ConstPluginIterator<EditPlugin> EditPluginContainer::EditPluginFactoryRangeIterator::end()
{
	return ConstPluginIterator<EditPlugin>(pm->editPlugins, pm->editPlugins.end(), b);
}

EditPluginContainer::EditPluginFactoryRangeIterator::EditPluginFactoryRangeIterator(
		const EditPluginContainer* pm,
		bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
