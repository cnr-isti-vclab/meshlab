/*****************************************************************************
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

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "filter_layer.h"

#include <QDir>
#include <QImageReader>
#include <QXmlStreamWriter>
#include <vcg/complex/append.h>

using namespace std;
using namespace vcg;

// Constructor
FilterLayerPlugin::FilterLayerPlugin()
{
	typeList = {
		FP_FLATTEN,
		FP_DELETE_MESH,
		FP_DELETE_NON_VISIBLE_MESH,
		FP_DELETE_RASTER,
		FP_DELETE_NON_SELECTED_RASTER,
		FP_SPLITSELECTEDFACES,
		FP_SPLITSELECTEDVERTICES,
		FP_SPLITCONNECTED,
		FP_RENAME_MESH,
		FP_RENAME_RASTER,
		FP_DUPLICATE,
		FP_EXPORT_CAMERAS,
		FP_IMPORT_CAMERAS};

	for (ActionIDType tt : types()) {
		QAction* act = new QAction(filterName(tt), this);
		actionList.push_back(act);
	}
}

QString FilterLayerPlugin::pluginName() const
{
	return "FilterLayer";
}

// ST() return the very short string describing each filtering action
QString FilterLayerPlugin::filterName(ActionIDType filterId) const
{
	switch (filterId) {
	case FP_SPLITSELECTEDFACES: return QString("Move selected faces to another layer");
	case FP_SPLITSELECTEDVERTICES: return QString("Move selected vertices to another layer");
	case FP_SPLITCONNECTED: return QString("Split in Connected Components");
	case FP_DUPLICATE: return QString("Duplicate Current layer");
	case FP_DELETE_MESH: return QString("Delete Current Mesh");
	case FP_DELETE_NON_VISIBLE_MESH: return QString("Delete all non visible Mesh Layers");
	case FP_DELETE_RASTER: return QString("Delete Current Raster");
	case FP_DELETE_NON_SELECTED_RASTER: return QString("Delete all Non Selected Rasters");
	case FP_FLATTEN: return QString("Flatten Visible Layers");
	case FP_RENAME_MESH: return QString("Rename Current Mesh");
	case FP_RENAME_RASTER: return QString("Rename Current Raster");
	case FP_EXPORT_CAMERAS: return QString("Export active rasters cameras to file");
	case FP_IMPORT_CAMERAS: return QString("Import cameras for active rasters from file");
	default: assert(0); return QString();
	}
}

QString FilterLayerPlugin::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case FP_SPLITSELECTEDFACES: return QString("generate_from_selected_faces");
	case FP_SPLITSELECTEDVERTICES: return QString("generate_from_selected_vertices");
	case FP_SPLITCONNECTED: return QString("generate_splitting_by_connected_components");
	case FP_DUPLICATE: return QString("generate_copy_of_current_mesh");
	case FP_DELETE_MESH: return QString("delete_current_mesh");
	case FP_DELETE_NON_VISIBLE_MESH: return QString("delete_non_visible_meshes");
	case FP_DELETE_RASTER: return QString("delete_current_raster");
	case FP_DELETE_NON_SELECTED_RASTER: return QString("delete_non_active_rasters");
	case FP_FLATTEN: return QString("generate_by_merging_visible_meshes");
	case FP_RENAME_MESH: return QString("set_mesh_name");
	case FP_RENAME_RASTER: return QString("set_raster_name");
	case FP_EXPORT_CAMERAS: return QString("save_active_raster_cameras");
	case FP_IMPORT_CAMERAS: return QString("load_active_raster_cameras");
	default: assert(0); return QString();
	}
}

// Info() return the longer string describing each filtering action
QString FilterLayerPlugin::filterInfo(ActionIDType filterId) const
{
	switch (filterId) {
	case FP_SPLITSELECTEDFACES:
		return QString(
			"Selected faces are moved (or duplicated) in a new layer. Warning! per-vertex and "
			"per-face user defined attributes will not be transferred.");
	case FP_SPLITSELECTEDVERTICES:
		return QString(
			"Selected vertices are moved (or duplicated) in a new layer. Warning! per-vertex user "
			"defined attributes will not be transferred.");
	case FP_DELETE_MESH: return QString("The current mesh layer is deleted");
	case FP_DELETE_NON_VISIBLE_MESH: return QString("All the non visible mesh layers are deleted");
	case FP_DELETE_RASTER: return QString("The current raster layer is deleted");
	case FP_DELETE_NON_SELECTED_RASTER:
		return QString("All non selected raster layers are deleted");
	case FP_SPLITCONNECTED:
		return QString("Split current Layer into many layers, one for each connected components");
	case FP_DUPLICATE:
		return QString("Create a new layer containing the same model as the current one");
	case FP_FLATTEN:
		return QString(
			"Flatten all or only the visible layers into a single new mesh. <br> Transformations "
			"are preserved. Existing layers can be optionally deleted");
	case FP_RENAME_MESH: return QString("Explicitly change the label shown for a given mesh");
	case FP_RENAME_RASTER: return QString("Explicitly change the label shown for a given raster");
	case FP_EXPORT_CAMERAS:
		return QString("Export active cameras to file, in the .out or Agisoft .xml formats");
	case FP_IMPORT_CAMERAS:
		return QString("Import cameras for active rasters from .out or Agisoft .xml formats");
	default: assert(0);
	}
	return NULL;
}

// This function define the needed parameters for each filter.
RichParameterList
FilterLayerPlugin::initParameterList(const QAction* action, const MeshDocument& md)
{
	RichParameterList  parlst;
	const MeshModel*   mm = md.mm();
	const RasterModel* rm = md.rm();
	switch (ID(action)) {
	case FP_SPLITSELECTEDVERTICES:
	case FP_SPLITSELECTEDFACES:
		parlst.addParam(RichBool(
			"DeleteOriginal",
			true,
			"Delete original selection",
			"Deletes the original selected faces/vertices, thus splitting the mesh among layers."
			"\n\nif false, the selected faces/vertices are duplicated in the new layer."));
		break;
	case FP_SPLITCONNECTED:
		parlst.addParam(RichBool(
			"delete_source_mesh",
			false,
			"Delete source mesh",
			"Deletes the source mesh after all the connected component meshes are generated."));
		break;
	case FP_FLATTEN:
		parlst.addParam(RichBool(
			"MergeVisible",
			true,
			"Merge Only Visible Layers",
			"If true, flatten only visible layers, otherwise, all layers are used."));
		parlst.addParam(RichBool(
			"DeleteLayer",
			true,
			"Delete Layers ",
			"Delete all the layers used as source in flattening. <br>If all layers are visible "
			"only a single layer will remain after the invocation of this filter."));
		parlst.addParam(RichBool(
			"MergeVertices",
			true,
			"Merge duplicate vertices",
			"Merge the vertices that are duplicated among different layers. \n\n"
			"Very useful when the layers are spliced portions of a single big mesh."));
		parlst.addParam(RichBool(
			"AlsoUnreferenced",
			true,
			"Keep unreferenced vertices",
			"Do not discard unreferenced vertices from source layers\n\n"
			"Necessary for point-cloud layers."));
		break;
	case FP_RENAME_MESH:
		parlst.addParam(RichString("newName", mm->label(), "New Label", "New Label for the mesh."));
		break;
	case FP_RENAME_RASTER:
		parlst.addParam(
			RichString("newName", rm ? rm->label() : "", "New Label", "New Label for the raster."));
		break;
	case FP_EXPORT_CAMERAS:
		parlst.addParam(RichEnum(
			"ExportFile",
			0,
			QStringList("Bundler .out") << "Agisoft xml",
			"Output format",
			"Choose the output format, The filter enables to export the cameras to both Bundler "
			"and Agisoft Photoscan."));
		parlst.addParam(RichString(
			"newName",
			"cameras",
			"Export file name (the right extension will be added at the end)",
			"Name of the output file, it will be saved in the same folder as the project file."));
		break;
	case FP_IMPORT_CAMERAS:
		parlst.addParam(RichOpenFile(
			"ImportFile",
			"",
			QStringList(
				"All Project Files (*.out *.xml);;Bundler Output (*.out);;Agisoft xml (*.xml)"),
			"Choose the camera file to be imported",
			"It's possible to import both Bundler .out and Agisoft .xml files. In both cases, "
			"distortion parameters won't be imported. In the case of Agisoft, it's necessary to "
			"undistort the images before exporting the xml file."));
		break;
	default: break; // do not add any parameter for the other filters
	}
	return parlst;
}

// Core Function doing the actual mesh processing.
std::map<std::string, QVariant> FilterLayerPlugin::applyFilter(
	const QAction*           filter,
	const RichParameterList& par,
	MeshDocument&            md,
	unsigned int& /*postConditionMask*/,
	vcg::CallBackPos* cb)
{
	int numFacesSel, numVertSel;

	switch (ID(filter)) {
	case FP_RENAME_MESH: md.mm()->setLabel(par.getString("newName")); break;

	case FP_RENAME_RASTER: {
		if (md.rm()) {
			md.rm()->setLabel(par.getString("newName"));
		}
		else {
			throw MLException("Error: Call to Rename Current Raster with no valid raster.");
		}

	} break;

	case FP_DELETE_MESH: {
		if (md.mm())
			md.delMesh(md.mm()->id());
	} break;

	case FP_DELETE_NON_VISIBLE_MESH: {
		for (auto it = md.meshBegin(); it != md.meshEnd();) {
			if (!it->isVisible()) {
				it = md.eraseMesh(it);
			}
			else {
				++it;
			}
		}
	} break;

	case FP_DELETE_RASTER:
		if (md.rm())
			md.delRaster(md.rm()->id());
		break;

	case FP_DELETE_NON_SELECTED_RASTER: {
		for (auto it = md.rasterBegin(); it != md.rasterEnd();) {
			if (!it->isVisible()) {
				it = md.eraseRaster(it);
			}
			else {
				++it;
			}
		}
	} break;

	case FP_SPLITSELECTEDVERTICES: {
		MeshModel* currentModel = md.mm();

		MeshModel* destModel = md.addNewMesh("", "SelectedVerticesSubset", true);
		destModel->updateDataMask(currentModel);

		numVertSel = (int) tri::UpdateSelection<CMeshO>::VertexCount(currentModel->cm);

		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm, true);
		for (const std::string& tex : destModel->cm.textures) {
			destModel->addTexture(tex, currentModel->getTexture(tex));
		}

		if (par.getBool("DeleteOriginal")) { // delete original vert/faces
			CMeshO::VertexIterator vi;

			int delfaces =
				(int) tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(currentModel->cm);
			for (CMeshO::FaceIterator fi = currentModel->cm.face.begin();
				 fi != currentModel->cm.face.end();
				 ++fi) {
				if (!(*fi).IsD() && (*fi).IsS())
					tri::Allocator<CMeshO>::DeleteFace(currentModel->cm, *fi);
			}
			for (vi = currentModel->cm.vert.begin(); vi != currentModel->cm.vert.end(); ++vi) {
				if (!(*vi).IsD() && (*vi).IsS())
					tri::Allocator<CMeshO>::DeleteVertex(currentModel->cm, *vi);
			}

			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			currentModel->clearDataMask(MeshModel::MM_FACEFACETOPO);

			log("Moved %i vertices to layer %i, deleted %i faces",
				numVertSel,
				currentModel->id(),
				delfaces);
		}
		else { // keep original faces
			log("Copied %i vertices to layer %i", numVertSel, currentModel->id());
		}
		vcg::tri::UpdateFlags<CMeshO>::VertexClear(destModel->cm, CMeshO::VertexType::SELECTED);

		// init new layer
		destModel->updateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
		destModel->updateDataMask(currentModel);
	} break;

	case FP_SPLITSELECTEDFACES: {
		MeshModel* currentModel = md.mm();

		MeshModel* destModel = md.addNewMesh("", "SelectedFacesSubset", true);
		destModel->updateDataMask(currentModel);

		// select all points involved
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currentModel->cm);
		numFacesSel = (int) tri::UpdateSelection<CMeshO>::FaceCount(currentModel->cm);
		numVertSel  = (int) tri::UpdateSelection<CMeshO>::VertexCount(currentModel->cm);

		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm, true);
		for (const std::string& tex : destModel->cm.textures) {
			destModel->addTexture(tex, currentModel->getTexture(tex));
		}

		if (par.getBool("DeleteOriginal")) { // delete original faces
			CMeshO::VertexIterator vi;
			CMeshO::FaceIterator   fi;
			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(currentModel->cm);
			for (fi = currentModel->cm.face.begin(); fi != currentModel->cm.face.end(); ++fi)
				if (!(*fi).IsD() && (*fi).IsS())
					tri::Allocator<CMeshO>::DeleteFace(currentModel->cm, *fi);
			for (vi = currentModel->cm.vert.begin(); vi != currentModel->cm.vert.end(); ++vi)
				if (!(*vi).IsD() && (*vi).IsS())
					tri::Allocator<CMeshO>::DeleteVertex(currentModel->cm, *vi);

			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			tri::UpdateSelection<CMeshO>::FaceClear(currentModel->cm);
			currentModel->clearDataMask(MeshModel::MM_FACEFACETOPO);

			log("Moved %i faces and %i vertices to layer %i",
				numFacesSel,
				numVertSel,
				currentModel->id());
		}
		else { // keep original faces
			log("Copied %i faces and %i vertices to layer %i",
				numFacesSel,
				numVertSel,
				currentModel->id());
		}
		vcg::tri::UpdateFlags<CMeshO>::VertexClear(destModel->cm, CMeshO::VertexType::SELECTED);
		vcg::tri::UpdateFlags<CMeshO>::FaceClear(destModel->cm, CMeshO::FaceType::SELECTED);

		// init new layer
		destModel->updateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
		destModel->updateDataMask(currentModel);
	} break;

	case FP_DUPLICATE: {
		MeshModel* currentModel = md.mm(); // source = current
		QString    newName      = currentModel->label() + "_copy";
		MeshModel* destModel    = md.addNewMesh(
            "",
            newName,
            true); // After Adding a mesh to a MeshDocument the new mesh is the current one
		destModel->updateDataMask(currentModel);
		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm);

		for (const std::string& tex : destModel->cm.textures) {
			destModel->addTexture(tex, currentModel->getTexture(tex));
		}

		log("Duplicated current model to layer %i", destModel->id());

		// init new layer
		destModel->updateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
	} break;

	case FP_FLATTEN: {
		bool deleteLayer      = par.getBool("DeleteLayer");
		bool mergeVisible     = par.getBool("MergeVisible");
		bool mergeVertices    = par.getBool("MergeVertices");
		bool alsoUnreferenced = par.getBool("AlsoUnreferenced");

		MeshModel* destModel = md.addNewMesh("", "Merged Mesh", true);

		std::list<unsigned int> toBeDeletedList;

		unsigned int cnt = 0;
		for (MeshModel& mmp : md.meshIterator()) {
			++cnt;
			if (mmp.isVisible() || !mergeVisible) {
				if (mmp.id() != destModel->id()) {
					cb(cnt * 100 / md.meshNumber(), "Merging layers...");
					tri::UpdatePosition<CMeshO>::Matrix(mmp.cm, mmp.cm.Tr, true);
					toBeDeletedList.push_back(mmp.id());
					if (!alsoUnreferenced) {
						vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmp.cm);
					}
					destModel->updateDataMask(&mmp);
					tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, mmp.cm);

					for (const std::string& txt : mmp.cm.textures) {
						destModel->addTexture(txt, mmp.getTexture(txt));
					}

					tri::UpdatePosition<CMeshO>::Matrix(mmp.cm, Inverse(mmp.cm.Tr), true);
				}
			}
		}

		if (deleteLayer) {
			log("Deleted %d merged layers", toBeDeletedList.size());
			for (unsigned int id : toBeDeletedList)
				md.delMesh(id);
			md.setCurrent(destModel); // setting again newly created model as current
		}

		if (mergeVertices) {
			int delvert = tri::Clean<CMeshO>::RemoveDuplicateVertex(destModel->cm);
			log("Removed %d duplicated vertices", delvert);
		}

		destModel->updateBoxAndNormals();
		log("Merged all the layers to single mesh of %i vertices", md.mm()->cm.vn);
	} break;

	case FP_SPLITCONNECTED: {
		MeshModel* currentModel     = md.mm();
		CMeshO&    cm               = md.mm()->cm;
		bool       removeSourceMesh = par.getBool("delete_source_mesh");
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		std::vector<std::pair<int, CMeshO::FacePointer>> connectedCompVec;
		int numCC = tri::Clean<CMeshO>::ConnectedComponents(cm, connectedCompVec);
		log("Found %i Connected Components", numCC);
		tri::UpdateSelection<CMeshO>::FaceClear(cm);
		tri::UpdateSelection<CMeshO>::VertexClear(cm);

		for (size_t i = 0; i < connectedCompVec.size(); ++i) {
			// select verts and faces of ith connected component
			connectedCompVec[i].second->SetS();
			tri::UpdateSelection<CMeshO>::FaceConnectedFF(cm /*,true*/);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(cm);

			// create a new mesh from the selection
			MeshModel* destModel = md.addNewMesh("", QString("CC %1").arg(i), true);
			destModel->updateDataMask(currentModel);
			tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, cm, true);

			// clear selection from source mesh and from newly created mesh
			tri::UpdateSelection<CMeshO>::FaceClear(cm);
			tri::UpdateSelection<CMeshO>::VertexClear(cm);
			tri::UpdateSelection<CMeshO>::FaceClear(destModel->cm);
			tri::UpdateSelection<CMeshO>::VertexClear(destModel->cm);

			// append any textures... (todo: append only the ACTUALLY USED ones)
			for (const std::string& txt : cm.textures) {
				destModel->addTexture(txt, md.mm()->getTexture(txt));
			}

			// init new layer
			destModel->updateBoxAndNormals();
			destModel->cm.Tr = currentModel->cm.Tr;
		}
		if (removeSourceMesh)
			md.delMesh(currentModel->id());
	} break;

	case FP_EXPORT_CAMERAS: {
		int     output = par.getEnum("ExportFile");
		QString name   = par.getString("newName");

		if (output == 0) {
			FILE* outfile = NULL;

			name    = name.append(".out");
			outfile = fopen(name.toStdString().c_str(), "wb");

			if (outfile == NULL) {
				throw MLException("Impossible to open file " + name);
			}

			int active = 0;
			for (const RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible())
					active++;
			}

			fprintf(outfile, "# Bundle file v0.3\n");
			fprintf(outfile, "%d %d\n", active, 0);

			for (const RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible()) {
					fprintf(
						outfile,
						"%f %d %d\n",
						rm.shot.Intrinsics.FocalMm / rm.shot.Intrinsics.PixelSizeMm[0],
						0,
						0);

					Matrix44m mat = rm.shot.Extrinsics.Rot();

					Matrix33m Rt = Matrix33m(Matrix44m(mat), 3);

					Point3f pos = Rt * rm.shot.Extrinsics.Tra();
					Rt.Transpose();

					fprintf(outfile, "%f %f %f\n", Rt[0][0], Rt[1][0], Rt[2][0]);
					fprintf(outfile, "%f %f %f\n", Rt[0][1], Rt[1][1], Rt[2][1]);
					fprintf(outfile, "%f %f %f\n", Rt[0][2], Rt[1][2], Rt[2][2]);
					fprintf(outfile, "%f %f %f\n", -pos[0], -pos[1], -pos[2]);
				}
			}
			fprintf(outfile, "%d %d %d\n", 0, 0, 0);
			fclose(outfile);
		}
		else if (output == 1) {
			name = name.append(".xml");

			QFileInfo fi(name);
			QDir      tmpDir = QDir::current();
			QDir::setCurrent(fi.absoluteDir().absolutePath());

			// QDomDocument doc("AgisoftXML");
			QFile file(name);
			file.open(QIODevice::WriteOnly);

			QXmlStreamWriter xmlWriter(&file);
			xmlWriter.setAutoFormatting(true);
			xmlWriter.writeStartDocument();

			xmlWriter.writeStartElement("document");
			xmlWriter.writeAttribute("version", "1.2.0");

			xmlWriter.writeStartElement("chunk");
			xmlWriter.writeStartElement("sensors");

			unsigned int i = 0;
			for (const RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible()) {
					float focal, pixelX, pixelY;
					if (rm.shot.Intrinsics.FocalMm > 1000) {
						focal  = rm.shot.Intrinsics.FocalMm / 500;
						pixelX = rm.shot.Intrinsics.PixelSizeMm[0] / 500;
						pixelY = rm.shot.Intrinsics.PixelSizeMm[1] / 500;
					}
					else {
						focal  = rm.shot.Intrinsics.FocalMm;
						pixelX = rm.shot.Intrinsics.PixelSizeMm[0];
						pixelY = rm.shot.Intrinsics.PixelSizeMm[1];
					}

					xmlWriter.writeStartElement("sensor");
					xmlWriter.writeAttribute("id", QString::number(i));
					xmlWriter.writeAttribute("label", "unknown" + QString::number(i));
					xmlWriter.writeAttribute("type", "frame");
					xmlWriter.writeStartElement("resolution");
					xmlWriter.writeAttribute(
						"width", QString::number(rm.shot.Intrinsics.ViewportPx[0]));
					xmlWriter.writeAttribute(
						"height", QString::number(rm.shot.Intrinsics.ViewportPx[1]));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "pixel_width");
					xmlWriter.writeAttribute("value", QString::number(pixelX));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "pixel_height");
					xmlWriter.writeAttribute("value", QString::number(pixelY));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "focal_length");
					xmlWriter.writeAttribute("value", QString::number(focal));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "fixed");
					xmlWriter.writeAttribute("value", "false");
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("calibration");
					xmlWriter.writeAttribute("type", "frame");
					xmlWriter.writeAttribute("class", "adjusted");
					xmlWriter.writeStartElement("resolution");
					xmlWriter.writeAttribute(
						"width", QString::number(rm.shot.Intrinsics.ViewportPx[0]));
					xmlWriter.writeAttribute(
						"height", QString::number(rm.shot.Intrinsics.ViewportPx[1]));
					xmlWriter.writeEndElement();
					xmlWriter.writeTextElement(
						"fx",
						QString::number(
							rm.shot.Intrinsics.FocalMm / rm.shot.Intrinsics.PixelSizeMm[0]));
					xmlWriter.writeTextElement(
						"fy",
						QString::number(
							rm.shot.Intrinsics.FocalMm / rm.shot.Intrinsics.PixelSizeMm[1]));
					xmlWriter.writeTextElement(
						"cx", QString::number(rm.shot.Intrinsics.CenterPx[0]));
					xmlWriter.writeTextElement(
						"cy", QString::number(rm.shot.Intrinsics.CenterPx[1]));
					xmlWriter.writeTextElement("k1", "0");
					xmlWriter.writeTextElement("k2", "0");
					xmlWriter.writeTextElement("p1", "0");
					xmlWriter.writeTextElement("p2", "0");
					xmlWriter.writeEndElement();
					xmlWriter.writeEndElement();
				}
				++i;
			}
			xmlWriter.writeEndElement();
			xmlWriter.writeStartElement("cameras");
			i = 0;
			for (const RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible()) {
					xmlWriter.writeStartElement("camera");
					xmlWriter.writeAttribute("id", QString::number(i));
					xmlWriter.writeAttribute("label", rm.currentPlane->shortName());
					xmlWriter.writeAttribute("sensor_id", QString::number(i));
					xmlWriter.writeAttribute("enabled", "true");
					Matrix44m mat       = rm.shot.Extrinsics.Rot();
					Point3f   pos       = rm.shot.Extrinsics.Tra();
					QString   transform = QString::number(mat[0][0]);
					transform.append(" " + QString::number(-mat[1][0]));
					transform.append(" " + QString::number(-mat[2][0]));
					transform.append(" " + QString::number(pos[0]));
					transform.append(" " + QString::number(mat[0][1]));
					transform.append(" " + QString::number(-mat[1][1]));
					transform.append(" " + QString::number(-mat[2][1]));
					transform.append(" " + QString::number(pos[1]));
					transform.append(" " + QString::number(mat[0][2]));
					transform.append(" " + QString::number(-mat[1][2]));
					transform.append(" " + QString::number(-mat[2][2]));
					transform.append(" " + QString::number(pos[2]));
					transform.append(" 0");
					transform.append(" 0");
					transform.append(" 0");
					transform.append(" 1");

					xmlWriter.writeTextElement("transform", transform);
					xmlWriter.writeEndElement();
				}
				++i;
			}
			xmlWriter.writeEndElement();
			xmlWriter.writeEndDocument();

			file.close();
			QDir::setCurrent(tmpDir.path());
		}
	} break;

	case FP_IMPORT_CAMERAS: {
		QString fileName = par.getOpenFileName("ImportFile");
		if (fileName.isEmpty()) {
			throw MLException("No file to open");
		}
		QFileInfo fi(fileName);

		if ((fi.suffix().toLower() == "out")) {
			unsigned int num_cams, num_points;
			FILE*        fp = fopen(qUtf8Printable(fileName), "r");
			if (!fp) {
				throw MLException("Impossible to open file " + fileName);
			}

			////Read header

			char line[100];
			fgets(line, 100, fp);
			if (line[0] == '\0')
				throw MLException("Error while parsing " + fileName);
			line[18] = '\0';
			if (0 != strcmp("# Bundle file v0.3", line))
				throw MLException("Error while parsing " + fileName);
			fgets(line, 100, fp);
			if (line[0] == '\0')
				throw MLException("Error while parsing " + fileName);
			sscanf(line, "%d %d", &num_cams, &num_points);

			///// Check if the number of active rasters and cameras is the same
			unsigned active = 0;
			for (RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible())
					active++;
			}

			if (active != num_cams) {
				throw MLException(
					"Wait! The number of active rasters and the number of cams in the Bundler file "
					"is not the same!");
			}

			//// Import cameras
			unsigned int i = 0;
			for (RasterModel& rm : md.rasterIterator()) {
				if (rm.isVisible()) {
					float        f, k1, k2;
					float        R[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
					vcg::Point3f t;

					fgets(line, 100, fp);
					if (line[0] == '\0')
						throw MLException("Error while parsing " + fileName);
					sscanf(line, "%f %f %f", &f, &k1, &k2);

					fgets(line, 100, fp);
					if (line[0] == '\0')
						throw MLException("Error while parsing " + fileName);
					sscanf(line, "%f %f %f", &(R[0]), &(R[1]), &(R[2]));
					R[3] = 0;
					fgets(line, 100, fp);
					if (line[0] == '\0')
						throw MLException("Error while parsing " + fileName);
					sscanf(line, "%f %f %f", &(R[4]), &(R[5]), &(R[6]));
					R[7] = 0;
					fgets(line, 100, fp);
					if (line[0] == '\0')
						throw MLException("Error while parsing " + fileName);
					sscanf(line, "%f %f %f", &(R[8]), &(R[9]), &(R[10]));
					R[11] = 0;

					fgets(line, 100, fp);
					if (line[0] == '\0')
						throw MLException("Error while parsing " + fileName);
					sscanf(line, "%f %f %f", &(t[0]), &(t[1]), &(t[2]));

					Matrix44f matF(R);
					Matrix44m mat;
					mat.Import(matF);

					Matrix33m Rt = Matrix33m(Matrix44m(mat), 3);
					Rt.Transpose();

					Point3f pos = Rt * Point3f(t[0], t[1], t[2]);

					rm.shot.Extrinsics.SetTra(Point3f(-pos[0], -pos[1], -pos[2]));
					rm.shot.Extrinsics.SetRot(mat);

					rm.shot.Intrinsics.FocalMm = f;
					rm.shot.Intrinsics.k[0] =
						0.0; // k1; To be uncommented when distortion is taken into account reliably
					rm.shot.Intrinsics.k[1]        = 0.0; // k2;
					rm.shot.Intrinsics.PixelSizeMm = vcg::Point2f(1, 1);
					QSize        size;
					QImageReader sizeImg(rm.currentPlane->fullPathFileName);
					size                          = sizeImg.size();
					rm.shot.Intrinsics.ViewportPx = vcg::Point2i(size.width(), size.height());
					rm.shot.Intrinsics.CenterPx[0] =
						(int) ((double) rm.shot.Intrinsics.ViewportPx[0] / 2.0f);
					rm.shot.Intrinsics.CenterPx[1] =
						(int) ((double) rm.shot.Intrinsics.ViewportPx[1] / 2.0f);
				}
				++i;
			}
		}
		else if ((fi.suffix().toLower() == "xml")) {
			QDomDocument doc;
			QFile        file(fileName);
			if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
				throw MLException("Error while opening " + fileName);
			std::vector<Shotf> shots;

			////// Read and store sensors list
			QDomNodeList sensors = doc.elementsByTagName("sensor");

			if (sensors.size() == 0) {
				throw MLException("Error!");
			}

			shots.resize(sensors.size());
			for (int i = 0; i < sensors.size(); i++) {
				QDomNode n  = sensors.item(i);
				int      id = n.attributes().namedItem("id").nodeValue().toInt();

				QDomNode node = n.firstChild();
				// Devices
				while (!node.isNull()) {
					if (QString::compare(node.nodeName(), "calibration") == 0) {
						QDomNode node1 = node.firstChild();
						// Devices
						while (!node1.isNull()) {
							if (QString::compare(node1.nodeName(), "resolution") == 0) {
								int width =
									node1.attributes().namedItem("width").nodeValue().toInt();
								int height =
									node1.attributes().namedItem("height").nodeValue().toInt();
								// TODO: read cu, cv
								// float cu                           = .0f;
								// float cv                           = .0f;
								shots[id].Intrinsics.ViewportPx[0] = width;
								shots[id].Intrinsics.ViewportPx[1] = height;
								shots[id].Intrinsics.CenterPx[0]   = (float) width / 2.0f;
								shots[id].Intrinsics.CenterPx[1]   = (float) height / 2.0f;
								// shots[id].Intrinsics.CenterPx[0]   = cu;
								// shots[id].Intrinsics.CenterPx[1]   = cv;
								// Log("Width %f, Height %f", shots[id].Intrinsics.CenterPx[0],
								// shots[id].Intrinsics.CenterPx[1]);
							}
							else if (QString::compare(node1.nodeName(), "fx") == 0) {
								float fx = node1.toElement().text().toFloat();
								if (fx > 100) {
									fx                                  = fx / 100;
									shots[id].Intrinsics.FocalMm        = fx;
									shots[id].Intrinsics.PixelSizeMm[0] = 0.01f;
									shots[id].Intrinsics.PixelSizeMm[1] = 0.01f;
								}
								else {
									shots[id].Intrinsics.FocalMm        = fx;
									shots[id].Intrinsics.PixelSizeMm[0] = 1;
									shots[id].Intrinsics.PixelSizeMm[1] = 1;
								}
								// Log("Focal %f", fx);
							}
							else if (QString::compare(node1.nodeName(), "k1") == 0) {
								float k1 = node1.toElement().text().toFloat();
								if (k1 != 0.0f) {
									log("Warning! Distortion parameters won't be imported! Please "
										"undistort the images in Photoscan before!"); // text
								}
							}
							node1 = node1.nextSibling();
						}
					}
					node = node.nextSibling();
				}
			}

			///////// Read and import cameras
			QDomNodeList cameras = doc.elementsByTagName("camera");

			if (cameras.size() == 0) {
				throw MLException("Error!");
			}

			for (int i = 0; i < cameras.size(); i++) {
				QDomNode n = cameras.item(i);
				// int id = n.attributes().namedItem("id").nodeValue().toInt();
				int     sensor_id = n.attributes().namedItem("sensor_id").nodeValue().toInt();
				QString name      = n.attributes().namedItem("label").nodeValue();

				RasterModel* rasterId = nullptr;
				for (RasterModel& rm : md.rasterIterator()) {
					if (rm.currentPlane->shortName() == name) {
						rasterId = &rm;
						break;
					}
				}

				QDomNode node = n.firstChild();

				while (!node.isNull() && rasterId != nullptr) {
					if (QString::compare(node.nodeName(), "transform") == 0) {
						rasterId->shot.Intrinsics.FocalMm = shots[sensor_id].Intrinsics.FocalMm;
						rasterId->shot.Intrinsics.ViewportPx[0] =
							shots[sensor_id].Intrinsics.ViewportPx[0];
						rasterId->shot.Intrinsics.ViewportPx[1] =
							shots[sensor_id].Intrinsics.ViewportPx[1];
						rasterId->shot.Intrinsics.CenterPx[0] =
							shots[sensor_id].Intrinsics.CenterPx[0];
						rasterId->shot.Intrinsics.CenterPx[1] =
							shots[sensor_id].Intrinsics.CenterPx[1];
						rasterId->shot.Intrinsics.PixelSizeMm[0] =
							shots[sensor_id].Intrinsics.PixelSizeMm[0];
						rasterId->shot.Intrinsics.PixelSizeMm[1] =
							shots[sensor_id].Intrinsics.PixelSizeMm[1];

						QStringList values =
							node.toElement().text().split(" ", QString::SkipEmptyParts);
						Matrix44m mat = rasterId->shot.Extrinsics.Rot();
						Point3f   pos = rasterId->shot.Extrinsics.Tra();

						mat[0][0] = values[0].toFloat();
						mat[1][0] = -values[1].toFloat();
						mat[2][0] = -values[2].toFloat();
						pos[0]    = values[3].toFloat();
						mat[0][1] = values[4].toFloat();
						mat[1][1] = -values[5].toFloat();
						mat[2][1] = -values[6].toFloat();
						pos[1]    = values[7].toFloat();
						mat[0][2] = values[8].toFloat();
						mat[1][2] = -values[9].toFloat();
						mat[2][2] = -values[10].toFloat();
						pos[2]    = values[11].toFloat();
						rasterId->shot.Extrinsics.SetRot(mat);
						rasterId->shot.Extrinsics.SetTra(pos);
					}
					node = node.nextSibling();
				}
			}
		}

		else {
			throw MLException("Unknown file type");
		}
	} break;
	default: wrongActionCalled(filter);

	} // end case
	return std::map<std::string, QVariant>();
}

