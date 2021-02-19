/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include "ml_plugin_gl_context.h"

#include <QThread>

MLPluginGLContext::MLPluginGLContext(const QGLFormat& frmt,QPaintDevice* dvc,MLSceneGLSharedDataContext& cont )
	:QGLContext(frmt,dvc),_shared(cont)
{
}

MLPluginGLContext::~MLPluginGLContext()
{
}

void MLPluginGLContext::drawMeshModel( int meshid) const
{
	MLPluginGLContext* id = const_cast<MLPluginGLContext*>(this);
	_shared.draw(meshid,id);
}

void MLPluginGLContext::setRenderingData( int meshid,MLRenderingData& dt )
{
	_shared.requestSetPerMeshViewRenderingData(QThread::currentThread(),meshid,this,dt);
}

void MLPluginGLContext::initPerViewRenderingData(int meshid,MLRenderingData& dt)
{
	_shared.requestInitPerMeshView(QThread::currentThread(),meshid,this,dt);
}

void MLPluginGLContext::removePerViewRenderindData()
{
	_shared.requestRemovePerMeshView(QThread::currentThread(),this);
}


void MLPluginGLContext::meshAttributesUpdated( int meshid,bool conntectivitychanged,const MLRenderingData::RendAtts& dt )
{
	_shared.requestMeshAttributesUpdated(QThread::currentThread(),meshid,conntectivitychanged,dt);
}


void MLPluginGLContext::smoothModalitySuggestedRenderingData( MLRenderingData& dt )
{
	MLRenderingData::RendAtts att;
	att[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
	att[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
	dt.set(MLRenderingData::PRIMITIVE_MODALITY::PR_SOLID,att);
}

void MLPluginGLContext::pointModalitySuggestedRenderingData( MLRenderingData& dt )
{
	MLRenderingData::RendAtts att;
	att[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
	att[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
	dt.set(MLRenderingData::PRIMITIVE_MODALITY::PR_POINTS,att);
}
