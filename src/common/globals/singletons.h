/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_SINGLETONS_H
#define MESHLAB_SINGLETONS_H

class RichParameterList;
class PluginManager;

namespace meshlab {

/**
 * @brief The MeshLabSingletons class
 * This class contains all the singleton instances used on MeshLab.
 * - pluginManager
 * - defaultGlobalParameterList
 */
class MeshLabSingletons
{
public:
	/** Singleton Instances **/
	static PluginManager& pluginManagerInstance(bool verbose = true);

	MeshLabSingletons(MeshLabSingletons const&) = delete;
	void operator=(MeshLabSingletons const&) = delete;
private:
	MeshLabSingletons();
};

}

#endif // MESHLAB_SINGLETONS_H
