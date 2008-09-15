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

#ifndef RFX_TGA_H_
#define RFX_TGA_H_

#include <QFile>
#include "rfx_textureloader.h"

/*
 * TGA File header and costants
 */
struct TGAHeader {
	char  IDLength;         /* Size of Image ID field */
	char  ColorMapType;     /* Color map type */
	char  ImageType;        /* Image type code */
	short CMapStart;        /* Color map origin */
	short CMapLength;       /* Color map length */
	char  CMapDepth;        /* Depth of color map entries */
	short XOffset;          /* X origin of image */
	short YOffset;          /* Y origin of image */
	short Width;            /* Width of image */
	short Height;           /* Height of image */
	char  PixelDepth;       /* Image pixel size */
	char  ImageDescriptor;  /* Image descriptor byte */
};

#define TGA_RGB             0x0
#define TGA_INDEXED         0x1

#define TGA_RAW_INDEXED     0x1
#define TGA_RAW_RGB         0x2
#define TGA_RAW_GREYSCALE   0x3
#define TGA_RLE_INDEXED     0x9
#define TGA_RLE_RGB         0xA

/*
 * TGA Plugin
 * (only loads uncompressed and non-indexed RGB images)
 */
class RfxTGAPlugin : public RfxTextureLoaderPlugin
{
public:
	RfxTGAPlugin() {};
	virtual ~RfxTGAPlugin() {};
	virtual QList<QByteArray> supportedFormats();
	virtual GLuint Load(const QString&, QList<RfxState*>&);
	virtual ImageInfo LoadAsQImage(const QString &f);
	virtual const QString PluginName() { return QString("TGA Plugin"); }

private:
	bool CheckHeader(const char*);
	GLint GetOGLFormat();
	void rgbSwapped(unsigned char*);
	void FlipV(unsigned char*);
	unsigned char* LoadImageData(const QString &);

	// image data
	int width;
	int height;
	int bpp;
	int imageType;
	int imageSize;
	int headerSize;
	bool needsVFlip;

	// gl data
	int texType;
	GLint texFormat;
	GLuint tex;
	GLubyte *pixels;
};

REGISTER_PLUGIN(RfxTGAPlugin)

#endif /* RFX_TGA_H_ */
