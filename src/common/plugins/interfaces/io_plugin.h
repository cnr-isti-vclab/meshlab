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

#ifndef MESHLAB_IO_PLUGIN_H
#define MESHLAB_IO_PLUGIN_H

#include <wrap/callback.h>

#include "meshlab_plugin_logger.h"
#include "meshlab_plugin.h"
#include "../../utilities/file_format.h"
#include "../../ml_document/raster_model.h"

/** 
 * @brief The IOPlugin is the base class for mesh or image loading and saving.
 *
 * Provides the base functions to open:
 * - a mesh file (open);
 * - [todo] a project file;
 * - a raster file (openRaster);
 * - an image file (openImage);
 *
 * and the base functions to save:
 * - a mesh file (save);
 * - [todo] a project file;
 * - an image file (saveImage);
 *
 * You can implement just a subset of these functions.
 * Provide the list of file formats supported by your plugin by implementing
 * the functions:
 * - importFormats()
 * - exportFormats()
 * - importRasterFormats()
 * - importImageFormats()
 * - exportImageFormats()
 */
class IOPlugin : virtual public MeshLabPlugin, virtual public MeshLabPluginLogger
{
public:
	IOPlugin() : MeshLabPluginLogger() { }
	virtual ~IOPlugin() { }

	/**
	 * @brief The importFormats function returns a list of all the
	 * input file formats supported by the plugin.
	 * This function must be implemented on any IO plugin.
	 * If yout plugin does not import any mesh format, just return an
	 * empty list.
	 */
	virtual std::list<FileFormat> importFormats() const = 0;

	/**
	 * @brief The exportFormats function returns a list of all the
	 * output file formats supported by the plugin.
	 * This function must be implemented on any IO plugin.
	 * If yout plugin does not export any mesh format, just return an
	 * empty list.
	 */
	virtual std::list<FileFormat> exportFormats() const = 0;

	/**
	 * @brief If your plugin supports loading also images, re-implement
	 * this function, returning the list of image formats supported by
	 * your openImage function.
	 */
	virtual std::list<FileFormat> importImageFormats() const
	{
		return std::list<FileFormat>();
	}

	/**
	 * @brief If your plugin supports saving also images, re-implement
	 * this function, returning the list of image formats supported by
	 * your saveImage function.
	 */
	virtual std::list<FileFormat> exportImageFormats() const
	{
		return std::list<FileFormat>();
	}

	/**
	 * @brief If your plugin supports loading also projects, re-implement
	 * this function, returning the list of project formats supported by
	 * your openProject function.
	 */
	virtual std::list<FileFormat> importProjectFormats() const
	{
		return std::list<FileFormat>();
	}

	/**
	 * @brief If your plugin supports saving also projects, re-implement
	 * this function, returning the list of project formats supported by
	 * your saveProject function.
	 */
	virtual std::list<FileFormat> exportProjectFormats() const
	{
		return std::list<FileFormat>();
	}

	/**
	 * @brief The initPreOpenParameter function is called to initialize the list
	 * of additional parameters that a OPENING plugin could require. It is
	 * called by the framework BEFORE the actual mesh loading to determine how
	 * to parse the input file. The instanced parameters are then passed to the
	 * open at the loading time.
	 * Typical example of use to decide what subportion of a mesh you have to
	 * load. If you do not need any additional processing simply do not override
	 * this and ignore the parameterList in the open member function
	 */
	virtual void initPreOpenParameter(
			const QString& /*format*/,
			RichParameterList& /*parameters*/)
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
	 * @brief this function returns the number of meshes that the open function
	 * is going to load from the file given as parameter. Default value is 1.
	 * If the file format can contain just one mesh per file, you don't need to
	 * re-implement this function.
	 * If your plugin could load more than one mesh from a single file, you
	 * should re-implement this function and return the actual number of meshes
	 * that are present in the file.
	 * The number returned by this function will be the number of MeshModel*
	 * contained in the meshModelList parameter passed to the open function
	 * that allows to open multiple meshes.
	 *
	 * Note: you MUST reimplement the open function that takes a list of
	 * MeshModel* as parameter ONLY if this function may return a value != 1.
	 *
	 * @param format
	 * @param fileName
	 * @return
	 */
	virtual unsigned int numberMeshesContainedInFile(
			const QString& format,
			const QString& fileName,
			const RichParameterList& preParams) const;

