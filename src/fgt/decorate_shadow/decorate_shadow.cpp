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

QString DecorateShadowPlugin::filterInfo(QAction *action) const
{
    switch(ID(action)){
        case DP_SHOW_SHADOW :           return tr("Enable shadow mapping");
        case DP_SHOW_SSAO:              return tr("Enable Screen Space Ambient Occlusion");
    default:     assert(0); return QString();
    }  
}

QString DecorateShadowPlugin::filterName(FilterIDType id) const
{
    switch(id){
        case DP_SHOW_SHADOW :           return tr("Enable shadow mapping");
        case DP_SHOW_SSAO:              return tr("Enable Screen Space Ambient Occlusion");
    default:     assert(0); return QString();
    }
}

void DecorateShadowPlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
    switch(ID(action)){
        case DP_SHOW_SHADOW : {
            assert(!(parset.hasParameter(this->DecorateShadowMethod())));
            int method = SH_MAP_VSM_BLUR;
            parset.addParam(
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
            assert(!(parset.hasParameter(this->DecorateShadowSSAORadius())));
            float radius = 0.25f;
            parset.addParam(
                    new RichFloat(this->DecorateShadowSSAORadius(),
                    radius,
                    "Uniform parameter for SSAO shader",
                    "SSAO radius"));
            break;
        }

        default: assert(0);
    }
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
                    this->_decoratorSH = new ShadowMapping();
                    break;

                case SH_MAP_VSM:
                    this->_decoratorSH = new VarianceShadowMapping();
                    break;

                case SH_MAP_VSM_BLUR:
                    this->_decoratorSH = new VarianceShadowMappingBlur();
                    break;

                default: assert(0);
            }
            result = this->_decoratorSH->init();
            return result;

        case DP_SHOW_SSAO:
            if(!parset->hasParameter(DecorateShadowMethod())){
                qDebug("Unable to find uniform variable radius for SSAO shader");
                assert(0);
            }
            this->_decoratorSSAO = new SSAO(parset->getFloat(DecorateShadowSSAORadius()));
            result = this->_decoratorSSAO->init();
            return result;

        default: assert(0);
    }
}

void DecorateShadowPlugin::Decorate(QAction *action, MeshModel &m, GLArea *gla, QFont /*qf*/)
{
    if(m.visible){
        switch(ID(action)){
            case DP_SHOW_SHADOW :
                this->_decoratorSH->runShader(m, gla);
                break;

            case DP_SHOW_SSAO:
                this->_decoratorSSAO->runShader(m, gla);
                break;

            default: assert(0);
        }
    }
}

Q_EXPORT_PLUGIN(DecorateShadowPlugin)
