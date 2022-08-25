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

#ifndef TEXTURE_RENDERING_H
#define TEXTURE_RENDERING_H

#include "texture_object.h"
#include "mesh_graph.h"

#include <vector>

class Mesh;
class MeshFace;

enum RenderMode {
    Nearest, Linear, Cubic, FaceColor
};

int FacesByTextureIndex(Mesh& m, std::vector<std::vector<Mesh::FacePointer>>& fv);

std::vector<std::shared_ptr<QImage>>
RenderTexture(Mesh& m, TextureObjectHandle textureObject, const std::vector<TextureSize> &texSizes,
              bool filter, RenderMode imode);

#endif // TEXTURE_RENDERING_H

