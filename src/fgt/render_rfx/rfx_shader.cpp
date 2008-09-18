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

#include "rfx_shader.h"

RfxShader::RfxShader()
{
}

RfxShader::~RfxShader()
{
	foreach (RfxGLPass *pass, shaderPasses) {
		delete pass;
		pass = 0;
	}
	shaderPasses.clear();

	foreach (RfxRenderTarget *rt, renderTargets) {
		delete rt;
		rt = 0;
	}
	renderTargets.clear();
}

int RfxShader::FindRT(const QString& theName)
{
	for (int i = 0; i < renderTargets.size(); ++i)
		if (renderTargets.at(i)->GetName() == theName)
			return i;

	return -1;
}

void RfxShader::SortPasses()
{
	for (int i = 0; i < shaderPasses.size() - 1; ++i)
		for (int j = 0; j < shaderPasses.size() - 1 - i; ++j)
			if (shaderPasses.at(j + 1)->GetPassIndex()
			    < shaderPasses.at(j)->GetPassIndex())
				shaderPasses.swap(j, j + 1);
}

void RfxShader::CompileAndLink()
{
	foreach (RfxGLPass *pass, shaderPasses)
		pass->CompileAndLink();
}

void RfxShader::Start(int passIdx)
{
	if (passIdx >= 0 && passIdx < shaderPasses.size()) {

		RfxGLPass *pass = shaderPasses.at(passIdx);
		RfxRenderTarget *rt = NULL;

		if (passIdx - 1 >= 0 && shaderPasses.at(passIdx - 1)->wantsRenderTarget()) {
			rt = shaderPasses.at(passIdx - 1)->GetRenderTarget();
			rt->Unbind();
		}

		if (pass->wantsRenderTarget()) {
			rt = pass->GetRenderTarget();
			if (rt->Setup(passIdx))
				rt->Bind(passIdx);
		}

		UpdateSemanticUniforms(passIdx);
		pass->Start();
	}
}

void RfxShader::UpdateSemanticUniforms(int passIdx)
{
	QMapIterator<SemanticValue, RfxUniform*> it(semUniforms);
	float params[16];
	float *unifVal;

	while (it.hasNext()) {
		it.next();
		unifVal = it.value()->GetValue();

		switch (it.key()) {

		// viewport data
		case VIEWPORTWIDTH:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = params[2] - params[0];
			break;
		case VIEWPORTHEIGHT:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = params[3] - params[1];
			break;
		case VIEWPORTDIMENSIONS:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = params[2] - params[0];
			*(unifVal + 1) = params[3] - params[1];
			break;
		case VIEWPORTWIDTHINVERSE:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = 1.0f / (params[2] - params[0]);
			break;
		case VIEWPORTHEIGHTINVERSE:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = 1.0f / (params[3] - params[1]);
			break;
		case INVERSEVIEWPORTDIMENSIONS:
			glGetFloatv(GL_VIEWPORT, params);
			*unifVal = 1.0f / (params[2] - params[0]);
			*(unifVal + 1) = 1.0f / (params[3] - params[1]);
			break;

		// pass index
		case PASSINDEX:
			*unifVal = passIdx;
			break;

		// view parameters
		case VIEWDIRECTION:
		case VIEWPOSITION:
		case VIEWSIDEVECTOR:
		case VIEWUPVECTOR:
		case FOV:
		case NEARCLIPPLANE:
		case FARCLIPPLANE:
			break;

		// view matrices
		case WORLD:
		case WORLDTRANSPOSE:
		case WORLDINVERSE:
		case WORLDINVERSETRANSPOSE:
			break;

		case VIEW:
		case VIEWTRANSPOSE:
		case VIEWINVERSE:
		case VIEWINVERSETRANSPOSE:
			break;

		case WORLDVIEW:
		case WORLDVIEWTRANSPOSE:
		case WORLDVIEWINVERSE:
		case WORLDVIEWINVERSETRANSPOSE:
			break;

		case PROJECTION:
		case PROJECTIONTRANSPOSE:
		case PROJECTIONINVERSE:
		case PROJECTIONINVERSETRANSPOSE:
			break;

		case VIEWPROJECTION:
		case VIEWPROJECTIONTRANSPOSE:
		case VIEWPROJECTIONINVERSE:
		case VIEWPROJECTIONINVERSETRANSPOSE:
			break;

		case WORLDVIEWPROJECTION:
		case WORLDVIEWPROJECTIONTRANSPOSE:
		case WORLDVIEWPROJECTIONINVERSE:
		case WORLDVIEWPROJECTIONINVERSETRANSPOSE:
			break;

		default:
			break;
		}
	}
}

// static member initialization - keep in sync with SemanticValue enum
const char *RfxShader::semantic[] = {
	"ViewportWidth", "ViewportHeight", "ViewportDimensions", "ViewportWidthInverse",
	"ViewportHeightInverse", "InverseViewportDimensions",
	"PassIndex",
	"ViewDirection", "ViewPosition", "ViewSideVector", "ViewUpVector", "FOV", "NearClipPlane", "FarClipPlane",
	"View", "ViewTranspose", "ViewInverse", "ViewInverseTranspose",
	"Projection", "ProjectionTranspose", "ProjectionInverse", "ProjectionInverseTranspose",
	"ViewProjection", "ViewProjectionTranspose", "ViewProjectionInverse", "ViewProjectionInverseTranspose",
	"World", "WorldTranspose", "WorldInverse", "WorldInverseTranspose",
	"WorldView", "WorldViewTranspose", "WorldViewInverse", "WorldViewInverseTranspose",
	"WorldViewProjection", "WorldViewProjectionTranspose", "WorldViewProjectionInverse", "WorldViewProjectionInverseTranspose"
};
bool RfxShader::AddSemanticUniform(RfxUniform *u, const QString &sem)
{
	for (int i = 0; i < TOT_SEMANTICS; ++i)
		if (semantic[i] == sem) {
			semUniforms.insert((SemanticValue)i , u);
			return true;
		}
	return false;
}
