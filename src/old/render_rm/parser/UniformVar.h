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
Revision 1.3  2007/12/03 10:46:06  corsini
code restyling


****************************************************************************/
#ifndef __UNIFORMVAR_H__
#define __UNIFORMVAR_H__

// Local headers
#include "GlState.h"

// Qt headers
#include <QList>
#include <QString>
#include <QStringList>
#include <QDomElement>
#include <QDebug>


class UniformVar
{
public:

	enum UniformType {
		INT,
		FLOAT,
		BOOL,
		VEC2, VEC3, VEC4,
		IVEC2, IVEC3, IVEC4,
		BVEC2, BVEC3, BVEC4,
		MAT2, MAT3, MAT4,
		SAMPLER1D, SAMPLER2D, SAMPLER3D, SAMPLERCUBE,
		SAMPLER1DSHADOW, SAMPLER2DSHADOW,
		OTHER,
		NUM_TYPES
	};

	UniformType type;
	QString name;
	QString typeString;

	union {
		int ivalue;
		float fvalue;
		bool bvalue;
		float vec2[2], vec3[3], vec4[4];
		int ivec2[2], ivec3[3], ivec4[4];
		bool bvec2[2], bvec3[3], bvec4[4];
		float mat2[2][2], mat3[3][3], mat4[4][4];
	};

	QString representerTagName;
	QString textureName;
	QString textureFilename;
	QList<GlState> textureGLStates;

	union { int imin; float fmin; };
	union { int imax; float fmax; };
	union { int irealmin; float frealmin; };
	union { int irealmax; float frealmax; };

	bool minSet;
	bool maxSet;
	bool realminSet;
	bool realmaxSet;

	bool valid;

	void setMin(int min);
	void setMin(float min);
	void setMax(int max);
	void setMax(float max);
	void testRealMin(int min);
	void testRealMin(float min);
	void testRealMax(int max);
	void testRealMax(float max);

	UniformVar() { valid = false; }
	UniformVar(QString &_name, QString &_typeString, UniformType _type);
	virtual ~UniformVar() {}

	bool isNull() { return !valid; }


	//* we search the xml tag element that has the default value of a uniform
	//* variable. It can happened a multiple declaration, so first we search
	//* in the same RmOpenGLEffect (effectElement), and then in the global document root
	bool getValueFromXmlDocument(QDomElement &root, bool echoNotFound = true);
	bool getValueFromXmlDocument(QDomElement &root, QDomElement &effectElement)
	{
		if (getValueFromXmlDocument(effectElement, false))
			return true;
		return getValueFromXmlDocument(root);
	}

	bool getUniformKnownButUnimplementedTag(QDomElement &root, QString tag, QString tagname);
	bool getUniformBooleanVectorFromTag(QDomElement &root, QString tag, int vecsize, bool * vec, bool * found = NULL);
	bool getUniformNumberVectorFromTag(QDomElement &root, QString tag, int vecsize, void * vec, bool intOrFloat, bool * found = NULL);
	bool getUniformNumberVectorFromTag(QDomElement &root, QString tag, int vecsize, int * vec, bool * found = NULL)
	{
		return getUniformNumberVectorFromTag(root, tag, vecsize, (void*)vec, true, found);
	}
	bool getUniformNumberVectorFromTag(QDomElement &root, QString tag, int vecsize, float *vec, bool *found = NULL)
	{
		return getUniformNumberVectorFromTag(root, tag, vecsize, (void*)vec, false, found);
	}
	bool getUniformTextureFromTag(QDomElement &root, QString tag, bool *found = NULL);


	static enum UniformType getTypeFromString(QString &type);
	static QString getXmlTagRomUniformType(UniformType type);
	static QString getStringFromUniformType(UniformType type);

	static bool getUniformNumberVectorFromXmlTag(QDomElement &el, int values, void *farr, bool intOrFloat, UniformVar *ptr);
	static bool getUniformBooleanVectorFromXmlTag(QDomElement &el, int values, bool *barr);

	void VarDump(int indent = 0, bool extendedVarDump = false);

private:
	static QString typeList[];
};
#endif /* __UNIFORMVAR_H__ */
