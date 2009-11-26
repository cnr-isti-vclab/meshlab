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

const QString DecorateShadowPlugin::Info(QAction *action)
{
    switch(ID(action)){
        case DP_SHOW_SHADOW :           return tr("Enable shadow mapping");
        case DP_SHOW_SSAO:              return tr("Enable Screen Space Ambient Occlusion");
    }
    assert(0);
    return QString();
}
 
void DecorateShadowPlugin::initGlobalParameterSet(QAction *action, RichParameterSet *parset)
{
    switch(ID(action)){
        case DP_SHOW_SHADOW : {
            assert(!(parset->hasParameter(this->DecorateShadowMethod())));
            int method = SH_MAP;//SH_MAP_VSM_BLUR;
            parset->addParam(
                    new RichEnum(
                        this->DecorateShadowMethod(),
                        method,
                        getSHMethods(),
                        "Shader used to perform shadow mapping decoration",
                        "Shadow mapping method")
                    );
            break;
        }

        case DP_SHOW_SSAO : {
            assert(!(parset->hasParameter(this->DecorateShadowSSAORadius())));
            float radius = 0.25f;
            parset->addParam(
                    new RichFloat(this->DecorateShadowSSAORadius(),
                    radius,
                    "Uniform parameter for SSAO shader",
                    "SSAO radius"));
            break;
        }

        default: assert(0);
    }
}		
		
const QString DecorateShadowPlugin::ST(FilterIDType filter) const
{
    switch(filter)
    {
        case DP_SHOW_SHADOW     : return QString("Shadow mapping");
        case DP_SHOW_SSAO       : return QString("SSAO");
        default: assert(0);
    }
    return QString("error!");
}

bool DecorateShadowPlugin::StartDecorate(QAction* action, MeshModel& m, RichParameterSet  * parset, GLArea* gla){
    bool result;
    switch(ID(action)){
        case DP_SHOW_SHADOW :
            if(!parset->hasParameter(DecorateShadowMethod())){
                qDebug("Unable to find Shadow mapping method");
                assert(0);
            }
            switch (parset->getEnum(DecorateShadowMethod())){
                case SH_MAP:
                    this->_decorator = new ShadowMapping();
                    break;

                case SH_MAP_VSM:
                    this->_decorator = new VarianceShadowMapping();
                    break;

                case SH_MAP_VSM_BLUR:
                    this->_decorator = new VarianceShadowMappingBlur();
                    break;

                default: assert(0);
            }
            result = true;
            break;
        case DP_SHOW_SSAO:
            if(!parset->hasParameter(DecorateShadowMethod())){
                qDebug("Unable to find uniform variable radius for SSAO shader");
                assert(0);
            }
            this->_decorator = new SSAO(parset->getFloat(DecorateShadowSSAORadius()));
            result = true;
            break;

        default: assert(0);
    }

    result = this->_decorator->init();
    return result;
}

void DecorateShadowPlugin::Decorate(QAction *, MeshModel &m, GLArea *gla, QFont /*qf*/)
{
    if(m.visible){
            this->_decorator->runShader(m, gla);
    }
}

Q_EXPORT_PLUGIN(DecorateShadowPlugin)
