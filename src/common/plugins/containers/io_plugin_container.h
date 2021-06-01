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

#ifndef MESHLAB_IO_PLUGIN_CONTAINER_H
#define MESHLAB_IO_PLUGIN_CONTAINER_H

#include "../interfaces/io_plugin.h"
#include "generic_container_iterator.h"

/**
 * @brief The IOPluginContainer class allows to organize
 * all the IO plugins contained in the PluginManager.
 *
 * Note: plugins are not owned by this container, but by the PluginManager,
 * since each plugin can inherit from more than one PluginInterface.
 */
class IOPluginContainer
{
public:
	class IOPluginRangeIterator;
	IOPluginContainer();

	size_t size() const;
	void clear();
	void pushIOPlugin(IOPlugin* iIO);
	void eraseIOPlugin(IOPlugin* iIO);

	bool isInputMeshFormatSupported(const QString& inputFormat) const;
	bool isOutputMeshFormatSupported(const QString& outputFormat) const;
	bool isInputImageFormatSupported(const QString& inputFormat) const;
	bool isOutputImageFormatSupported(const QString& outputFormat) const;
	IOPlugin* inputMeshPlugin(const QString& inputFormat) const;
	IOPlugin* outputMeshPlugin(const QString& outputFormat) const;
	IOPlugin* inputImagePlugin(const QString& inputFormat) const;
	IOPlugin* outputImagePlugin(const QString& outputFormat) const;

	QStringList inputMeshFormatList() const;
	QStringList outputMeshFormatList() const;
	QStringList inputImageFormatList() const;
	QStringList outputImageFormatList() const;

	IOPluginRangeIterator ioPluginIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	std::vector<IOPlugin*> ioPlugins;
	QMap<QString,IOPlugin*> inputMeshFormatToPluginMap;
	QMap<QString,IOPlugin*> outputMeshFormatToPluginMap;
	QMap<QString,IOPlugin*> inputImageFormatToPluginMap;
	QMap<QString,IOPlugin*> outputImageFormatToPluginMap;
};

class IOPluginContainer::IOPluginRangeIterator
{
	friend class IOPluginContainer;
public:
	ConstPluginIterator<IOPlugin> begin();
	ConstPluginIterator<IOPlugin> end();
private:
	IOPluginRangeIterator(
			const IOPluginContainer* pm,
			bool iterateAlsoDisabledPlugins = false);
	const IOPluginContainer* pm;
	bool b;
};

#endif // MESHLAB_IO_PLUGIN_CONTAINER_H
