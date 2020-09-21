/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#include "decorate_shadow.h"

using namespace vcg;

QString DecorateShadowPlugin::decorationInfo(FilterIDType id) const
{
	switch (id) {
	case DP_SHOW_SHADOW:           return tr("Enable shadow mapping");
	case DP_SHOW_SSAO:              return tr("Enable Screen Space Ambient Occlusion");
	default:     assert(0); return QString();
	}
}

QString DecorateShadowPlugin::decorationName(FilterIDType id) const
{
	switch (id) {
	case DP_SHOW_SHADOW:           return tr("Enable shadow mapping");
	case DP_SHOW_SSAO:              return tr("Enable Screen Space Ambient Occlusion");
	default:     assert(0); return QString();
	}
}

QString DecorateShadowPlugin::pluginName() const
{
	return "DecorateShadow";
}

void DecorateShadowPlugin::initGlobalParameterList(QAction *action, RichParameterList &parset)
{
	switch (ID(action)) {
	case DP_SHOW_SHADOW: {
		assert(!(parset.hasParameter(this->DecorateShadowMethod())));
		int method = SH_MAP_VSM_BLUR;
		parset.addParam(
			RichEnum(
				this->DecorateShadowMethod(),
				method,
				getSHMethods(),
				"Shader", "Shader used to perform shadow mapping decoration")
		);
		parset.addParam(RichDynamicFloat(this->DecorateShadowIntensity(), 0.3f, 0.0f, 1.0f, "Intensity", "Shadow Intensity"));
		break;
	}

	case DP_SHOW_SSAO: {
		assert(!(parset.hasParameter(this->DecorateShadowSSAORadius())));
		float radius = 0.25f;
		parset.addParam(
			RichFloat(this->DecorateShadowSSAORadius(),
				radius,"SSAO radius",
				"Uniform parameter for SSAO shader"));
		break;
	}

	default: assert(0);
	}
}

bool DecorateShadowPlugin::startDecorate(const QAction* action, MeshDocument& /*m*/, const RichParameterList* parset, GLArea* /*gla*/)
{
	bool result;

	switch (ID(action))
	{
	case DP_SHOW_SHADOW:
		if (!parset->hasParameter(DecorateShadowMethod())) {
			qDebug("Unable to find Shadow mapping method");
			assert(0);
		}
		switch (parset->getEnum(DecorateShadowMethod()))
		{
		case SH_MAP:
		{
			smShader = new ShadowMapping(0.1f);
			this->_decoratorSH = smShader; break;
		}
		case SH_MAP_VSM:
		{
			vsmShader = new VarianceShadowMapping(0.1f);
			this->_decoratorSH = vsmShader; break;
		}
		case SH_MAP_VSM_BLUR:
		{
			vsmbShader = new VarianceShadowMappingBlur(0.1f);
			this->_decoratorSH = vsmbShader; break;
		}
		}
		this->_decoratorSH->setShadowIntensity(parset->getDynamicFloat(this->DecorateShadowIntensity()));
		result = this->_decoratorSH->init();
		return result;

	case DP_SHOW_SSAO:
		_decoratorSSAO = new SSAO(0.1f);
		this->_decoratorSSAO->setRadius(parset->getFloat(DecorateShadowSSAORadius()));
		result = this->_decoratorSSAO->init();
		return result;

	default: assert(0);
	}
	return false;
}

void DecorateShadowPlugin::endDecorate(const QAction* action, MeshDocument & /*md*/, const RichParameterList * parset, GLArea * /*gla*/)
{
	switch (ID(action))
	{
		case DP_SHOW_SHADOW:
		{
			if (!parset->hasParameter(DecorateShadowMethod()))
			{
				qDebug("Unable to find Shadow mapping method");
				assert(0);
			}
			switch (parset->getEnum(DecorateShadowMethod()))
			{
				case SH_MAP:
				{
					delete smShader;
					smShader = NULL;
					break;
				}
				case SH_MAP_VSM:
				{
					delete vsmShader;
					vsmShader = NULL;
					break;
				}
				case SH_MAP_VSM_BLUR:
				{
					delete vsmbShader;
					vsmbShader = NULL;
					break;
				}
			}
			_decoratorSH = NULL;
			break;
		}
		case DP_SHOW_SSAO:
		{
			delete _decoratorSSAO;
			_decoratorSSAO = NULL;
			break;
		}
		default: 
			break;
	}
}


void DecorateShadowPlugin::decorateDoc(const QAction* action, MeshDocument &md, const RichParameterList *, GLArea *gla, QPainter *, GLLogStream &)
{
	switch (ID(action)) {
	case DP_SHOW_SHADOW:
		this->_decoratorSH->runShader(md, gla);
		break;

	case DP_SHOW_SSAO:
		this->_decoratorSSAO->runShader(md, gla);
		break;

	default: assert(0);
	}
}


MESHLAB_PLUGIN_NAME_EXPORTER(DecorateShadowPlugin)
