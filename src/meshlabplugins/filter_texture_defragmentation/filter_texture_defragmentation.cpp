/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#include "filter_texture_defragmentation.h"

#include <string>

#include <QFileInfo>
#include <QDir>

#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>

#include <common/GLExtensionsManager.h>

#include "TextureDefragmentation/src/mesh.h"
#include "TextureDefragmentation/src/texture_object.h"
#include "TextureDefragmentation/src/mesh_attribute.h"
#include "TextureDefragmentation/src/mesh_graph.h"
#include "TextureDefragmentation/src/texture_optimization.h"
#include "TextureDefragmentation/src/seam_remover.h"
#include "TextureDefragmentation/src/packing.h"
#include "TextureDefragmentation/src/texture_rendering.h"

#include "TextureDefragmentation/src/logging.h"

using namespace vcg;

FilterTextureDefragPlugin::FilterTextureDefragPlugin()
{
	typeList = {
	    FP_TEXTURE_DEFRAG,
	};
	
	for(ActionIDType tt: types())
		actionList.push_back(new QAction(filterName(tt), this));

	LOG_INIT(logging::Level::Warning);
	LOG_SET_THREAD_NAME("TextureDefrag");
}

QString FilterTextureDefragPlugin::pluginName() const
{
	return "FilterTextureDefrag";
}

QString FilterTextureDefragPlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_TEXTURE_DEFRAG:
		return QString("Texture Map Defragmentation");
	default:
		assert(0);
	}
	return {};
}

QString FilterTextureDefragPlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_TEXTURE_DEFRAG:
		return QString("Reduces the texture fragmentation by merging atlas charts. \
		               The used algorithm is: <br><b>Texture Defragmentation for Photo-Reconstructed 3D Models</b><br> \
		               <i>Andrea Maggiordomo, Paolo Cignoni and Marco Tarini</i> <br>\
		               Eurographics 2021");
	default:
		assert(0);
	}
	return QString("Unknown Filter");
}

int FilterTextureDefragPlugin::getPreConditions(const QAction *a) const
{
	switch (ID(a)) {
	case FP_TEXTURE_DEFRAG:
		return MeshModel::MM_WEDGTEXCOORD;
	default:
		assert(0);
	}
	return MeshModel::MM_NONE;
}

int FilterTextureDefragPlugin::getRequirements(const QAction *a)
{
	switch (ID(a)) {
	case FP_TEXTURE_DEFRAG:
		return MeshModel::MM_FACEFACETOPO;
	default:
		assert(0);
	}
	return MeshModel::MM_NONE;
}

bool FilterTextureDefragPlugin::requiresGLContext(const QAction* a) const
{
	switch (ID(a)) {
	case FP_TEXTURE_DEFRAG:
		return true;
	default:
		assert(0);
		return false;
	}
}

int FilterTextureDefragPlugin::postCondition(const QAction *a) const
{
	switch (ID(a)) {
	case FP_TEXTURE_DEFRAG:
		return MeshModel::MM_WEDGTEXCOORD | MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE; // just to disable preview...
	default:
		assert(0);
	}
	return MeshModel::MM_NONE;
}

FilterTextureDefragPlugin::FilterClass FilterTextureDefragPlugin::getClass(const QAction *a) const
{
	switch (ID(a)) {
	case FP_TEXTURE_DEFRAG:
		return FilterPlugin::Texture;
	default:
		assert(0);
	}
	return FilterPlugin::Generic;
}

RichParameterList FilterTextureDefragPlugin::initParameterList(const QAction *action, const MeshDocument &)
{
	RichParameterList parlst;
	switch (ID(action)) {
	case FP_TEXTURE_DEFRAG:
		parlst.addParam(RichFloat(
		                    "matchingThreshold",
		                    2.0,
		                    "Matching Error Threshold",
		                    "Threshold on the seam alignment error. Using a higher threshold can reduce the fragmentation but increase runtime and distortion."));
		parlst.addParam(RichFloat(
		                    "boundaryTolerance",
		                    0.2,
		                    "Seam to chart-boundary-length tolerance",
		                    "Cutoff on the minimum fractional seam length. Seams with lower fractional length (relative to the chart perimeter) are not merged to keep the "
		                    "chart borders compact."));
		parlst.addParam(RichFloat(
		                    "distortionTolerance",
		                    0.5,
		                    "Local ARAP distortion tolerance",
		                    "Local UV-optimization distortion tolerance when merging a seam. If the local energy is higher than this value, the operation is reverted."));
		parlst.addParam(RichFloat(
		                    "globalDistortionTolerance",
		                    0.025,
		                    "Global ARAP distortion tolerance",
		                    "Global ARAP distortion tolerance when merging a seam. If the global atlas energy is higher than this value, the operation is reverted."));
		parlst.addParam(RichFloat(
		                    "offsetFactor",
		                    5.0,
		                    "Local expansion coefficient",
		                    "Coefficient used to control the extension of the UV-optimization area. Larger values can increase the efficacy of the defragmentation, "
		                    "but increase the cost of the geometric optimization and the algorithm runtime."));
		parlst.addParam(RichFloat(
		                    "timelimit",
		                    0.0,
		                    "Time limit (seconds)",
		                    "Time limit for the defragmentation process (zero means unlimited)."));
		break;
	default:
		break;
	}
	return parlst;
}

