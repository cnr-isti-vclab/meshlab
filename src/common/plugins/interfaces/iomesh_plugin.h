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

#ifndef MESHLAB_IOMESH_PLUGIN_H
#define MESHLAB_IOMESH_PLUGIN_H

#include <wrap/callback.h>

#include "meshlab_plugin_logger.h"
#include "meshlab_plugin.h"
#include "../../utilities/file_format.h"

/** 
 * @brief The IOMeshPlugin is the base class for all the single mesh loading plugins.
 */
class IOMeshPlugin : virtual public MeshLabPlugin, virtual public MeshLabPluginLogger
{
public:
	IOMeshPlugin() : MeshLabPluginLogger() {  }
	virtual ~IOMeshPlugin() {}

	virtual std::list<FileFormat> importFormats() const = 0;
	virtual QList<FileFormat> exportFormats() const = 0;

	// This function is called to initialize the list of additional parameters that a OPENING filter could require
	// it is called by the framework BEFORE the actual mesh loading to perform to determine how parse the input file
	// The instanced parameters are then passed to the open at the loading time.
	// Typical example of use to decide what subportion of a mesh you have to load.
	// If you do not need any additional processing simply do not override this and ignore the parameterSet in the open
	virtual void initPreOpenParameter(const QString &/*format*/, const QString &/*fileName*/, RichParameterList & /*par*/) {}

	// This function is called to initialize the list of additional parameters that a OPENING filter could require
	// it is called by the framework AFTER the mesh is already loaded to perform more or less standard processing on the mesh.
	// typical example: unifying vertices in stl models.
	// If you do not need any additional processing do nothing.
	virtual void initOpenParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterList & /*par*/) {}

	// This is the corresponding function that is called after the mesh is loaded with the initialized parameters
	virtual void applyOpenParameter(const QString &/*format*/, MeshModel &/*m*/, const RichParameterList &/*par*/) {}

	// This function is called to initialize the list of additional parameters that a SAVING filter could require
	// it is called by the framework after the mesh is loaded to perform more or less standard processing on the mesh.
	// typical example: ascii or binary format for ply or stl
	// If you do not need any additional parameter simply do nothing.
	virtual void initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterList & /*par*/) {}


	virtual void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const = 0;

	/// callback used to actually load a mesh from a file
	virtual bool open(
		const QString &format, /// the extension of the format e.g. "PLY"
		const QString &fileName, /// The name of the file to be opened
		MeshModel &m, /// The mesh that is filled with the file content
		int &mask, /// a bit mask that will be filled reporting what kind of data we have found in the file (per vertex color, texture coords etc)
		const RichParameterList & par, /// The parameters that have been set up in the initPreOpenParameter()
		vcg::CallBackPos *cb = nullptr, /// standard callback for reporting progress in the loading
		QWidget *parent = nullptr) = 0; /// you should not use this...

	virtual bool save(
		const QString &format, // the extension of the format e.g. "PLY"
		const QString &fileName,
		MeshModel &m,
		const int mask,// a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
		const RichParameterList & par,
		vcg::CallBackPos *cb = 0,
		QWidget *parent = 0) = 0;

	/// This function is invoked by the framework when the import/export plugin fails to give some info to the user about the failure
	/// io plugins should avoid using QMessageBox for reporting errors.
	/// Failure should put some meaningful information inside the errorMessage string.
	const QString& errorMsg() const 
	{
		return this->errorMessage;
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

#define IOMESH_PLUGIN_IID "vcg.meshlab.IOMeshPlugin/1.0"
Q_DECLARE_INTERFACE(IOMeshPlugin, IOMESH_PLUGIN_IID)

#endif // MESHLAB_IOMESH_PLUGIN_H
