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
#include <algorithm>
#include "rfx_textureloader.h"
#include "dds_spec.h"

class RfxDDSPlugin : public RfxTextureLoaderPlugin
{
public:
	RfxDDSPlugin() {}
	virtual ~RfxDDSPlugin() {}
	virtual QList<QByteArray> supportedFormats();
	virtual GLuint Load(const QString&, QList<RfxState*>&);
	virtual const QString PluginName() { return QString("DDS Plugin"); }

private:
	GLuint tex;
	int texTarget;
	int texFormat;
	bool compressed;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int mipCount;
};

REGISTER_PLUGIN(RfxDDSPlugin)

#endif /* RFX_DDS_H_ */
