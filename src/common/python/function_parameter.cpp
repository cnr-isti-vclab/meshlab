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

pymeshlab::FunctionParameter::FunctionParameter(const RichParameter& parameter) :
	parameter(parameter.clone())
{
}

pymeshlab::FunctionParameter::FunctionParameter(
		const pymeshlab::FunctionParameter& oth):
	parameter(oth.parameter->clone())
{
}

pymeshlab::FunctionParameter::FunctionParameter(pymeshlab::FunctionParameter&& oth)
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
	return parameter->pythonName();
}

QString pymeshlab::FunctionParameter::meshlabName() const
{
	return parameter->name();
}

QString pymeshlab::FunctionParameter::pythonTypeString() const
{
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
	if (parameter->isOfType<RichAbsPerc>()) {
		RichAbsPerc* rabs = dynamic_cast<RichAbsPerc*>(parameter);
		float abs = parameter->value().getFloat();
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
		o << rm->value().getMeshId();
		return;
	}
	if (parameter->value().isFileName()){
		o << "'" << parameter->value().getFileName().toStdString() << "'";
		return;
	}
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
	return parameter->pythonName() < oth.parameter->pythonName();
}

bool pymeshlab::FunctionParameter::operator==(const pymeshlab::FunctionParameter& oth) const
{
	return parameter->pythonName() == oth.parameter->pythonName();
}

void pymeshlab::FunctionParameter::swap(pymeshlab::FunctionParameter& oth)
{
	std::swap(parameter, oth.parameter);
}
