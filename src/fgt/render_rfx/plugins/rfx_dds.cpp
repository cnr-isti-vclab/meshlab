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

ImageInfo RfxDDSPlugin::LoadAsQImage(const QString &f)
{
	ImageInfo ii;
	QList<RfxState*> e = QList<RfxState*>();

	// instead of reading raw pixels (which may be compressed)
	// let opengl load texture and then read it back with glGetTexImage
	GLuint newtex = Load(f, e);

	if (!newtex)
		return ii;

	// 2D texture is read as is
	// Cubemap is unfolded and layed horizontally this way -+--
	// if Volume texture only one "slice" is read
	int w;
	int h;
	switch (texTarget) {
	case GL_TEXTURE_2D:
		w = width;
		h = height;
		ii.texType = "2D Texture";
		break;
	case GL_TEXTURE_3D:
		w = width;
		h = height;
		ii.texType = "Volume Texture";
		break;
	case GL_TEXTURE_CUBE_MAP:
		w = (width * 4);
		h = (height * 3);
		ii.texType = "Cubemap Texture";
		break;
	default:
		return ii;
	}
	ii.width = width;
	ii.height = height;
	ii.depth = depth;

	if (isCompressed) {
		switch (texFormat) {
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			ii.format = "DXT1 Compressed RGB";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			ii.format = "DXT1 Compressed RGBA";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			ii.format = "DXT3 Compressed RGBA";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			ii.format = "DXT5 Compressed RGBA";
			break;
		default:
			return ii;
		}
	} else {
		switch (texFormat) {
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE:
			ii.format = QString().setNum(components * 8) + "bpp B/W";
			break;
		case GL_RGBA:
			ii.format = "32bpp RGBA";
			break;
		case GL_RGB:
			ii.format = "24bpp RGB";
			break;
		default:
			return ii;
		}
	}

	QImage img(w, h, QImage::Format_ARGB32);

	switch (texTarget) {
	case GL_TEXTURE_2D: {
		unsigned char *tempBuf = new unsigned char[w * h * 3];
		unsigned char *tempBufPtr = tempBuf;
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBuf);
		for (int i = 0; i < h; ++i) {
			QRgb *scanLine = (QRgb*)img.scanLine(i);
			for (int j = 0; j < w; ++j) {
				scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
				tempBufPtr += 3;
			}
		}
		delete[] tempBuf;
		break;
	}
	case GL_TEXTURE_CUBE_MAP: {

		//
		// we want cubemap "unfolded" this way so we'll get each cubemap face
		// and copy into the bigger image in the right position
		//  _ _ _ _
		// |_|X|_|_|
		// |X|X|X|X|
		// |_|X|_|_|
		//

		// fill whole area with transparent color first
		img.fill(0x0);

		// offsets for each cube face
		// (expressed in number of blocks in the 4 * 3 dst rectangle)
		//
		//                       X+ X- Y+ Y- Z+ Z-
		const int woffsets[6] = {2, 0, 1, 1, 1, 3};
		const int hoffsets[6] = {1, 1, 0, 2, 1, 1};

		int cubefaceSize = width * height * 3;
		unsigned char *cfaceImg = new unsigned char[cubefaceSize];
		for (int i = 0; i < 6; ++i) {

			glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			              GL_UNSIGNED_BYTE, cfaceImg);

			// copy cubemap face into the bigger unfolded image
			unsigned char *facePtr = cfaceImg;
			int yPos = hoffsets[i] * height;
			int xOffset = woffsets[i] * width;

			// we have to copy 'height' lines
			for (int y = 0; y < height; ++y) {
				QRgb *dst = (QRgb*)img.scanLine(yPos + y) + xOffset;
				for (int x = 0; x < width; ++x) {
					dst[x] = qRgb(facePtr[0], facePtr[1], facePtr[2]);
					facePtr += 3;
				}
			}
		}
		delete[] cfaceImg;
		break;
	}

	case GL_TEXTURE_3D: {
		unsigned char *realImg = new unsigned char[w * h * 3 * depth];
		unsigned char *realImgPtr = realImg;
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RGB, GL_UNSIGNED_BYTE, realImg);
		for (int i = 0; i < h; ++i) {
			QRgb *scanLine = (QRgb*)img.scanLine(i);
			for (int j = 0; j < w; ++j) {
				scanLine[j] = qRgb(realImgPtr[0], realImgPtr[1], realImgPtr[2]);
				realImgPtr += 3;
			}
		}
		delete[] realImg;
		break;
	}
	default:
		return ii;
	}

	// delete texture
	glDeleteTextures(1, &newtex);

	ii.preview = img;
	return ii;
}

