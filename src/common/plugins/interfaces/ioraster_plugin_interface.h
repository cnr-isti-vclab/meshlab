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

#ifndef MESHLAB_IORASTER_PLUGIN_INTERFACE_H
#define MESHLAB_IORASTER_PLUGIN_INTERFACE_H

#include "meshlab_plugin.h"
#include "../../utilities/file_format.h"
#include "../../ml_document/raster_model.h"

class IORasterPluginInterface : virtual public MeshLabPlugin
{
public:
	IORasterPluginInterface() : MeshLabPlugin() {}
	virtual ~IORasterPluginInterface() {}
	
	virtual QList<FileFormat> importFormats() const = 0;
	
	virtual bool open(
		const QString& format,
		const QString& filename,
		RasterModel& rm,
		vcg::CallBackPos* cb = nullptr) = 0;

	/// This function is invoked by the framework when the import/export plugin fails to give some info to the user about the failure
	/// io plugins should avoid using QMessageBox for reporting errors.
	/// Failure should put some meaningful information inside the errorMessage string.
	const QString& errorMsg() const
	{
		return errorMessage;
	}
	void clearErrorString() 
	{
		errorMessage.clear();
	}
protected:
	// this string is used to pass back to the framework error messages in case of failure of a filter apply.
	// NEVER EVER use a msgbox to say something to the user.
	QString errorMessage;
};

#define IORASTER_PLUGIN_INTERFACE_IID "vcg.meshlab.IORasterPluginInterface/1.0"
Q_DECLARE_INTERFACE(IORasterPluginInterface, IORASTER_PLUGIN_INTERFACE_IID)

#endif // MESHLAB_IORASTER_PLUGIN_INTERFACE_H
