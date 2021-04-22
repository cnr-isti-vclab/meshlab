/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/attribute_seam.h>

#include "gl_utils.h" // required for obj importer to use glu::tessellator

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

#include <string>

#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QImageReader>

#include "mesh.h"
#include "texture_object.h"
#include "timer.h"
#include "utils.h"
#include "logging.h"

bool LoadMesh(const char *fileName, Mesh& m, TextureObjectHandle& textureObject, int &loadMask)
{
    m.Clear();
    textureObject = std::make_shared<TextureObject>();
    loadMask = 0;

    QFileInfo fi(fileName);
    fi.makeAbsolute();

    if (!fi.exists() || !fi.isReadable()) {
        LOG_ERR << "Unable to read " << fileName;
        return false;
    }

    std::string dirname = fi.dir().dirName().toStdString();
    m.name = dirname + "_" + fi.fileName().toStdString();

    QString wd = QDir::currentPath();
    QDir::setCurrent(fi.absoluteDir().absolutePath());

    int r = tri::io::Importer<Mesh>::Open(m, fi.fileName().toStdString().c_str(), loadMask);
    if (tri::io::Importer<Mesh>::ErrorCritical(r)) {
        LOG_ERR << tri::io::Importer<Mesh>::ErrorMsg(r);
        return false;
    } else if (r) {
        LOG_WARN << tri::io::Importer<Mesh>::ErrorMsg(r);
    }

    for (auto& f : m.face)
        f.SetMesh();

    LOG_INFO << "Loaded mesh " << fileName << " (VN " <<  m.VN() << ", FN " << m.FN() << ")";

    for (const string& textureName : m.textures) {
        QFileInfo textureFile(textureName.c_str());
        textureFile.makeAbsolute();
        if (!textureFile.exists() || !textureFile.isReadable()) {
            LOG_ERR << "Error: Texture file " << textureName.c_str() << " does not exist or is not readable.";
            return false;
        }

        if (!textureObject->AddImage(textureFile.absoluteFilePath().toStdString())) {
            LOG_ERR << "Error: Unable to load texture file " << textureName.c_str();
            return false;
        }
    }

    QDir::setCurrent(wd);
    return true;
}

bool SaveMesh(const char *fileName, Mesh& m, const std::vector<std::shared_ptr<QImage>>& textureImages, bool color)
{
    int mask = tri::io::Mask::IOM_WEDGTEXCOORD;

    m.textures.clear();
    for (std::size_t i = 0; i < textureImages.size(); ++i) {
        std::stringstream suffix;
        suffix << "_texture_" << i << ".png";
        std::string s(fileName);
        m.textures.push_back(s.substr(0, s.find_last_of('.')).append(suffix.str()));
    }

    Timer t;
    LOG_INFO << "Saving mesh file " << fileName;
    if (color) mask = mask | tri::io::Mask::IOM_FACEQUALITY | tri::io::Mask::IOM_FACECOLOR;
    int err;
    if ((err = tri::io::Exporter<Mesh>::Save(m, fileName, mask))) {
        LOG_ERR << "Error: " << tri::io::Exporter<Mesh>::ErrorMsg(err);
        return false;
    }
    LOG_INFO << "Saving mesh took " << t.TimeElapsed() << " seconds";

    QFileInfo fi(fileName);
    ensure (fi.exists());

    QString wd = QDir::currentPath();
    QDir::setCurrent(fi.absoluteDir().absolutePath());

    t.Reset();
    LOG_INFO << "Saving texture files... ";
    for (std::size_t i = 0; i < textureImages.size(); ++i) {
        if (textureImages[i]->save(m.textures[i].c_str(), "png", 66) == false) {
            LOG_ERR << "Error saving texture file " << m.textures[i];
            return false;
        }
    }
    LOG_INFO << "Writing textures took " << t.TimeElapsed() << " seconds";

    QDir::setCurrent(wd);
    return true;
}

void ScaleTextureCoordinatesToImage(Mesh& m, TextureObjectHandle textureObject)
{
    for (auto& f : m.face) {
        int ti = f.WT(0).N();
        for (int i = 0; i < f.VN(); ++i) {
            f.WT(i).P().X() *= (ti < (int) textureObject->ArraySize()) ? textureObject->TextureWidth(ti) : 1.0;
            f.WT(i).P().Y() *= (ti < (int) textureObject->ArraySize()) ? textureObject->TextureHeight(ti) : 1.0;
        }
    }
}

void ScaleTextureCoordinatesToParameterArea(Mesh& m, TextureObjectHandle textureObject)
{
    for (auto& f : m.face) {
        int ti = f.WT(0).N();
        for (int i = 0; i < f.VN(); ++i) {
            f.WT(i).P().X() /= (ti < (int) textureObject->ArraySize()) ? textureObject->TextureWidth(ti) : 1.0;
            f.WT(i).P().Y() /= (ti < (int) textureObject->ArraySize()) ? textureObject->TextureHeight(ti) : 1.0;
        }
    }
}

Box2d UVBox(const Mesh& m)
{
    Box2d uvbox;
    for(auto const& f : m.face) {
        for (int i = 0; i < 3; ++i) {
            uvbox.Add(f.cWT(i).P());
        }
    }
    return uvbox;
}

Box2d UVBoxVertex(const Mesh& m)
{
    Box2d uvbox;
    for(auto const& f : m.face) {
        for (int i = 0; i < 3; ++i) {
            uvbox.Add(f.cV(i)->T().P());
        }
    }
    return uvbox;
}

/* Utility functions for AttributeSeam */
static inline void vExt(const Mesh& msrc, const MeshFace& f, int k, const Mesh& mdst, MeshVertex& v)
{
    (void) msrc;
    (void) mdst;
    v.ImportData(*(f.cV(k)));
    v.T() = f.cWT(k);
}

static inline bool vCmp(const Mesh& mdst, const MeshVertex& v1, const MeshVertex& v2)
{
    (void) mdst;
    return v1.T() == v2.T();
}

void CutAlongSeams(Mesh& m)
{
    tri::AttributeSeam::SplitVertex(m, vExt, vCmp);
    tri::Allocator<Mesh>::CompactVertexVector(m);
    tri::UpdateTopology<Mesh>::FaceFace(m);
    tri::UpdateTopology<Mesh>::VertexFace(m);
}

void MeshFromFacePointers(const std::vector<Mesh::FacePointer>& vfp, Mesh& out)
{
    out.Clear();
    std::unordered_map<Mesh::VertexPointer, Mesh::VertexPointer> vpmap;
    vpmap.reserve(vfp.size() * 2);
    std::size_t vn = 0;
    for (auto fptr : vfp) {
        for (int i = 0; i < 3; ++i) {
            if (vpmap.count(fptr->V(i)) == 0) {
                vn++;
                vpmap[fptr->V(i)] = nullptr;
            }
        }
    }
    auto mvi = tri::Allocator<Mesh>::AddVertices(out, vn);
    auto mfi = tri::Allocator<Mesh>::AddFaces(out, vfp.size());
    for (auto fptr : vfp) {
        Mesh::FacePointer mfp = &*mfi++;
        for (int i = 0; i < 3; ++i) {
            Mesh::VertexPointer vp = fptr->V(i);
            typename Mesh::VertexPointer& mvp = vpmap[vp];
            if (mvp == nullptr) {
                mvp = &*mvi++;
                mvp->P() = vp->P();
            }
            mfp->V(i) = mvp;
            mfp->WT(i) = fptr->WT(i);
        }
        mfp->SetMesh();
    }
}
