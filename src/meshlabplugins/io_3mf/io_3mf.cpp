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

void Lib3MFPlugin::open(
                        const QString& formatName,
                        const QString& fileName,
                        MeshModel& m,
                        int& mask,
                        const RichParameterList& par,
                        vcg::CallBackPos* cb)
{

  using namespace vcg::tri::io;

  const QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

  if( !QFile::exists( fileName ) )
  {
    throw MLException(errorMsgFormat.arg(fileName, "File does not exist"));
  }

  if( cb != nullptr)
  {
    (*cb)(0, "Loading...");
  }

  mask = Mask::IOM_VERTCOORD | Mask::IOM_FACEINDEX;


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

  const auto& object_iterator = model->GetMeshObjects();
  if( object_iterator == nullptr )
  {
    throw MLException(errorMsgFormat.arg(fileName, "Failed to iterate over objects in file"));
  }

  while(object_iterator->MoveNext())
  {
    const auto& mesh_object = object_iterator->GetCurrentMeshObject();
    m.setLabel(QString::fromStdString(mesh_object->GetName()));
    const auto n_vertices  = mesh_object->GetVertexCount();
    const auto n_triangles = mesh_object->GetTriangleCount();
    auto vertex_iterator = vcg::tri::Allocator<decltype(m.cm)>::AddVertices(m.cm, n_vertices);
    auto face_iterator = vcg::tri::Allocator<decltype(m.cm)>::AddFaces(m.cm, n_triangles);
    for(size_t i = 0; i < n_vertices; ++i)
    {
      const auto& pos = mesh_object->GetVertex(i).m_Coordinates;
      (*vertex_iterator).P()[0] = pos[0];
      (*vertex_iterator).P()[1] = pos[1];
      (*vertex_iterator).P()[2] = pos[2];
      ++vertex_iterator;
    }
    for(size_t i = 0; i < n_triangles; ++i)
    {
      const auto& tri = mesh_object->GetTriangle(i).m_Indices;
      (*face_iterator).V(0) = &m.cm.vert[tri[0]];
      (*face_iterator).V(1) = &m.cm.vert[tri[1]];
      (*face_iterator).V(2) = &m.cm.vert[tri[2]];
      ++face_iterator;
    }
  }

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
