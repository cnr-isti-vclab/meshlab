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

#include <QMessageBox>
#include <QFile>
#include <common/ml_document/mesh_document.h>

using namespace std;
using namespace vcg;




bool ExtraMeshIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
	mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");
	
	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	//QString error_2MsgFormat = "Error encountered while loading file:\n\"%1\"\n\n File with more than a mesh.\n Load only the first!";
	
	string filename = QFile::encodeName(fileName).constData ();
	//string filename = fileName.toUtf8().data();
	
	if (formatName.toUpper() == tr("3DS"))
	{
		vcg::tri::io::_3dsInfo info;	
		info.cb = cb;
		Lib3dsFile *file = NULL;
		
		
		file = lib3ds_file_load(filename.c_str());
		if (!file)
		{
			int result = vcg::tri::io::Importer3DS<CMeshO>::E_CANTOPEN;
			errorMessage = errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result));
			return false;
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
		
		if( !file->nodes)
			return false;
		
		lib3ds_file_eval(file, 0);
		
		bool singleLayer = true;
		if ( file->nodes->next)
		{
			if ( QMessageBox::question(parent, tr("3DS Import Option"),
									   tr("File with more than a mesh.\n\nDo you want to import each mesh as a separate layer?"),
									   QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
				singleLayer = false;
		}
		
		if (!singleLayer)
		{
			Lib3dsNode *p;
			mask = 0;
			int i=1;
			for (p=file->nodes; p!=0; p=p->next, ++i)
			{
				bool normalsUpdated = false;
				
				MeshModel &mm = *m.parent->addNewMesh(qUtf8Printable(fileName), QString(p->name), false);
				if (cb != NULL)	(*cb)(i, (QString("Loading Mesh ")+QString(p->name)).toStdString().c_str());
				
				vcg::tri::io::Importer3DS<CMeshO>::LoadMask(file, p, info);
				mm.Enable(info.mask);
				
				int result = vcg::tri::io::Importer3DS<CMeshO>::Load(mm.cm, file, p, info);
				if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR)
				{
					QMessageBox::warning(parent, tr("3DS Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
					continue;
				}
				
				if(info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
					normalsUpdated = true;
				
				mask |= info.mask;
				
				// verify if texture files are present
				QString missingTextureFilesMsg = "The following texture files were not found:\n";
				bool someTextureNotFound = false;
				for ( unsigned textureIdx = 0; textureIdx < mm.cm.textures.size(); ++textureIdx)
				{
					FILE* pFile = fopen (mm.cm.textures[textureIdx].c_str(), "r");
					if (pFile == NULL)
					{
						missingTextureFilesMsg.append("\n");
						missingTextureFilesMsg.append(mm.cm.textures[textureIdx].c_str());
						someTextureNotFound = true;
					}
					fclose (pFile);
				}
				if (someTextureNotFound)
					QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);
				
				vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm);					// updates bounding box
				if (!normalsUpdated)
					vcg::tri::UpdateNormal<CMeshO>::PerVertex(mm.cm);		// updates normals
			}
		}
		else
		{
			bool normalsUpdated = false;
			
			vcg::tri::io::Importer3DS<CMeshO>::LoadMask(file, 0, info);
			m.Enable(info.mask);
			
			int result = vcg::tri::io::Importer3DS<CMeshO>::Load(m.cm, file, 0, info);
			if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR)
			{
				QMessageBox::warning(parent, tr("3DS Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
				lib3ds_file_free(file);
				return false;
			}
			
			if(info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
				normalsUpdated = true;
			
			mask = info.mask;
			
			
			// verify if texture files are present
			QString missingTextureFilesMsg = "The following texture files were not found:\n";
			bool someTextureNotFound = false;
			for ( unsigned textureIdx = 0; textureIdx < m.cm.textures.size(); ++textureIdx)
			{
				FILE* pFile = fopen (m.cm.textures[textureIdx].c_str(), "r");
				if (pFile == NULL)
				{
					missingTextureFilesMsg.append("\n");
					missingTextureFilesMsg.append(m.cm.textures[textureIdx].c_str());
					someTextureNotFound = true;
				}
				fclose (pFile);
			}
			if (someTextureNotFound)
				QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);
			
			vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
			if (!normalsUpdated) 
				vcg::tri::UpdateNormal<CMeshO>::PerVertex(m.cm);		// updates normals
		}
		
		if (cb != NULL)	(*cb)(99, "Done");
		
		// freeing memory
		lib3ds_file_free(file);
		
		return true;
	}
	return false;
}

bool ExtraMeshIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList &, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
	//string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	
	if(formatName.toUpper() == tr("3DS"))
	{
		int result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter3DS<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}
	else 
		assert(0); // unknown format
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<FileFormat> ExtraMeshIOPlugin::importFormats() const
{
	QList<FileFormat> formatList;
	formatList << FileFormat("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<FileFormat> ExtraMeshIOPlugin::exportFormats() const
{
	QList<FileFormat> formatList;
	formatList << FileFormat("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

QString ExtraMeshIOPlugin::pluginName() const
{
	return "IO3DS";
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void ExtraMeshIOPlugin::GetExportMaskCapability(const QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("3DS")){capability=defaultBits= vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	return;
}

MESHLAB_PLUGIN_NAME_EXPORTER(ExtraMeshIOPlugin)
