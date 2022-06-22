/*****************************************************************************
 * MeshLab                                                           o o     *
 * An extendible mesh processor                                    o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2022                                           \/)\/    *
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

#include "filter_history.h"

FilterHistory::iterator FilterHistory::begin()
{
	return history.begin();
}

FilterHistory::iterator FilterHistory::end()
{
	return history.end();
}

FilterHistory::const_iterator FilterHistory::begin() const
{
	return history.begin();
}

FilterHistory::const_iterator FilterHistory::end() const
{
	return history.end();
}
