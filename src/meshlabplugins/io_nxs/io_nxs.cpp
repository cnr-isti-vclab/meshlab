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

std::list<FileFormat> IONXSPlugin::importFormats() const
{
	return {};
}

std::list<FileFormat> IONXSPlugin::exportFormats() const
{
	return {FileFormat("Multiresolution Nexus Model", "NXS")};
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

void IONXSPlugin::exportMaskCapability(
		const QString&,
		int& capability,
		int& defaultBits) const
{
	capability = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTNORMAL | vcg::tri::io::Mask::IOM_VERTTEXCOORD;
	defaultBits = capability;
}

RichParameterList IONXSPlugin::initSaveParameter(
		const QString& format,
		const MeshModel&) const
{
	RichParameterList params;
	if (format.toUpper() == "NXS"){
		params.addParam(RichInt("node_faces", 1<<15, "Node faces", "Number of faces per patch"));
		params.addParam(RichInt("top_node_faces", 4096, "Top node faces", "Number of triangles in the top node"));
		params.addParam(RichFloat("vertex_quantization", 0, "Vertex Quantization", "Vertex quantization grid size (might be approximated)"));
		params.addParam(RichInt("tex_quality", 92, "Texture quality [0-100]", "jpg texture quality"));
		params.addParam(RichInt("ram", 2000, "Ram buffer", "Max ram used (in MegaBytes)"));
		params.addParam(RichInt("workers", 4, "N. Threads", "number of workers"));
		//params.addParam(RichFloat("scaling", 0.5, "Scaling", "Simplification ratio"));
		//params.addParam(RichString("decimation", "quadric", "", ""));
		params.addParam(RichInt("skiplevels", 0, "Skip levels", "Decimation skipped for n levels"));
		params.addParam(RichPoint3f("origin", Point3m(0,0,0), "Origin", "new origin for the model"));
		params.addParam(RichBool("center", false, "Center", "Set origin in the bounding box center"));
		params.addParam(RichBool("original_textures", false, "Original Textures", "Use original textures, no repacking"));
		params.addParam(RichBool("pow_2_textures", false, "Pow 2 textures", "Create textures to be power of 2"));
		params.addParam(RichBool("deepzoom", false, "Deepzoom", "Save each node and texture to a separated file"));
	}
	return params;
}

void IONXSPlugin::save(
		const QString& fileFormat,
		const QString& fileName,
		MeshModel& m,
		const int mask,
		const RichParameterList& params,
		vcg::CallBackPos*)
{
	if (fileFormat.toUpper() == "NXS"){
		//parameters:
		bool has_colors = mask & vcg::tri::io::Mask::IOM_VERTCOLOR;
		bool has_normals = mask & vcg::tri::io::Mask::IOM_VERTNORMAL;
		bool has_textures = mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD;

		int node_size = params.getInt("node_faces");
		int top_node_size = params.getInt("top_node_faces");
		float vertex_quantization = params.getFloat("vertex_quantization");
		int tex_quality = params.getInt("tex_quality");
		float scaling(0.5);                 //simplification ratio
		//QString decimation("quadric");      //simplification method
		int skiplevels = params.getInt("skiplevels");
		int ram_buffer = params.getInt("ram");
		int n_threads = params.getInt("workers");


		vcg::Point3d origin = vcg::Point3d::Construct(params.getPoint3m("origin"));
		bool center = params.getBool("center");


		bool point_cloud = m.cm.fn == 0;
		bool useOrigTex = params.getBool("original_textures");
		bool create_pow_two_tex = params.getBool("pow_2_textures");
		bool deepzoom = params.getBool("deepzoom");

		quint32 components = 0;
		if(!point_cloud) components |= NexusBuilder::FACES;

		if(has_normals) {
			components |= NexusBuilder::NORMALS;
		}
		if(has_colors) {
			components |= NexusBuilder::COLORS;
		}
		if(has_textures) {
			components |= NexusBuilder::TEXTURES;
		}

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

			loader->load(&m.cm, has_colors, has_normals, has_textures);
			stream->load(loader);

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