GLuint RfxDDSPlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	unsigned char *pixels = LoadImageData(fName);
	if (!pixels)
		return 0;

	if (texFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
		if (DXT1CheckAlpha(pixels, ((width + 3) / 4) * ((height + 3) / 4) * 8))
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

	glGenTextures(1, &tex);
	glBindTexture(texTarget, tex);

	// default parameters if no states set
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (mipCount > 1) {
		glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		if (GLEW_SGIS_generate_mipmap) {
			glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
			glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if (texTarget == GL_TEXTURE_CUBE_MAP) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	foreach (RfxState *s, states)
		s->SetEnvironment(texTarget);

	int offset = 0;
	for (int i = 0; i < ((isCubemap)? 6 : 1); ++i) {

		// mipmap generation have to be disabled for each face of the cubemap
		if ((texTarget == GL_TEXTURE_CUBE_MAP) && mipCount > 1)
			glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);

		int cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		int w = width;
		int h = height;
		int d = depth;
		int size;

		for (int lev = 0; lev < mipCount; ++lev) {

			// calculate size of data to read based on texture type
			size = ((isCompressed)?
			         (((w + 3) / 4) * ((h + 3) / 4) * components) :
			         (w * h * components)) * d;

			// vertically flip block if not a cubemap texture
			if (!isCubemap)
				flipImg((char*)pixels, w, h, d, size);

			switch (texTarget) {
			case GL_TEXTURE_2D:
			case GL_TEXTURE_CUBE_MAP: {
				int target = (isCubemap)? cubeFace : GL_TEXTURE_2D;

				if (isCompressed) {
					glCompressedTexImage2D(target, lev, texFormat, w, h, 0,
					                       size, pixels + offset);
				} else {
					glTexImage2D(target, lev, components, w, h, 0,
					             texFormat, GL_UNSIGNED_BYTE, pixels + offset);
				}
				break;
			}
			case GL_TEXTURE_3D:
				if (isCompressed) {
					glCompressedTexImage3D(GL_TEXTURE_3D, lev, texFormat,
				                           w, h, d, 0, size, pixels + offset);
				} else {
					glTexImage3D(GL_TEXTURE_3D, lev, components, w, h, d,
					             0, texFormat, GL_UNSIGNED_BYTE, pixels + offset);
				}
				break;
			}

			// half size for each mip-map level
			w = qMax(w / 2, 1);
			h = qMax(h / 2, 1);
			d = qMax(d / 2, 1);
			offset += size;
		}
	}

	delete[] pixels;
	glTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL, mipCount - 1);

	return tex;
}

unsigned char* RfxDDSPlugin::LoadImageData(const QString &fName)
{
	QFile f(fName);
	f.open(QIODevice::ReadOnly);

	DDSHeader ddsh;
	f.read((char*)&ddsh, sizeof(DDSHeader));
	f.seek(ddsh.dwSize + 4);

	if (!ValidateHeader(ddsh)) {
		f.close();
		return NULL;
	}

	// set appropriate texture type
	texTarget = GL_TEXTURE_2D;
	if (isCubemap)
		texTarget = GL_TEXTURE_CUBE_MAP;
	if (isVolume)
		texTarget = GL_TEXTURE_3D;

	if (!GetOGLFormat(ddsh)) {
		f.close();
		return NULL;
	}

	int totalImgSize = ComputeImageSize();
	unsigned char *pData = new unsigned char[totalImgSize];
	f.read((char*)pData, totalImgSize);
	f.close();

	return pData;
}

int RfxDDSPlugin::ComputeImageSize()
{
	int sz = 0;
	for (int i = 0; i < ((isCubemap)? 6 : 1); ++i) {
		int w = width;
		int h = height;
		int d = qMax(depth, 1);
		for (int j = 0; j < mipCount; ++j) {
			if (isCompressed) {
				sz += ((w + 3) / 4) * ((h + 3) / 4) * d * components;
			} else {
				sz += w * h * d * components;
			}
			w = qMax(w / 2, 1);
			h = qMax(h / 2, 1);
			d = qMax(d / 2, 1);
		}
	}

	return sz;
}

