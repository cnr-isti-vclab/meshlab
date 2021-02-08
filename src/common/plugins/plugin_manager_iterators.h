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

#ifndef MESHLAB_PLUGIN_MANAGER_ITERATORS_H
#define MESHLAB_PLUGIN_MANAGER_ITERATORS_H

#include "plugin_manager.h"

class PluginManager::NamePluginPairRangeIterator
{
	friend class PluginManager;
public:
	std::map<QString, PluginFileInterface*>::const_iterator begin() {return pm->allPlugins.begin();}
	std::map<QString, PluginFileInterface*>::const_iterator end() {return pm->allPlugins.end();}
private:
	NamePluginPairRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::FilterPluginRangeIterator
{
	friend class PluginManager;
public:
	QVector<FilterPluginInterface*>::const_iterator begin() {return pm->filterPlugins.begin();}
	QVector<FilterPluginInterface*>::const_iterator end() {return pm->filterPlugins.end();}
private:
	FilterPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::IOMeshPluginIterator
{
	friend class PluginManager;
public:
	QVector<IOMeshPluginInterface*>::const_iterator begin() {return pm->ioMeshPlugins.begin();}
	QVector<IOMeshPluginInterface*>::const_iterator end() {return pm->ioMeshPlugins.end();}
private:
	IOMeshPluginIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::IORasterPluginIterator
{
	friend class PluginManager;
public:
	QVector<IORasterPluginInterface*>::const_iterator begin() {return pm->ioRasterPlugins.begin();}
	QVector<IORasterPluginInterface*>::const_iterator end() {return pm->ioRasterPlugins.end();}
private:
	IORasterPluginIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::RenderPluginRangeIterator
{
	friend class PluginManager;
public:
	QVector<RenderPluginInterface*>::const_iterator begin() {return pm->renderPlugins.begin();}
	QVector<RenderPluginInterface*>::const_iterator end() {return pm->renderPlugins.end();}
private:
	RenderPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::DecoratePluginRangeIterator
{
	friend class PluginManager;
public:
	QVector<DecoratePluginInterface*>::const_iterator begin() {return pm->decoratePlugins.begin();}
	QVector<DecoratePluginInterface*>::const_iterator end() {return pm->decoratePlugins.end();}
private:
	DecoratePluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::EditPluginFactoryRangeIterator
{
	friend class PluginManager;
public:
	QVector<EditPluginInterfaceFactory*>::const_iterator begin() {return pm->editPlugins.begin();}
	QVector<EditPluginInterfaceFactory*>::const_iterator end() {return pm->editPlugins.end();}
private:
	EditPluginFactoryRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

#endif // MESHLAB_PLUGIN_MANAGER_ITERATORS_H
