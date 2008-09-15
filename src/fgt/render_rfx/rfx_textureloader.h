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

#ifndef RFX_TEXTUREOADER_H_
#define RFX_TEXTUREOADER_H_

#include <cassert>
#include <QString>
#include <QMap>
#include <QFileInfo>
#include <QImage>
#include <GL/glew.h>
#include "rfx_state.h"

/*
 * ImageInfo - contains a texture preview (ie unfolded cubemaps) and texture
 *             properties
 */
struct ImageInfo {
	QImage preview;
	int width;
	int height;
	int depth;
	QString format;
	QString texType;
};

/******************************************************************************
 * RfxTextureLoaderPlugin: abstract class that defines a plugin for the       *
 *                         Texture Loader.                                    *
 ******************************************************************************/
class RfxTextureLoaderPlugin
{
public:
	virtual ~RfxTextureLoaderPlugin() {}

	// plugin should return a list of file suffix supported (lowercase)
	virtual QList<QByteArray> supportedFormats() = 0;

	// plugin should load and bind texture and return its id as returned by
	// glGenTexture(), or 0 if something went wrong with loading
	virtual GLuint Load(const QString &f, QList<RfxState*> &s) = 0;

	// plugin should be able to read texture into a QImage
	virtual ImageInfo LoadAsQImage(const QString &f) = 0;

	// returns plugin name
	virtual const QString PluginName() = 0;
};

#define REGISTER_PLUGIN(PNAME) \
	static PNAME reg##PNAME; \
	struct PNAME##regHelp { \
		PNAME##regHelp() { RfxTextureLoader::RegisterPlugin(&reg##PNAME); } \
		~PNAME##regHelp() { RfxTextureLoader::UnregisterPlugin(&reg##PNAME); } \
	}; \
	static PNAME##regHelp PNAME##regHelpIns;

/******************************************************************************/



class RfxTextureLoader
{
public:
	static bool LoadTexture(const QString &fName, QList<RfxState*> &states, GLuint *tex);
	static ImageInfo LoadAsQImage(const QString &fName);
	static void RegisterPlugin(RfxTextureLoaderPlugin*);
	static void UnregisterPlugin(RfxTextureLoaderPlugin*);

private:
	static QMap<QByteArray, RfxTextureLoaderPlugin*> *plugins;
};

#endif /* RFX_TEXTUREOADER_H_ */
