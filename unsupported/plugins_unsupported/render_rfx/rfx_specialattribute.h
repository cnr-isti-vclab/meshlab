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

#ifndef RFX_SPECIALATTRIBUTE_H_
#define RFX_SPECIALATTRIBUTE_H_

#include <cassert>
#include <QString>
#include <common/meshmodel.h>

class RfxSpecialAttribute
{
public:
     //Special attribute types used to pass particular values taken from meshlab to the shader.
     enum SpecialAttributeType {
           NONE=-1,MSHLB_CURVATURE,MSHLB_QUALITY,
          TOTAL_SPECIAL_TYPES
         };

	RfxSpecialAttribute(const QString&);
	virtual ~RfxSpecialAttribute();
	
	/*
     Verifies if the attribute is a "special" one or not.
     If the name of the attribute is one of the attribute special name it sets the special type of the attribute.
     Otherwise the attribute will have special value sets to "NONE"
     */
    static RfxSpecialAttribute::SpecialAttributeType getSpecialType(const QString&);
	static char *SpecialAttributeTypeString[]; 
	static char *SpecialAttributeDescription[]; 
	int getDataMask();
        void setValueByType(GLint, CVertexO*);
        char* getTypeName();
        char* getDescription();
	
	SpecialAttributeType getSpecialAttributeType(){return this->_specialType;}
	

private:   
	SpecialAttributeType _specialType;
    
};

#endif /* RFX_SPECIALATTRIBUTE_H_*/
