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

/*
 * References for the bicubic interpolated texture lookup:
 *  - GPU gems 2 ch 20 (Sigg and Hadwiger, 2005)
 *  - Efficient GPU-Based Texture Interpolation using Uniform B-Splines  (Ruijters et al., 2009)
 * */

#include "mesh.h"
#include "texture_rendering.h"
#include "gl_utils.h"
#include "pushpull.h"
#include "mesh_attribute.h"
#include "logging.h"

#include <iostream>
#include <algorithm>

#include <QImage>



static const char *vs_text[] = {
    "attribute vec2 position;                                    \n"
    "attribute vec2 texcoord;                                    \n"
    "attribute vec4 color;                                       \n"
    "varying vec2 uv;                                            \n"
    "varying vec4 fcolor;                                        \n"
    "                                                            \n"
    "void main(void)                                             \n"
    "{                                                           \n"
    "    uv = texcoord;                                          \n"
    "    fcolor = color;                                         \n"
    "    //if (uv.s < 0) uv = vec2(0.0, 0.0);                    \n"
    "    vec2 p = 2.0 * position - vec2(1.0, 1.0);               \n"
    "    gl_Position = vec4(p, 0.5, 1.0);                        \n"
    "}                                                           \n"
};

static const char *fs_text[] = {
    "uniform sampler2D img0;                                               \n"
    "                                                                      \n"
    "uniform vec2 texture_size;                                            \n"
    "uniform int render_mode;                                              \n"
    "                                                                      \n"
    "varying vec2 uv;                                                      \n"
    "varying vec4 fcolor;                                                  \n"
    "                                                                      \n"
    "void main(void)                                                       \n"
    "{                                                                     \n"
    "    if (render_mode == 0) {                                           \n"
    "        if (uv.s < float(0))                                          \n"
    "            gl_FragColor = vec4(0, 1, 0, 1);                          \n"
    "        else                                                          \n"
    "            gl_FragColor = vec4(texture2D(img0, uv).rgb, 1);          \n"
    "    } else if (render_mode == 1) {                                    \n"
    "        vec2 coord = uv * texture_size - vec2(0.5, 0.5);              \n"
    "        vec2 idx = floor(coord);                                      \n"
    "        vec2 fraction = coord - idx;                                  \n"
    "        vec2 one_frac = vec2(1.0, 1.0) - fraction;                    \n"
    "        vec2 one_frac2 = one_frac * one_frac;                         \n"
    "        vec2 fraction2 = fraction * fraction;                         \n"
    "        vec2 w0 = (1.0/6.0) * one_frac2 * one_frac;                   \n"
    "        vec2 w1 = (2.0/3.0) - 0.5 * fraction2 * (2.0 - fraction);     \n"
    "        vec2 w2 = (2.0/3.0) - 0.5 * one_frac2 * (2.0 - one_frac);     \n"
    "        vec2 w3 = (1.0/6.0) * fraction2 * fraction;                   \n"
    "        vec2 g0 = w0 + w1;                                            \n"
    "        vec2 g1 = w2 + w3;                                            \n"
    "        vec2 h0 = (w1 / g0) - 0.5 + idx;                              \n"
    "        vec2 h1 = (w3 / g1) + 1.5 + idx;                              \n"
    "        vec4 tex00 = texture2D(img0, vec2(h0.x, h0.y) / texture_size);\n"
    "        vec4 tex10 = texture2D(img0, vec2(h1.x, h0.y) / texture_size);\n"
    "        vec4 tex01 = texture2D(img0, vec2(h0.x, h1.y) / texture_size);\n"
    "        vec4 tex11 = texture2D(img0, vec2(h1.x, h1.y) / texture_size);\n"
    "        tex00 = mix(tex00, tex01, g1.y);                              \n"
    "        tex10 = mix(tex10, tex11, g1.y);                              \n"
    "        gl_FragColor = mix(tex00, tex10, g1.x);                       \n"
    "    } else {                                                          \n"
    "        gl_FragColor = fcolor;                                        \n"
    "    }                                                                 \n"
    "}                                                                     \n"
};



static std::shared_ptr<QImage> RenderTexture(std::vector<Mesh::FacePointer>& fvec,
                                             Mesh &m, TextureObjectHandle textureObject,
                                             bool filter, RenderMode imode,
                                             int textureWidth, int textureHeight);


int FacesByTextureIndex(Mesh& m, std::vector<std::vector<Mesh::FacePointer>>& fv)
{
    fv.clear();

    // Detect the number of required textures
    int nTex = 1;
    for (auto&f : m.face) {
        nTex = std::max(nTex, f.cWT(0).N() + 1);
    }

    fv.resize(nTex);

    for (auto& f : m.face) {
        int ti = f.cWT(0).N();
        ensure(ti < nTex);
        fv[ti].push_back(&f);
    }

    return fv.size();
}

std::vector<std::shared_ptr<QImage>> RenderTexture(Mesh& m, TextureObjectHandle textureObject, const std::vector<TextureSize> &texSizes,
                                                   bool filter, RenderMode imode)
{
    std::vector<std::vector<Mesh::FacePointer>> facesByTexture;
    int nTex = FacesByTextureIndex(m, facesByTexture);

    ensure(nTex <= (int) texSizes.size());

    std::vector<std::shared_ptr<QImage>> newTextures;
    for (int i = 0; i < nTex; ++i) {
        std::shared_ptr<QImage> teximg = RenderTexture(facesByTexture[i], m, textureObject, filter, imode, texSizes[i].w, texSizes[i].h);
        newTextures.push_back(teximg);
    }

    return newTextures;
}

