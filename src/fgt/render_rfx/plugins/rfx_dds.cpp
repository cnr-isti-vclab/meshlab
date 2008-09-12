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

GLubyte* RfxDDSPlugin::LoadAsImage(const QString &f, int *w, int *h)
{
	QList<RfxState*> e = QList<RfxState*>();
	GLuint newtex = Load(f, e);

	if (newtex == 0) {
		*w = 0;
		*h = 0;
		return NULL;
	}

	*w = 0;
	*h = 0;
	int imgSize = 0;

	// 2d texture is read as is, cubemap is unfolded and for volume textures
	// we read only one "slice"
	switch (texTarget) {
	case GL_TEXTURE_2D:
		imgSize = width * height * 4; /* RGBA = 4 components */
		*w = width;
		*h = height;
		break;
	case GL_TEXTURE_CUBE_MAP:
		*w = width * 4;
		*h = height * 3;
		imgSize = (width * 4) * (height * 3) * 4;
		break;
	case GL_TEXTURE_3D:
		*w = width;
		*h = height;
		imgSize = width * height * 4; /* RGBA = 4 components */
		break;
	default:
		return NULL;
	}

	unsigned char *img = new unsigned char[imgSize];

	switch (texTarget) {
	case GL_TEXTURE_2D:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		break;
	case GL_TEXTURE_CUBE_MAP: {

		/*
		 * we want cubemap "unfolded" this way so we'll get each cubemap face
		 * and copy into the bigger image in the right position
		 *  _ _ _ _
		 * |_|X|_|_|
		 * |X|X|X|X|
		 * |_|X|_|_|
		 */

		// fill whole area with transparent color first
		memset(img, 0x0, imgSize);

		// offsets for each cube face
		// (expressed in number of blocks in the 4 * 3 dst rectangle)
		//
		//                       X+ X- Y+ Y- Z+ Z-
		const int woffsets[6] = {2, 0, 1, 1, 1, 3};
		const int hoffsets[6] = {1, 1, 0, 2, 1, 1};

		// not that we really need to define these, but they clarify
		// a bit next instructions :)
		const int bpp = 4;
		int cubefaceSize = width * height * bpp;

		unsigned char *cfaceImg = new unsigned char[cubefaceSize];
		for (int i = 0; i < 6; ++i) {
			glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			              GL_UNSIGNED_BYTE, cfaceImg);

			// cubemaps use DX coord system. We have to vertically flip image.
			unsigned char tmp;
			int lineLen = width * bpp;
			unsigned char *line1 = cfaceImg;
			unsigned char *line2 = cfaceImg + (height - 1) * lineLen;

			for (; line1 < line2; line2 -= (lineLen * 2)) {
				for (int j = 0; j < lineLen; ++j, ++line1, ++line2) {
					tmp    = *line1;
					*line1 = *line2;
					*line2 = tmp;
				}
			}

			// then copy cubemap face into the bigger unfolded image
			unsigned char *imgPtr = img;
			unsigned char *facePtr = cfaceImg;

			// calculate where we want to copy the cubemap face
			imgPtr += ((woffsets[i] * width) +
			           (hoffsets[i] * width * 4 * height)) * bpp;

			// we have to copy 'height' lines
			for (unsigned int i = 0; i < height; ++i) {

				// copy the whole cubemap scanline
				memcpy(imgPtr, facePtr, (width * bpp));

				// advance to next scanline both in cubemap face and
				imgPtr += (width * bpp * 4);
				facePtr += (width * bpp);
			}
		}
		delete[] cfaceImg;
		break;
	}
	case GL_TEXTURE_3D: {
		unsigned char *realImg = new unsigned char[imgSize * depth];
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, realImg);
		memcpy(img, realImg, imgSize);
		delete[] realImg;
		break;
	}
	default:
		return NULL;
	}

	// take care of deleting texture
	glDeleteTextures(1, &newtex);
	return img;
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
		    !GLEW_EXT_texture_compression_s3tc) {
			f.close();
			return 0;
		}

		switch (ddsh.ddpfPixelFormat.dwFourCC) {
		case FOURCC_DXT1:
			texFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
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
	if (mipCount > 1) {
		glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texTarget == GL_TEXTURE_CUBE_MAP) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	foreach (RfxState *s, states)
		s->SetEnvironment(texTarget);

	for (int i = 0; i < ((texTarget == GL_TEXTURE_CUBE_MAP)? 6 : 1); ++i) {

		// mipmap generation have to be disabled for each face of the cubemap
		if ((texTarget == GL_TEXTURE_CUBE_MAP) && mipCount > 1)
			glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);

		int cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		int w = width;
		int h = height;
		int d = depth;
		int size;

		for (unsigned int lev = 0; lev < mipCount; ++lev) {

			// calculate size of data to read based on texture type
			size = ((compressed)?
			         (((w + 3) / 4) * ((h + 3) / 4) * blockBytes) :
			         (w * h * components)) * d;

			GLubyte *pixels = new GLubyte[size];
			f.read((char*)pixels, size);
			f.seek(f.pos() + size);

			// vertically flip block if not a cubemap texture
			if (texTarget != GL_TEXTURE_CUBE_MAP)
				flipImg((char*)pixels, w, h, d, size);

			switch (texTarget) {
			case GL_TEXTURE_2D:
			case GL_TEXTURE_CUBE_MAP:
				if (compressed) {
					glCompressedTexImage2D(((texTarget == GL_TEXTURE_2D)?
					                         GL_TEXTURE_2D : cubeFace),
					                       lev, texFormat, w, h, 0,
					                       size, pixels);
				} else {
					glTexImage2D(((texTarget == GL_TEXTURE_2D)?
	                               GL_TEXTURE_2D : cubeFace),
	                             lev, components, w, h, 0,
					             texFormat, GL_UNSIGNED_BYTE, pixels);
				}
				break;

			case GL_TEXTURE_3D:
				if (compressed) {
					glCompressedTexImage3D(GL_TEXTURE_3D, lev, texFormat,
				                           w, h, d, 0, size, pixels);
				} else {
					glTexImage3D(GL_TEXTURE_3D, lev, components, w, h, d,
					             0, texFormat, GL_UNSIGNED_BYTE, pixels);
				}
				break;
			}

			// half size for each mip-map level
			w /= 2;
			h /= 2;
			d /= 2;

			if (w < 1)
				w = 1;
			if (h < 1)
				h = 1;
			if (d < 1)
				d = 1;

			delete[] pixels;
		}
	}

	f.close();

	glTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL, mipCount - 1);

	return tex;
}