FilterLayerPlugin::FilterClass FilterLayerPlugin::getClass(const QAction* a) const
{
	switch (ID(a)) {
	case FP_RENAME_MESH:
	case FP_SPLITSELECTEDFACES:
	case FP_SPLITSELECTEDVERTICES:
	case FP_DUPLICATE:
	case FP_FLATTEN:
	case FP_SPLITCONNECTED:
	case FP_DELETE_MESH:
	case FP_DELETE_NON_VISIBLE_MESH: return FilterPlugin::Layer;
	case FP_RENAME_RASTER:
	case FP_DELETE_RASTER:
	case FP_DELETE_NON_SELECTED_RASTER:
	case FP_EXPORT_CAMERAS: return FilterPlugin::RasterLayer;
	case FP_IMPORT_CAMERAS: return FilterClass(FilterPlugin::Camera + FilterPlugin::RasterLayer);
	default: assert(0);
	}
	return FilterPlugin::Generic;
}

FilterPlugin::FilterArity FilterLayerPlugin::filterArity(const QAction* filter) const
{
	switch (ID(filter)) {
	case FP_RENAME_MESH:
	case FP_SPLITSELECTEDFACES:
	case FP_SPLITSELECTEDVERTICES:
	case FP_DUPLICATE:
	case FP_SPLITCONNECTED:
	case FP_DELETE_MESH: return FilterPlugin::SINGLE_MESH;
	case FP_RENAME_RASTER:
	case FP_DELETE_RASTER:
	case FP_DELETE_NON_SELECTED_RASTER:
	case FP_EXPORT_CAMERAS:
	case FP_IMPORT_CAMERAS: return FilterPlugin::NONE;
	case FP_FLATTEN:
	case FP_DELETE_NON_VISIBLE_MESH: return FilterPlugin::VARIABLE;
	}
	return FilterPlugin::NONE;
}

int FilterLayerPlugin::postCondition(const QAction* filter) const
{
	switch (ID(filter)) {
	case FP_RENAME_MESH:
	case FP_DUPLICATE:
	case FP_DELETE_MESH:
	case FP_DELETE_NON_VISIBLE_MESH:
	case FP_FLATTEN:
	case FP_RENAME_RASTER:
	case FP_DELETE_RASTER:
	case FP_DELETE_NON_SELECTED_RASTER:
	case FP_EXPORT_CAMERAS:
	case FP_IMPORT_CAMERAS:
	case FP_SPLITCONNECTED: return MeshModel::MM_NONE;

	case FP_SPLITSELECTEDFACES:
	case FP_SPLITSELECTEDVERTICES: return MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE;

	default: assert(0);
	}
	return FilterPlugin::Generic;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterLayerPlugin)
