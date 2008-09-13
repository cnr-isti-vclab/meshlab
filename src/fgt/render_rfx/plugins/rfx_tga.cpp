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

GLubyte* RfxTGAPlugin::LoadAsImage(const QString &f, int *w, int *h)
{
	QList<RfxState*> e = QList<RfxState*>();
	GLuint newTex = Load(f, e);

	if (newTex == 0) {
		*w = 0;
		*h = 0;
		return NULL;
	}

	*w = width;
	*h = height;
	unsigned char *img = new unsigned char[width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glDeleteTextures(1, &newTex);
	return img;
}

GLuint RfxTGAPlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	QFile f(fName);
	f.open(QIODevice::ReadOnly);

	// parse tga header
	// we use '18' directly instead of sizeof(TGAHeader) due to potential padding
	// problems (eg. sizeof(TGAHeader) could be 20)
	unsigned char *head = new unsigned char[18];
	f.read((char*)head, 18);
	f.seek(18);

	// fill header struct (getting rid of padding and endianness issues)
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

	delete[] head;

	// sanity checks
	if (!CheckHeader()) {
		f.close();
		return 0;
	}

	// compute image size in bytes
	imageSize = width * height * (tgah.PixelDepth / 8);
	pixels = new GLubyte[imageSize];

	// only raw rgb mode is supported a.t.m.
	switch (tgah.ImageType) {
	case TGA_RAW_RGB:
	case TGA_RAW_GREYSCALE:
		texType = (tgah.ImageType == TGA_RAW_RGB)?
			((tgah.PixelDepth == 32)? GL_RGBA : GL_RGB) :
			GL_LUMINANCE;
		f.seek(f.pos() + tgah.IDLength);
		f.read((char*)pixels, imageSize);
		break;
	default:
		delete[] pixels;
		f.close();
		return 0;
	}

	// convert BGR to RGB
	unsigned char *pixPtr = pixels;
	unsigned char tmp;
	long totPixels = width * height;
	short bpp = tgah.PixelDepth / 8;

	// swap R and B pixels with good old tmp = a; a = b; b = tmp; method :)
	for (int i = 0; i < totPixels; ++i) {
		tmp       = pixPtr[0];     // B -> temp
		pixPtr[0] = pixPtr[2];     // R -> B
		pixPtr[2] = tmp;           // temp -> R

		pixPtr += bpp;             // next pixel
	}

	// do we have to vertically flip image?
	if (tgah.ImageDescriptor & 0x20) {

		// already declared vars - reusing
		//unsigned char tmp;
		//short bpp = tgah.PixelDepth / 8;

		int lineLen = width * bpp;
		unsigned char *line1 = pixels;
		unsigned char *line2 = pixels + (height - 1) * lineLen;

		for (; line1 < line2; line2 -= (lineLen * 2)) {
			for (int i = 0; i < lineLen; ++i, ++line1, ++line2) {
				tmp    = *line1;
				*line1 = *line2;
				*line2 = tmp;
			}
		}
	}

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
	             texType, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
	f.close();

	return tex;
}

bool RfxTGAPlugin::CheckHeader()
{
	if (tgah.ColorMapType != TGA_RGB)
		return false;

	if (tgah.ImageType != TGA_RAW_RGB && tgah.ImageType != TGA_RAW_GREYSCALE)
		return false;

	width = tgah.Width - tgah.XOffset;
	height = tgah.Height - tgah.YOffset;
	if (width < 1 || height < 1)
		return false;

	if (tgah.ImageDescriptor > 0x20)
		return false;

	return true;
}
