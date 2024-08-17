/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2023 - 2024                                         \/)\/    *
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

#include "io_3mf.h"

#include "common/ml_document/cmesh.h"
#include "common/ml_document/mesh_model.h"
#include "common/parameters/rich_parameter/rich_bool.h"
#include "common/parameters/rich_parameter_list.h"
#include "lib3mf_implicit.hpp"
#include "lib3mf_types.hpp"
#include "vcg/complex/allocate.h"
#include "vcg/space/color4.h"
#include "wrap/io_trimesh/io_mask.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>

namespace {
Lib3MF::PModel get_model_from_file(const QString& fileName)
{
	const QString errorMsgFormat =
		"Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	if (!QFile::exists(fileName)) {
		throw MLException(errorMsgFormat.arg(fileName, "File does not exist"));
	}

	const auto& wrapper = Lib3MF::CWrapper::loadLibrary();
	if (wrapper == nullptr) {
		throw MLException(errorMsgFormat.arg(fileName, "Failed to initialize 3MF library"));
	}

	const auto& model = wrapper->CreateModel();
	if (model == nullptr) {
		throw MLException(errorMsgFormat.arg(fileName, "Failed to create 3MF internal model"));
	}

	const auto& reader = model->QueryReader("3mf");
	if (model == nullptr) {
		throw MLException(errorMsgFormat.arg(fileName, "Failed to create 3MF reader object"));
	}

	reader->ReadFromFile(fileName.toStdString());

	return model;
}

Lib3MF::PLib3MFMeshObjectIterator get_mesh_iterator(const QString& fileName)
{
	const QString errorMsgFormat =
		"Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	const auto& model = get_model_from_file(fileName);

	return model->GetMeshObjects();
}

Lib3MF::PLib3MFBuildItemIterator get_build_item_iterator(const Lib3MF::PModel& model)
{
	return model->GetBuildItems();
}