// The Real Core Function doing the actual mesh processing.
std::map<std::string, QVariant> FilterTextureDefragPlugin::applyFilter(
        const QAction *filter,
        const RichParameterList &par,
        MeshDocument &md,
        unsigned int& /*postConditionMask*/,
        CallBackPos *cb)
{
	const MeshModel &currentModel = *(md.mm());
	switch(ID(filter)) {
	case FP_TEXTURE_DEFRAG:
	{
		MeshModel& mm = *(md.addNewMesh(md.mm()->cm, "texdefrag_" + currentModel.label()));
		mm.updateDataMask(&currentModel);
		for (const std::string& txtname : currentModel.cm.textures){
			mm.addTexture(txtname, currentModel.getTexture(txtname));
		}

		QString path = currentModel.pathName();

		tri::UpdateTopology<CMeshO>::FaceFace(mm.cm);
		if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(mm.cm) > 0)
			log(GLLogStream::Levels::WARNING, "Texture Defragmentation: mesh has non-manifold edges, seam topology may be unreliable");

		// switch working directory
		QDir wd = QDir::current();
		QDir::setCurrent(path);

		tri::Allocator<CMeshO>::CompactEveryVector(mm.cm);

		// build mesh object
		Mesh defragMesh;
		auto fi = tri::Allocator<Mesh>::AddFaces(defragMesh, mm.cm.FN());
		auto vi = tri::Allocator<Mesh>::AddVertices(defragMesh, mm.cm.VN());

		for (int i = 0; i < mm.cm.VN(); ++i) {
			vi->P().X() = mm.cm.vert[i].P().X();
			vi->P().Y() = mm.cm.vert[i].P().Y();
			vi->P().Z() = mm.cm.vert[i].P().Z();
			++vi;
		}

		for (int i = 0; i < mm.cm.FN(); ++i) {
			for (int k = 0; k < 3; ++k) {
				fi->V(k) = &defragMesh.vert[mm.cm.face[i].cV(k)->Index()];
				fi->WT(k).U() = mm.cm.face[i].cWT(k).U();
				fi->WT(k).V() = mm.cm.face[i].cWT(k).V();
				fi->WT(k).N() = mm.cm.face[i].cWT(k).N();
			}
			++fi;
		}

		for (auto& f : defragMesh.face)
			f.SetMesh();

		// build textureobjecthandle object
		TextureObjectHandle textureObject = std::make_shared<TextureObject>();

		for (const string& textureName : mm.cm.textures) {
			textureObject->AddImage(mm.getTexture(textureName));
		}

		AlgoParameters ap;

		ap.matchingThreshold = par.getFloat("matchingThreshold");
		ap.boundaryTolerance = par.getFloat("boundaryTolerance");
		ap.distortionTolerance = par.getFloat("distortionTolerance");
		ap.globalDistortionThreshold = par.getFloat("globalDistortionTolerance");
		ap.UVBorderLengthReduction = 0.0;
		ap.offsetFactor = par.getFloat("offsetFactor");
		ap.timelimit = par.getFloat("timelimit");

		tri::UpdateTopology<Mesh>::FaceFace(defragMesh);
		tri::UpdateNormal<Mesh>::PerFaceNormalized(defragMesh);
		tri::UpdateNormal<Mesh>::PerVertexNormalized(defragMesh);

		ScaleTextureCoordinatesToImage(defragMesh, textureObject);

		// setup proxy mesh
		{
			Compute3DFaceAdjacencyAttribute(defragMesh);

			CutAlongSeams(defragMesh);
			tri::Allocator<Mesh>::CompactEveryVector(defragMesh);

			tri::UpdateTopology<Mesh>::FaceFace(defragMesh);
			while (tri::Clean<Mesh>::SplitNonManifoldVertex(defragMesh, 0))
				;
			tri::UpdateTopology<Mesh>::VertexFace(defragMesh);

			tri::Allocator<Mesh>::CompactEveryVector(defragMesh);
		}

		ComputeWedgeTexCoordStorageAttribute(defragMesh);

		GraphHandle graph = ComputeGraph(defragMesh, textureObject);

		std::map<RegionID, bool> flipped;
		for (auto& c : graph->charts)
			flipped[c.first] = c.second->UVFlipped();

		ReorientCharts(graph);

		// run defragmentation algorithm

		std::map<ChartHandle, int> anchorMap;
		AlgoStateHandle state = InitializeState(graph, ap);

		cb(20, "Defragmenting atlas...");

		GreedyOptimization(graph, state, ap);

		int vndupOut;
		Finalize(graph, &vndupOut);

		bool colorize = true;

		if (colorize)
			tri::UpdateColor<Mesh>::PerFaceConstant(defragMesh, vcg::Color4b(91, 130, 200, 255));

		for (auto& entry : graph->charts) {
			ChartHandle chart = entry.second;
			double zeroResamplingChartArea;
			int anchor = RotateChartForResampling(chart, state->changeSet, flipped, colorize, &zeroResamplingChartArea);
			if (anchor != -1) {
				anchorMap[chart] = anchor;
			}
		}

		cb(70, "Packing new atlas...");
		// clear texture coordinates of empty-area charts
		std::vector<ChartHandle> chartsToPack;
		for (auto& entry : graph->charts) {
			if (entry.second->AreaUV() != 0) {
				chartsToPack.push_back(entry.second);
			} else {
				for (auto fptr : entry.second->fpVec) {
					for (int j = 0; j < fptr->VN(); ++j) {
						fptr->V(j)->T().P() = Point2d::Zero();
						fptr->V(j)->T().N() = 0;
						fptr->WT(j).P() = Point2d::Zero();
						fptr->WT(j).N() = 0;
					}
				}
			}
		}

		std::vector<TextureSize> texszVec;
		int npacked = Pack(chartsToPack, textureObject, texszVec);

		// this should never happen
		if (npacked < (int) chartsToPack.size())
			throw MLException("Error: Packing failed (not all charts were packed)");

		TrimTexture(defragMesh, texszVec, false);

		IntegerShift(defragMesh, chartsToPack, texszVec, anchorMap, flipped);

		glContext->makeCurrent();
		GLExtensionsManager::initializeGLextensions();
		std::vector<std::shared_ptr<QImage>> newTextures = RenderTexture(defragMesh, textureObject, texszVec, true, RenderMode::Linear);
		glContext->doneCurrent();

		// Copy wedge tex coords from defragMesh to cm
		if (mm.cm.FN() != defragMesh.FN())
			throw MLException("TextureDefragmentation: Unexpected face count mismatch with proxy mesh");

		for (int i = 0; i < defragMesh.FN(); ++i) {
			for (int k = 0; k < 3; ++k) {
				mm.cm.face[i].WT(k).U() = defragMesh.face[i].WT(k).U();
				mm.cm.face[i].WT(k).V() = defragMesh.face[i].WT(k).V();
				mm.cm.face[i].WT(k).N() = defragMesh.face[i].WT(k).N();
			}
		}

		// save and assign textures
		cb(90, "Saving textures...");
		mm.clearTextures();

		const char *imageFormat = "png";
		QString textureBase = QFileInfo(currentModel.fullName()).baseName() + "_optimized_texture_";
		for (unsigned i = 0; i < newTextures.size(); ++i) {
			QString tname = textureBase + QString(std::to_string(i).c_str()) + "." + imageFormat;
			mm.addTexture(tname.toStdString(), *newTextures[i]);
		}

		cb(100, "Done!");

		// restore working dir
		QDir::setCurrent(wd.absolutePath());
	}
		break;

	default:
		wrongActionCalled(filter);
	}
	
	return std::map<std::string, QVariant>();
}

FilterPlugin::FilterArity FilterTextureDefragPlugin::filterArity(const QAction * filter ) const
{
	switch(ID(filter)) {
	case FP_TEXTURE_DEFRAG:
		return FilterPlugin::SINGLE_MESH;
	}

	return FilterPlugin::NONE;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterTextureDefragPlugin)