	/**
	 * @brief The open function is called by the framework everytime a mesh is
	 * loaded.
	 * If the file format can contain just one mesh per file, you don't need to
	 * re-implement this function, but you can implement just the next "open"
	 * function.
	 * Re-implement this function if your plugin supports loading format
	 * file that could load more than one mesh layer.
	 * @param format: the extension of the format e.g. "PLY"
	 * @param fileName: the name of the file to be opened (including its path)
	 * @param meshModelList: the list of meshes that is filled with the file content
	 *        the number of meshes in the list is given by the
	 *        numberMeshesContainedInFile() function.
	 * @param maskList: a list of bit masks that will be filled reporting what kind of data
	 *        we have found in each mesh present in the file (per vertex color,
	 *        texture coords etc). the number of meshes in the list is given by
	 *        the numberMeshesContainedInFile() function.
	 * @param par: the parameters that have been set up in the
	 *        initPreOpenParameter()
	 * @param cb: standard callback for reporting progress in the loading
	 */
	virtual void open(
		const QString &format,
		const QString &fileName,
		const std::list<MeshModel*>& meshModelList,
		std::list<int>& maskList,
		const RichParameterList & par,
		vcg::CallBackPos *cb = nullptr);

	/**
	 * @brief The open function is called by the framework everytime a mesh is
	 * loaded. Re-implement this function if the format file you want to open
	 * can contain just one mesh. If your file can contain more than one mesh,
	 * re-implement the "open" function that takes as input the MeshDocument
	 * instead of the MeshModel (see the function above).
	 * @param format: the extension of the format e.g. "PLY"
	 * @param fileName: the name of the file to be opened (including its path)
	 * @param m: the mesh that is filled with the file content
	 * @param mask: a bit mask that will be filled reporting what kind of data
	 *        we have found in the file (per vertex color, texture coords etc)
	 * @param par: the parameters that have been set up in the
	 *        initPreOpenParameter()
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
	 * @brief The save function is called by the framework everytime a mesh is
	 * saved.
	 * @param format: the extension of the format e.g. "PLY"
	 * @param fileName: the name of the file on which save the mesh m
	 *        (including its path)
	 * @param m: the mesh to be saved in the file
	 * @param mask: a bit mask indicating what kind of the data present in the
	 *        mesh should be saved (e.g. you could not want to save normals in
	 *        ply files)
	 * @param par: the parameters that have been set up in the initSaveParameter()
	 * @param cb: standard callback for reporting progress in the saving
	 */
	virtual void save(
		const QString &format,
		const QString &fileName,
		MeshModel &m, /** NOTE: this is going to be const MeshModel&: try to use only const functions!! **/
		const int mask,
		const RichParameterList & par,
		vcg::CallBackPos* cb = nullptr) = 0;

	/**
	 * @brief The openImage function is called by the framework everytime an image
	 * needs to be loaded. Could be called when loading textures or rasters.
	 * @param format: the extension of the format, e.g. "PNG"
	 * @param fileName: the name of the file from which load the image (including its path)
	 * @param cb: standard callback for reporting progresso in the loading
	 * @return the loaded QImage
	 */
	virtual QImage openImage(
		const QString& format,
		const QString& /*fileName*/,
		vcg::CallBackPos* /*cb*/ = nullptr)
	{
		wrongOpenFormat(format);
		return QImage();
	};

