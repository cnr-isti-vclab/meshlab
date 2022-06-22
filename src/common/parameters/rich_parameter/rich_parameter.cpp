/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
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

#include "rich_parameter.h"

#include <common/ml_document/mesh_document.h>
#include <common/python/python_utils.h>

RichParameter::RichParameter(const RichParameter& rp) :
	pName(rp.pName),
	val(rp.value().clone()),
	fieldDesc(rp.fieldDesc),
	tooltip(rp.tooltip),
	advanced(rp.advanced),
	defaultValue(rp.defaultValue),
	pCategory(rp.pCategory)
{
}

RichParameter::RichParameter(RichParameter&& rp)
{
	swap(rp);
}

RichParameter::RichParameter(
		const QString& nm,
		const Value& v,
		const QString& desc,
		const QString& tltip,
		bool isAdvanced,
		const QString& category) :
	pName(nm),
	val(v.clone()),
	fieldDesc(desc),
	tooltip(tltip),
	advanced(isAdvanced),
	defaultValue(true),
	pCategory(category)
{
}

RichParameter::~RichParameter()
{
	delete val;
}

const QString& RichParameter::name() const
{
	return pName;
}

const Value& RichParameter::value() const
{
	return *val;
}

const QString& RichParameter::fieldDescription() const
{
	return fieldDesc;
}

const QString& RichParameter::toolTip() const
{
	return tooltip;
}

bool RichParameter::isAdvanced() const
{
	return advanced;
}

bool RichParameter::isValueDefault() const
{
	return defaultValue;
}

const QString& RichParameter::category() const
{
	return pCategory;
}

void RichParameter::setName(const QString& newName)
{
	pName = newName;
}

void RichParameter::setValue(const Value& ov, bool isDefault)
{
	assert(val->typeName() == ov.typeName());
	delete val;
	val = ov.clone();
	defaultValue = isDefault;
}

void RichParameter::setDefaultValue(bool isDefault)
{
	defaultValue = isDefault;
}

QDomElement RichParameter::fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip) const
{
	QDomElement parElem = doc.createElement("Param");
	parElem.setAttribute("name", pName);
	parElem.setAttribute("type", stringType());
	if (saveDescriptionAndTooltip) {
		parElem.setAttribute("description", fieldDesc);
		parElem.setAttribute("tooltip",tooltip);
	}
	val->fillToXMLElement(parElem);
	return parElem;
}

/**
 * @brief returns the name of the parameter used in python (for pymeshlab)
 */
QString RichParameter::pythonName() const
{
	return pymeshlab::computePythonName(pName);
}

/**
 * @brief returns the type if the parameter (as a string) used in python
 * (for pymeshlab)
 */
QString RichParameter::pythonType() const
{
	return pymeshlab::computePythonTypeString(*this);
}

RichParameter& RichParameter::operator=(const RichParameter& rp)
{
	if (&rp != this){
		delete val;
		val = rp.value().clone();
		pName = rp.pName;
		fieldDesc = rp.fieldDesc;
		tooltip = rp.tooltip;
		advanced = rp.advanced;
		defaultValue = rp.defaultValue;
		pCategory = rp.pCategory;
	}
	return *this;
}

RichParameter& RichParameter::operator=(RichParameter&& rp)
{
	::swap(*this, rp);
	return *this;
}

void RichParameter::swap(RichParameter &rp)
{
	::swap(*this, rp);
}

void swap(RichParameter &rp1, RichParameter &rp2)
{
	using std::swap;
	swap(rp1.pName, rp2.pName);
	swap(rp1.val, rp2.val);
	swap(rp1.fieldDesc, rp2.fieldDesc);
	swap(rp1.tooltip, rp2.tooltip);
	swap(rp1.advanced, rp2.advanced);
	swap(rp1.defaultValue, rp2.defaultValue);
	swap(rp1.pCategory, rp2.pCategory);
}
