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

#ifndef TEXTURE_OBJECT_H
#define TEXTURE_OBJECT_H

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include <QImage>

struct TextureObject;

typedef std::shared_ptr<TextureObject> TextureObjectHandle;

struct TextureSize {
    int w;
    int h;
};

struct TextureImageInfo {
    QImage texture;
};

/* wrapper to an array of textures */
struct TextureObject {

    std::vector<TextureImageInfo> texInfoVec;
    std::vector<uint32_t> texNameVec;

    TextureObject();
    ~TextureObject();

    TextureObject(const TextureObject &) = delete;
    TextureObject &operator=(const TextureObject &) = delete;

    /* Add QImage ref to the texture object */
    bool AddImage(std::string path);
    bool AddImage(const QImage& image);

    /* Binds the texture at index i */
    void Bind(int i);

    /* Releases the texture i, without unbinding it if it is bound */
    void Release(int i);

    int TextureWidth(std::size_t i);
    int TextureHeight(std::size_t i);

    int MaxSize();
    std::vector<TextureSize> GetTextureSizes();

    std::size_t ArraySize();

    int64_t TextureArea(std::size_t i);
    double GetResolutionInMegaPixels();

    std::vector<std::pair<double, double>> ComputeRelativeSizes();
};

/* Vertically mirrors a QImage in-place, useful to match the OpenGL convention
 * for texture data storage */
void Mirror(QImage& img);


#endif // TEXTURE_OBJECT_H
