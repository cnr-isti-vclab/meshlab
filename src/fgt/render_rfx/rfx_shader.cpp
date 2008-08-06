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
	foreach (RfxGLPass *pass, shader_pass) {
		delete pass;
		pass = 0;
	}
	shader_pass.clear();
}

void RfxShader::AddGLPass(RfxGLPass *pass)
{
	shader_pass.append(pass);
	// TODO: reorder passes by their idx
}

void RfxShader::CompileAndLink(QGLContext *ctx)
{
	foreach (RfxGLPass *pass, shader_pass)
		pass->CompileAndLink(ctx);
}

void RfxShader::Start()
{
	// TODO: add support for multiple pass shader
	shader_pass.first()->Start();
}
