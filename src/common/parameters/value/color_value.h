/*****************************************************************************
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

#ifndef MESHLAB_COLOR_VALUE_H
#define MESHLAB_COLOR_VALUE_H

#include "value.h"

class ColorValue : public Value
{
public:
	ColorValue(QColor val);
	~ColorValue() {}

	QColor      getColor() const;
	bool        isColor() const;
	QString     typeName() const;
	void        set(const Value& p);
	ColorValue* clone() const;
	bool        operator==(const Value& p) const;
	void        fillToXMLElement(QDomElement& element) const;

private:
	QColor pval;
};

#endif // MESHLAB_COLOR_VALUE_H
