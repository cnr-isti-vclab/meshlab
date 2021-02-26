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
#include "function.h"

#include <QStringList>
#include "../parameters/rich_parameter.h"

pymeshlab::Function::Function()
{
}

pymeshlab::Function::Function(
		const QString pythonFunctionName,
		const QString meshlabFilterName,
		const QString description) :
	pythonFunName(pythonFunctionName), meshlabFunName(meshlabFilterName), funDescription(description)
{
}

void pymeshlab::Function::addParameter(const pymeshlab::FunctionParameter& p)
{
	parameters.push_back(p);
}

QString pymeshlab::Function::pythonFunctionName() const
{
	return pythonFunName;
}

QString pymeshlab::Function::meshlabFunctionName() const
{
	return meshlabFunName;
}

QString pymeshlab::Function::description() const
{
	return funDescription;
}

unsigned int pymeshlab::Function::parametersNumber() const
{
	return parameters.size();
}

QStringList pymeshlab::Function::pythonFunctionParameters() const
{
	QStringList list;
	for (const FunctionParameter& p : parameters)
		list.push_back(p.pythonName());
	return list;
}

bool pymeshlab::Function::contains(const QString& pythonParameter) const
{
	iterator it = std::find_if(parameters.begin(), parameters.end(),
			[&pythonParameter](const FunctionParameter& x)
			{
				return x.pythonName() == pythonParameter;
			});
	return it != parameters.end();
}

const pymeshlab::FunctionParameter& pymeshlab::Function::getFilterFunctionParameter(
		const QString& pythonParameter) const
{
	iterator it = std::find_if(parameters.begin(), parameters.end(),
			[&pythonParameter](const FunctionParameter& x)
			{
				return x.pythonName() == pythonParameter;
			});
	return *it;
}

bool pymeshlab::Function::operator<(const pymeshlab::Function& oth) const
{
	return pythonFunName < oth.pythonFunName;
}
