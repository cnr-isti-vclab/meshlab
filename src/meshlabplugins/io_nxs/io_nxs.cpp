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
#include <thread>

#include <QTextStream>
#include <QTemporaryDir>
#include <nxsbuild/nexusbuilder.h>
#include <nxsbuild/meshstream.h>
#include <nxsbuild/vcgloader.h>
#include <nxsbuild/kdtree.h>

#include <nxsbuild/nexusbuilder.h>
#include <common/traversal.h>
#include <nxsedit/extractor.h>

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
	return {
		FileFormat("Multiresolution Nexus Model", "NXS"),
		FileFormat("Compressed Multiresolution Nexus Model", "NXZ")
	};
}

void IONXSPlugin::open(
		const QString& fileFormat,
		const QString&,
		MeshModel&,
		int& ,
		const RichParameterList& ,
		vcg::CallBackPos*)
{
	wrongOpenFormat(fileFormat);
}

void IONXSPlugin::exportMaskCapability(
		const QString& format,
		int& capability,
		int& defaultBits) const
{
	if (format.toUpper() == "NXS" || format.toUpper() == "NXZ"){
		capability = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTNORMAL | vcg::tri::io::Mask::IOM_VERTTEXCOORD;
		defaultBits = capability;
	}
}

RichParameterList IONXSPlugin::initSaveParameter(
		const QString& format,
		const MeshModel&) const
{
	RichParameterList params;
	if (format.toUpper() == "NXS" || format.toUpper() == "NXZ"){
		params.addParam(RichInt("node_faces", 1<<15, "Node faces", "Number of faces per patch"));
		params.addParam(RichInt("top_node_faces", 4096, "Top node faces", "Number of triangles in the top node"));
		params.addParam(RichInt("tex_quality", 100, "Texture quality [0-100]", "jpg texture quality"));
		params.addParam(RichInt("ram", 2000, "Ram buffer", "Max ram used (in MegaBytes)"));
		params.addParam(RichInt("skiplevels", 0, "Skip levels", "Decimation skipped for n levels"));
		params.addParam(RichPoint3f("origin", Point3m(0,0,0), "Origin", "new origin for the model"));
		params.addParam(RichBool("center", false, "Center", "Set origin in the bounding box center"));
		params.addParam(RichBool("pow_2_textures", false, "Pow 2 textures", "Create textures to be power of 2"));
		params.addParam(RichBool("deepzoom", false, "Deepzoom", "Save each node and texture to a separated file"));
		params.addParam(RichDynamicFloat("adaptive", 0.333, 0, 1, "Adaptive", "Split nodes adaptively"));
	}
	if (format.toUpper() == "NXZ") { //additional parameters for nxz
		params.addParam(RichFloat("nxz_vertex_quantization", 0.0, "NXZ Vertex quantization", "absolute side of quantization grid (uses quantization factor, instead)"));
		params.addParam(RichInt("vertex_bits", 0, "Vertex bits", "number of bits in vertex coordinates when compressing (uses quantization factor, instead)"));
		params.addParam(RichFloat("quantization_factor", 0.1, "Quantization factor", "Quantization as a factor of error"));
		params.addParam(RichInt("luma_bits", 6, "Luma bits", "Quantization of luma channel"));
		params.addParam(RichInt("chroma_bits", 6, "Chroma bits", "Quantization of chroma channel"));
		params.addParam(RichInt("alpha_bits", 5, "Alpha bits", "Quantization of alpha channel"));
		params.addParam(RichInt("normal_bits", 10, "Normal bits", "Quantization of normals"));
		params.addParam(RichFloat("textures_precision", 0.25, "Textures precision", "Quantization of textures, precision in pixels per unit"));
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
		saveNxs(fileName, m, mask, params);
	}
	else if (fileFormat.toUpper() == "NXZ") {
		saveNxz(fileName, m, mask, params);
	}
	else {
		wrongSaveFormat(fileFormat);
	}
}

