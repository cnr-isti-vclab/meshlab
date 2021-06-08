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

#include "texture_object.h"
#include "logging.h"
#include "utils.h"
#include "gl_utils.h"

#include <cmath>

#include <QImageReader>
#include <QImage>


TextureObject::TextureObject()
{
}

TextureObject::~TextureObject()
{
    for (std::size_t i = 0; i < texNameVec.size(); ++i)
        Release(i);
}

bool TextureObject::AddImage(std::string path)
{
    QImageReader qir(QString(path.c_str()));
    if (qir.canRead()) {
        TextureImageInfo tii = {QImage(path.c_str())};
        texInfoVec.push_back(tii);
        texNameVec.push_back(0);
        return true;
    } else return false;
}

bool TextureObject::AddImage(const QImage& image)
{
    TextureImageInfo tii = {QImage(image)};
    texInfoVec.push_back(tii);
    texNameVec.push_back(0);
    return true;
}

void TextureObject::Bind(int i)
{
    ensure(i >= 0 && i < (int) texInfoVec.size());
    // load texture from qimage on first use
    if (texNameVec[i] == 0) {
        QImage& img = texInfoVec[i].texture;
        ensure(!img.isNull());
        if ((img.format() != QImage::Format_RGB32) || (img.format() != QImage::Format_ARGB32)) {
            QImage glimg = img.convertToFormat(QImage::Format_ARGB32);
            img = glimg;
        }
        glGenTextures(1, &texNameVec[i]);

        Mirror(img);
        glBindTexture(GL_TEXTURE_2D, texNameVec[i]);
        int miplevels = std::log2((float) img.width());
        int width = img.width();
        int height = img.height();
        for (int m = 0; m < miplevels; m++) {
            glTexImage2D(GL_TEXTURE_2D, m, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
            width = std::max(1, (width / 2));
            height = std::max(1, (height / 2));
        }
        //glTexStorage2D(GL_TEXTURE_2D, miplevels, GL_RGBA8, img.width(), img.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width(), img.height(), GL_BGRA, GL_UNSIGNED_BYTE, img.constBits());
        glGenerateMipmap(GL_TEXTURE_2D);
        CheckGLError();
        Mirror(img);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, texNameVec[i]);
        CheckGLError();
    }
}

void TextureObject::Release(int i)
{
    ensure(i >= 0 && i < (int) texInfoVec.size());
    if (texNameVec[i]) {
        glDeleteTextures(1, &texNameVec[i]);
        texNameVec[i] = 0;
    }
}

int TextureObject::TextureWidth(std::size_t i)
{
    ensure(i < texInfoVec.size());
    return texInfoVec[i].texture.width();
}

int TextureObject::TextureHeight(std::size_t i)
{
    ensure(i < texInfoVec.size());
    return texInfoVec[i].texture.height();
}

int TextureObject::MaxSize()
{
    int maxsz = 0;
    for (unsigned i = 0; i < ArraySize(); ++i) {
        maxsz = std::max(maxsz, TextureWidth(i));
        maxsz = std::max(maxsz, TextureHeight(i));
    }
    return maxsz;
}

std::vector<TextureSize> TextureObject::GetTextureSizes()
{
    std::vector<TextureSize> texszVec;
    for (unsigned i = 0; i < ArraySize(); ++i)
        texszVec.push_back({TextureWidth(i), TextureHeight(i)});
    return texszVec;

}

std::size_t TextureObject::ArraySize()
{
    return texInfoVec.size();
}

int64_t TextureObject::TextureArea(std::size_t i)
{
    ensure(i < ArraySize());
    return ((int64_t) TextureWidth(i)) * TextureHeight(i);
}

double TextureObject::GetResolutionInMegaPixels()
{
    int64_t totArea = 0;
    for (unsigned i = 0; i < ArraySize(); ++i) {
        totArea += TextureArea(i);
    }
    return totArea / 1000000.0;
}

std::vector<std::pair<double, double>> TextureObject::ComputeRelativeSizes()
{
    std::vector<TextureSize> texSizeVec = GetTextureSizes();
    int maxsz = 0;
    for (auto tsz : texSizeVec) {
        maxsz = std::max(maxsz, tsz.h);
        maxsz = std::max(maxsz, tsz.w);
    }
    std::vector<std::pair<double, double>> trs;
    for (auto tsz : texSizeVec) {
        double rw = tsz.w / (double) maxsz;
        double rh = tsz.h / (double) maxsz;
        trs.push_back(std::make_pair(rw, rh));
    }
    return trs;
}

void Mirror(QImage& img)
{
    int i = 0;
    while (i < (img.height() / 2)) {
        QRgb *line0 = (QRgb *) img.scanLine(i);
        QRgb *line1 = (QRgb *) img.scanLine(img.height() - 1 - i);
        i++;
        for (int j = 0; j < img.width(); ++j)
            std::swap(line0[j], line1[j]);
    }
}

