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
/****************************************************************************
History
$Log$
Revision 1.2  2007/12/03 10:53:04  corsini
code restyling


****************************************************************************/
#include "RmPass.h"

UniformVar RmPass::searchFragmentUniformVariable(QString &name)
{
	return searchUniformVariable(name, FRAGMENT);
}

UniformVar RmPass::searchVertexUniformVariable(QString &name)
{
	return searchUniformVariable(name, VERTEX);
}

UniformVar RmPass::searchUniformVariable(QString &name, CodeType codetype)
{
	QString &source = (codetype == FRAGMENT) ? fragment : vertex;

	int namelen = name.length();
	int start = 0;
	int pos_name, pos_unif;

	while( (pos_name = source.indexOf(name, start)) != -1) {
		start += namelen;
		if ((pos_unif = source.lastIndexOf("uniform", pos_name)) == -1)
			continue;

		QString declaration =
			source.mid(pos_unif, pos_name - pos_unif + namelen + 1);
		QStringList list = declaration.split(QRegExp("\\s+"));

		if (list.size() != 3 ||
		    list[0] != "uniform" ||
		    (list[2] != name && list[2] != name + QString(";")))
			continue;

		return UniformVar(name, list[1],
		                  UniformVar::getTypeFromString(list[1]));
	}
	return UniformVar();
}


