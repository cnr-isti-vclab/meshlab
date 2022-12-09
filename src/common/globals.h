/*****************************************************************************
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

#ifndef MESHLAB_GLOBALS_H
#define MESHLAB_GLOBALS_H

#define meshlab_xstr(a) mlstringify(a)
#define mlstringify(a) #a

#include <QString>
#ifdef MESHLAB_IS_NIGHTLY
#include <QFile>
#include <QTextStream>
#endif

#ifndef MESHLAB_VERSION
#error "MESHLAB_VERSION needs to be defined!"
#endif
#ifndef MESHLAB_SCALAR
#error "MESHLAB_SCALAR needs to be defined!"
#endif

class RichParameterList;
class PluginManager;
class ActionSearcher;

namespace meshlab {

QString defaultPluginPath();
QString defaultShadersPath();
QString logDebugFileName();

RichParameterList& defaultGlobalParameterList();
PluginManager&     pluginManagerInstance();
ActionSearcher&    actionSearcherInstance();

// keep these functions inlined please
// each plugin that uses them need to have their own definition
// plugins cannot link them!!
inline std::string meshlabVersion()
{
	return std::string(meshlab_xstr(MESHLAB_VERSION));
}

std::string meshlabCompleteVersion();

inline bool builtWithDoublePrecision()
{
	return std::string(meshlab_xstr(MESHLAB_SCALAR)) == std::string("double");
}

} // namespace meshlab

namespace pymeshlab {
class FunctionSet;

FunctionSet& functionSetInstance();
} // namespace pymeshlab

#endif // MESHLAB_GLOBALS_H
