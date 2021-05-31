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

#ifndef MESHLAB_LOAD_SAVE_H
#define MESHLAB_LOAD_SAVE_H

#include "../ml_shared_data_context/ml_shared_data_context.h"
#include "../plugins/interfaces/io_plugin.h"

/**
 * Utility functions to load/save meshes using plugins loaded in the plugin
 * manager.
 */

namespace meshlab {

void loadMesh(
		const QString& fileName,
		IOPlugin* ioPlugin,
		const RichParameterList& prePar,
		const std::list<MeshModel*>& meshList,
		std::list<int>& maskList,
		vcg::CallBackPos *cb);

void loadMeshWithStandardParameters(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos *cb);

void reloadMesh(
		const QString& filename,
		const std::list<MeshModel*>& meshList,
		vcg::CallBackPos* cb);

void loadRaster(
		const QString& filename,
		RasterModel& rm,
		vcg::CallBackPos *cb);

}

#endif // MESHLAB_LOAD_SAVE_H
