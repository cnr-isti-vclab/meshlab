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
#include <exception>
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

// Loads all the textures from the model into QImage-s and returns them in a map
// where the key is the unique resource ID of the texture
std::map<std::string, QImage> load_textures(const Lib3MF::PModel& model)
{
	std::map<std::string, QImage> result;
	auto                          textures = model->GetTexture2Ds();
	if (textures == nullptr) {
		throw std::runtime_error("Could not get iterator to textures");
	}

	while (textures->MoveNext()) {
		auto current_texture = textures->GetCurrentTexture2D();
		auto id              = current_texture->GetUniqueResourceID();
		auto attachment      = current_texture->GetAttachment();
		if (attachment == nullptr) {
			throw std::runtime_error("Attachment to texture returned a nullptr");
		}
		std::vector<Lib3MF_uint8> buffer;
		attachment->WriteToBuffer(buffer);
		QImage image;
		image.loadFromData(buffer.data(), buffer.size());
		result.insert({std::to_string(id), image});
	}

	return result;
}

std::map<int, CMeshO> load_meshes(const Lib3MF::PModel& model)
{
	std::map<int, CMeshO> result;
	if (model == nullptr) {
		throw std::runtime_error("Got a null model!");
	}

	auto meshes = model->GetMeshObjects();
	if (meshes == nullptr) {
		throw std::runtime_error("Could not access mesh iterators!");
	}

	while (meshes->MoveNext()) {
		auto current_mesh = meshes->GetCurrentMeshObject();
		if (current_mesh == nullptr) {
			throw std::runtime_error("Error accessing mesh from iterator");
		}

		auto   id = current_mesh->GetUniqueResourceID();
		CMeshO cmesh;

		auto n_vertices  = current_mesh->GetVertexCount();
		auto n_triangles = current_mesh->GetTriangleCount();

		auto vertex_iterator = vcg::tri::Allocator<decltype(cmesh)>::AddVertices(cmesh, n_vertices);
		auto face_iterator   = vcg::tri::Allocator<decltype(cmesh)>::AddFaces(cmesh, n_triangles);

		for (size_t i = 0; i < n_vertices; ++i) {
			const auto& pos           = current_mesh->GetVertex(i).m_Coordinates;
			(*vertex_iterator).P()[0] = pos[0];
			(*vertex_iterator).P()[1] = pos[1];
			(*vertex_iterator).P()[2] = pos[2];
			++vertex_iterator;
		}

		for (size_t i = 0; i < n_triangles; ++i) {
			const auto& tri       = current_mesh->GetTriangle(i).m_Indices;
			(*face_iterator).V(0) = &cmesh.vert[tri[0]];
			(*face_iterator).V(1) = &cmesh.vert[tri[1]];
			(*face_iterator).V(2) = &cmesh.vert[tri[2]];
			++face_iterator;
		}

		result.insert({id, cmesh});
	}

	return result;
}

