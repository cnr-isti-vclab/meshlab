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

#ifndef MESHLAB_VALUE_H
#define MESHLAB_VALUE_H

#include "../../ml_document/cmesh.h"
#include <QColor>
#include <QString>
class MeshModel;
class MeshDocument;
class QDomElement;

/**
 * @brief The Value class
 *
 * Represents a generic parameter value for meshlab.
 * Specializations inherit from this class, depending on the type of the
 * value. Value class is an attribute of the RichParameter class.
 */
class Value
{
public:
	virtual ~Value() {}

	virtual bool getBool() const
	{
		assert(0);
		return bool();
	}
	virtual int getInt() const
	{
		assert(0);
		return int();
	}
	virtual Scalarm getFloat() const
	{
		assert(0);
		return Scalarm();
	}
	virtual QString getString() const
	{
		assert(0);
		return QString();
	}
	virtual Matrix44m getMatrix44() const
	{
		assert(0);
		return Matrix44m();
	}
	virtual Point3m getPoint3() const
	{
		assert(0);
		return Point3m();
	}
	virtual Shotm getShot() const
	{
		assert(0);
		return Shotm();
	}
	virtual QColor getColor() const
	{
		assert(0);
		return QColor();
	}

	virtual bool isBool() const { return false; }
	virtual bool isInt() const { return false; }
	virtual bool isFloat() const { return false; }
	virtual bool isString() const { return false; }
	virtual bool isMatrix44() const { return false; }
	virtual bool isPoint3() const { return false; }
	virtual bool isShot() const { return false; }
	virtual bool isColor() const { return false; }

	virtual QString typeName() const                             = 0;
	virtual void    set(const Value& p)                          = 0;
	virtual Value*  clone() const                                = 0;
	virtual bool    operator==(const Value& p) const             = 0;
	virtual void    fillToXMLElement(QDomElement& element) const = 0;

	template <class Val>
	bool isOfType() const
	{
		const Val* t = dynamic_cast<const Val*>(this);
		return (t != nullptr);
	}
};

#endif //MESHLAB_VALUE_H
