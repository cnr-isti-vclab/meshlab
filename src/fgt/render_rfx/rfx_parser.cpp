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

/*
	Verifies if the document is valid.
	@return true if the document is valid, false otherwise.
*/
bool RfxParser::isValidDoc()
{
     if (!document.setContent(rmShader))
		return false;

	root = document.documentElement();
	QDomElement OGLEffect =
		root.elementsByTagName("RmOpenGLEffect").at(0).toElement();

	// no <RmOpenGLEffect> found, parsing failed
	if (OGLEffect.isNull())
		return false;
     return true;
}

bool RfxParser::Parse(MeshDocument &md)
{
     document.setContent(rmShader);
	root = document.documentElement();
	QDomElement OGLEffect =
		root.elementsByTagName("RmOpenGLEffect").at(0).toElement();

	// before looping between passes, check for renderable textures
	QDomNodeList rtex = root.elementsByTagName("RmRenderableTexture");
	for (int i = 0; i < rtex.size(); ++i) {
		QDomElement rtEl = rtex.at(i).toElement();

		RfxRenderTarget *rt = new RfxRenderTarget(rtEl.attribute("NAME"));
		rt->SetSize(rtEl.attribute("WIDTH").toInt(),
		            rtEl.attribute("HEIGHT").toInt());

		rt->UseViewPortDim(rtEl.attribute("USE_VIEWPORT_DIMENSIONS") == "TRUE");
		rt->GenMipmaps(rtEl.attribute("GENERATE_MIPMAPS") == "TRUE");

		rfxShader->AddRT(rt);
	}

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
			QList<RfxState*> rslist = ParseGLStates(statelist,
			                                        RfxState::RFX_RENDERSTATE);
			foreach (RfxState *rs, rslist)
				theGLPass->AddGLState(rs);
		}

		// RenderTarget if this pass renders to texture
		QDomElement eRT = glpass.firstChildElement("RmRenderTarget");
		if (!eRT.isNull()) {
			int idx = rfxShader->FindRT(eRT.attribute("NAME"));
			if (idx != -1) {
				RfxRenderTarget *rt = rfxShader->GetRT(idx);

				// we need pass number
				int pass = theGLPass->GetPassIndex();

				// depth clear value
				float depthClear = -1.0f;
				if (eRT.attribute("DEPTH_CLEAR") == "TRUE")
					depthClear = eRT.attribute("DEPTH_CLEAR_VALUE").toFloat();

				// and color clear
				float *colorClear = NULL;
				if (eRT.attribute("COLOR_CLEAR") == "TRUE") {
					long pColor = eRT.attribute("CLEAR_COLOR_VALUE").toLong();
					colorClear = RfxState::DecodeColor(pColor);
				}

				rt->SetClear(pass, depthClear, colorClear);
				theGLPass->SetRenderToTexture(true);
				theGLPass->LinkRenderTarget(rt);
			}
		}

		// shader sources
		QDomNodeList sources = glpass.elementsByTagName("RmGLShader");
		for (int j = 0; j < sources.size(); ++j) {
			QDomElement source = sources.at(j).toElement();
			bool isPixel = (source.attribute("PIXEL_SHADER") == "TRUE");

			theGLPass->SetShaderSource(source.text(), isPixel);

			// also check for uniforms declaration, will save some time later
			ParseUniforms(source.text());

			ParseAttributes(source.text(), theGLPass);
		}

		// shader uniforms
		QDomNodeList constlist = glpass.elementsByTagName("RmShaderConstant");
		for (int j = 0; j < constlist.size(); ++j) {
			QString varName = constlist.at(j).toElement().attribute("NAME");

			// check that this uniform is used in shaders, else ignore it
			if (!uniformType.contains(varName))
				continue;

               RfxUniform *unif;
               if(RfxSpecialUniform::getSpecialType(varName) == RfxSpecialUniform::NONE){
                    unif = new RfxUniform(varName, uniformType[varName]);
                    float *parsedValue = ValueFromRfx(varName, unif);
			     unif->SetValue(parsedValue);

			     if (parsedValue[16] != 0.0 || parsedValue[17] != 0.0)
				     unif->SetValRange(parsedValue[16], parsedValue[17]);

			     delete parsedValue;
               }
               else
                    unif = new RfxSpecialUniform(varName, uniformType[varName], &md);
			

			QString sem = GetSemantic(varName, unif->GetType());
			if (!sem.isNull()) {
				if (rfxShader->AddSemanticUniform(unif, sem))
					unif->SetSemantic(sem);
			}

			theGLPass->AddUniform(unif);
		}

		// shader uniforms - textures and states
		GLint tu = 0;
		QDomNodeList samplist = glpass.elementsByTagName("RmSampler");
		for (int j = 0; j < samplist.size(); ++j) {
			QString TOString = samplist.at(j).toElement().attribute("NAME");

			QDomNodeList TOList =
				glpass.elementsByTagName("RmTextureObject");
			for (int k = 0; k < TOList.size(); k++) {
				QDomElement to = TOList.at(k).toElement();
				if (to.attribute("NAME") == TOString) {
					QDomElement tr =
						to.firstChildElement("RmTextureReference");
					QString texName = tr.attribute("NAME");

					// check that this uniform is used in shaders
					if (!uniformType.contains(TOString))
						break;

					RfxUniform *unif = new RfxUniform(TOString,
					                                  uniformType[TOString]);

					// set texture unit to use
					unif->SetTU(tu++);

					// texture maybe from a file or a renderable one
					int rtIdx = -1;
					QString txPath = TextureFromRfx(texName, unif->GetType());
					if (txPath.startsWith("RT:")) {
						rtIdx = txPath.split(":").at(1).toInt();
						unif->SetValue(rfxShader->GetRT(rtIdx));
					} else {
						unif->SetValue(txPath);
					}

					// set any sampler state
					QDomNodeList statelist = to.elementsByTagName("RmState");
					QList<RfxState*> rslist =
						ParseGLStates(statelist, RfxState::RFX_SAMPLERSTATE);

					// if texture is renderable add states directly to RT class
					// since it's the one that will glGen the texture
					if (unif->isRenderable()) {
						RfxRenderTarget *rt = rfxShader->GetRT(rtIdx);
						int pass = theGLPass->GetPassIndex();

						foreach (RfxState *rs, rslist)
							rt->AddGLState(pass, rs);
					} else {
						foreach (RfxState *rs, rslist)
							unif->AddGLState(rs);
					}

					// add uniform to pass
					theGLPass->AddUniform(unif);
				}
			}
		}
		// clear uniform list for next pass
		uniformType.clear();

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

