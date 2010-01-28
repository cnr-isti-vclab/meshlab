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
#ifndef SAMPLE_DECORATE_PLUGIN_H
#define SAMPLE_DECORATE_PLUGIN_H

#include <QObject>
#include <common/interfaces.h>

#include "decorate_shader.h"
#include "shadow_mapping.h"
#include "variance_shadow_mapping.h"
#include "variance_shadow_mapping_blur.h"
#include "ssao.h"

class DecorateShadowPlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)

    enum {
        DP_SHOW_SHADOW,
        DP_SHOW_SSAO
    };

  //3 different implementations of shadow mapping
    enum{
        SH_MAP,             //simple shadow mapping
        SH_MAP_VSM,         //variance shadow mapping
        SH_MAP_VSM_BLUR     //variance shadow mapping with blur
    };

    static QStringList getSHMethods(){
        return QStringList()
                <<"Shadow mapping"
                <<"Variance shadow mapping"
                <<"Variance shadow mapping with blur";
    }

    virtual QString filterInfo(QAction *) const;
    virtual QString filterName(FilterIDType filter) const;

public:
     
    DecorateShadowPlugin(){
        typeList <<
        DP_SHOW_SHADOW <<
        DP_SHOW_SSAO;

        FilterIDType tt;
        foreach(tt , types()){
          actionList << new QAction(filterName(tt), this);
        }
        QAction *ap;
        foreach(ap,actionList){
            ap->setCheckable(true);
        }
    }

	QList<QAction *> actions () const {return actionList;}
    virtual bool StartDecorate(QAction * /*mode*/, MeshModel &/*m*/, RichParameterSet  * /*parent*/ par, GLArea * /*parent*/);
    virtual void Decorate(QAction *a, MeshModel &m,  GLArea *gla,QFont qf);
    virtual void initGlobalParameterSet(QAction *, RichParameterSet  & globalparam);
private:
    DecorateShader* _decoratorSH, *_decoratorSSAO;
    inline const QString DecorateShadowSSAORadius() { return  "MeshLab::Decoration::SSAORadius" ; }
    inline const QString DecorateShadowMethod() { return  "MeshLab::Decoration::ShadowMethod" ; }
};

#endif
