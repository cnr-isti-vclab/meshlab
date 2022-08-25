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
#include <QDir>
#include "rfx_shader.h"
#include "rfx_glpass.h"
#include "rfx_rendertarget.h"
#include "rfx_specialuniform.h"
#include "rfx_specialattribute.h"

class RfxParser
{
public:
	RfxParser(const QString&);
	virtual ~RfxParser();
	bool Parse(MeshDocument &md);

	/*
		Verifies if the document is valid.
		@return true if the document is valid, false otherwise.
	*/
    bool isValidDoc();

	RfxShader* GetShader() { return rfxShader; }
	void setMeshTexture(QString tn){meshTextureName = tn;}
private:
	QList<RfxState*> ParseGLStates(QDomNodeList, RfxState::StateType);
	void ParseUniforms(const QString&);

	/*
		Verifies if the shader sources contains some special attributes.
		For each special attribute declared creates a new instance of SpecialAttribute and appends it in the GLPass.
		@param source the source of the shader.
		@param pass the GLPass.
	*/
	void ParseAttributes(const QString&, RfxGLPass*);

	float* ValueFromRfx(const QString&, RfxUniform* unif);
	QString TextureFromRfx(const QString&, RfxUniform::UniformType);
	QString GetSemantic(const QString& VarName, RfxUniform::UniformType VarType);

	QMap<QString, QString> uniformType;
	
	QFile *rmShader;
	QDomDocument document;
	QDomElement root;
	RfxShader *rfxShader;
  
	// the absolute filepath of the texture of the current mesh,
	// It automatically substitute any texture with the special name "TEXTURE0.PNG"
	QString meshTextureName;
	
	static const char *UniformToRfx[];
	static const short UniformToElements[];
};

#endif /* RFX_PARSER_H_ */