void load_mesh_to_meshmodel(
	const Lib3MF::PModel&                model,
	const Lib3MF::PMeshObject&           lib3mf_mesh_object,
	MeshModel&                           mesh_model,
	const std::string&                   name_postfix,
	const std::map<std::string, QImage>& textures,
	bool                                 load_material_data)
{
	auto load_or_get_texture_id = [&mesh_model, &model, &textures](const int id) {
		const std::string string_id = std::to_string(id);

		// If the mesh model doesn't have this texture yet, load it
		if (mesh_model.getTexture(string_id).isNull()) {
			if (textures.find(string_id) == textures.end()) {
				throw std::runtime_error(
					"One of the meshes uses a texture that does not exist in the model!");
			}
			auto texture = textures.at(string_id);
			mesh_model.addTexture(string_id, texture);
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
	catch (const std::exception& e) {
		std::stringstream message_stream;
		message_stream << "An exception occurred while opening the 3MF file.\n" << e.what();
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
	catch (...) {
		std::stringstream message_stream;
		message_stream << "An unkown error occurred while opening the 3MF file.\n";
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
}

void to_cmesh(const Lib3MF::PMeshObject& mesh_object, CMeshO& target)
{
	auto n_vertices  = mesh_object->GetVertexCount();
	auto n_triangles = mesh_object->GetTriangleCount();

	auto vertex_iterator =
		vcg::tri::Allocator<std::remove_reference_t<decltype(target)>>::AddVertices(
			target, n_vertices);
	auto face_iterator = vcg::tri::Allocator<std::remove_reference_t<decltype(target)>>::AddFaces(
		target, n_triangles);

	for (int i = 0; i < n_vertices; ++i) {
		const auto& pos           = mesh_object->GetVertex(i).m_Coordinates;
		(*vertex_iterator).P()[0] = pos[0];
		(*vertex_iterator).P()[1] = pos[1];
		(*vertex_iterator).P()[2] = pos[2];
		++vertex_iterator;
	}

	for (size_t i = 0; i < n_triangles; ++i) {
		const auto& tri       = mesh_object->GetTriangle(i).m_Indices;
		(*face_iterator).V(0) = &target.vert[tri[0]];
		(*face_iterator).V(1) = &target.vert[tri[1]];
		(*face_iterator).V(2) = &target.vert[tri[2]];
		++face_iterator;
	}
}

bool append_props(const Lib3MF::PModel model, const Lib3MF::PMeshObject mesh_object, CMeshO& cmesh)
{
	auto n_triangles = mesh_object->GetTriangleCount();

	bool result = false;

	for (int i = 0; i < n_triangles; ++i) {
		Lib3MF::sTriangleProperties props;
		mesh_object->GetTriangleProperties(i, props);
		if (props.m_ResourceID == 0) {
			continue;
		}

		switch (model->GetPropertyTypeByID(props.m_ResourceID)) {
		case Lib3MF::ePropertyType::BaseMaterial: {
			result            = true;
			auto baseMaterial = model->GetBaseMaterialGroupByID(props.m_ResourceID);
			auto color        = baseMaterial->GetDisplayColor(props.m_PropertyIDs[0]);
			cmesh.face[i].C() =
				vcg::Color4b {color.m_Red, color.m_Green, color.m_Blue, color.m_Alpha};
			break;
		}
		case Lib3MF::ePropertyType::TexCoord: {
			auto group      = model->GetTexture2DGroupByID(props.m_ResourceID);
			auto texture_id = std::distance(
				cmesh.textures.begin(),
				std::find(
					cmesh.textures.begin(),
					cmesh.textures.end(),
					std::to_string(group->GetTexture2D()->GetUniqueResourceID())));
			auto coord0 = group->GetTex2Coord(props.m_PropertyIDs[0]);
			auto coord1 = group->GetTex2Coord(props.m_PropertyIDs[1]);
			auto coord2 = group->GetTex2Coord(props.m_PropertyIDs[2]);

			cmesh.face[i].WT(0).U() = coord0.m_U;
			cmesh.face[i].WT(0).V() = coord0.m_V;
			cmesh.face[i].WT(0).N() = texture_id;

			cmesh.face[i].WT(1).U() = coord1.m_U;
			cmesh.face[i].WT(1).V() = coord1.m_V;
			cmesh.face[i].WT(1).N() = texture_id;

			cmesh.face[i].WT(2).U() = coord2.m_U;
			cmesh.face[i].WT(2).V() = coord2.m_V;
			cmesh.face[i].WT(2).N() = texture_id;
			break;
		}
		case Lib3MF::ePropertyType::Colors: {
			// mesh_model.enable(vcg::tri::io::Mask::IOM_FACECOLOR);
			result          = true;
			auto colorGroup = model->GetColorGroupByID(props.m_ResourceID);
			auto color0     = colorGroup->GetColor(props.m_PropertyIDs[0]);
			cmesh.face[i].C() =
				vcg::Color4b {color0.m_Red, color0.m_Green, color0.m_Blue, color0.m_Alpha};
			break;
		}
		default: break;
		};
	}

	return result;
}

void read_components(
	int                              level,
	const Lib3MF::PModel&            model,
	const Lib3MF::PComponentsObject& componentsObject,
	MeshModel&                       meshModel,
	Matrix44m                        T)
{
	for (int iComponent = 0; iComponent < componentsObject->GetComponentCount(); ++iComponent) {
		auto component        = componentsObject->GetComponent(iComponent);
		auto objectResource   = component->GetObjectResource();
		auto currentTransform = T;
		if (component->HasTransform()) {
			auto      transform = component->GetTransform();
			Matrix44m componentT;
			componentT.ElementAt(0, 0) = transform.m_Fields[0][0];
			componentT.ElementAt(0, 1) = transform.m_Fields[0][1];
			componentT.ElementAt(0, 2) = transform.m_Fields[0][2];
			componentT.ElementAt(1, 0) = transform.m_Fields[1][0];
			componentT.ElementAt(1, 1) = transform.m_Fields[1][1];
			componentT.ElementAt(1, 2) = transform.m_Fields[1][2];
			componentT.ElementAt(2, 0) = transform.m_Fields[2][0];
			componentT.ElementAt(2, 1) = transform.m_Fields[2][1];
			componentT.ElementAt(2, 2) = transform.m_Fields[2][2];
			componentT.ElementAt(3, 0) = transform.m_Fields[3][0];
			componentT.ElementAt(3, 1) = transform.m_Fields[3][1];
			componentT.ElementAt(3, 2) = transform.m_Fields[3][2];
			componentT.ElementAt(3, 3) = 1.0;
			currentTransform           = currentTransform * componentT.transpose();
		}
		if (objectResource->IsMeshObject()) {
			auto   meshObject  = model->GetMeshObjectByID(objectResource->GetUniqueResourceID());
			auto   n_vertices  = meshObject->GetVertexCount();
			auto   n_triangles = meshObject->GetTriangleCount();
			CMeshO new_cmesh;
			for (const auto& texture : meshModel.getTextures()) {
				new_cmesh.textures.push_back(texture.first);
			}
			to_cmesh(meshObject, new_cmesh);
			new_cmesh.face.EnableColor();
			new_cmesh.face.EnableWedgeTexCoord();
			meshModel.enable(
				vcg::tri::io::Mask::IOM_FACECOLOR | vcg::tri::io::Mask::IOM_WEDGTEXCOORD);
			append_props(model, meshObject, new_cmesh);
			vcg::tri::UpdatePosition<decltype(meshModel.cm)>::Matrix(new_cmesh, currentTransform);
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(meshModel.cm, new_cmesh);
		}
		else if (objectResource->IsComponentsObject()) {
			std::cout << "Component " << objectResource->GetUniqueResourceID() << std::endl;
			read_components(
				level + 1,
				model,
				model->GetComponentsObjectByID(objectResource->GetUniqueResourceID()),
				meshModel,
				currentTransform);
		}
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

	// Lib3MF doesn't seem to account for the fact that an object may contain a
	// sequence of components and meshes, we can only access either a single
	// mesh or a single component!
	// Go over every build item
	// Get the object that the build item refers to
	//   If it's a mesh, load it into a cmesh and return it
	//   If it's a components object, visit the children components recursively, keep track of the
	//   transformations until we find a mesh and then load it.

	try {
		using namespace vcg::tri::io;

		if (cb != nullptr) {
			(*cb)(0, std::string("Loading " + fileName.toStdString()).c_str());
		}

		auto lib3mf_model        = get_model_from_file(fileName);
		auto build_item_iterator = get_build_item_iterator(lib3mf_model);
		auto mesh_model_iterator = meshModelList.begin();
		auto textures            = load_textures(lib3mf_model);
		auto meshes              = load_meshes(lib3mf_model);

		auto build_item_count = 0;

		auto delta_progress = 100 / build_item_iterator->Count();

		while (build_item_iterator->MoveNext()) {
			if (cb != nullptr) {
				(*cb)(
					delta_progress * build_item_count,
					std::string("Loading mesh " + std::to_string(build_item_count)).c_str());
			}
			auto& mesh_model         = *(mesh_model_iterator++);
			auto  current_build_item = build_item_iterator->GetCurrent();
			auto  object_resource    = current_build_item->GetObjectResource();
			for (const auto& texture : textures) {
				const auto& id    = texture.first;
				const auto& image = texture.second;
				mesh_model->addTexture(id, image);
			}
			if (object_resource->IsMeshObject()) {
				auto mesh_object =
					lib3mf_model->GetMeshObjectByID(object_resource->GetUniqueResourceID());
				to_cmesh(mesh_object, mesh_model->cm);
				mesh_model->enable(
					vcg::tri::io::Mask::IOM_FACECOLOR | vcg::tri::io::Mask::IOM_WEDGTEXCOORD);
				append_props(lib3mf_model, mesh_object, mesh_model->cm);
			}
			else if (object_resource->IsComponentsObject()) {
				read_components(
					1,
					lib3mf_model,
					lib3mf_model->GetComponentsObjectByID(object_resource->GetUniqueResourceID()),
					*mesh_model,
					Matrix44m::Identity());
			}

			if (current_build_item->HasObjectTransform()) {
				auto      transform = current_build_item->GetObjectTransform();
				Matrix44m T;
				T.ElementAt(0, 0) = transform.m_Fields[0][0];
				T.ElementAt(0, 1) = transform.m_Fields[0][1];
				T.ElementAt(0, 2) = transform.m_Fields[0][2];
				T.ElementAt(1, 0) = transform.m_Fields[1][0];
				T.ElementAt(1, 1) = transform.m_Fields[1][1];
				T.ElementAt(1, 2) = transform.m_Fields[1][2];
				T.ElementAt(2, 0) = transform.m_Fields[2][0];
				T.ElementAt(2, 1) = transform.m_Fields[2][1];
				T.ElementAt(2, 2) = transform.m_Fields[2][2];
				T.ElementAt(3, 0) = transform.m_Fields[3][0];
				T.ElementAt(3, 1) = transform.m_Fields[3][1];
				T.ElementAt(3, 2) = transform.m_Fields[3][2];
				T.ElementAt(3, 3) = 1.0;
				if (par.getBool("forcetransform")) {
					vcg::tri::UpdatePosition<decltype(mesh_model->cm)>::Matrix(
						mesh_model->cm, T.transpose(), true);
				}
				else {
					mesh_model->cm.Tr = T.transpose();
				}
			}
			build_item_count++;
		}
	}
	catch (const Lib3MF::ELib3MFException& e) {
		std::stringstream message_stream;
		message_stream << "An exception occurred while opening the 3MF file.\n" << e.what();
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
	catch (const std::exception& e) {
		std::stringstream message_stream;
		message_stream << "An exception occurred while opening the 3MF file.\n" << e.what();
		log(message_stream.str());
		throw MLException(
			errorMsgFormat.arg(fileName, QString::fromStdString((message_stream.str()))));
	}
	catch (...) {
		std::stringstream message_stream;
		message_stream << "An unkown error occurred while opening the 3MF file.\n";
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
