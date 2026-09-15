#pragma once

#include <common/ml_document/mesh_model.h>
#include <common/ml_shared_data_context/ml_shared_data_context.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <QFile>
#include <iostream>
#include <stdexcept>

using Color = vcg::Color4b;
using Mask = vcg::tri::io::Mask;
using Importer = vcg::tri::io::ImporterPLY<CMeshO>;
using Exporter = vcg::tri::io::ExporterPLY<CMeshO>;
using RD = MLRenderingData;
using Att = RD::ATT_NAMES;
using Policies = MLPoliciesStandAloneFunctions;

inline void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

inline int loadMesh(MeshModel& model, const QString& path = EDGE_COLOR_FIXTURE)
{
    const auto filename = QFile::encodeName(path);
    int mask = 0;
    require(Importer::LoadMask(filename.constData(), mask), "read PLY mask");
    model.enable(mask);
    const int result = Importer::Open(model.cm, filename.constData(), mask);
    require(!Importer::ErrorCritical(result), Importer::ErrorMsg(result));
    model.updateBoxAndNormals();
    return mask;
}

inline RD defaults(MeshModel& model)
{
    RD data;
    Policies::suggestedDefaultPerViewRenderingData(&model, data);
    return data;
}

inline bool attribute(const RD& data, RD::PRIMITIVE_MODALITY primitive, unsigned int name)
{
    RD::RendAtts attributes;
    require(data.get(primitive, attributes), "get rendering attributes");
    return attributes[name];
}

inline void addFace(CMeshO& mesh)
{
    vcg::tri::Allocator<CMeshO>::AddFaces(mesh, 1);
    for (int i = 0; i < 3; ++i) mesh.face.back().V(i) = &mesh.vert[i];
    mesh.face.back().N() = Point3m(0, 0, 1);
    for (auto& v : mesh.vert) v.N() = Point3m(0, 0, 1);
}

inline void saveText(const QString& path, const QByteArray& contents)
{
    QFile file(path);
    require(file.open(QIODevice::WriteOnly), "open fixture output");
    require(file.write(contents) == contents.size(), "write fixture output");
}

inline QByteArray fixtureText()
{
    QFile file(EDGE_COLOR_FIXTURE);
    require(file.open(QIODevice::ReadOnly), "open example fixture");
    return file.readAll();
}
