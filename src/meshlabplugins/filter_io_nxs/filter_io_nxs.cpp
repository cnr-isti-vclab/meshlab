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
#include "filter_io_nxs.h"
#include <thread>

#include <QTextStream>
#include <QTemporaryDir>
#include <nxsbuild/nexusbuilder.h>
#include <nxsbuild/meshstream.h>
#include <nxsbuild/vcgloader.h>
#include <nxsbuild/plyloader.h>
#include <nxsbuild/kdtree.h>

#include <nxsbuild/nexusbuilder.h>
#include <common/traversal.h>
#include <nxsedit/extractor.h>

FilterIONXSPlugin::FilterIONXSPlugin()
{
	typeList = {
		FP_NXS_BUILDER,
		FP_NXS_COMPRESS
	};

	for(ActionIDType tt: types())
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterIONXSPlugin::pluginName() const
{
	return "FilterIONXS";
}

std::list<FileFormat> FilterIONXSPlugin::importFormats() const
{
	return {};
}

std::list<FileFormat> FilterIONXSPlugin::exportFormats() const
{
	return {
		FileFormat("Multiresolution Nexus Model", "NXS"),
		FileFormat("Compressed Multiresolution Nexus Model", "NXZ")
	};
}

void FilterIONXSPlugin::open(
		const QString& fileFormat,
		const QString&,
		MeshModel&,
		int& ,
		const RichParameterList& ,
		vcg::CallBackPos*)
{
	wrongOpenFormat(fileFormat);
}

void FilterIONXSPlugin::exportMaskCapability(
		const QString& format,
		int& capability,
		int& defaultBits) const
{
	if (format.toUpper() == "NXS" || format.toUpper() == "NXZ"){
		capability = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTNORMAL | vcg::tri::io::Mask::IOM_VERTTEXCOORD | vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
		defaultBits = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTTEXCOORD | vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
	}
}

RichParameterList FilterIONXSPlugin::initSaveParameter(
		const QString& format,
		const MeshModel&) const
{
	RichParameterList params;
	if (format.toUpper() == "NXS" || format.toUpper() == "NXZ"){
		params = nxsParameters();
	}
	if (format.toUpper() == "NXZ") { //additional parameters for nxz
		params.join(nxzParameters(true));
	}
	return params;
}

void FilterIONXSPlugin::save(
		const QString& fileFormat,
		const QString& fileName,
		MeshModel& m,
		const int mask,
		const RichParameterList& params,
		vcg::CallBackPos* cb)
{
	if (fileFormat.toUpper() == "NXS"){
		cb(1, "Saving NXS File...");
		buildNxs(fileName, params, &m, mask);
		cb(100, "NXS File saved");
	}
	else if (fileFormat.toUpper() == "NXZ") {
		QFileInfo finfo(fileName);
		QTemporaryDir tmpdir;
		QString nxsFileName = tmpdir.path() + "/"+ finfo.baseName() + ".nxs";
		cb(1, "Building NXS...");
		buildNxs(nxsFileName, params, &m, mask);
		cb(50, "Compressing NXS...");
		compressNxs(nxsFileName, fileName, params);
		cb(99, "Clearing tmp file...");
		QFile::remove(nxsFileName);
		cb(100, "NXZ File saved");
	}
	else {
		wrongSaveFormat(fileFormat);
	}
}

QString FilterIONXSPlugin::filterName(ActionIDType filter) const
{
	switch(filter) {
	case FP_NXS_BUILDER :
		return "NXS Build";
	case FP_NXS_COMPRESS :
		return "NXS Compress";
	default :
		assert(0);
		return "";
	}
}

QString FilterIONXSPlugin::filterInfo(ActionIDType filter) const
{
	QString commonDescription =
			"<a href=\"http://vcg.isti.cnr.it/nexus/\">Nexus</a> is a collection "
			"of tools for streaming visualization of large 3D models in OpenGL.<br>";
	switch(filter) {
	case FP_NXS_BUILDER :
		return commonDescription +
				"This filter is the equivalent of calling "
				"<a href=\"http://vcg.isti.cnr.it/nexus/#nxsbuild\">nxsbuild</a>: "
				"it creates a nxs file starting from a obj, ply or stl.";
	case FP_NXS_COMPRESS:
		return commonDescription +
				"This filter is the equivalent of calling nxscompress, which"
				"creates a nxz (compressed nexus) file starting from a nxs.";
	default :
		assert(0);
		return "Unknown Filter";
	}
}

FilterPlugin::FilterClass FilterIONXSPlugin::getClass(const QAction* a) const
{
	switch(ID(a)) {
	case FP_NXS_BUILDER :
	case FP_NXS_COMPRESS:
		return FilterPlugin::Other;
	default :
		assert(0);
		return FilterPlugin::Generic;
	}
}

FilterPlugin::FilterArity FilterIONXSPlugin::filterArity(const QAction*) const
{
	return NONE;
}

int FilterIONXSPlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_NONE;
}

int FilterIONXSPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_NONE;
}

RichParameterList FilterIONXSPlugin::initParameterList(const QAction* action, const MeshModel&)
{
	RichParameterList params;
	QString defDir = QDir::home().path();
	switch(ID(action)) {
	case FP_NXS_BUILDER :
		params.addParam(RichOpenFile("input_file", "", {"*.ply *.obj *.stl", "*.ply", "*.obj", "*.stl"}, "Input File", "The input file from which create the .nxs file."));
		params.addParam(RichOpenFile("input_mtl_file", "", {"*.mtl"}, "Input mtl File", "The input material file; required if loading an obj."));
		params.addParam(RichSaveFile("output_file", "", "*.nxs", "Output File", "The name of the output nxs file."));
		params.join(nxsParameters());
		break;
	case FP_NXS_COMPRESS:
		params.addParam(RichOpenFile("input_file", "", {"*.nxs"}, "Input File", "The input nxs file to compress into an nxz file."));
		params.addParam(RichSaveFile("output_file", "", "*.nxz", "Output File", "The name of the output nxz file."));
		params.join(nxzParameters(false));
		break;
	default :
		assert(0);
	}
	return params;
}

std::map<string, QVariant> FilterIONXSPlugin::applyFilter(
		const QAction* action,
		const RichParameterList& params,
		MeshDocument&,
		unsigned int&,
		vcg::CallBackPos* cb)
{
	QString inFile;
	QString outFile;
	switch(ID(action)) {
	case FP_NXS_BUILDER :
		outFile = params.getString("output_file");
		cb(1, "Saving NXS File...");
		buildNxs(outFile, params, nullptr, 0);
		cb(100, "NXS File saved");
		break;
	case FP_NXS_COMPRESS:
		inFile = params.getString("input_file");
		outFile = params.getString("output_file");
		cb(1, "Compressing NXS File...");
		compressNxs(inFile, outFile, params);
		cb(100, "NXZ File saved");
		break;
	default :
		wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}

RichParameterList FilterIONXSPlugin::nxsParameters() const
{
	RichParameterList params;
	params.addParam(RichInt("node_faces", 1<<15, "Node faces",
					"Number of faces per patch, (min ~1000, max 32768)\n"
					"This parameter controls the granularity of the multiresolution: "
					"smaller values result in smaller changes (less 'pop')."
					"Small nodes are less efficient in rendering and compression.\n"
					"Meshes with very large textures and few vertices benefit from small nodes."));
	params.addParam(RichInt("top_node_faces", 4096, "Top node faces",
					"Number of triangles in the top node. Controls the size of the smallest "
					"LOD. Higher values will delay the first rendering but with higher quality."));
	params.addParam(RichInt("tex_quality", 95, "JPEG texture quality [0-100]", "jpg texture quality"));
	params.addParam(RichInt("ram", 2000, "Ram buffer", "Max ram used in MegaBytes (WARNING: just an approximation)", true));
	params.addParam(RichInt("skiplevels", 0, "Skip levels",
					"Decimation skipped for n levels. Use for meshes with large textures "
					"and very few vertices."));
	params.addParam(RichPosition("origin", Point3m(0,0,0), "Origin", "new origin for the model"));
	params.addParam(RichBool("center", false, "Center", "Set origin in the bounding box center", true));
	params.addParam(RichBool("pow_2_textures", false, "Pow 2 textures", "Create textures to be power of 2", true));
	params.addParam(RichBool("deepzoom", false, "Deepzoom",
					"Save each node and texture to a separated file. Used for server "
					"which do not support http range requests (206). Will generate MANY files.", true));
	params.addParam(RichDynamicFloat("adaptive", 0.333, 0, 1, "Adaptive",
					"Split nodes adaptively. Different settings might help with "
					"very uneven distribution of geometry."));
	return params;
}

RichParameterList FilterIONXSPlugin::nxzParameters(bool categorize) const
{
	QString category;
	if (categorize)
		category = "NXZ parameters";
	RichParameterList params;
	params.addParam(RichFloat("nxz_vertex_quantization", 0.0, "NXZ Vertex quantization", "absolute side of quantization grid (uses quantization factor, instead)", false, category));
	params.addParam(RichInt("vertex_bits", 0, "Vertex bits", "number of bits in vertex coordinates when compressing (uses quantization factor, instead)", false, category));
	params.addParam(RichFloat("quantization_factor", 0.1, "Quantization factor", "Quantization as a factor of error", false, category));
	params.addParam(RichInt("luma_bits", 6, "Luma bits", "Quantization of luma channel", true, category));
	params.addParam(RichInt("chroma_bits", 6, "Chroma bits", "Quantization of chroma channel", true, category));
	params.addParam(RichInt("alpha_bits", 5, "Alpha bits", "Quantization of alpha channel", true, category));
	params.addParam(RichInt("normal_bits", 10, "Normal bits", "Quantization of normals", true, category));
	params.addParam(RichFloat("textures_precision", 0.25, "Textures precision", "Quantization of textures, precision in pixels per unit", true, category));
	return params;
}

void FilterIONXSPlugin::buildNxs(
		const QString& outputFile,
		const RichParameterList& params,
		const MeshModel* m,
		int mask)
{
	//if m is nullptr, we load the mesh directly from file (same of nxsbuild).
	//the name of the file is into the parameter list "input_file"
	//if m is not nullptr, it means that we are saving a nxs from the given
	//MeshModel.

	QString inputFile;
	if (m == nullptr)
		inputFile = params.getOpenFileName("input_file");

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

	bool useOrigTex = false;
	bool create_pow_two_tex = params.getBool("pow_2_textures");
	bool deepzoom = params.getBool("deepzoom");
	QVariant adaptive = params.getDynamicFloat("adaptive");

	Stream* stream = nullptr;
	VcgLoader<CMeshO>* loader = nullptr;
	KDTree* tree = nullptr;

	bool point_cloud = false;

	try {
		quint64 max_memory = (1<<20)*(uint64_t)ram_buffer/4; //hack 4 is actually an estimate...

		if (m == nullptr) {
			//autodetect point cloud ply
			if(inputFile.endsWith(".ply")) {
				PlyLoader autodetect(inputFile);
				if(autodetect.nTriangles() == 0)
					point_cloud = true;
			}
		}
		else {
			point_cloud = m->cm.FN() == 0;
		}

		if (point_cloud) {
			stream = new StreamCloud("cache_stream");
		}
		else
			stream = new StreamSoup("cache_stream");

		stream->setVertexQuantization(vertex_quantization);
		stream->setMaxMemory(max_memory);
		if(center) {
			if (m == nullptr) {
				vcg::Box3d box = stream->getBox(QStringList(inputFile));
				stream->origin = box.Center();
			}
			else {
				stream->origin = m->cm.bbox.Center();
			}
		}
		else {
			stream->origin = origin;
		}

		vcg::Point3d &o = stream->origin;
		if(o[0] != 0.0 || o[1] != 0.0 || o[2] != 0.0) {
			int lastPoint = outputFile.lastIndexOf(".");
			QString ref = outputFile.left(lastPoint) + ".js";
			QFile file(ref);
			if(!file.open(QFile::ReadWrite)) {
				throw MLException("Could not save reference file: " + ref);
			}
			QTextStream stream(&file);
			stream.setRealNumberPrecision(12);
			stream << "{ \"origin\": [" << o[0] << ", " << o[1] << ", " << o[2] << "] }\n";
		}

		bool has_colors = false;
		bool has_normals = false;
		bool has_v_textures = false;
		bool has_f_textures = false;
		std::vector<QImage> textures;
		//TODO: actually the stream will store textures or normals or colors even if not needed
		if (m == nullptr) {
			QString mtlFile = params.getString("input_mtl_file");
			stream->load(QStringList(inputFile), mtlFile);
			has_colors = stream->hasColors();
			has_normals = stream->hasNormals();
			has_v_textures = stream->hasTextures();
			has_f_textures = false;
		}
		else {
			has_colors = mask & vcg::tri::io::Mask::IOM_VERTCOLOR;
			has_normals = mask & vcg::tri::io::Mask::IOM_VERTNORMAL;
			has_v_textures = mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD;
			has_f_textures = mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
			loader = new VcgLoader<CMeshO>;
			loader->load(&m->cm, has_colors, has_normals, has_v_textures, has_f_textures);
			stream->load(loader);
		}

		quint32 components = 0;
		if(!point_cloud) components |= NexusBuilder::FACES;

		if(!point_cloud || has_normals) {
			components |= NexusBuilder::NORMALS;
		}
		if(has_colors) {
			components |= NexusBuilder::COLORS;
		}
		if(has_v_textures || has_f_textures) {
			components |= NexusBuilder::TEXTURES;
		}

		//WORKAROUND to save loading textures not needed
		if(!(components & NexusBuilder::TEXTURES)) {
			stream->textures.clear();
		}
		else {
			if (m != nullptr) {
				for (const std::string& tn : m->cm.textures){
					textures.push_back(m->getTexture(tn));
				}
			}
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

		bool success = true;
		if (m == 0)
			success = builder.initAtlas(stream->textures);
		else
			builder.initAtlas(textures);
		if(!success) {
			throw MLException("Fail when initializing atlas");
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

		builder.create(tree, stream, top_node_size);
		builder.save(outputFile);
	}
	catch(const MLException& error) {
		if(tree)   delete tree;
		if(stream) delete stream;
		if (loader) delete loader;
		throw error;
	}
	catch(QString error) {
		if(tree)   delete tree;
		if(stream) delete stream;
		if (loader) delete loader;
		throw MLException("Fatal error: " + error);

	}
	catch(const char *error) {
		if(tree)   delete tree;
		if(stream) delete stream;
		if (loader) delete loader;
		throw MLException("Fatal error: " + QString(error));
	}

	if(tree)   delete tree;
	if(stream) delete stream;
	if (loader) delete loader;
}

void FilterIONXSPlugin::compressNxs(
		const QString& inputFile,
		const QString& outputFile,
		const RichParameterList& params)
{
	float coord_step = params.getFloat("nxz_vertex_quantization");
	int position_bits = params.getInt("vertex_bits");
	float error_q = params.getFloat("quantization_factor");
	int luma_bits = params.getInt("luma_bits");
	int chroma_bits = params.getInt("chroma_bits");
	int alpha_bits = params.getInt("alpha_bits");
	int norm_bits = params.getInt("normal_bits");
	float tex_step = params.getFloat("textures_precision");

	bool compress = true;

	nx::NexusData nexus;
	try {
		nexus.open(inputFile.toStdString().c_str());
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
		extractor.save(outputFile, signature);
	}
	catch (QString error) {
		throw MLException("Fatal error: " + error);
	}
	catch (const char *error) {
		throw MLException("Fatal error: " + QString(error));
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterIONXSPlugin)