bool RfxDDSPlugin::ValidateHeader(DDSHeader &header)
{
	if (strncmp((char*)&header.dwMagic, "DDS ", 4))
		return false;

	if (header.dwSize != 124)
		return false;

	if (!(header.dwFlags & DDSD_PIXELFORMAT) || !(header.dwFlags & DDSD_CAPS))
		return false;

	isCompressed = header.ddpfPixelFormat.dwFlags & DDPF_FOURCC;
	isCubemap = header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP;
	isVolume = header.ddsCaps.dwCaps2 & DDSCAPS2_VOLUME;

	width = header.dwWidth;
	height = header.dwHeight;
	if (width < 1 || height < 1)
		return false;

	depth = qMax(1u, header.dwDepth);
	mipCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;

	return true;
}

bool RfxDDSPlugin::GetOGLFormat(DDSHeader &header)
{
	if (isCompressed) {
		// texture compression support needed
		// TODO: unpack block data when no hw support is present
		if (!GLEW_ARB_texture_compression ||
		    !GLEW_EXT_texture_compression_s3tc) {
			return false;
		}

		// OGL only supports DXT1, DXT3 and DXT5 compressed data
		switch (header.ddpfPixelFormat.dwFourCC) {
		case FOURCC_DXT1:
			texFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			components = 8;
			break;
		case FOURCC_DXT3:
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			components = 16;
			break;
		case FOURCC_DXT5:
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			components = 16;
			break;
		default:
			return false;
		}

	} else {

		if (header.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE) {
			if (header.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
				texFormat = GL_LUMINANCE_ALPHA;
			else
				texFormat = GL_LUMINANCE;

		} else if (header.ddpfPixelFormat.dwFlags & DDPF_RGB) {
			if (header.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
				texFormat = GL_RGBA;
			else
				texFormat = GL_RGB;

		} else {
			return false;
		}

		components = header.ddpfPixelFormat.dwRGBBitCount / 8;
	}

	return true;
}

bool RfxDDSPlugin::DXT1CheckAlpha(unsigned char *blocks, int size)
{
	DXTColBlock *colBlock(reinterpret_cast<DXTColBlock*>(blocks));
	unsigned char byte = colBlock[0].row[0];

	for (unsigned i = 0, n = (size / 8); i < n; i++)
		if (colBlock[i].col0 <= colBlock[i].col1)
			for (unsigned j = 0; j < 4; byte = colBlock[i].row[++j])
				for (unsigned p = 0; p < 4; p++, byte >>= 2)
					if ((byte & 3) == 3)
						return true;

	return false;
}

/*
 * flipImg - vertically flip an image (either uncompressed pixels or a dxt{1,3,5} block)
 *           (code adapted from OpenSG [http://opensg.vrsource.org])
 */
void RfxDDSPlugin::flipImg(char *image, int width, int height, int depth, int size)
{
	int linesize;
	int offset;

	if (!isCompressed) {
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

	gBits[0][0] = (char)(bits & mask); bits >>= 3;
	gBits[0][1] = (char)(bits & mask); bits >>= 3;
	gBits[0][2] = (char)(bits & mask); bits >>= 3;
	gBits[0][3] = (char)(bits & mask); bits >>= 3;
	gBits[1][0] = (char)(bits & mask); bits >>= 3;
	gBits[1][1] = (char)(bits & mask); bits >>= 3;
	gBits[1][2] = (char)(bits & mask); bits >>= 3;
	gBits[1][3] = (char)(bits & mask);

	bits = 0;
	memcpy(&bits, &block->row[3], 3);

	gBits[2][0] = (char)(bits & mask); bits >>= 3;
	gBits[2][1] = (char)(bits & mask); bits >>= 3;
	gBits[2][2] = (char)(bits & mask); bits >>= 3;
	gBits[2][3] = (char)(bits & mask); bits >>= 3;
	gBits[3][0] = (char)(bits & mask); bits >>= 3;
	gBits[3][1] = (char)(bits & mask); bits >>= 3;
	gBits[3][2] = (char)(bits & mask); bits >>= 3;
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
// swap two sections of memory
void RfxDDSPlugin::swap(void *byte1, void *byte2, const int size)
{
	unsigned char *tmp =new unsigned char[size];

	memcpy(tmp, byte1, size);
	memcpy(byte1, byte2, size);
	memcpy(byte2, tmp, size);
	delete tmp;
}
