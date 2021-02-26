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
#include "containers/generic_container_iterator.h"

/** Range iterators **/

class PluginManager::PluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<PluginFileInterface> begin() {return ConstPluginIterator<PluginFileInterface>(pm->allPlugins, pm->allPlugins.begin(), b);}
	ConstPluginIterator<PluginFileInterface> end()   {return ConstPluginIterator<PluginFileInterface>(pm->allPlugins, pm->allPlugins.end(), b);}
private:
	PluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const PluginManager* pm;
	bool b;
};

//class PluginManager::FilterPluginRangeIterator
//{
//	friend class PluginManager;
//public:
//	ConstPluginIterator<FilterPluginInterface> begin() {return ConstPluginIterator<FilterPluginInterface>(pm->filterPlugins, pm->filterPlugins.begin(), b);}
//	ConstPluginIterator<FilterPluginInterface> end()   {return ConstPluginIterator<FilterPluginInterface>(pm->filterPlugins, pm->filterPlugins.end(), b);}
//private:
//	FilterPluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
//	const PluginManager* pm;
//	bool b;
//};

class PluginManager::IOMeshPluginIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<IOMeshPluginInterface> begin() {return ConstPluginIterator<IOMeshPluginInterface>(pm->ioMeshPlugins, pm->ioMeshPlugins.begin(), b);}
	ConstPluginIterator<IOMeshPluginInterface> end()   {return ConstPluginIterator<IOMeshPluginInterface>(pm->ioMeshPlugins, pm->ioMeshPlugins.end(), b);}
	//std::vector<IOMeshPluginInterface*>::const_iterator begin() {return pm->ioMeshPlugins.begin();}
	//std::vector<IOMeshPluginInterface*>::const_iterator end() {return pm->ioMeshPlugins.end();}
private:
	IOMeshPluginIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const PluginManager* pm;
	bool b;
};

class PluginManager::IORasterPluginIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<IORasterPluginInterface> begin() {return ConstPluginIterator<IORasterPluginInterface>(pm->ioRasterPlugins, pm->ioRasterPlugins.begin(), b);}
	ConstPluginIterator<IORasterPluginInterface> end()   {return ConstPluginIterator<IORasterPluginInterface>(pm->ioRasterPlugins, pm->ioRasterPlugins.end(), b);}
	//std::vector<IORasterPluginInterface*>::const_iterator begin() {return pm->ioRasterPlugins.begin();}
	//std::vector<IORasterPluginInterface*>::const_iterator end() {return pm->ioRasterPlugins.end();}
private:
	IORasterPluginIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins)  {}
	const PluginManager* pm;
	bool b;
};

class PluginManager::RenderPluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<RenderPluginInterface> begin() {return ConstPluginIterator<RenderPluginInterface>(pm->renderPlugins, pm->renderPlugins.begin(), b);}
	ConstPluginIterator<RenderPluginInterface> end()   {return ConstPluginIterator<RenderPluginInterface>(pm->renderPlugins, pm->renderPlugins.end(), b);}
	//std::vector<RenderPluginInterface*>::const_iterator begin() {return pm->renderPlugins.begin();}
	//std::vector<RenderPluginInterface*>::const_iterator end() {return pm->renderPlugins.end();}
private:
	RenderPluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const PluginManager* pm;
	bool b;
};

class PluginManager::DecoratePluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<DecoratePluginInterface> begin() {return ConstPluginIterator<DecoratePluginInterface>(pm->decoratePlugins, pm->decoratePlugins.begin(), b);}
	ConstPluginIterator<DecoratePluginInterface> end()   {return ConstPluginIterator<DecoratePluginInterface>(pm->decoratePlugins, pm->decoratePlugins.end(), b);}
	//std::vector<DecoratePluginInterface*>::const_iterator begin() {return pm->decoratePlugins.begin();}
	//std::vector<DecoratePluginInterface*>::const_iterator end() {return pm->decoratePlugins.end();}
private:
	DecoratePluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const PluginManager* pm;
	bool b;
};

class PluginManager::EditPluginFactoryRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<EditPluginInterfaceFactory> begin() {return ConstPluginIterator<EditPluginInterfaceFactory>(pm->editPlugins, pm->editPlugins.begin(), b);}
	ConstPluginIterator<EditPluginInterfaceFactory> end()   {return ConstPluginIterator<EditPluginInterfaceFactory>(pm->editPlugins, pm->editPlugins.end(), b);}
	//std::vector<EditPluginInterfaceFactory*>::const_iterator begin() {return pm->editPlugins.begin();}
	//std::vector<EditPluginInterfaceFactory*>::const_iterator end() {return pm->editPlugins.end();}
private:
	EditPluginFactoryRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false) : pm(pm), b(iterateAlsoDisabledPlugins) {}
	const PluginManager* pm;
	bool b;
};

#endif // MESHLAB_PLUGIN_MANAGER_ITERATORS_H