void load_mesh_to_meshmodel(
	const Lib3MF::PModel&      model,
	const Lib3MF::PMeshObject& lib3mf_mesh_object,
	MeshModel&                 mesh_model,
	const std::string&         name_postfix,
	bool                       load_material_data)
{
	auto load_or_get_texture_id = [&mesh_model, &model](const int id) {
		const std::string string_id = std::to_string(id);
		const auto&       texture   = mesh_model.getTexture(string_id);
		if (texture.isNull()) {
			std::vector<Lib3MF_uint8> buffer;
			model->GetTexture2DByID(id)->GetAttachment()->WriteToBuffer(buffer);
			QImage image;
			image.loadFromData(buffer.data(), buffer.size());
			mesh_model.addTexture(string_id, image);
		}

		auto texture_id = std::distance(
			mesh_model.cm.textures.begin(),
			std::find(mesh_model.cm.textures.begin(), mesh_model.cm.textures.end(), string_id));
		return texture_id;
	};

	std::vector<size_t> available_property_ids;

	auto base_material_groups = model->GetBaseMaterialGroups();
	while (base_material_groups->MoveNext()) {
		auto current = base_material_groups->GetCurrentBaseMaterialGroup();
		available_property_ids.push_back(current->GetUniqueResourceID());
	}

	auto color_groups = model->GetColorGroups();
	while (color_groups->MoveNext()) {
		auto current = color_groups->GetCurrentColorGroup();
		available_property_ids.push_back(current->GetUniqueResourceID());
	}

	auto texture_groups = model->GetTexture2DGroups();
	while (texture_groups->MoveNext()) {
		auto current = texture_groups->GetCurrentTexture2DGroup();
		available_property_ids.push_back(current->GetUniqueResourceID());
	}

	std::string mesh_name = lib3mf_mesh_object->GetName();
	if (mesh_name.empty()) {
		mesh_name = "Mesh" + name_postfix;
	}
	mesh_model.setLabel(QString::fromStdString(mesh_name));
	const auto n_vertices  = lib3mf_mesh_object->GetVertexCount();
	const auto n_triangles = lib3mf_mesh_object->GetTriangleCount();
	auto       vertex_iterator =
		vcg::tri::Allocator<decltype(mesh_model.cm)>::AddVertices(mesh_model.cm, n_vertices);
	auto face_iterator =
		vcg::tri::Allocator<decltype(mesh_model.cm)>::AddFaces(mesh_model.cm, n_triangles);
	for (size_t i = 0; i < n_vertices; ++i) {
		const auto& pos           = lib3mf_mesh_object->GetVertex(i).m_Coordinates;
		(*vertex_iterator).P()[0] = pos[0];
		(*vertex_iterator).P()[1] = pos[1];
		(*vertex_iterator).P()[2] = pos[2];
		++vertex_iterator;
	}

	// Load the triangles only, but no colors yet
	for (size_t i = 0; i < n_triangles; ++i) {
		const auto& tri       = lib3mf_mesh_object->GetTriangle(i).m_Indices;
		(*face_iterator).V(0) = &mesh_model.cm.vert[tri[0]];
		(*face_iterator).V(1) = &mesh_model.cm.vert[tri[1]];
		(*face_iterator).V(2) = &mesh_model.cm.vert[tri[2]];
		++face_iterator;
	}

	// Load colors or textures, if necessary

	if (load_material_data) {
		for (size_t i = 0; i < n_triangles; ++i) {
			Lib3MF::sTriangleProperties props;
			lib3mf_mesh_object->GetTriangleProperties(i, props);

			if (std::find_if(
					available_property_ids.begin(),
					available_property_ids.end(),
					[props](auto resourceId) { return resourceId == props.m_ResourceID; }) !=
				available_property_ids.end()) {
				const auto& tri = lib3mf_mesh_object->GetTriangle(i).m_Indices;
				switch (model->GetPropertyTypeByID(props.m_ResourceID)) {
				case Lib3MF::ePropertyType::BaseMaterial: {
					mesh_model.enable(vcg::tri::io::Mask::IOM_FACECOLOR);
					auto baseMaterial = model->GetBaseMaterialGroupByID(props.m_ResourceID);
					auto color        = baseMaterial->GetDisplayColor(props.m_PropertyIDs[0]);
					mesh_model.cm.face[i].C() =
						vcg::Color4b {color.m_Red, color.m_Green, color.m_Blue, color.m_Alpha};
					break;
				}
				case Lib3MF::ePropertyType::TexCoord: {
					mesh_model.enable(vcg::tri::io::Mask::IOM_WEDGTEXCOORD);
					auto group = model->GetTexture2DGroupByID(props.m_ResourceID);
					auto texture_id =
						load_or_get_texture_id(group->GetTexture2D()->GetUniqueResourceID());
					auto coord0 = group->GetTex2Coord(props.m_PropertyIDs[0]);
					auto coord1 = group->GetTex2Coord(props.m_PropertyIDs[1]);
					auto coord2 = group->GetTex2Coord(props.m_PropertyIDs[2]);

					mesh_model.cm.face[i].WT(0).U() = coord0.m_U;
					mesh_model.cm.face[i].WT(0).V() = coord0.m_V;
					mesh_model.cm.face[i].WT(0).N() = texture_id;

					mesh_model.cm.face[i].WT(1).U() = coord1.m_U;
					mesh_model.cm.face[i].WT(1).V() = coord1.m_V;
					mesh_model.cm.face[i].WT(1).N() = texture_id;

					mesh_model.cm.face[i].WT(2).U() = coord2.m_U;
					mesh_model.cm.face[i].WT(2).V() = coord2.m_V;
					mesh_model.cm.face[i].WT(2).N() = texture_id;
					break;
				}
				case Lib3MF::ePropertyType::Colors: {
					mesh_model.enable(vcg::tri::io::Mask::IOM_FACECOLOR);
					auto colorGroup = model->GetColorGroupByID(props.m_ResourceID);
					auto color0     = colorGroup->GetColor(props.m_PropertyIDs[0]);
					mesh_model.cm.face[i].C() =
						vcg::Color4b {color0.m_Red, color0.m_Green, color0.m_Blue, color0.m_Alpha};
					break;
				}
				default: break;
				};
			}
		}
	}
}

} // namespace

Lib3MFPlugin::Lib3MFPlugin()
{
}

QString Lib3MFPlugin::pluginName() const
{
	return "3MF importer and exporter";
}

std::list<FileFormat> Lib3MFPlugin::importFormats() const
{
	return {FileFormat {"3MF File Format", tr("3MF")}};
}

std::list<FileFormat> Lib3MFPlugin::exportFormats() const
{
	return {FileFormat {"3MF File Format", tr("3MF")}};
}

RichParameterList Lib3MFPlugin::initPreOpenParameter(const QString& /*format*/) const
{
	RichParameterList result;
	result.addParam(RichBool(
		"usecolors",
		false,
		"Load colors and textures",
		"When turned on, loads color and texture information from the file. Turn off if you "
		"experience slow rendering performance."));
	result.addParam(RichBool(
		"forcetransform",
		false,
		"Transform vertices instead of using transformation matrix",
		"When turned on, transform the vertices directly, instead of creating "
		"a transformation "
		"matrix"));
	return result;
}

