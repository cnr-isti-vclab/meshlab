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

#include "rfx_specialattribute.h"

char *RfxSpecialAttribute::SpecialAttributeTypeString[] = {
	"MSHLB_CURVATURE","MSHLB_QUALITY"
};

char *RfxSpecialAttribute::SpecialAttributeDescription[] = {
	"Vertex Curvature","Vertex Quality"
};

RfxSpecialAttribute::RfxSpecialAttribute(const QString &_name)
{
     this->_specialType = getSpecialType(_name);
}

RfxSpecialAttribute::~RfxSpecialAttribute()
{
}


/*
Verifies if the Attribute is a special one or not.
If the name of the Attribute is one of the Attribute special name it sets the special type of the Attribute.
Otherwise will have special value sets to "NONE"
*/
RfxSpecialAttribute::SpecialAttributeType RfxSpecialAttribute::getSpecialType(const QString& name)
{
	for(int i = 0; i < RfxSpecialAttribute::TOTAL_SPECIAL_TYPES; ++i) {
		if (name == SpecialAttributeTypeString[i])
               return (SpecialAttributeType)i;
	}
     return NONE;
}

void RfxSpecialAttribute::setValueByType(GLint loc, CVertexO *vert)
{
float qual;
	switch(this->_specialType){
		case RfxSpecialAttribute::MSHLB_CURVATURE :
			glVertexAttrib1f(loc, 2.0);
			break;

		case RfxSpecialAttribute::MSHLB_QUALITY :
			qual=vert->Q();
			glVertexAttrib1f(loc, qual);
			break;
	}
}

char* RfxSpecialAttribute::getTypeName()
{
	return RfxSpecialAttribute::SpecialAttributeTypeString[this->_specialType];
}

char* RfxSpecialAttribute::getDescription()
{
	return RfxSpecialAttribute::SpecialAttributeDescription[this->_specialType];
}

int RfxSpecialAttribute::getDataMask()
{
	switch(this->_specialType){
		case RfxSpecialAttribute::MSHLB_CURVATURE :
			return MeshModel::MM_VERTCURV;
			break;

			case RfxSpecialAttribute::MSHLB_QUALITY :
				return MeshModel::MM_VERTQUALITY;
			break;
			default:
				return MeshModel::MM_NONE;
				break;
	}
}