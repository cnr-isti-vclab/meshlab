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

#include "decorate_plugin_container.h"

DecoratePluginContainer::DecoratePluginContainer()
{
}

void DecoratePluginContainer::clear()
{
	decoratePlugins.clear();
}

void DecoratePluginContainer::pushDecoratePlugin(DecoratePluginInterface* iDecorate)
{
	decoratePlugins.push_back(iDecorate);
	for(QAction *decoratorAction : iDecorate->actions()) {
		iDecorate->initGlobalParameterList(decoratorAction, meshlab::defaultGlobalParameterList());
	}
}

void DecoratePluginContainer::eraseDecoratePlugin(DecoratePluginInterface* iDecorate)
{
	decoratePlugins.erase(std::find(decoratePlugins.begin(), decoratePlugins.end(), iDecorate));
}

DecoratePluginInterface* DecoratePluginContainer::decoratePlugin(const QString& name)
{
	for(DecoratePluginInterface *tt : decoratePlugins) {
		for( QAction *ac: tt->actions())
			if( name == tt->decorationName(ac) ) return tt;
	}
	return nullptr;
}

DecoratePluginContainer::DecoratePluginRangeIterator DecoratePluginContainer::decoratePluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return DecoratePluginRangeIterator(this, iterateAlsoDisabledPlugins);
}
