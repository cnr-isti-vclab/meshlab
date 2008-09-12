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

#include "rfx_textureloader.h"

// static member initialization
QMap<QByteArray, RfxTextureLoaderPlugin*> *RfxTextureLoader::plugins = NULL;

bool RfxTextureLoader::LoadTexture(const QString& fName, QList<RfxState*> &states, GLuint *tex)
{
	QByteArray ext = QFileInfo(fName).suffix().toLower().toLocal8Bit();

	if (plugins && plugins->contains(ext)) {
		*tex = plugins->value(ext)->Load(fName, states);
		return (*tex != 0);
	}

	return false;
}

QImage RfxTextureLoader::LoadImage(const QString& fName)
{
	QByteArray ext = QFileInfo(fName).suffix().toLower().toLocal8Bit();

	if (plugins && plugins->contains(ext)) {
		int width = 0;
		int height = 0;
		unsigned char *pixels = NULL;

		pixels = plugins->value(ext)->LoadAsImage(fName, &width, &height);
		if (pixels == NULL)
			return QImage();

		QImage img = QImage(width, height, QImage::Format_ARGB32);
		memcpy(img.bits(), pixels, (width * height * 4));
		delete[] pixels;

		/* code from Qt4 QGLFramebufferObject::toImage() method */
		if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
			// OpenGL gives RGBA; Qt wants ARGB
			uint *p = (uint*)img.bits();
			uint *end = p + width * height;
			while (p < end) {
				uint a = *p << 24;
				*p = (*p >> 8) | a;
				p++;
			}
		} else {
			// OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
			img = img.rgbSwapped();
		}
		return img.mirrored();
	}

	return QImage();
}

void RfxTextureLoader::RegisterPlugin(RfxTextureLoaderPlugin *p)
{
	assert(p);
	if (!plugins)
		plugins = new QMap<QByteArray, RfxTextureLoaderPlugin*>();

	foreach (QByteArray format, p->supportedFormats())
		plugins->insert(format, p);
}

void RfxTextureLoader::UnregisterPlugin(RfxTextureLoaderPlugin *p)
{
	if (plugins) {
		foreach (QByteArray key, plugins->keys(p))
			plugins->remove(key);

		if (plugins->isEmpty()) {
			delete plugins;
			plugins = NULL;
		}
	}
}
