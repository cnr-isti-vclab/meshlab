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
/****************************************************************************
History
$Log$
Revision 1.4  2007/12/11 16:19:37  corsini
add mapping between rm code and opengl code

Revision 1.3  2007/12/06 14:47:04  corsini
code restyling

Revision 1.2  2007/12/03 10:52:58  corsini
code restyling


****************************************************************************/

// Local headers
#include "RmXmlParser.h"
using std::make_pair;


RmXmlParser::RmXmlParser()
{
	initializeCodeMapping();
}

RmXmlParser::RmXmlParser(QString filename)
{
	setFileName(filename);
	initializeCodeMapping();
}

void RmXmlParser::initializeCodeMapping()
{
	// GL_TextureWrapS
	mapcode["GL_TextureWrapS1"] =
		make_pair("GL_CLAMP", GL_CLAMP);
	mapcode["GL_TextureWrapS2"] =
		make_pair("GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE);
	mapcode["GL_TextureWrapS3"] =
		make_pair("GL_REPEAT", GL_REPEAT);
	mapcode["GL_TextureWrapS4"] =
		make_pair("GL_CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER);
	mapcode["GL_TextureWrapS5"] =
		make_pair("GL_MIRRORED_REPEAT", GL_MIRRORED_REPEAT);

	// GL_TextureWrapT
	mapcode["GL_TextureWrapT1"] =
		make_pair("GL_CLAMP", GL_CLAMP);
	mapcode["GL_TextureWrapT2"] =
		make_pair("GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE);
	mapcode["GL_TextureWrapT3"] =
		make_pair("GL_REPEAT", GL_REPEAT);
	mapcode["GL_TextureWrapT4"] =
		make_pair("GL_CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER);
	mapcode["GL_TextureWrapT5"] =
		make_pair("GL_MIRRORED_REPEAT", GL_MIRRORED_REPEAT);

	// GL_TextureWrapR
	mapcode["GL_TextureWrapR1"] =
		make_pair("GL_CLAMP", GL_CLAMP);
	mapcode["GL_TextureWrapR2"] =
		make_pair("GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE);
	mapcode["GL_TextureWrapR3"] =
		make_pair("GL_REPEAT", GL_REPEAT);
	mapcode["GL_TextureWrapR4"] =
		make_pair("GL_CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER);
	mapcode["GL_TextureWrapR5"] =
		make_pair("GL_MIRRORED_REPEAT", GL_MIRRORED_REPEAT);


	// GL_TextureMagnify
	mapcode["GL_TextureMagnify0"] =
		make_pair("GL_NEAREST", GL_NEAREST);
	mapcode["GL_TextureMagnify1"] =
		make_pair("GL_LINEAR", GL_LINEAR);

	// GL_TextureMinify
	mapcode["GL_TextureMinify0"] =
		make_pair("GL_NEAREST", GL_NEAREST);
	mapcode["GL_TextureMinify1"] =
		make_pair("GL_LINEAR", GL_LINEAR);
	mapcode["GL_TextureMinify3"] =
		make_pair("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	mapcode["GL_TextureMagnify4"] =
		make_pair("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	mapcode["GL_TextureMinify5"] =
		make_pair("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	mapcode["GL_TextureMinify6"] =
		make_pair("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);
}

QString RmXmlParser::convertGlStateToString(GlState &glstate)
{
	GlParamType par;
	std::stringstream strstream;

	strstream << glstate.getName().toStdString();
	QString strvalue = QString("%1").arg(glstate.getValue());
	strstream << strvalue.toStdString();

	par = mapcode[strstream.str()];
	return QString::fromStdString(par.first);
}

int RmXmlParser::convertGlStateToInt(GlState &glstate)
{
	GlParamType par;
	std::stringstream strstream;

	strstream << glstate.getName().toStdString();
	QString strvalue = QString("%1").arg(glstate.getValue());
	strstream << strvalue.toStdString();

	par = mapcode[strstream.str()];
	return par.second;
}


bool RmXmlParser::parse(QString _filename)
{
	if (!_filename.isNull())
		setFileName(_filename);

	QFile file(filename);
	effects.clear();

	// open the file
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		error = "Impossible to open the specified file: ";
		error += filename;
		return false;
	}

	QString errorMsg;
	int errorLine, errorColumn;

	// parse the xml document
	if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
		error = QString("Xml Parse Error ") + filename;
		error += QString("(") + QString().setNum(errorLine) +
		         QString(",") + QString().setNum(errorColumn) +
		         QString("): ") + errorMsg;
		return false;
	}

	QDomElement root = doc.documentElement();

	/*
	 * This is what i understood and what this parser does.
	 * the xml looks like to have one or more RmOpenGLEffect, and each one can
	 * contain one or more RmGLPass. The uniform variables are tagged inside the
	 * RmGLPass as RmShaderConstant or RmSampler. They are a RmShaderConstant when
	 * the type is a "primitive" one (float int vec2, mat4 ..); if they are
	 * a RmSampler then they probably refers to a texture. In this case inside the
	 * RmGLPass is specified a tag RmTextureObject with the name of variable.
	 * This tag tells us the opengl state for the texture and a texture reference.
	 * Note, it can happend that in the source code is specified a uniform variable
	 * that is not used: in this case no RmShaderConstant neither a RmSampler will
	 * appear in RmGLPass.
	 *
	 * The values of the uniform variable (such as default value, min and max value
	 * for RmShaderConstant, and the file name for the RmSampler) are kept in tags
	 * at the top level of the xml tree, and they have as tagname something like
	 * RmMatrixVariable, RmVectorVariable, RmFloatVariable, Rm2DTextureVariable,
	 * RmCubemapVariable etc.. according to the variable type
	 */
	// we're looking for RmOpenGLEffect xml tag
	QDomNodeList list = root.elementsByTagName("RmOpenGLEffect");
	for (int i = 0; i < list.size(); i++) {
		QDomElement effectElement = list.at(i).toElement();
		RmEffect eff(effectElement.attribute("NAME", "name not set"));

		// each effect has a number (0-n) of RmGLPass.
		QDomNodeList passlist =
			effectElement.elementsByTagName("RmGLPass");

		for (int j = 0; j < passlist.size(); j++) {
			// get the pass name
			QDomNode passNode = passlist.at(j);
			QDomElement elp = passNode.toElement();
			bool ok;
			int index = elp.attribute("PASS_INDEX").toInt(&ok);
			RmPass pass(elp.attribute("NAME", "name not set"),
			            (ok ? index : -1));

			// openGL state
			QDomElement stateEl =
				passNode.firstChildElement("RmRenderStateBlock");

			if(!stateEl.isNull()) {
				QDomNodeList statelist =
					stateEl.elementsByTagName("RmState");

				for (int k = 0; k < statelist.size(); k++) {
					GlState s(statelist.at(k).toElement());
					if (s.isValid())
						pass.addOpenGLState(s);
				}
			}

			// get the render target
			QDomElement renderEl =
				passNode.firstChildElement("RmRenderTarget");
			if (!renderEl.isNull())	{
				RenderTarget rt(renderEl.attribute("NAME"));
				rt.renderToScreen = renderEl.attribute("RENDER_TO_SCREEN") == "TRUE";
				rt.colorClear = renderEl.attribute("COLOR_CLEAR") == "TRUE";
				rt.depthClear = renderEl.attribute("DEPTH_CLEAR") == "TRUE";
				rt.clearColorValue = renderEl.attribute("CLEAR_COLOR_VALUE").toDouble();
				rt.depthClearValue = renderEl.attribute("DEPTH_CLEAR_VALUE").toDouble();
				pass.setRenderTarget(rt);
			}

			// get the source code of fragment and vertex program
			QDomNodeList sourcelist =
				elp.elementsByTagName("RmGLShader");
			for (int k = 0; k < sourcelist.size(); k++) {
				QDomNode elnode = sourcelist.at(k);
				QDomElement elsource = elnode.toElement();
				QString name = elsource.attribute("NAME");
				if (name == "Fragment Program" ||
				    name == "Fragment Shader") {
					pass.setFragment(elsource.text());
				} else if (name == "Vertex Program" ||
				           name == "Vertex Shader") {
					pass.setVertex(elsource.text());
				}
			}

			// get the name of constant uniform variables and search
			// in the whole document for their values
			QDomNodeList constlist =
				elp.elementsByTagName("RmShaderConstant");
			for (int k = 0; k < constlist.size(); k++) {
				QString name =
					constlist.at(k).toElement().attribute("NAME");
				UniformVar var =
					pass.searchFragmentUniformVariable(name);
				if (!var.isNull()) {
					var.getValueFromXmlDocument(root, effectElement);
					pass.addFragmentUniform(var);
				}
				var = pass.searchVertexUniformVariable(name);
				if (!var.isNull()) {
					var.getValueFromXmlDocument(root, effectElement);
					pass.addVertexUniform(var);
				}
			}

			// and texture uniform variables
			QDomNodeList textureObjectList = elp.elementsByTagName("RmTextureObject");
			QDomNodeList samplerlist = elp.elementsByTagName("RmSampler");
			for (int k = 0; k < samplerlist.size(); k++) {
				QString name = samplerlist.at(k).toElement().attribute("NAME");
				QString textureName;
				QList<GlState> GLstates;

				// First get the textureObject xml tag relative to this texture
				for (int q = 0; q < textureObjectList.size(); q++) {
					QDomElement textEl = textureObjectList.at(q).toElement();
					QString toName = textEl.attribute("NAME");
					if (toName == name) {
						QDomElement trEl =
							textEl.firstChildElement("RmTextureReference");
						if (!trEl.isNull())
							textureName = trEl.attribute("NAME");

						// save the GlStates
						QDomNodeList statesList =
							textEl.elementsByTagName("RmState");
						for (int w = 0; w < statesList.size(); w++) // * LOL! i,j,k,q,w used !
							GLstates.append( statesList.at(w).toElement());
						break;
					}
				}

				// then search the variable in the source code
				UniformVar var = pass.searchFragmentUniformVariable(name);
				if (!var.isNull()) {
					var.textureName = textureName;
					var.textureGLStates = GLstates;
					var.getValueFromXmlDocument(root, effectElement);
					pass.addFragmentUniform(var);
				}

				var = pass.searchVertexUniformVariable(name);
				if (!var.isNull()) {
					var.textureName = textureName;
					var.textureGLStates = GLstates;
					var.getValueFromXmlDocument(root, effectElement);
					pass.addVertexUniform(var);
				}
			}
			eff.addPass( pass );
		}
		eff.sortPasses();
		effects.append(eff);
	}
	return true;
}

void RmXmlParser::VarDump(bool extendedDump)
{
	qDebug() << "";
	qDebug() << "Dumping:" << filename;
	qDebug() << "Found" << effects.size() << "RmOpenGLEffect:";

	for (int i = 0; i < effects.size(); i++) {
		RmEffect eff = effects[i];

		qDebug() << "  RmEffect" << eff.getName() << "with" << eff.size() << "pass";
		for (int j = 0; j < eff.size(); j++) {
			RmPass pass = eff[j];

			qDebug() << "   "
			         << (pass.hasIndex()?
			              QString(QString().setNum(pass.getIndex()) + "'").toLatin1().data() :
			              QString("").toLatin1().data())
			         << "Pass" << pass.getName() << "on model"
			         << pass.getModelReference()
			         << "(" << pass.getModelReferenceFN().toLatin1().data() << ")";

			if (pass.hasRenderTarget()) {
				RenderTarget rt = pass.getRenderTarget();
				qDebug() << "          Render Target:"
				         << rt.name
				         << (rt.colorClear ?
				              " (with color clear: " + QString().setNum(rt.clearColorValue) + ")" :
				              "")
				         << (rt.colorClear ?
				              " (with depth clear: " + QString().setNum(rt.depthClearValue) + ")" :
				              "");
			}

			if (pass.openGLStatesSize() == 0)
				qDebug() << "          with no opengl states set";
			else {
				qDebug() << "          with" << pass.openGLStatesSize() << "opengl states set";

				for (int k = 0; k < pass.openGLStatesSize(); k++)
					qDebug() << "          " << pass.getOpenGLState(k).getName() << " (" << QString().setNum(pass.getOpenGLState(k).getState()).toLatin1().data() << ") =>" << pass.getOpenGLState(k).getValue();
			}

			qDebug() << "        - Fragment (" << pass.getFragment().length() << "bytes ): " << pass.getFragment().mid(0, 50).replace( "\n", " " ).replace("\r","") << "...";
			qDebug() << "          There are" << pass.fragmentUniformVariableSize() << "uniform variables";

			for( int k = 0; k < pass.fragmentUniformVariableSize(); k++ )
				pass.getFragmentUniform( k ).VarDump(12, extendedDump);

			qDebug() << "        - Vertex (" << pass.getVertex().length() << "bytes ): " << pass.getVertex().mid(0, 50).replace( "\n", " " ).replace("\r","") << "...";
			qDebug() << "          There are" << pass.vertexUniformVariableSize() << "uniform variables";

			for( int k = 0; k < pass.vertexUniformVariableSize(); k++ )
				pass.getVertexUniform( k ).VarDump(12, extendedDump);
		}
	}

	qDebug() << "";
}



QDomElement RmXmlParser::getDomElement(QDomElement &root, QString tagname, QString name)
{
	QDomNodeList list = root.elementsByTagName(tagname);
	for (int i = 0; i < list.size(); i++) {
		QString elName = list.at(i).toElement().attribute("NAME");
		if (name == elName)
			return list.at(i).toElement();
	}

	return QDomElement();
}

