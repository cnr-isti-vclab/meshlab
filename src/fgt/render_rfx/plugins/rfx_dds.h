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

#ifndef RFX_DDS_H_
#define RFX_DDS_H_

#include <QFile>
#include <GL/glew.h>
#include "rfx_textureloader.h"

/*
 * bunch of declarations and defines for the DDS file format
 */
struct DDPIXELFORMAT {
	unsigned int dwSize;
	unsigned int dwFlags;
	unsigned int dwFourCC;
	unsigned int dwRGBBitCount;
	unsigned int dwRBitMask;
	unsigned int dwGBitMask;
	unsigned int dwBBitMask;
	unsigned int dwAlphaBitMask;
};

struct DDSCAPS2 {
	unsigned int dwCaps1;
	unsigned int dwCaps2;
	unsigned int dwReserved[2];
};

// DDS Header as found in MS reference:
// http://msdn.microsoft.com/en-us/library/bb943981(VS.85).aspx
struct DDSHeader {
	unsigned int dwMagic;
	unsigned int dwSize;
	unsigned int dwFlags;
	unsigned int dwHeight;
	unsigned int dwWidth;
	unsigned int dwPitchOrLinearSize;
	unsigned int dwDepth;
	unsigned int dwMipMapCount;
	unsigned int dwReserved1[11];
	DDPIXELFORMAT ddpfPixelFormat;
	DDSCAPS2 ddsCaps;
	unsigned int dwReserved2;
};

//  DDSHeader.dwFlags
#define DDSD_CAPS                   0x00000001
#define DDSD_HEIGHT                 0x00000002
#define DDSD_WIDTH                  0x00000004
#define DDSD_PITCH                  0x00000008
#define DDSD_PIXELFORMAT            0x00001000
#define DDSD_MIPMAPCOUNT            0x00020000
#define DDSD_LINEARSIZE             0x00080000
#define DDSD_DEPTH                  0x00800000

//  DDSHeader.ddpfPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001
#define DDPF_ALPHA                  0x00000002
#define DDPF_FOURCC                 0x00000004
#define DDPF_INDEXED                0x00000020
#define DDPF_RGB                    0x00000040
#define DDPF_COMPRESSED             0x00000080
#define DDPF_LUMINANCE              0x00020000

//  DDSHeader.ddpfPixelFormat.dwFourCC
#define FOURCC_DXT1                 0x31545844
#define FOURCC_DXT3                 0x33545844
#define FOURCC_DXT5                 0x35545844

//  DDSHeader.ddsCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008
#define DDSCAPS_TEXTURE             0x00001000
#define DDSCAPS_MIPMAP              0x00400000

//  DDSHeader.ddsCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME             0x00200000



/*
 * RfxDDSPlugin Loader
 */
class RfxDDSPlugin : public RfxTextureLoaderPlugin
{
public:
	RfxDDSPlugin() {}
	virtual ~RfxDDSPlugin() {}
	virtual QList<QByteArray> supportedFormats();
	virtual GLuint Load(const QString&, QList<RfxState*>&);
	virtual ImageInfo LoadAsQImage(const QString &f);
	virtual const QString PluginName() { return QString("DDS Plugin"); }

private:
	bool ValidateHeader(DDSHeader&);
	bool GetOGLFormat(DDSHeader&);
	bool DXT1CheckAlpha(unsigned char*, int);
	int ComputeImageSize();
	unsigned char* LoadImageData(const QString&);
	void flipImg(char *image, int width, int height, int depth, int size);
	void swap(void *byte1, void *byte2, int size);

	struct DXTColBlock {
		short col0;
		short col1;
		char row[4];
	};

	struct DXT3AlphaBlock {
		short row[4];
	};

	struct DXT5AlphaBlock {
		char alpha0;
		char alpha1;
		char row[6];
	};

	void flip_blocks_dxtc1(DXTColBlock *line, int numBlocks);
	void flip_blocks_dxtc3(DXTColBlock *line, int numBlocks);
	void flip_blocks_dxtc5(DXTColBlock *line, int numBlocks);
	void flip_dxt5_alpha(DXT5AlphaBlock *block);

	GLuint tex;
	int texTarget;
	int texFormat;
	bool isCompressed;
	bool isCubemap;
	bool isVolume;
	int width;
	int height;
	int depth;
	int mipCount;
	int components;
};

REGISTER_PLUGIN(RfxDDSPlugin)

#endif /* RFX_DDS_H_ */
