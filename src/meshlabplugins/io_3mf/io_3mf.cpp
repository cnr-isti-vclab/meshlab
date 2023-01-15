/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "common/ml_document/mesh_model.h"
#include "wrap/io_trimesh/io_mask.h"

#include "lib3mf_implicit.hpp"

namespace
{
  Lib3MF::PLib3MFMeshObjectIterator get_mesh_iterator(const QString& fileName)
  {
    const QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

    if( !QFile::exists( fileName ) )
    {
      throw MLException(errorMsgFormat.arg(fileName, "File does not exist"));
    }

    const auto& wrapper = Lib3MF::CWrapper::loadLibrary();
    if( wrapper == nullptr )
    {
      throw MLException(errorMsgFormat.arg(fileName, "Failed to initialize 3MF library"));
    }

    const auto& model = wrapper->CreateModel();
    if( model == nullptr )
    {
      throw MLException(errorMsgFormat.arg(fileName, "Failed to create 3MF internal model"));
    }

    const auto& reader = model->QueryReader("3mf");
    if( model == nullptr )
    {
      throw MLException(errorMsgFormat.arg(fileName, "Failed to create 3MF reader object"));
    }

    reader->ReadFromFile(fileName.toStdString());
    return model->GetMeshObjects();
  }

  void load_mesh_to_meshmodel(const Lib3MF::PMeshObject& lib3mf_mesh_object, MeshModel& mesh_model)
  {
    mesh_model.setLabel(QString::fromStdString(lib3mf_mesh_object->GetName()));
    const auto n_vertices  = lib3mf_mesh_object->GetVertexCount();
    const auto n_triangles = lib3mf_mesh_object->GetTriangleCount();
    auto vertex_iterator = vcg::tri::Allocator<decltype(mesh_model.cm)>::AddVertices(mesh_model.cm, n_vertices);
    auto face_iterator = vcg::tri::Allocator<decltype(mesh_model.cm)>::AddFaces(mesh_model.cm, n_triangles);
    for(size_t i = 0; i < n_vertices; ++i)
    {
      const auto& pos = lib3mf_mesh_object->GetVertex(i).m_Coordinates;
      (*vertex_iterator).P()[0] = pos[0];
      (*vertex_iterator).P()[1] = pos[1];
      (*vertex_iterator).P()[2] = pos[2];
      ++vertex_iterator;
    }
    for(size_t i = 0; i < n_triangles; ++i)
    {
      const auto& tri = lib3mf_mesh_object->GetTriangle(i).m_Indices;
      (*face_iterator).V(0) = &mesh_model.cm.vert[tri[0]];
      (*face_iterator).V(1) = &mesh_model.cm.vert[tri[1]];
      (*face_iterator).V(2) = &mesh_model.cm.vert[tri[2]];
      ++face_iterator;
    }
  }

}

Lib3MFPlugin::Lib3MFPlugin()
{
}

QString Lib3MFPlugin::pluginName() const
{
  return "3MF importer and exporter";
}

std::list<FileFormat> Lib3MFPlugin::importFormats() const
{
  return{FileFormat{"3MF File Format", tr("3MF")}};
}

std::list<FileFormat> Lib3MFPlugin::exportFormats() const
{
  return{FileFormat{"3MF File Format", tr("3MF")}};
}

unsigned int Lib3MFPlugin::numberMeshesContainedInFile(
                                         const QString& format,
                                         const QString& fileName,
                                         const RichParameterList& preParams) const 
{
  const QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

  const auto& object_iterator = get_mesh_iterator(fileName);
  if( object_iterator == nullptr )
  {
    throw MLException(errorMsgFormat.arg(fileName, "Failed to iterate over objects in file"));
  }
  return object_iterator->Count();

}

void Lib3MFPlugin::open(
                        const QString &format,
                        const QString &fileName,
                        const std::list<MeshModel*>& meshModelList,
                        std::list<int>& maskList,
                        const RichParameterList & par,
                        vcg::CallBackPos *cb)
{

  using namespace vcg::tri::io;

  const QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

  auto mesh_iterator = get_mesh_iterator(fileName);
  auto mesh_model_iterator = meshModelList.begin();

  if(meshModelList.size() != mesh_iterator->Count())
  {
    throw MLException(errorMsgFormat.arg(fileName, "Internal error while loading mesh objects: inconsistent number of meshes encontered"));
  }

  for(size_t i_mesh = 0; i_mesh < meshModelList.size(); ++i_mesh)
  {
    mesh_iterator->MoveNext();
    const auto& current_mesh_object = mesh_iterator->GetCurrentMeshObject();
    if(current_mesh_object == nullptr)
    {
      throw MLException(errorMsgFormat.arg(fileName, "Internal error while loading mesh objects: invalid mesh object"));
    }

    auto current_mesh_model = (*mesh_model_iterator);
    if(current_mesh_model == nullptr)
    {
      throw MLException(errorMsgFormat.arg(fileName, "Internal error while loading mesh objects: invalid mesh model"));
    }

    load_mesh_to_meshmodel(current_mesh_object, *current_mesh_model);
    maskList.push_back(Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX);
    current_mesh_model->enable( Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX);

    mesh_model_iterator++;
  }
}

void Lib3MFPlugin::open(
                        const QString& formatName,
                        const QString& fileName,
                        MeshModel& m,
                        int& mask,
                        const RichParameterList& par,
                        vcg::CallBackPos* cb)
{
  using namespace vcg::tri::io;

  mask = Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX;

  const QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

  const auto& object_iterator = get_mesh_iterator(fileName);
  if( object_iterator == nullptr )
  {
    throw MLException(errorMsgFormat.arg(fileName, "Failed to iterate over objects in file"));
  }

  const auto& mesh_object = object_iterator->GetCurrentMeshObject();
  if(mesh_object == nullptr)
  {
    throw MLException(errorMsgFormat.arg(fileName, "Invalid mesh object encountered"));
  }

  load_mesh_to_meshmodel(mesh_object, m);

  m.enable(mask);

  return;
}

void Lib3MFPlugin::save(
                        const QString &formatName,
                        const QString &fileName,
                        MeshModel &m,
                        const int mask,
                        const RichParameterList& par,
                        vcg::CallBackPos* cb)
{
}

void Lib3MFPlugin::exportMaskCapability(
                                        const QString &format,
                                        int& capability,
                                        int& defaultBits) const 
{
}

MESHLAB_PLUGIN_NAME_EXPORTER(Lib3MFPlugin)
