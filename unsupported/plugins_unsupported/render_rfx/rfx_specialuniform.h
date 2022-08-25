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

#ifndef RFX_SPECIALUNIFORM_H_
#define RFX_SPECIALUNIFORM_H_

#include <cassert>
#include <QString>
#include <common/meshmodel.h>
#include "rfx_uniform.h"
#include <vcg/complex/algorithms/stat.h>

class RfxSpecialUniform : public RfxUniform
{
public:
     //Special uniform types used to pass particular values taken from meshlab to the shader.
     enum SpecialUniformType {
          MSHLB_BBOX_MIN,MSHLB_BBOX_MAX,MSHLB_QUALITY_MIN,MSHLB_QUALITY_MAX,
          TOTAL_SPECIAL_TYPES,
          NONE
    };

    RfxSpecialUniform(const QString&, const QString&, MeshDocument*);
    virtual ~RfxSpecialUniform();

    void initialize();

    /*
        Verifies if the uniform is a special one or not.
        If the name of the uniform is one of the uniform special name it returns the special type of the uniform.
        Otherwise returns "NONE".
        @param name the name of the uniform
        @return the special uniform type.
    */
    static RfxSpecialUniform::SpecialUniformType getSpecialType(const QString&);

private:
     /* A reference to the current mesh document used to retrieve values*/
     MeshDocument* _mDoc;
     /* The type of this special uniform */
     SpecialUniformType _specialType;



     static const char *SpecialUniformTypeString[];
};

#endif /* RFX_SPECIALUNIFORM_H_*/