void IONXSPlugin::saveNxs(
		const QString& fileName,
		const MeshModel& m,
		const int mask,
		const RichParameterList& params)
{
	bool has_colors = mask & vcg::tri::io::Mask::IOM_VERTCOLOR;
	bool has_normals = mask & vcg::tri::io::Mask::IOM_VERTNORMAL;
	bool has_textures = mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD;

	//parameters:
	int node_size = params.getInt("node_faces");
	int top_node_size = params.getInt("top_node_faces");
	float vertex_quantization = 0;
	int tex_quality = params.getInt("tex_quality");
	float scaling = 0.5;
	int skiplevels = params.getInt("skiplevels");
	int ram_buffer = params.getInt("ram");
	int n_threads = std::thread::hardware_concurrency() / 2;
	if (n_threads == 0)
		n_threads = 1;

	vcg::Point3d origin = vcg::Point3d::Construct(params.getPoint3m("origin"));
	bool center = params.getBool("center");

	bool point_cloud = m.cm.fn == 0;
	bool useOrigTex = false;
	bool create_pow_two_tex = params.getBool("pow_2_textures");
	bool deepzoom = params.getBool("deepzoom");
	QVariant adaptive = params.getDynamicFloat("adaptive");

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

void IONXSPlugin::saveNxz(
		const QString& fileName,
		const MeshModel& m,
		const int mask,
		const RichParameterList& params)
{
	QFileInfo finfo(fileName);
	QTemporaryDir tmpdir;
	QString nxsFileName = tmpdir.path() + "/"+ finfo.baseName() + ".nxs";
	saveNxs(nxsFileName, m, mask, params);

	float coord_step = params.getFloat("nxz_vertex_quantization");
	int position_bits = params.getInt("vertex_bits");
	float error_q = params.getFloat("quantization_factor");
	int luma_bits = params.getInt("luma_bits");
	int chroma_bits = params.getInt("chroma_bits");
	int alpha_bits = params.getInt("alpha_bits");
	int norm_bits = params.getInt("normal_bits");
	float tex_step = params.getFloat("textures_bits");

	bool compress = true;

	nx::NexusData nexus;
	try {
		nexus.open(nxsFileName.toStdString().c_str());
		Extractor extractor(&nexus);

		nx::Signature signature = nexus.header.signature;
		if(compress) {
			signature.flags &= ~(nx::Signature::MECO | nx::Signature::CORTO);
			signature.flags |= nx::Signature::CORTO;

			if(coord_step) {  //global precision, absolute value
				extractor.error_factor = 0.0; //ignore error factor.
				//do nothing
			} else if(position_bits) {
				vcg::Sphere3f &sphere = nexus.header.sphere;
				coord_step = sphere.Radius()/pow(2.0f, position_bits);
				extractor.error_factor = 0.0;

			} else if(error_q) {
				//take node 0:
				uint32_t sink = nexus.header.n_nodes -1;
				coord_step = error_q*nexus.nodes[0].error/2;
				for(unsigned int i = 0; i < sink; i++){
					nx::Node &n = nexus.nodes[i];
					nx::Patch &patch = nexus.patches[n.first_patch];
					if(patch.node != sink)
						continue;
					double e = error_q*n.error/2;
					if(e < coord_step && e > 0)
						coord_step = e; //we are looking at level1 error, need level0 estimate.
				}
				extractor.error_factor = error_q;
			}
			//cout << "Vertex quantization step: " << coord_step << endl;
			//cout << "Texture quantization step: " << tex_step << endl;
			extractor.coord_q =(int)log2(coord_step);
			extractor.norm_bits = norm_bits;
			extractor.color_bits[0] = luma_bits;
			extractor.color_bits[1] = chroma_bits;
			extractor.color_bits[2] = chroma_bits;
			extractor.color_bits[3] = alpha_bits;
			extractor.tex_step = tex_step; //was (int)log2(tex_step * pow(2, -12));, moved to per node value
			//cout << "Texture step: " << extractor.tex_step << endl;
		}

		//cout << "Saving with flag: " << signature.flags;
		//if (signature.flags & nx::Signature::MECO) cout << " (compressed with MECO)";
		//else if (signature.flags & nx::Signature::CORTO) cout << " (compressed with CORTO)";
		//else cout << " (not compressed)";
		//cout << endl;

		extractor.save(fileName, signature);
		QFile::remove(nxsFileName);
	}
	catch (QString error) {
		throw MLException("Fatal error: " + error);
	}
	catch (const char *error) {
		throw MLException("Fatal error: " + QString(error));
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IONXSPlugin)