unsigned int Lib3MFPlugin::numberMeshesContainedInFile(
	const QString&           format,
	const QString&           fileName,
	const RichParameterList& preParams) const
{
	const QString errorMsgFormat =
		"Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	try {
		const auto& model               = get_model_from_file(fileName);
		const auto& build_item_iterator = get_build_item_iterator(model);

		if (build_item_iterator == nullptr) {
			throw MLException(
				errorMsgFormat.arg(fileName, "Failed to iterate over build items in file"));
		}

		if (build_item_iterator->Count() == 0) {
			throw MLException(
				errorMsgFormat.arg(fileName, "The file does not contain any models!"));
		}

		return build_item_iterator->Count();
	}
	catch (const Lib3MF::ELib3MFException& e) {
		std::stringstream message_stream;
		message_stream << "An exception occurred while opening the 3MF file.\n" << e.what();
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
}

void Lib3MFPlugin::open(
	const QString&               format,
	const QString&               fileName,
	const std::list<MeshModel*>& meshModelList,
	std::list<int>&              maskList,
	const RichParameterList&     par,
	vcg::CallBackPos*            cb)
{
	const QString errorMsgFormat =
		"Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	try {
		using namespace vcg::tri::io;

		if (cb != nullptr) {
			(*cb)(0, std::string("Loading " + fileName.toStdString()).c_str());
		}

		auto lib3mf_model        = get_model_from_file(fileName);
		auto build_item_iterator = get_build_item_iterator(lib3mf_model);
		auto mesh_model_iterator = meshModelList.begin();
		if (meshModelList.size() != build_item_iterator->Count()) {
			throw MLException(errorMsgFormat.arg(
				fileName,
				"Internal error while loading mesh objects: inconsistent number of meshes "
				"encontered"));
		}

		auto delta_percent = 100 / meshModelList.size();

		for (size_t i_mesh = 0; i_mesh < meshModelList.size(); ++i_mesh) {
			build_item_iterator->MoveNext();
			const auto& current_build_item = build_item_iterator->GetCurrent();
			if (current_build_item == nullptr) {
				throw MLException(errorMsgFormat.arg(fileName, "Failed to access build item"));
			}

			const auto& object = current_build_item->GetObjectResource();
			if (!object->IsMeshObject()) {
				throw MLException(errorMsgFormat.arg(
					fileName, "Error while loading mesh object: build item is not a mesh"));
			}

			const auto& current_mesh_object =
				lib3mf_model->GetMeshObjectByID(object->GetResourceID());

			if (current_mesh_object == nullptr) {
				throw MLException(errorMsgFormat.arg(
					fileName, "Internal error while loading mesh objects: invalid mesh object"));
			}

			auto current_mesh_model = (*mesh_model_iterator);
			if (current_mesh_model == nullptr) {
				throw MLException(errorMsgFormat.arg(
					fileName, "Internal error while loading mesh objects: invalid mesh model"));
			}

			// TODO (lvk88): even if enable WEDGCOLOR, meshlab will crash when trying to add a
			// per vertex wedge color later on maskList.push_back(Mask::IOM_VERTCOORD |
			// Mask::IOM_FACEINDEX | Mask::IOM_WEDGCOLOR); current_mesh_model->enable(
			// Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX | Mask::Mask::IOM_WEDGCOLOR);

			maskList.push_back(Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX);
			current_mesh_model->enable(Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX);

			load_mesh_to_meshmodel(
				lib3mf_model,
				current_mesh_object,
				*current_mesh_model,
				"_" + std::to_string(i_mesh),
				par.getBool("usecolors"));

			if (current_build_item->HasObjectTransform()) {
				auto      transform = current_build_item->GetObjectTransform();
				Matrix44m tr;
				tr.SetZero();
				tr.V()[0]  = transform.m_Fields[0][0];
				tr.V()[1]  = transform.m_Fields[0][1];
				tr.V()[2]  = transform.m_Fields[0][2];
				tr.V()[4]  = transform.m_Fields[1][0];
				tr.V()[5]  = transform.m_Fields[1][1];
				tr.V()[6]  = transform.m_Fields[1][2];
				tr.V()[8]  = transform.m_Fields[2][0];
				tr.V()[9]  = transform.m_Fields[2][1];
				tr.V()[10] = transform.m_Fields[2][2];
				tr.V()[12] = transform.m_Fields[3][0];
				tr.V()[13] = transform.m_Fields[3][1];
				tr.V()[14] = transform.m_Fields[3][2];
				tr.V()[15] = 1.0;
				if (par.getBool("forcetransform")) {
					vcg::tri::UpdatePosition<decltype(current_mesh_model->cm)>::Matrix(
						current_mesh_model->cm, tr.transpose(), true);
				}
				else {
					current_mesh_model->cm.Tr = tr.transpose();
				}
			}

			mesh_model_iterator++;
			if (cb != nullptr) {
				cb(i_mesh * delta_percent,
				   std::string(
					   "Finished reading mesh " + std::to_string(i_mesh + 1) + " of " +
					   std::to_string(meshModelList.size()))
					   .c_str());
			}
		}
	}
	catch (const Lib3MF::ELib3MFException& e) {
		std::stringstream message_stream;
		message_stream << "An exception occurred while opening the 3MF file.\n" << e.what();
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
}

