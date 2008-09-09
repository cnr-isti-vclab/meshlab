/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#include "rfx_dds.h"

QList<QByteArray> RfxDDSPlugin::supportedFormats()
{
	return QList<QByteArray>() << "dds";
}

GLuint RfxDDSPlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	QFile f(fName);
	f.open(QIODevice::ReadOnly);

	DDSHeader ddsh;
	f.read((char*)&ddsh, sizeof(DDSHeader));
	f.seek(ddsh.dwSize + 4);

	if (strncmp((char*)&ddsh.dwMagic, "DDS ", 4) != 0 || ddsh.dwSize != 124 ||
	    !(ddsh.dwFlags & DDSD_PIXELFORMAT) || !(ddsh.dwFlags & DDSD_CAPS)) {

		f.close();
		return 0;
	}

	// find out texture type
	texTarget = GL_TEXTURE_2D;
	if (ddsh.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP)
		texTarget = GL_TEXTURE_CUBE_MAP;
	if ((ddsh.ddsCaps.dwCaps2 & DDSCAPS2_VOLUME) && (ddsh.dwDepth > 0))
		texTarget = GL_TEXTURE_3D;

	compressed = (ddsh.ddpfPixelFormat.dwFlags & DDPF_FOURCC);
	mipCount = (ddsh.dwFlags & DDSD_MIPMAPCOUNT) ? ddsh.dwMipMapCount : 1;
	width = ddsh.dwWidth;
	height = ddsh.dwHeight;
	depth = (ddsh.dwDepth <= 0)? 1 : ddsh.dwDepth;

	short blockBytes = 0;
	short components = 0;
	if (compressed) {
		// texture compression support needed
		if (!GLEW_ARB_texture_compression ||
		    !GLEW_EXT_texture_compression_s3tc)
			return 0;

		switch (ddsh.ddpfPixelFormat.dwFourCC) {
		case FOURCC_DXT1:
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockBytes = 8;
			break;
		case FOURCC_DXT3:
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockBytes = 16;
			break;
		case FOURCC_DXT5:
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockBytes = 16;
			break;
		default:
			f.close();
			return 0;
		}

	} else {
		if (ddsh.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE) {
			if (ddsh.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
				texFormat = GL_LUMINANCE_ALPHA;
			else
				texFormat = GL_LUMINANCE;
			components = ddsh.ddpfPixelFormat.dwRGBBitCount / 8;

		} else if (ddsh.ddpfPixelFormat.dwFlags & DDPF_RGB) {
			if (ddsh.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
				texFormat = GL_RGBA;
			else
				texFormat = GL_RGB;
			components = ddsh.ddpfPixelFormat.dwRGBBitCount / 8;

		} else {
			f.close();
			return 0;
		}
	}

	glGenTextures(1, &tex);
	glBindTexture(texTarget, tex);

	// default parameters if no states set
	glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// do not generate mipmaps
	glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);

	foreach (RfxState *s, states)
		s->SetEnvironment(texTarget);

	for (int i = 0; i < ((texTarget == GL_TEXTURE_CUBE_MAP)? 6 : 1); ++i) {

		int cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i;
		int w = width;
		int h = height;
		int d = depth;
		int size;

		for (unsigned int lev = 0; lev < mipCount; ++lev) {

			// calculate size of data to read based on texture type
			size = d * (compressed) ?
				((w + 3) / 4) * ((h + 3) / 4) * blockBytes :
				w * h * components;

			GLubyte *pixels = new GLubyte[size];
			f.read((char*)pixels, size);
			f.seek(f.pos() + size);

			switch (texTarget) {

			case GL_TEXTURE_2D:
				if (compressed)
					glCompressedTexImage2D(GL_TEXTURE_2D, lev, texFormat,
					                       w, h, 0, size, pixels);
				else
					glTexImage2D(GL_TEXTURE_2D, lev, components, w, h, 0,
					             texFormat, GL_UNSIGNED_BYTE, pixels);
				break;

			case GL_TEXTURE_3D:
				if (compressed)
					glCompressedTexImage3D(GL_TEXTURE_3D, lev, texFormat,
				                           w, h, d, 0, size, pixels);
				else
					glTexImage3D(GL_TEXTURE_3D, lev, components, w, h, d,
					             0, texFormat, GL_UNSIGNED_BYTE, pixels);
				break;

			case GL_TEXTURE_CUBE_MAP:
				if (compressed)
					glCompressedTexImage2D(cubeFace, lev, texFormat, w, h,
					                       0, size, pixels);
				else
					glTexImage2D(cubeFace, lev, components, w, h, 0, texFormat,
					             GL_UNSIGNED_BYTE, pixels);
				break;
			}

			// half size for each mip-map level
			w /= 2;
			h /= 2;
			d /= 2;

			if (w == 0)
				w = 1;
			if (h == 0)
				h = 1;
			if (d == 0)
				d = 1;

			delete[] pixels;
		}
	}

	f.close();

	glTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL, mipCount - 1);

	return tex;
}
