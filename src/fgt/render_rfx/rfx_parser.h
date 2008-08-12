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

#ifndef RFX_PARSER_H_
#define RFX_PARSER_H_

#include <QMap>
#include <QRegExp>
#include <QStringList>
#include <QDomDocument>
#include "rfx_shader.h"
#include "rfx_glpass.h"

class RfxParser
{
public:
	RfxParser(const QString&);
	virtual ~RfxParser();
	bool Parse();
	RfxShader* GetShader() { return rfxShader; }

private:
	void AppendGLStates(RfxGLPass*, QDomNodeList, RfxState::StateType);
	void ParseUniforms(const QString&);
	float* ValueFromRfx(const QString&, RfxUniform::UniformType);
	QString TextureFromRfx(const QString&, RfxUniform::UniformType);

	QMap<QString, QString> uniformType;
	QFile *rmShader;
	QDomDocument document;
	QDomElement root;
	RfxShader *rfxShader;

	static const char *UniformToRfx[];
	static const short UniformToElements[];
};

#endif /* RFX_PARSER_H_ */
