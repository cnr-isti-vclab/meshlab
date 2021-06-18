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

#include <common/ml_document/mesh_model.h>
#include "io_nxs.h"

#include <QTextStream>
#include <nxsbuild/nexusbuilder.h>
#include <nxsbuild/meshstream.h>
#include <nxsbuild/vcgloader.h>
#include <nxsbuild/kdtree.h>

QString IONXSPlugin::pluginName() const
{
	return "IONXS";
}

/*
	returns the list of the file's type which can be imported
*/
std::list<FileFormat> IONXSPlugin::importFormats() const
{
	return {};
}

/*
	returns the list of the file's type which can be exported
*/
std::list<FileFormat> IONXSPlugin::exportFormats() const
{
	return {FileFormat("Multiresolution Nexus Model", "NXS")};
}

/*
	returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void IONXSPlugin::exportMaskCapability(
		const QString&,
		int& capability,
		int& defaultBits) const
{
	capability=defaultBits=0;
	return;
}


void IONXSPlugin::open(
		const QString& fileFormat,
		const QString& fileName,
		MeshModel& m,
		int& ,
		const RichParameterList& ,
		vcg::CallBackPos*)
{
	wrongOpenFormat(fileFormat);
}

void IONXSPlugin::save(
		const QString& fileFormat,
		const QString& fileName,
		MeshModel& m,
		const int mask,
		const RichParameterList&,
		vcg::CallBackPos*)
{
	if (fileFormat.toUpper() == "NXS"){
		int node_size = 1<<15;
		int top_node_size = 4096;
		float vertex_quantization = 0.0f;   //optionally quantize vertices position.
		int tex_quality(92);                //default jpg texture quality
		//QString decimation("quadric");      //simplification method
		int ram_buffer(2000);               //Mb of ram to use
		int n_threads = 4;
		float scaling(0.5);                 //simplification ratio
		int skiplevels = 0;
		vcg::Point3d origin(0, 0, 0);
		bool center = false;


		bool point_cloud = false;
		bool normals = false;
		bool no_normals = false;
		bool colors = false;
		bool no_colors = false;
		bool no_texcoords = false;
		bool useOrigTex = false;
		bool create_pow_two_tex = false;
		bool deepzoom = false;

		QVariant adaptive(0.333f);

		Stream* stream = nullptr;
		VcgLoader<CMeshO>* loader = nullptr;
		KDTree* tree = nullptr;
		try {
			quint64 max_memory = (1<<20)*(uint64_t)ram_buffer/4; //hack 4 is actually an estimate...
			std::string input = "mesh";
			if (point_cloud) {
				input = "pointcloud";
				stream = new StreamCloud("cache_stream");
			}
			else
				stream = new StreamSoup("cache_stream");
			stream->setVertexQuantization(vertex_quantization);
			stream->setMaxMemory(max_memory);
			if(center) {
				stream->origin = m.cm.bbox.Center();
			} else
				stream->origin = origin;

			vcg::Point3d &o = stream->origin;
			if(o[0] != 0.0 || o[1] != 0.0 || o[2] != 0.0) {
				int lastPoint = fileName.lastIndexOf(".");
				QString ref = fileName.left(lastPoint) + ".js";
				QFile file(ref);
				if(!file.open(QFile::ReadWrite)) {
					throw MLException("Could not save reference file: " + ref);
				}
				QTextStream stream(&file);
				stream.setRealNumberPrecision(12);
				stream << "{ \"origin\": [" << o[0] << ", " << o[1] << ", " << o[2] << "] }\n";
			}

			loader = new VcgLoader<CMeshO>;
			//todo
			bool has_colors = false, has_normals  = false, has_textures  = false;
			loader->load(&m.cm, has_colors, has_normals, has_textures);
			stream->load(loader);

			quint32 components = 0;
			if(!point_cloud) components |= NexusBuilder::FACES;

			if((!no_normals && (!point_cloud || has_normals)) || normals) {
				components |= NexusBuilder::NORMALS;
				cout << "Normals enabled\n";
			}
			if((has_colors  && !no_colors ) || colors ) {
				components |= NexusBuilder::COLORS;
				cout << "Colors enabled\n";
			}
			if(has_textures && !no_texcoords) {
				components |= NexusBuilder::TEXTURES;
				cout << "Textures enabled\n";
			}

			//WORKAROUND to save loading textures not needed
			if(!(components & NexusBuilder::TEXTURES)) {
				stream->textures.clear();
			}

			NexusBuilder builder(components);
			builder.skipSimplifyLevels = skiplevels;
			builder.setMaxMemory(max_memory);
			builder.n_threads = n_threads;
			builder.setScaling(scaling);
			builder.useNodeTex = !useOrigTex;
			builder.createPowTwoTex = create_pow_two_tex;
			if(deepzoom)
				builder.header.signature.flags |= nx::Signature::Flags::DEEPZOOM;
			builder.tex_quality = tex_quality;
			bool success = builder.initAtlas(stream->textures);
			if(!success) {
				throw MLException("Fail");
			}
			if(point_cloud)
				tree = new KDTreeCloud("cache_tree", adaptive.toFloat());
			else
				tree = new KDTreeSoup("cache_tree", adaptive.toFloat());

			tree->setMaxMemory((1<<20)*(uint64_t)ram_buffer/2);
			KDTreeSoup *treesoup = dynamic_cast<KDTreeSoup *>(tree);
			if(treesoup)
				treesoup->setTrianglesPerBlock(node_size);

			KDTreeCloud *treecloud = dynamic_cast<KDTreeCloud *>(tree);
			if(treecloud)
				treecloud->setTrianglesPerBlock(node_size);

			builder.create(tree, stream,  top_node_size);
			builder.save(fileName);
			delete tree;
			delete loader;
			delete stream;
		}
		catch(QString error) {
			delete tree;
			delete loader;
			delete stream;
			throw MLException("Fatal error: " + error);
		} catch(const char *error) {
			delete tree;
			delete loader;
			delete stream;
			throw MLException("Fatal error: " + QString(error));
		}
	}
	else {
		wrongSaveFormat(fileFormat);
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IONXSPlugin)
