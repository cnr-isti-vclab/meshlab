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

#include "rfx_parser.h"

RfxParser::RfxParser(const QString &rfxFile)
{
	rmShader = new QFile(rfxFile);
	rmShader->open(QIODevice::ReadOnly | QIODevice::Text);

	rfxShader = new RfxShader();
}

RfxParser::~RfxParser()
{
	rmShader->close();
	delete rmShader;
}

/*
 * Each .rfx file (should be valid XML) contains a DTD for its own validation,
 * followed by a list of RmRegistryBranch.
 * This header is documented in default RM installation directory:
 * [Data/RmDTDHeader.rfx] contains DTD
 * [Data/rm_registry.rfx] contains the list of registers and their value ranges
 *
 * Rfx file may contain RmDirectXEffect, RmOpenGLEffect and RmOpenGLESEffect.
 * We only care about the second type.
 *
 * <!DOCTYPE RENDERMONKEY[
 *   ... (DTD here)
 * ]>
 * <RENDER_MONKEY>
 *   <RmRegistryBranch>
 *     ...
 *   </RmRegistryBranch>
 *   <RmOpenGLEffect ... >
 *     ...
 *   </RRmOpenGLEffect>
 * </RENDER_MONKEY>
 *
 * TODO: use DTD in rfx header to validate contents
 */
bool RfxParser::Parse()
{
	if (!document.setContent(rmShader))
		return false;

	root = document.documentElement();
	QDomElement OGLEffect = root.elementsByTagName("RmOpenGLEffect").at(0).toElement();

	// no <RmOpenGLEffect> found, parsing failed
	if (OGLEffect.isNull())
		return false;

	QDomNodeList list = OGLEffect.elementsByTagName("RmGLPass");
	for (int i = 0; i < list.size(); ++i) {
		QDomElement glpass = list.at(i).toElement();

		if (glpass.attribute("ENABLED") == "FALSE")
			continue;

		RfxGLPass *theGLPass = new RfxGLPass();
		theGLPass->SetPassName(glpass.attribute("NAME"));
		theGLPass->SetPassIndex(glpass.attribute("PASS_INDEX").toInt());

		// OpenGL states if any
		QDomElement state;
		state = glpass.firstChildElement("RmRenderStateBlock");
		if (!state.isNull()) {
			QDomNodeList statelist = state.elementsByTagName("RmState");
			AppendGLStates(theGLPass, statelist, RfxState::RFX_RENDERSTATE);
		}

		// TODO: add rendertarget for multiple pass support

		// shader sources
		QDomNodeList sources = glpass.elementsByTagName("RmGLShader");
		for (int j = 0; j < sources.size(); ++j) {
			QDomElement source = sources.at(j).toElement();
			bool isPixel = (source.attribute("PIXEL_SHADER") == "TRUE");

			theGLPass->SetShaderSource(source.text(), isPixel);

			// (also check for uniforms declaration, will save some time later)
			ParseUniforms(source.text());
		}

		// shader uniforms
		QDomNodeList constlist = glpass.elementsByTagName("RmShaderConstant");
		for (int j = 0; j < constlist.size(); ++j) {
			QString varName = constlist.at(j).toElement().attribute("NAME");

			// check that this uniform is used in shaders, else ignore it
			if (!uniformType.contains(varName))
				continue;

			RfxUniform *unif = new RfxUniform(varName, uniformType[varName]);
			float *parsedValue = ValueFromRfx(varName, unif->GetType());
			unif->SetValue(parsedValue);
			delete parsedValue;

			theGLPass->AddUniform(unif);
		}

		// shader uniforms - textures and states
		GLint tu = 0;
		QDomNodeList samplist = glpass.elementsByTagName("RmSampler");
		for (int j = 0; j < samplist.size(); ++j) {
			QString TOString = samplist.at(j).toElement().attribute("NAME");

			QDomNodeList TOList = glpass.elementsByTagName("RmTextureObject");
			for (int k = 0; k < TOList.size(); k++) {
				QDomElement to = TOList.at(k).toElement();
				if (to.attribute("NAME") == TOString) {
					QDomElement tr = to.firstChildElement("RmTextureReference");
					QString texName = tr.attribute("NAME");

					// check that this uniform is used in shaders, else ignore it
					if (!uniformType.contains(TOString))
						break;

					RfxUniform *unif = new RfxUniform(TOString, uniformType[TOString]);
					unif->SetValue(TextureFromRfx(texName, unif->GetType()));
					unif->SetTU(tu++);
					theGLPass->AddUniform(unif);

					QDomNodeList statelist = to.elementsByTagName("RmState");
					AppendGLStates(theGLPass, statelist, RfxState::RFX_SAMPLERSTATE);
				}
			}
		}

		// finally add to the pass list
		rfxShader->AddGLPass(theGLPass);
	}

	rfxShader->SortPasses();
	return true;
}

