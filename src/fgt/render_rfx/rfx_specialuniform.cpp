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

#include "rfx_specialuniform.h"

// static member initialization
const char *RfxSpecialUniform::SpecialUniformTypeString[] = {
	"MSHLB_BBOX_MIN","MSHLB_BBOX_MAX","MSHLB_QUALITY_MIN", "MSHLB_QUALITY_MAX"
};

RfxSpecialUniform::RfxSpecialUniform(const QString &_name, const QString &_type, MeshDocument* mDoc) : RfxUniform(_name, _type)
{
     this->_mDoc = mDoc;
     this->_specialType = getSpecialType(_name);
}

RfxSpecialUniform::~RfxSpecialUniform()
{
	
}

/*
	Initializes the value of the uniform according to its special type.
*/
void RfxSpecialUniform::initialize(){
     float val[16];

	vcg::Box3f bb;
	 std::pair<float, float> qMinMax;

     switch(this->_specialType){
          case MSHLB_BBOX_MIN:
               bb = this->_mDoc->mm()->cm.bbox;
               val[0] = bb.min[0];
               val[1] = bb.min[1];
               val[2] = bb.min[2];
               val[3] = 1.0;

               this->SetValue(val);
               break;
          

          case MSHLB_BBOX_MAX:
               bb = this->_mDoc->mm()->cm.bbox;
               val[0] = bb.max[0];
               val[1] = bb.max[1];
               val[2] = bb.max[2];
               val[3] = 1.0;


               this->SetValue(val);
               break;

		case MSHLB_QUALITY_MIN:
			qMinMax = vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(this->_mDoc->mm()->cm);
			val[0] = qMinMax.first;
			this->SetValue(val);
			break;

		case MSHLB_QUALITY_MAX:
			qMinMax = vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(this->_mDoc->mm()->cm);
			val[0] = qMinMax.second;
			this->SetValue(val);
			break;

          default:
	          break;
          }
}

/*
	Verifies if the uniform is a special one or not.
	If the name of the uniform is one of the uniform special name it returns the special type of the uniform.
	Otherwise returns "NONE".
	@param name the name of the uniform
	@return the special uniform type.
*/
RfxSpecialUniform::SpecialUniformType RfxSpecialUniform::getSpecialType(const QString& name)
{
	for(int i = 0; i < RfxSpecialUniform::TOTAL_SPECIAL_TYPES; ++i) {
		if (name == SpecialUniformTypeString[i])
               return (SpecialUniformType)i;
	}
     return NONE;
}
