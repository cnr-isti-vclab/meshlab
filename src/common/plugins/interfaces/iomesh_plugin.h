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

	/**
	 * @brief The importFormats function returns a list of all the
	 * input file formats supported by the plugin.
	 * This function must be implemented on any IOMesh plugin.
	 */
	virtual std::list<FileFormat> importFormats() const = 0;

	/**
	 * @brief The exportFormats function returns a list of all the
	 * output file formats supported by the plugin.
	 * This function must be implemented on any IOMesh plugin.
	 */
	virtual std::list<FileFormat> exportFormats() const = 0;

	/**
	 * @brief The initPreOpenParameter function is called to initialize the list
	 * of additional parameters that a OPENING filter could require. It is
	 * called by the framework BEFORE the actual mesh loading to determine how
	 * to parse the input file. The instanced parameters are then passed to the
	 * open at the loading time.
	 * Typical example of use to decide what subportion of a mesh you have to load.
	 * If you do not need any additional processing simply do not override this
	 * and ignore the parameterList in the open member function
	 */
	virtual void initPreOpenParameter(
			const QString& /*format*/,
			RichParameterList& /*parameters*/)
	{
	}

	/**
	 * @brief The initOpenParameter function is called to initialize the list
	 * of additional parameters that a OPENING filter could require.
	 * It is called by the framework AFTER the mesh is already loaded to
	 * perform more or less standard processing on the mesh.
	 * typical example: unifying vertices in stl models.
	 * If you do not need any additional processing do nothing.
	 */
	virtual void initOpenParameter(
			const QString& /*format*/,
			MeshModel& /*m*/,
			RichParameterList& /*parameters*/)
	{
	}

	/**
	 * @brief The applyOpenParameter function is the corrispondent function to
	 * the initOpenParameter. It is called after the mesh is loaded,
	 * and it should apply the given parameters to the loaded mesh.
	 * If you haven't implemented the initOpenParameter member function,
	 * you do not need to implement this.
	 */
	virtual void applyOpenParameter(
			const QString& /*format*/,
			MeshModel& /*m*/,
			const RichParameterList& /*parameters*/)
	{
	}

	/**
	 * @brief The initSaveParameter function is called to initialize the list
	 * of additional parameters that a SAVING filter could require. It is called
	 * by the framework after the output format is selected by the user.
	 * typical example: ascii or binary format for ply or stl
	 * If you do not need any additional parameters, simply do not implement
	 * this function.
	 */
	virtual void initSaveParameter(
			const QString& /*format*/,
			const MeshModel& /*m*/,
			RichParameterList& /*par*/)
	{
	}

	/**
	 * @brief The exportMaskCapability function tells to the framework which
	 * export capabilities are supported by the given format (e.g. if the format
	 * supports saving face colors, vertex quality...).
	 * It also tells to the framework which of these export capabilities are
	 * set by default.
	 */
	virtual void exportMaskCapability(
			const QString &format,
			int& capability,
			int& defaultBits) const = 0;

	/**
	 * @brief The open function is called by the framework everytime a mesh is loaded.
	 * @param format: the extension of the format e.g. "PLY"
	 * @param fileName: the name of the file to be opened (including its path)
	 * @param m: the mesh that is filled with the file content
	 * @param mask: a bit mask that will be filled reporting what kind of data we have found in the file (per vertex color, texture coords etc)
	 * @param par: the parameters that have been set up in the initPreOpenParameter()
	 * @param cb: standard callback for reporting progress in the loading
	 */
	virtual void open(
		const QString &format,
		const QString &fileName,
		MeshModel &m,
		int &mask,
		const RichParameterList & par,
		vcg::CallBackPos *cb = nullptr) = 0;

	/**
	 * @brief The save function is called by the framework everytime a mesh is saved.
	 * @param format: the extension of the format e.g. "PLY"
	 * @param fileName: the name of the file on which save the mesh m (including its path)
	 * @param m: the mesh to be saved in the file
	 * @param mask: a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
	 * @param par: the parameters that have been set up in the initSaveParameter()
	 * @param cb: standard callback for reporting progress in the saving
	 */
	virtual void save(
		const QString &format,
		const QString &fileName,
		MeshModel &m, /** NOTE: this is going to be const MeshModel&: try to use only const functions!! **/
		const int mask,
		const RichParameterList & par,
		vcg::CallBackPos *cb) = 0;

	/**
	 * @brief The reportWarning function should be used everytime that a non-critical
	 * error while loading or saving a file happens. This function appends the
	 * warning message passed as parameter to a string that will be shown
	 * by the framework at the end of the execution of the load/save function
	 * @param warningMessage
	 */
	void reportWarning(const QString& warningMessage) const
	{
		MeshLabPluginLogger::log(GLLogStream::WARNING, warningMessage.toStdString());
		warnString += "\n" + warningMessage;
	}

	/**
	 * @brief call this function in any of the import functions
	 * (initPreOpenParameters, load...) whenever you receive as parameter a
	 * format that is not supported by your plugin
	 */
	void wrongOpenFormat(const QString& format)
	{
		throw MLException("Internal error: unknown open format " + format + " to " + pluginName() + " plugin.");
	};

	/**
	 * @brief call this function in any of the export functions
	 * (exportMaskCapability, save...) whenever you receive as parameter a
	 * format that is not supported by your plugin
	 */
	void wrongSaveFormat(const QString& format)
	{
		throw MLException("Internal error: unknown save format " + format + " to " + pluginName() + " plugin.");
	};

	/**
	 * @brief The warningMessageString is invoked by the framework after the execution
	 * of load/save function. It returns the warning string containing all the
	 * warinings produced by the function, and it clears the string.
	 */
	QString warningMessageString() const
	{
		QString tmp = warnString;
		warnString.clear();
		return tmp;
	};

private:
	mutable QString warnString;

};

#define IOMESH_PLUGIN_IID "vcg.meshlab.IOMeshPlugin/1.0"
Q_DECLARE_INTERFACE(IOMeshPlugin, IOMESH_PLUGIN_IID)

#endif // MESHLAB_IOMESH_PLUGIN_H
