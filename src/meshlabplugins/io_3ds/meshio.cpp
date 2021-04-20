/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>

#include "meshio.h"

#include <lib3ds/file.h>
#include "import_3ds.h"
#include <wrap/io_trimesh/export_3ds.h>

#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_vrml.h>
#include <wrap/io_trimesh/export_dxf.h>

#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/import_ptx.h>

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>

#include <QFile>
#include <array>
#include <common/ml_document/mesh_document.h>

using namespace std;
using namespace vcg;

static const std::array<QString, 1> paramNames{
	"separate_layers"
};
enum paramEnum {
	SEPARATE_LAYERS = 0
};

QString ExtraMeshIOPlugin::pluginName() const
{
	return "IO3DS";
}

std::list<FileFormat> ExtraMeshIOPlugin::importFormats() const
{
	return {FileFormat("3D-Studio File Format" ,tr("3DS"))};
}

std::list<FileFormat> ExtraMeshIOPlugin::exportFormats() const
{
	return {FileFormat("3D-Studio File Format" ,tr("3DS"))};
}

void ExtraMeshIOPlugin::exportMaskCapability(const QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("3DS")){capability=defaultBits= vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	return;
}

unsigned int ExtraMeshIOPlugin::numberMeshesContainedInFile(
		const QString& format,
		const QString& fileName) const
{
	if (format.toUpper() == tr("3DS")) {
		Lib3dsFile *file = NULL;
		file = lib3ds_file_load(fileName.toStdString().c_str());
		if (!file)
			throw MLException("Malformed file.");
		// No nodes?  Fabricate nodes to display all the meshes.
		if( !file->nodes && file->meshes) {
			Lib3dsMesh *mesh;
			Lib3dsNode *node;

			for (mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
				node = lib3ds_node_new_object();
				strcpy(node->name, mesh->name);
				node->parent_id = LIB3DS_NO_PARENT;
				lib3ds_file_insert_node(file, node);
			}
		}
		if( !file->nodes) {
			lib3ds_file_free(file);
			throw MLException("Malformed file.");
		}
		lib3ds_file_eval(file, 0);
		unsigned int i = 0;
		Lib3dsNode *p = file->nodes;
		while (p) {
			i++;
			p = p->next;
		}
		log("Expected meshes in file: " + std::to_string(i) );
		lib3ds_file_free(file);
		return i;
	}
	else {
		wrongOpenFormat(format);
		return 0;
	}
}

void ExtraMeshIOPlugin::open(
		const QString& formatName,
		const QString&,
		MeshModel&,
		int&,
		const RichParameterList&,
		CallBackPos*)
{
	wrongOpenFormat(formatName);
}

void ExtraMeshIOPlugin::open(
		const QString &formatName,
		const QString &fileName,
		const std::list<MeshModel *>& meshList,
		std::list<int>& maskList,
		const RichParameterList&,
		CallBackPos *cb)
{
	// initializing mask
	maskList.clear();
	for (unsigned int i = 0; i < meshList.size(); i++)
		maskList.push_back(0);
	
	// initializing progress bar status
	if (cb != nullptr)
		(*cb)(0, "Loading...");
	
	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	//QString error_2MsgFormat = "Error encountered while loading file:\n\"%1\"\n\n File with more than a mesh.\n Load only the first!";
	
	string filename = QFile::encodeName(fileName).constData ();
	//string filename = fileName.toUtf8().data();
	
	if (formatName.toUpper() == tr("3DS")) {
		vcg::tri::io::_3dsInfo info;	
		info.cb = cb;
		Lib3dsFile *file = NULL;
		
		
		file = lib3ds_file_load(filename.c_str());
		if (!file) {
			int result = vcg::tri::io::Importer3DS<CMeshO>::E_CANTOPEN;
			throw MLException(errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
		}
		
		// No nodes?  Fabricate nodes to display all the meshes.
		if( !file->nodes && file->meshes)
		{
			Lib3dsMesh *mesh;
			Lib3dsNode *node;
			
			for (mesh = file->meshes; mesh != NULL; mesh = mesh->next)
			{
				node = lib3ds_node_new_object();
				strcpy(node->name, mesh->name);
				node->parent_id = LIB3DS_NO_PARENT;
				lib3ds_file_insert_node(file, node);
			}
		}
		
		if( !file->nodes) {
			throw MLException("Malformed file.");
		}
		
		lib3ds_file_eval(file, 0);

		Lib3dsNode *p;
		int i=1;
		auto iter = meshList.begin();
		auto miter = maskList.begin();
		for (p=file->nodes; p!=nullptr; p=p->next, ++iter, ++miter)
		{
			bool normalsUpdated = false;

			MeshModel &mm = *(*iter);
			mm.setLabel(QString(p->name));
			if (cb != nullptr)
				(*cb)(i/meshList.size() * 100, (QString("Loading Mesh ")+QString(p->name)).toStdString().c_str());

			vcg::tri::io::Importer3DS<CMeshO>::LoadMask(file, p, info);
			mm.Enable(info.mask);

			int result = vcg::tri::io::Importer3DS<CMeshO>::Load(mm.cm, file, p, info);
			if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR) {
				reportWarning("3DS Opening Error: " + errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
				continue;
			}

			if(info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
				normalsUpdated = true;

			(*miter) |= info.mask;

			// verify if texture files are present
			QString missingTextureFilesMsg = "The following texture files were not found:\n";
			bool someTextureNotFound = false;
			for ( unsigned textureIdx = 0; textureIdx < mm.cm.textures.size(); ++textureIdx)
			{
				FILE* pFile = fopen (mm.cm.textures[textureIdx].c_str(), "r");
				if (pFile == nullptr) {
					missingTextureFilesMsg.append("\n");
					missingTextureFilesMsg.append(mm.cm.textures[textureIdx].c_str());
					someTextureNotFound = true;
				}
				else {
					fclose (pFile);
				}
			}
			if (someTextureNotFound){
				reportWarning("Missing texture files: " + missingTextureFilesMsg);
			}

			vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm); // updates bounding box
			if (!normalsUpdated)
				vcg::tri::UpdateNormal<CMeshO>::PerVertex(mm.cm); // updates normals
		}

		if (cb != NULL)	(*cb)(99, "Done");
		
		// freeing memory
		lib3ds_file_free(file);
	}
	else {
		wrongOpenFormat(formatName);
	}
}

void ExtraMeshIOPlugin::save(
		const QString &formatName,
		const QString &fileName,
		MeshModel &m,
		const int mask,
		const RichParameterList &,
		vcg::CallBackPos *cb)
{
	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
	//string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	
	if(formatName.toUpper() == tr("3DS")) {
		int result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result!=0) {
			throw MLException("Saving Error: " + errorMsgFormat.arg(fileName, vcg::tri::io::Exporter3DS<CMeshO>::ErrorMsg(result)));
		}
	}
	else {
		wrongSaveFormat(formatName);
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(ExtraMeshIOPlugin)
