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

#ifndef MESHLAB_GLOBALS_H
#define MESHLAB_GLOBALS_H

#define meshlab_xstr(a) mlstringify(a)
#define mlstringify(a) #a

#include <QString>

class RichParameterList;
class PluginManager;

namespace meshlab {

QString defaultPluginPath();
QString defaultShadersPath();

RichParameterList& defaultGlobalParameterList();
PluginManager& pluginManagerInstance();

//keep this functions inlined please
inline std::string meshlabVersion() 
{
	return std::string(meshlab_xstr(MESHLAB_VERSION));
};

inline bool builtWithDoublePrecision()
{
	return std::string(meshlab_xstr(MESHLAB_SCALAR)) == std::string("double");
}

}

#endif // MESHLAB_GLOBALS_H