void Lib3MFPlugin::open(
	const QString&           formatName,
	const QString&           fileName,
	MeshModel&               m,
	int&                     mask,
	const RichParameterList& par,
	vcg::CallBackPos*        cb)
{
	wrongOpenFormat("This should have not happened!");
}

void Lib3MFPlugin::save(
	const QString&           formatName,
	const QString&           fileName,
	MeshModel&               m,
	const int                mask,
	const RichParameterList& par,
	vcg::CallBackPos*        cb)
{
	const QString errorMsgFormat =
		"Error encountered while saving file:\n\"%1\"\n\nError details: %2";

	const auto& wrapper = Lib3MF::CWrapper::loadLibrary();
	if (wrapper == nullptr) {
		throw MLException(errorMsgFormat.arg(fileName, "Could not init 3mf library"));
	}

	const auto& model = wrapper->CreateModel();
	if (model == nullptr) {
		throw MLException(errorMsgFormat.arg(fileName, "Could not create model for writing"));
	}

	const auto& metadata_group = model->GetMetaDataGroup();
	metadata_group->AddMetaData("", "Application", "Meshlab", "string", false);

	const auto& mesh = model->AddMeshObject();
	mesh->SetName(m.label().toStdString());

	std::vector<Lib3MF::sPosition> vertex_buffer;
	vertex_buffer.reserve(m.cm.vert.size());

	std::vector<int> vertex_ids(m.cm.vert.size());

	std::vector<Lib3MF::sTriangle> triangle_buffer;
	triangle_buffer.reserve(m.cm.face.size());

	int number_of_vertices = 0;
	for (auto vertex_it = m.cm.vert.begin(); vertex_it != m.cm.vert.end(); ++vertex_it) {
		if (vertex_it->IsD())
			continue;
		vertex_ids[vertex_it - m.cm.vert.begin()] = number_of_vertices;
		Lib3MF::sPosition pos;
		pos.m_Coordinates[0] = vertex_it->P()[0];
		pos.m_Coordinates[1] = vertex_it->P()[1];
		pos.m_Coordinates[2] = vertex_it->P()[2];
		vertex_buffer.push_back(pos);
		number_of_vertices++;
	}

	for (auto face_it = m.cm.face.begin(); face_it != m.cm.face.end(); ++face_it) {
		if (face_it->IsD())
			continue;
		if (face_it->VN() != 3) {
			throw MLException(
				errorMsgFormat.arg(fileName, "Only triangular meshes can be written to 3mf files"));
		}
		Lib3MF::sTriangle triangle;
		triangle.m_Indices[0] = vertex_ids[vcg::tri::Index(m.cm, face_it->V(0))];
		triangle.m_Indices[1] = vertex_ids[vcg::tri::Index(m.cm, face_it->V(1))];
		triangle.m_Indices[2] = vertex_ids[vcg::tri::Index(m.cm, face_it->V(2))];
		triangle_buffer.push_back(triangle);
	}

	mesh->SetGeometry(vertex_buffer, triangle_buffer);
	model->AddBuildItem(mesh.get(), wrapper->GetIdentityTransform());
	const auto& writer = model->QueryWriter("3mf");
	writer->WriteToFile(fileName.toStdString());
}

void Lib3MFPlugin::exportMaskCapability(const QString& format, int& capability, int& defaultBits)
	const
{
	capability  = 0;
	defaultBits = 0;
}

MESHLAB_PLUGIN_NAME_EXPORTER(Lib3MFPlugin)
