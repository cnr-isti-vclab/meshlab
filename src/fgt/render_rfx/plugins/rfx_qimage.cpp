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

#include "rfx_qimage.h"

// default QImage supported formats as reported in QT doc.
// [http://doc.trolltech.com/4.4/qimagereader.html#supportedImageFormats]
const char *RfxQImagePlugin::Formats[] = {
	"bmp", "gif", "jpg", "jpeg", "mng", "png", "pbm", "pgm", "ppm",
	"tiff", "xbm", "xpm", NULL
};

QList<QByteArray> RfxQImagePlugin::supportedFormats()
{
	QList<QByteArray> fmts;
	for (int i = 0; Formats[i] != NULL; ++i)
		fmts.append(Formats[i]);

	return fmts;
}

ImageInfo RfxQImagePlugin::LoadAsQImage(const QString &f)
{
	ImageInfo iInfo;
	if (!iInfo.preview.load(f))
		return iInfo;

	iInfo.width = iInfo.preview.width();
	iInfo.height = iInfo.preview.height();
	iInfo.depth = 1;
	iInfo.texType = "2D Texture";

	QString TexImgformat = "Unknown";
	switch (iInfo.preview.format()) {
	case QImage::Format_Invalid:
		TexImgformat = "Unknown";
		break;
	case QImage::Format_Mono:
	case QImage::Format_MonoLSB:
		TexImgformat = "1bpp B/W";
		break;
	case QImage::Format_Indexed8:
		TexImgformat = "8bpp Indexed";
		break;
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
		TexImgformat = "24bpp RGB";
		break;
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		TexImgformat = "32bpp ARGB";
		break;
	case QImage::Format_RGB16:
		TexImgformat = "16bpp RGB (5-6-5)";
		break;
	case QImage::Format_ARGB8565_Premultiplied:
		TexImgformat = "24bpp ARGB (8-5-6-5)";
		break;
	case QImage::Format_RGB666:
		TexImgformat = "24bpp RGB (6-6-6)";
		break;
	case QImage::Format_ARGB6666_Premultiplied:
		TexImgformat = "24bpp ARGB (6-6-6-6)";
		break;
	case QImage::Format_RGB555:
		TexImgformat = "16bpp RGB (5-5-5)";
		break;
	case QImage::Format_ARGB8555_Premultiplied:
		TexImgformat = "24bpp ARGB (8-5-5-5)";
		break;
	case QImage::Format_RGB444:
		TexImgformat = "16bpp RGB (4-4-4)";
		break;
	case QImage::Format_ARGB4444_Premultiplied:
		TexImgformat = "16bpp ARGB (4-4-4-4)";
		break;
	default:
		break;
	}
	iInfo.format = TexImgformat;

	return iInfo;
}

GLuint RfxQImagePlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	if (!LoadRGBAQImage(fName))
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

	glTexImage2D(GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

	return tex;
}

bool RfxQImagePlugin::LoadRGBAQImage(const QString &fName)
{
	if (!img.load(fName))
		return false;

	// mirror and convert to RGBA
	img = img.convertToFormat(QImage::Format_ARGB32);
	img = img.mirrored();
	if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
		for (int i = 0; i < img.height(); i++) {
			uint *p = (uint*)img.scanLine(i);
			uint *end = p + img.width();
			while (p < end) {
				*p = (*p << 8) | ((*p >> 24) & 0xFF);
				p++;
			}
		}
	} else {
		img = img.rgbSwapped();
	}

	return true;
}