// static member initialization
// this is a lookup table for mapping between uniform types
// (ie "vec2") and their counterparts in rfx file (ie "RmVectorVariable")
const char *RfxParser::UniformToRfx[] = {
		"RmIntegerVariable", "RmFloatVariable", "RmBooleanVariable",
		"RmVectorVariable", "RmVectorVariable", "RmVectorVariable",
		"RmVectorVariable", "RmVectorVariable", "RmVectorVariable",
		"RmVectorVariable", "RmVectorVariable", "RmVectorVariable",
		"RmMatrixVariable", "RmMatrixVariable", "RmMatrixVariable",
		"?", "Rm2DTextureVariable", "Rm3DTextureVariable", "RmCubemapVariable",
		"?", "?"
};

// lookup table for mapping between uniform types and the number
// of their elements (ie: mat3 => 9 elements)
const short RfxParser::UniformToElements[] = {
		1, 1, 1,
		2, 3, 4,
		2, 3, 4,
		2, 3, 4,
		4, 9, 16,
		-1, -1, -1, -1,
		-1, -1
};

float* RfxParser::ValueFromRfx(const QString& VarName, RfxUniform::UniformType VarType)
{
	// read a bool, int or float vector (max 4 elements)
	// or a float matrix (max 16 elements).
	float *result = new float[16];

	// initialization (unnecessary?)
	for (int i = 0; i < 16; ++i)
		result[i] = 0.0f;

	// use lookup table to init data
	int elementNum = UniformToElements[VarType];
	QString elementName = UniformToRfx[VarType];

	// first thing first find the DOM Node of declared var
	QDomElement varNode;
	QDomNodeList candidates = root.elementsByTagName(elementName);

	for (int i = 0; i < candidates.size(); ++i)
		if (candidates.at(i).toElement().attribute("NAME") == VarName)
			varNode = candidates.at(i).toElement();

	// if we're looking for a vec4, it could be in a RmColorVariable,
	// so try a second search if the first was unsuccessful
	if (VarType == RfxUniform::VEC4 && varNode.isNull()) {
		candidates = root.elementsByTagName("RmColorVariable");

		for (int i = 0; i < candidates.size(); ++i)
			if (candidates.at(i).toElement().attribute("NAME") == VarName)
				varNode = candidates.at(i).toElement();
	}

	// retrieve values stored in XML
	for (int i = 0; i < elementNum; ++i) {
		QString attrValue = "VALUE";
		if (elementNum > 1)
			attrValue.append("_").append(QString().setNum(i));

		// now if value is bool, toFloat() will hardly success in its
		// conversion, so help out a bit
		QString val = varNode.attribute(attrValue);
		bool ok = false;
		if (val == "TRUE" || val == "FALSE")
			result[i] = (val == "TRUE") ? 1.0f : 0.0f;
		else {
			result[i] = val.toFloat(&ok);
		}
	}

	return result;
}

QString RfxParser::TextureFromRfx(const QString& VarName,
                                        RfxUniform::UniformType VarType)
{
	QString elementName = UniformToRfx[VarType];
	QString filePath = "/not/found";

	QDomElement varNode;
	QDomNodeList candidates = root.elementsByTagName(elementName);

	for (int i = 0; i < candidates.size(); ++i) {
		varNode = candidates.at(i).toElement();
		if (varNode.attribute("NAME") == VarName) {

			// start from rfx file directory
			QDir fDir(QFileInfo(*rmShader).absolutePath());

			// ... get relative path to texture file
			QString fName(varNode.attribute("FILE_NAME").replace('\\', '/'));

			// then join together and return the absolute path
			QFileInfo thefile(fDir, fName);
			filePath = thefile.canonicalFilePath();
		}
	}

	return filePath;
}

void RfxParser::AppendGLStates(RfxGLPass *theGLPass, QDomNodeList statelist,
                               RfxState::StateType statetype)
{
	for (int j = 0; j < statelist.size(); ++j) {
		QDomElement stateEl = statelist.at(j).toElement();
		RfxState *glstate = new RfxState(statetype);
		glstate->SetState(stateEl.attribute("STATE").toInt());
		glstate->SetValue(stateEl.attribute("VALUE").toULong());

		if (statetype == RfxState::RFX_RENDERSTATE)
			theGLPass->AddGLState(glstate);
		else if (statetype == RfxState::RFX_SAMPLERSTATE)
			theGLPass->GetLastUniform()->AddGLState(glstate);
	}
}

void RfxParser::ParseUniforms(const QString &source)
{
	// basically we're looking for declarations like this:
	//    uniform <type> <identifier> ;

	QString txtSource = source;
	int position = 0;
	int unif;
	while ((unif = txtSource.indexOf("uniform", position)) != -1) {
		int semicol = txtSource.indexOf(";", unif);
		int declLen = semicol - unif;
		QStringList decl = txtSource.mid(unif, declLen).split(QRegExp("\\s+"));
		position = semicol;

		// store mappings between uniform declarations and their types
		// for easy lookup.
		// (eg: uniformType["MyTexture"] = "sampler2D")
		enum declContent {UNIFORM, TYPE, IDENTIFIER};
		uniformType[decl[IDENTIFIER]] = decl[TYPE];
	}
}