/*
 * flipImg - vertically flip an image (either uncompressed pixels or a dxt{1,3,5} block)
 *           (code adapted from OpenSG [http://opensg.vrsource.org])
 */
void RfxDDSPlugin::flipImg(char *image, int width, int height, int depth, int size)
{
	int linesize;
	int offset;

	if (!compressed) {
		assert(depth > 0);

		int imagesize = size / depth;
		linesize = imagesize / height;

		for (int n = 0; n < depth; n++) {
			offset = imagesize * n;
			char *top = image + offset;
			char *bottom = top + (imagesize - linesize);

			for (int i = 0; i < (height / 2); i++) {
				swap(bottom, top, linesize);

				top += linesize;
				bottom -= linesize;
			}
		}
	} else {

		void (RfxDDSPlugin::*flipblocks)(DXTColBlock*, int);
		int xblocks = width / 4;
		int yblocks = height / 4;
		int blocksize;

		switch (texFormat) {
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			blocksize = 8;
			flipblocks = &RfxDDSPlugin::flip_blocks_dxtc1;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			blocksize = 16;
			flipblocks = &RfxDDSPlugin::flip_blocks_dxtc3;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			blocksize = 16;
			flipblocks = &RfxDDSPlugin::flip_blocks_dxtc5;
			break;
		default:
			return;
		}

		linesize = xblocks * blocksize;

		DXTColBlock *top;
		DXTColBlock *bottom;

		for (int j = 0; j < (yblocks / 2); j++) {
			top = (DXTColBlock*)(image + j * linesize);
			bottom = (DXTColBlock*)(image + (((yblocks-j)-1) * linesize));

			(this->*flipblocks)(top, xblocks);
			(this->*flipblocks)(bottom, xblocks);

			swap(bottom, top, linesize);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// flip a DXT1 color block
void RfxDDSPlugin::flip_blocks_dxtc1(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;

	for (int i = 0; i < numBlocks; i++) {
		swap(&curblock->row[0], &curblock->row[3], 1 /* sizeof(char) */);
		swap(&curblock->row[1], &curblock->row[2], 1 /* sizeof(char) */);

		curblock++;
	}
}

///////////////////////////////////////////////////////////////////////////////
// flip a DXT3 color block
void RfxDDSPlugin::flip_blocks_dxtc3(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	DXT3AlphaBlock *alphablock;

	for (int i = 0; i < numBlocks; i++) {
		alphablock = (DXT3AlphaBlock*)curblock;

		swap(&alphablock->row[0], &alphablock->row[3], sizeof(short));
		swap(&alphablock->row[1], &alphablock->row[2], sizeof(short));

		curblock++;

		swap(&curblock->row[0], &curblock->row[3], 1 /* sizeof(char) */);
		swap(&curblock->row[1], &curblock->row[2], 1 /* sizeof(char) */);

		curblock++;
	}
}

///////////////////////////////////////////////////////////////////////////////
// flip a DXT5 alpha block
void RfxDDSPlugin::flip_dxt5_alpha(DXT5AlphaBlock *block)
{
	char gBits[4][4];

	const int mask = 0x00000007;          // bits = 00 00 01 11
	int bits = 0;
	memcpy(&bits, &block->row[0], 3);

	gBits[0][0] = (char)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (char)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (char)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (char)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (char)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (char)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (char)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (char)(bits & mask);

	bits = 0;
	memcpy(&bits, &block->row[3], 3);

	gBits[2][0] = (char)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (char)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (char)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (char)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (char)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (char)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (char)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (char)(bits & mask);

	int *pBits = ((int*) &(block->row[0]));

	*pBits = *pBits | (gBits[3][0] << 0);
	*pBits = *pBits | (gBits[3][1] << 3);
	*pBits = *pBits | (gBits[3][2] << 6);
	*pBits = *pBits | (gBits[3][3] << 9);

	*pBits = *pBits | (gBits[2][0] << 12);
	*pBits = *pBits | (gBits[2][1] << 15);
	*pBits = *pBits | (gBits[2][2] << 18);
	*pBits = *pBits | (gBits[2][3] << 21);

	pBits = ((int*) &(block->row[3]));

	if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
		*pBits &= 0x000000ff;
	else
		*pBits &= 0xff000000;

	*pBits = *pBits | (gBits[1][0] << 0);
	*pBits = *pBits | (gBits[1][1] << 3);
	*pBits = *pBits | (gBits[1][2] << 6);
	*pBits = *pBits | (gBits[1][3] << 9);

	*pBits = *pBits | (gBits[0][0] << 12);
	*pBits = *pBits | (gBits[0][1] << 15);
	*pBits = *pBits | (gBits[0][2] << 18);
	*pBits = *pBits | (gBits[0][3] << 21);
}

///////////////////////////////////////////////////////////////////////////////
// flip a DXT5 color block
void RfxDDSPlugin::flip_blocks_dxtc5(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	DXT5AlphaBlock *alphablock;

	for (int i = 0; i < numBlocks; i++) {
		alphablock = (DXT5AlphaBlock*)curblock;
		flip_dxt5_alpha(alphablock);

		curblock++;

		swap(&curblock->row[0], &curblock->row[3], 1 /* sizeof(char) */);
		swap(&curblock->row[1], &curblock->row[2], 1 /* sizeof(char) */);

		curblock++;
	}
}

///////////////////////////////////////////////////////////////////////////////
// swap to sections of memory
void RfxDDSPlugin::swap(void *byte1, void *byte2, int size)
{
	unsigned char *tmp = new unsigned char[size];

	memcpy(tmp, byte1, size);
	memcpy(byte1, byte2, size);
	memcpy(byte2, tmp, size);

	delete[] tmp;
}
