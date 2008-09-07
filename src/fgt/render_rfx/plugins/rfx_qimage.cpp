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

GLuint RfxQImagePlugin::Load(const QString &fName, QList<RfxState*> &states)
{
	if (!img.load(fName))
		return 0;

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

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// default parameters if no states set
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	foreach (RfxState *s, states)
		s->SetEnvironment(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

	return tex;
}
