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

#ifndef MESHLAB_GENERIC_CONTAINER_ITERATOR_H
#define MESHLAB_GENERIC_CONTAINER_ITERATOR_H

#include <vector>

template <typename PluginType>
class ConstPluginIterator
{
public:
	using Container         = std::vector<PluginType*>;
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = PluginType*;
	using pointer           = typename Container::const_iterator;
	using reference         = PluginType*&;

	ConstPluginIterator(
			const Container& c,
			const pointer& it,
			bool iterateAlsoDisabledPlugins = false)
		: c(c), m_ptr(it), iterateAlsoDisabledPlugins(iterateAlsoDisabledPlugins)
	{
		if (m_ptr != c.end() && !iterateAlsoDisabledPlugins && !(*m_ptr)->isEnabled())
			++(*this);
	}
	value_type operator*() const {return *m_ptr; }
	pointer operator->() { return m_ptr; }
	ConstPluginIterator& operator++() {
		if (iterateAlsoDisabledPlugins)
			m_ptr++;
		else {
			do {
				m_ptr++;
			} while((m_ptr != c.end()) && !(*m_ptr)->isEnabled());
		}
		return *this;
	}
	ConstPluginIterator operator++(int) {ConstPluginIterator tmp = *this; ++(*this); return tmp; }
	friend bool operator== (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr == b.m_ptr; };
	friend bool operator!= (const ConstPluginIterator& a, const ConstPluginIterator& b) { return a.m_ptr != b.m_ptr; };

private:
	const Container& c;
	pointer m_ptr;
	bool iterateAlsoDisabledPlugins;
};

#endif // MESHLAB_GENERIC_CONTAINER_ITERATOR_H
