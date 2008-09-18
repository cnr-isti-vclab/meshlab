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

#include "rfx_tga.h"

QList<QByteArray> RfxTGAPlugin::supportedFormats()
{
	return QList<QByteArray>() << "tga";
}

ImageInfo RfxTGAPlugin::LoadAsQImage(const QString &f)
{
	ImageInfo iInfo;

	unsigned char *pBits = LoadImageData(f);
	if (!pBits)
		return iInfo;

	iInfo.width = width;
	iInfo.height = height;
	iInfo.depth = 1;
	iInfo.texType = "2D Texture";
	if (imageType == TGA_RAW_RGB) {
		if (bpp == 4)
			iInfo.format = "32bpp RGBA";
		else if (bpp == 3)
			iInfo.format = "24bpp RGB";
	} else if (imageType == TGA_RAW_GREYSCALE) {
		iInfo.format = "8bpp B/W";
	} else {
		return iInfo;
	}

	QImage img(width, height, QImage::Format_RGB32);
	for (int i = 0; i < height; ++i) {
		QRgb *scanline = (QRgb*)img.scanLine(i);

		if (imageType == TGA_RAW_GREYSCALE) {
			for (int j = 0; j < width; ++j) {
				scanline[j] = qRgb(*pBits, *pBits, *pBits);
				++pBits;
			}
		} else if (imageType == TGA_RAW_RGB) {
			for (int j = 0; j < width; ++j) {
				scanline[j] = qRgb(pBits[0], pBits[1], pBits[2]);
				pBits += bpp;
			}
		}
	}
	iInfo.preview = img.mirrored();

	return iInfo;
}

GLuint RfxTGAPlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	pixels = LoadImageData(fName);

	if (pixels == NULL)
		return 0;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// default parameters if no states set
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (GLEW_SGIS_generate_mipmap) {
		glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	foreach (RfxState *s, states)
		s->SetEnvironment(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0,
	             texFormat, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
	return tex;
}

unsigned char* RfxTGAPlugin::LoadImageData(const QString &fName)
{
	QFile f(fName);
	f.open(QIODevice::ReadOnly);

	// read tga header
	// we use '18' directly instead of sizeof(TGAHeader) due to potential padding
	// problems (eg. sizeof(TGAHeader) could be 20)
	char *head = new char[18];
	f.read(head, 18);
	f.seek(headerSize);

	// sanity checks
	if (!CheckHeader(head)) {
		delete[] head;
		f.close();
		return NULL;
	}
	delete[] head;

	// compute image size in bytes and read whole pixel data
	unsigned char *pData = new unsigned char[imageSize];
	f.read((char*)pData, imageSize);
	f.close();

	// determine OpenGL texture internal format
	texFormat = GetOGLFormat();

	// convert BGR to RGB
	if (imageType == TGA_RAW_RGB)
		rgbSwapped(pData);

	// do we have to vertically flip image?
	if (needsVFlip)
		FlipV(pData);

	return pData;
}

// swap R and B components
void RfxTGAPlugin::rgbSwapped(unsigned char *imgData)
{
	unsigned char *pixPtr = imgData;
	long int nPix = width * height;
	unsigned char tmp;

	// swap R and B pixels with good old tmp = a; a = b; b = tmp; method :)
	for (int i = 0; i < nPix; ++i) {
		tmp       = pixPtr[0];     // B -> temp
		pixPtr[0] = pixPtr[2];     // R -> B
		pixPtr[2] = tmp;           // temp -> R

		pixPtr += bpp;             // next pixel
	}
}

// vertically flip image
void RfxTGAPlugin::FlipV(unsigned char *imgData)
{
	unsigned char tmp;

	int lineLen = width * bpp;
	unsigned char *line1 = imgData;
	unsigned char *line2 = imgData + (height - 1) * lineLen;

	for (; line1 < line2; line2 -= (lineLen * 2)) {
		for (int i = 0; i < lineLen; ++i, ++line1, ++line2) {
			tmp    = *line1;
			*line1 = *line2;
			*line2 = tmp;
		}
	}
}

// return texture internal format based on image colors
GLint RfxTGAPlugin::GetOGLFormat()
{
	switch (imageType) {
	case TGA_RAW_RGB:
		switch (bpp) {
		case 4:
			return GL_RGBA;
		case 3:
			return GL_RGB;
		default:
			return 0;
		}

	case TGA_RAW_GREYSCALE:
		return GL_LUMINANCE;

	default:
		return 0;
	}
}

// check header correctness and if image type is supported
bool RfxTGAPlugin::CheckHeader(const char *head)
{
	// fill header struct (getting rid of padding and endianness issues)
	TGAHeader tgah;
	tgah.IDLength = head[0];
	tgah.ColorMapType = head[1];
	tgah.ImageType = head[2];
	memcpy(&tgah.CMapStart, &head[3], 2);
	memcpy(&tgah.CMapLength, &head[5], 2);
	tgah.CMapDepth = head[7];
	memcpy(&tgah.XOffset, &head[8], 2);
	memcpy(&tgah.YOffset, &head[10], 2);
	memcpy(&tgah.Width, &head[12], 2);
	memcpy(&tgah.Height, &head[14], 2);
	tgah.PixelDepth = head[16];
	tgah.ImageDescriptor = head[17];

	headerSize = 18 + tgah.IDLength;

	if (tgah.ColorMapType != TGA_RGB)
		return false;

	if (tgah.ImageType != TGA_RAW_RGB && tgah.ImageType != TGA_RAW_GREYSCALE)
		return false;
	imageType = tgah.ImageType;

	width = tgah.Width - tgah.XOffset;
	height = tgah.Height - tgah.YOffset;
	bpp = tgah.PixelDepth / 8;
	if (width < 1 || height < 1)
		return false;

	imageSize = width * height * bpp;

	if (tgah.ImageDescriptor & 0x20)
		needsVFlip = true;
	else
		needsVFlip = false;

	return true;
}
