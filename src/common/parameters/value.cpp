/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2021                                           \/)\/    *
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

#include "value.h"

#include "../ml_document/mesh_document.h"

void BoolValue::fillToXMLElement(QDomElement& element) const
{
	QString v =  pval ? "true" : "false";
	element.setAttribute("value", v);
}

void IntValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}

void FloatValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}

void StringValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", pval);
}

void Matrix44fValue::fillToXMLElement(QDomElement& element) const
{
	for(unsigned int ii = 0;ii < 16;++ii)
		element.setAttribute(QString("val")+QString::number(ii),QString::number(pval.V()[ii]));
}

void Point3fValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("x",QString::number(pval.X()));
	element.setAttribute("y",QString::number(pval.Y()));
	element.setAttribute("z",QString::number(pval.Z()));
}

void ShotfValue::fillToXMLElement(QDomElement&) const
{
	assert(0);
	//TODO!!!
}

void ColorValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("r",QString::number(pval.red()));
	element.setAttribute("g",QString::number(pval.green()));
	element.setAttribute("b",QString::number(pval.blue()));
	element.setAttribute("a",QString::number(pval.alpha()));
}