QString RfxParser::GetSemantic(const QString& VarName, RfxUniform::UniformType VarType)
{
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

	if (!varNode.isNull())
		if (varNode.attribute("VARIABLE_SEMANTIC") != "")
			return varNode.attribute("VARIABLE_SEMANTIC");

	return QString();
}

float* RfxParser::ValueFromRfx(const QString& VarName, RfxUniform* unif)
{
	// read a bool, int or float vector (max 4 elements)
	// or a float matrix (max 16 elements).
	// values 17 and 18 are reserved for min and max values (if present)
	float *result = new float[18];

	// initialization (unnecessary?)
	for (int i = 0; i < 18; ++i)
		result[i] = 0.0f;

	RfxUniform::UniformType VarType = unif->GetType();
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
			if (candidates.at(i).toElement().attribute("NAME") == VarName) {
				varNode = candidates.at(i).toElement();
				unif->setIsRmColorVariable(true);
				result[16] = 0.0;
				result[17] = 1.0;
			}
	}

	if (!varNode.isNull()) {
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

		if (!varNode.attribute("MIN").isNull())
			result[16] = varNode.attribute("MIN").toFloat();
		if (!varNode.attribute("MAX").isNull())
			result[17] = varNode.attribute("MAX").toFloat();
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

	int i;
	for (i = 0; i < candidates.size(); ++i) {
		varNode = candidates.at(i).toElement();
		if (varNode.attribute("NAME") == VarName) {

			// start from rfx file directory
			QDir fDir(QFileInfo(*rmShader).absolutePath());

			// ... get relative path to texture file
			QString fName(varNode.attribute("FILE_NAME").replace('\\', '/'));
			
			// The textures with the special name TEXTURE0.png
			// are interpreted as 'the current texture of the mesh'
			// to allow a seamless integration of the shaders with
			// the textured mesh of meshlab.
			if(fName==QString("TEXTURE0.PNG"))
			{
				QFileInfo thefile(meshTextureName);
				if(thefile.exists())		qDebug("The texture %s exists.", qPrintable(meshTextureName));
				filePath = thefile.absoluteFilePath();
				qDebug("Using the texture of the mesh %s",qPrintable(filePath));
			}
			else 
			{
			// then join together and return the absolute path
			QFileInfo thefile(fDir, fName);
			filePath = thefile.absoluteFilePath();
			}
			break;
		}
	}

	// if nothing was found and we're looking for a sampler2D, maybe it's
	// a RmRenderableTexture
	// ***
	// note that we quit previous loop with break, so the only reason
	// for i being equal candidates.size() is that nothing was found
	if (i == candidates.size() && VarType == RfxUniform::SAMPLER2D) {
		int rt = rfxShader->FindRT(VarName);

		// convention: if texture is a renderable texture, pass
		// "RT:<index in list>" string to uniform class
		if (rt != -1)
			filePath = "RT:" + QString().setNum(rt);
	}

	return filePath;
}

QList<RfxState*> RfxParser::ParseGLStates(QDomNodeList statelist,
                                           RfxState::StateType statetype)
{
	QList<RfxState*> glStates;

	for (int i = 0; i < statelist.size(); ++i) {
		QDomElement stateEl = statelist.at(i).toElement();
		RfxState *glstate = new RfxState(statetype);
		glstate->SetState(stateEl.attribute("STATE").toInt());
		glstate->SetValue(stateEl.attribute("VALUE").toLong());

		glStates.append(glstate);
	}

	return glStates;
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

/*
	Verifies if the shader sources contains some special attributes.
	For each special attribute declared creates a new instance of SpecialAttribute and appends it in the GLPass.
	@param source the source of the shader.
	@param pass the GLPass.
*/
void RfxParser::ParseAttributes(const QString &source, RfxGLPass *pass)
{
	QString txtSource = source;
	int position = 0;
	int attrib;
	while ((attrib = txtSource.indexOf("attribute", position)) != -1) {
		int semicol = txtSource.indexOf(";", attrib);
		int declLen = semicol - attrib;
		QStringList decl = txtSource.mid(attrib, declLen).split(QRegExp("\\s+|\\,"));
		position = semicol;
		QString temp;
		for (int i =2; i < decl.size(); ++i){
			temp = decl.at(i);
                        //qDebug("temp : ",temp);
			if(RfxSpecialAttribute::getSpecialType(temp) != RfxSpecialAttribute::NONE)
				pass->AddSpecialAttribute(new RfxSpecialAttribute(temp));
		}
	}
}