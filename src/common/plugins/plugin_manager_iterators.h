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

class ConstPluginIterator 
{
public:
	using Container         = std::vector<PluginFileInterface*>;
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = PluginFileInterface*;
	using pointer           = Container::const_iterator;  
	using reference         = PluginFileInterface*&;
	
	ConstPluginIterator(const Container& c, const pointer& it) : c(c), m_ptr(it){}
	value_type operator*() const {return *m_ptr; } 
	pointer operator->() { return m_ptr; }
	ConstPluginIterator& operator++() { m_ptr++; return *this; } 
	ConstPluginIterator operator++(int) { ConstPluginIterator tmp = *this; ++(*this); return tmp; }
	friend bool operator== (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr == b.m_ptr; };
	friend bool operator!= (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr != b.m_ptr; };
	
private:
	const Container& c;
	pointer m_ptr;
};

/** Range iterators **/

class PluginManager::PluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator begin() {return ConstPluginIterator(pm->allPlugins, pm->allPlugins.begin());}
	ConstPluginIterator end()   {return ConstPluginIterator(pm->allPlugins, pm->allPlugins.end());}
private:
	PluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::FilterPluginRangeIterator
{
	friend class PluginManager;
public:
	std::vector<FilterPluginInterface*>::const_iterator begin() {return pm->filterPlugins.begin();}
	std::vector<FilterPluginInterface*>::const_iterator end() {return pm->filterPlugins.end();}
private:
	FilterPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::IOMeshPluginIterator
{
	friend class PluginManager;
public:
	std::vector<IOMeshPluginInterface*>::const_iterator begin() {return pm->ioMeshPlugins.begin();}
	std::vector<IOMeshPluginInterface*>::const_iterator end() {return pm->ioMeshPlugins.end();}
private:
	IOMeshPluginIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::IORasterPluginIterator
{
	friend class PluginManager;
public:
	std::vector<IORasterPluginInterface*>::const_iterator begin() {return pm->ioRasterPlugins.begin();}
	std::vector<IORasterPluginInterface*>::const_iterator end() {return pm->ioRasterPlugins.end();}
private:
	IORasterPluginIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::RenderPluginRangeIterator
{
	friend class PluginManager;
public:
	std::vector<RenderPluginInterface*>::const_iterator begin() {return pm->renderPlugins.begin();}
	std::vector<RenderPluginInterface*>::const_iterator end() {return pm->renderPlugins.end();}
private:
	RenderPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::DecoratePluginRangeIterator
{
	friend class PluginManager;
public:
	std::vector<DecoratePluginInterface*>::const_iterator begin() {return pm->decoratePlugins.begin();}
	std::vector<DecoratePluginInterface*>::const_iterator end() {return pm->decoratePlugins.end();}
private:
	DecoratePluginRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

class PluginManager::EditPluginFactoryRangeIterator
{
	friend class PluginManager;
public:
	std::vector<EditPluginInterfaceFactory*>::const_iterator begin() {return pm->editPlugins.begin();}
	std::vector<EditPluginInterfaceFactory*>::const_iterator end() {return pm->editPlugins.end();}
private:
	EditPluginFactoryRangeIterator(const PluginManager* pm) : pm(pm) {}
	const PluginManager* pm;
};

#endif // MESHLAB_PLUGIN_MANAGER_ITERATORS_H
