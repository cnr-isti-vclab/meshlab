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

#ifndef RFX_QIMAGE_H_
#define RFX_QIMAGE_H_

#include <QImage>
#include "rfx_textureloader.h"
#include <QGLContext>

class RfxQImagePlugin: public RfxTextureLoaderPlugin
{
public:
	RfxQImagePlugin() {}
	virtual ~RfxQImagePlugin() {}
	virtual QList<QByteArray> supportedFormats();
	virtual GLuint Load(const QString&, QList<RfxState*>&);
	virtual ImageInfo LoadAsQImage(const QString &f);
	virtual const QString PluginName() { return QString("QImage Plugin"); }

private:
	bool LoadRGBAQImage(const QString &fName);

	static const char *Formats[];
	GLuint tex;
	QImage img;
};

REGISTER_PLUGIN(RfxQImagePlugin)

#endif /* RFX_QIMAGE_H_ */