static std::shared_ptr<QImage> RenderTexture(std::vector<Mesh::FacePointer>& fvec,
                                      Mesh &m, TextureObjectHandle textureObject,
                                      bool filter, RenderMode imode,
                                      int textureWidth, int textureHeight)
{
    auto WTCSh = GetWedgeTexCoordStorageAttribute(m);

    // sort the faces in increasing order of input texture unit
    auto FaceComparatorByInputTexIndex = [&WTCSh](const Mesh::FacePointer& f1, const Mesh::FacePointer& f2) {
        return WTCSh[f1].tc[0].N() < WTCSh[f2].tc[0].N();
    };

    std::sort(fvec.begin(), fvec.end(), FaceComparatorByInputTexIndex);

    // OpenGL setup

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLint program = CompileShaders(vs_text, fs_text);
    glUseProgram(program);

    CheckGLError();

    // Allocate vertex data

    GLuint vertexbuf;
    glGenBuffers(1, &vertexbuf);

    std::vector<TextureSize> inTexSizes;
    for (std::size_t i = 0; i < textureObject->ArraySize(); ++i) {
        int iw = textureObject->TextureWidth(i);
        int ih = textureObject->TextureHeight(i);
        inTexSizes.push_back({iw, ih});
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuf);
    glBufferData(GL_ARRAY_BUFFER, m.FN()*15*sizeof(float), NULL, GL_STATIC_DRAW);
    float *p = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (auto fptr : fvec) {
        int ti = WTCSh[fptr].tc[0].N();
        for (int i = 0; i < 3; ++i) {
            *p++ = fptr->cWT(i).U();
            *p++ = fptr->cWT(i).V();
            vcg::Point2d uv = WTCSh[fptr].tc[i].P();
            *p++ = uv.X() / inTexSizes[ti].w;
            *p++ = uv.Y() / inTexSizes[ti].h;
            unsigned char *colorptr = (unsigned char *) p;
            *colorptr++ = fptr->C()[0];
            *colorptr++ = fptr->C()[1];
            *colorptr++ = fptr->C()[2];
            *colorptr++ = fptr->C()[3];
            p++;

        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    GLint pos_location = glGetAttribLocation(program, "position");
    glVertexAttribPointer(pos_location, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    glEnableVertexAttribArray(pos_location);

    GLint tc_location = glGetAttribLocation(program, "texcoord");
    glVertexAttribPointer(tc_location, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *) (2*sizeof(float)));
    glEnableVertexAttribArray(tc_location);

    GLint color_location = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, 5*sizeof(float), (void *) (4*sizeof(float)));
    glEnableVertexAttribArray(color_location);

    p = nullptr;
    glBindBuffer(GL_ARRAY_BUFFER, 0); // done, unbind

    int renderedTexWidth = textureWidth;
    int renderedTexHeight = textureHeight;

    GLint drawBuffer;
    glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, renderedTexWidth, renderedTexHeight);

    GLuint renderTarget;
    glGenTextures(1, &renderTarget);
    glBindTexture(GL_TEXTURE_2D, renderTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderedTexWidth, renderedTexHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTarget, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERR << "Framebuffer is not complete " << glCheckFramebufferStatus(GL_FRAMEBUFFER);
        std::exit(-1);
    }

    std::shared_ptr<QImage> textureImage = std::make_shared<QImage>(renderedTexWidth, renderedTexHeight, QImage::Format_ARGB32);

    // disable depth and stencil test (if they were enabled) as the render target does not have the buffers attached
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(0.0f, 1.0f, 0.0f, 128 / float(255));

    glClear(GL_COLOR_BUFFER_BIT);

    auto f0 = fvec.begin();
    auto fbase = f0;
    while (fbase != fvec.end()) {
        auto fcurr = fbase;
        int currTexIndex = WTCSh[*fcurr].tc[0].N();
        while (fcurr != fvec.end() && WTCSh[*fcurr].tc[0].N() == currTexIndex)
            fcurr++;
        int baseIndex = std::distance(f0, fbase) * 3;
        int count = std::distance(fbase, fcurr) * 3;

        // Load texture image
        glActiveTexture(GL_TEXTURE0);
        LOG_DEBUG << "Binding texture unit " << currTexIndex;
        textureObject->Bind(currTexIndex);

        GLint loc_img0 = glGetUniformLocation(program, "img0");
        glUniform1i(loc_img0, 0);
        GLint loc_texture_size = glGetUniformLocation(program, "texture_size");
        glUniform2f(loc_texture_size, float(textureObject->TextureWidth(currTexIndex)), float(textureObject->TextureHeight(currTexIndex)));


        GLint loc_render_mode = glGetUniformLocation(program, "render_mode");
        glUniform1i(loc_render_mode, 0);
        switch (imode) {
        case Cubic:
            glUniform1i(loc_render_mode, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
            break;
        case Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
            break;
        case Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case FaceColor:
            glUniform1i(loc_render_mode, 2);
            break;
        default:
            ensure(0 && "Should never happen");
        }

        glDrawArrays(GL_TRIANGLES, baseIndex, count);
        CheckGLError();

        textureObject->Release(currTexIndex);

        fbase = fcurr;
    }

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, renderedTexWidth, renderedTexHeight, GL_BGRA, GL_UNSIGNED_BYTE, textureImage->bits());

    // clean up
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    glDeleteTextures(1, &renderTarget);
    glDeleteFramebuffers(1, &fbo);
    glDeleteBuffers(1, &vertexbuf);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);

    glDrawBuffer(drawBuffer);

    if (filter)
        vcg::PullPush(*textureImage, qRgba(0, 255, 0, 128));

    Mirror(*textureImage);

    return textureImage;
}
