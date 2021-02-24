/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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
#include "function_parameter.h"

#include "python_utils.h"
#include "../parameters/rich_parameter_list.h"

pymeshlab::FunctionParameter::FunctionParameter(const QString& pName,
		const RichParameter& parameter) :
	pName(pName),
	parameter(parameter.clone())
{
}

pymeshlab::FunctionParameter::FunctionParameter(
		const pymeshlab::FunctionParameter& oth):
	pName(oth.pName),
	parameter(oth.parameter->clone())
{
}

pymeshlab::FunctionParameter::FunctionParameter(pymeshlab::FunctionParameter&& oth):
	pName(oth.pName)
{
	parameter = oth.parameter;
	oth.parameter = nullptr;
}

pymeshlab::FunctionParameter::~FunctionParameter()
{
	delete parameter;
}

QString pymeshlab::FunctionParameter::pythonName() const
{
	//return pName;
	return parameter->pythonName();
}

QString pymeshlab::FunctionParameter::meshlabName() const
{
	return parameter->name();
}

QString pymeshlab::FunctionParameter::pythonTypeString() const
{
	//return pType;
	return parameter->pythonType();
}

QString pymeshlab::FunctionParameter::meshlabTypeString() const
{
	return parameter->stringType();
}

QString pymeshlab::FunctionParameter::description() const
{
	if (parameter){
		return parameter->fieldDescription();
	}
	return QString();
}

QString pymeshlab::FunctionParameter::longDescription() const
{
	if (parameter){
		return parameter->toolTip();
	}
	return QString();
}

const Value& pymeshlab::FunctionParameter::defaultValue() const
{
	return parameter->value();
}

const RichParameter& pymeshlab::FunctionParameter::richParameter() const
{
	return *parameter;
}

void pymeshlab::FunctionParameter::printDefaultValue(std::ostream& o) const
{
	if (!parameter)
		o << "no_value";
	if (parameter->value().isEnum()) {
		RichEnum* ren = dynamic_cast<RichEnum*>(parameter);
		o << "'" << ren->enumvalues.at(ren->value().getEnum()).toStdString() << "'";
		return;
	}
	if (parameter->value().isAbsPerc()) {
		RichAbsPerc* rabs = dynamic_cast<RichAbsPerc*>(parameter);
		float abs = parameter->value().getAbsPerc();
		float perc = (abs - rabs->min) / (rabs->max - rabs->min) * 100;
		o << perc << "%";
		return;
	}
	if (parameter->value().isDynamicFloat()) {
		RichDynamicFloat* rdyn = dynamic_cast<RichDynamicFloat*>(parameter);
		o << parameter->value().getDynamicFloat() <<
			 " [min: " << rdyn->min << "; max: " << rdyn->max << "]";
		return;
	}
	if (parameter->value().isBool()) {
		o << (parameter->value().getBool() ? "True" : "False");
		return;
	}
	if (parameter->value().isInt()) {
		o << parameter->value().getInt();
		return;
	}
	if (parameter->value().isFloat()){
		o << parameter->value().getFloat();
		return;
	}
	if (parameter->value().isString()){
		o << "'" << parameter->value().getString().toStdString() << "'";
		return;
	}
	if (parameter->value().isMatrix44f()){
		const MESHLAB_SCALAR* v = parameter->value().getMatrix44f().V();
		o << "[[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "],"
			<< "[" << v[4] << ", " << v[5] << ", " << v[6] << ", " << v[7] << "],"
			<< "[" << v[8] << ", " << v[9] << ", " << v[10] << ", " << v[11] << "],"
			<< "[" << v[12] << ", " << v[13] << ", " << v[14] << ", " << v[15] << "]]";
		return;
	}
	if (parameter->value().isPoint3f()) {
		o << "[" << parameter->value().getPoint3f().X() << ", "
			<< parameter->value().getPoint3f().Y() << ", "
			<< parameter->value().getPoint3f().Z() << "]";
		return;
	}
	if (parameter->value().isShotf()) {
		o << "None";
		return;
	}
	if (parameter->value().isColor()) {
		QColor c = parameter->value().getColor();
		o <<
			"[" + std::to_string(c.red()) + "; " +
			std::to_string(c.green()) + "; " +
			std::to_string(c.blue()) + "; " +
			std::to_string(c.alpha()) + "]";
		return;
	}
	if (parameter->value().isMesh()){
		const RichMesh* rm = dynamic_cast<const RichMesh*>(parameter);
		o << rm->meshindex;
		return;
	}
	if (parameter->value().isFileName()){
		o << "'" << parameter->value().getFileName().toStdString() << "'";
		return;
	}

	//to support: dynamicfloat, filename, mesh
}

QString pymeshlab::FunctionParameter::defaultValueString() const
{
	std::stringstream ss;
	printDefaultValue(ss);
	return QString::fromStdString(ss.str());
}

pymeshlab::FunctionParameter& pymeshlab::FunctionParameter::operator=(pymeshlab::FunctionParameter oth)
{
	this->swap(oth);
	return *this;
}

bool pymeshlab::FunctionParameter::operator<(const pymeshlab::FunctionParameter& oth) const
{
	return pName < oth.pName;
}

bool pymeshlab::FunctionParameter::operator==(const pymeshlab::FunctionParameter& oth) const
{
	return pName == oth.pName;
}

void pymeshlab::FunctionParameter::swap(pymeshlab::FunctionParameter& oth)
{
	std::swap(pName, oth.pName);
	std::swap(parameter, oth.parameter);
}

QString pymeshlab::FunctionParameter::computePythonTypeString(const RichParameter& par)
{
	const Value& v = par.value();
	if (v.isEnum())
		return PYTHON_TYPE_ENUM;
	if (v.isAbsPerc())
		return PYTHON_TYPE_ABSPERC;
	if (v.isDynamicFloat())
		return PYTHON_TYPE_DYNAMIC_FLOAT;
	if (v.isBool())
		return PYTHON_TYPE_BOOL;
	if (v.isInt())
		return PYTHON_TYPE_INT;
	if (v.isFloat())
		return PYTHON_TYPE_FLOAT;
	if (v.isString())
		return PYTHON_TYPE_STRING;
	if (v.isMatrix44f())
		return PYTHON_TYPE_MATRIX44F;
	if (v.isPoint3f())
		return PYTHON_TYPE_POINT3F;
	if (v.isShotf())
		return PYTHON_TYPE_SHOTF;
	if (v.isColor())
		return PYTHON_TYPE_COLOR;
	if (v.isMesh())
		return PYTHON_TYPE_MESH;
	if (v.isFileName())
		return PYTHON_TYPE_FILENAME;
	return "still_unsupported";
}