	/**
	 * @brief The saveImage function is called by the framework everytime an image
	 * needs to be saved (e.g. when saving a texture).
	 * @param format: the extension of the format, e.g. "PNG"
	 * @param fileName: the name of the file on which save the image (including its path)
	 * @param image: the image to save in the given fileName
	 * @param cb: standard callback for reporting progresso in the loading
	 */
	virtual void saveImage(
			const QString& format,
			const QString& /*fileName*/,
			const QImage& /*image*/,
			int /*quality*/ = 66,
			vcg::CallBackPos* /*cb*/ = nullptr)
	{
		wrongSaveFormat(format);
	}

	/**
	 * @brief The openProject function is called by the framework everytime a
	 * project needs to be loaded.
	 *
	 * The function takes an list of filenames because some project files
	 * are composed by more than one file. In this specific case, you should
	 * re-implement the function projectFileRequiresAdditionalFiles.
	 * The number of filenames contained in the input list will be
	 * 1 + number of elements in the list returned by the function
	 *     projectFileRequiresAdditionalFiles (default: 0).
	 *
	 * If the meshes contained in the project are saved in separate files and not
	 * into the project, you should use the functions provided into the file
	 * "common/utilities/load_save.h". These functions will take care to load
	 * a mesh with any of the formats supported by meshlab.
	 *
	 * @param format: the extension of the format, e.g. "MLP"
	 * @param fileName: the name of the file from which load the project (including its path)
	 * @param md: MeshDocument on which store the content of the loaded project
	 *            note: the document could not be empty!
	 * @param cb: standard callback for reporting progresso in the loading
	 * @return the list of MeshModel that have been loaded from the given project
	 */
	virtual std::list<MeshModel*> openProject(
			const QString& format,
			const QStringList& /*filenames*/,
			MeshDocument& /*md*/,
			vcg::CallBackPos* /*cb*/ = nullptr)
	{
		wrongOpenFormat(format);
		return std::list<MeshModel*>();
	}

	/**
	 * @brief some project file formats require the load of more than one file
	 * (e.g. bundler.out requires also a txt containing raster infos).
	 *
	 * If this is your case, you should implement this returning a list of
	 * *FileFormats* for each additional file that should be loaded.
	 * Then, the framework will ask the user to select the additional required
	 * files. Leaving the returned list empty means that the format of the
	 * project does not need additional files to be loaded.
	 *
	 * The list of files will then passed to the user in the openProject.
	 *
	 * @return
	 */
	virtual std::list<FileFormat> projectFileRequiresAdditionalFiles(
			const QString& /*format*/,
			const QString& /*filename*/)
	{
		return std::list<FileFormat>();
	};

	/**
	 * @brief The reportWarning function should be used everytime that a
	 * non-critical error while loading or saving a file happens. This function
	 * appends the warning message passed as parameter to a string that will be
	 * shown by the framework at the end of the execution of the load/save
	 * function
	 * @param warningMessage
	 */
	void reportWarning(const QString& warningMessage) const;

	/**
	 * @brief call this function in any of the import functions
	 * (initPreOpenParameters, load...) whenever you receive as parameter a
	 * format that is not supported by your plugin
	 */
	void wrongOpenFormat(const QString& format) const;

	/**
	 * @brief call this function in any of the export functions
	 * (exportMaskCapability, save...) whenever you receive as parameter a
	 * format that is not supported by your plugin
	 */
	void wrongSaveFormat(const QString& format) const;

	/**
	 * @brief The warningMessageString is invoked by the framework after the
	 * execution of load/save function. It returns the warning string containing
	 * all the warinings produced by the function, and it clears the string.
	 */
	QString warningMessageString() const;

private:
	mutable QString warnString;

};

#define IO_PLUGIN_IID "vcg.meshlab.IOPlugin/1.0"
Q_DECLARE_INTERFACE(IOPlugin, IO_PLUGIN_IID)

#endif // MESHLAB_IO_PLUGIN_H
