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

#include "rich_parameters.h"

bool RichParameterAdapter::create( const QDomElement& np,RichParameter*& par )
{
	QString name=np.attribute("name");
	QString type=np.attribute("type");
	QString desc=np.attribute("description");
	QString tooltip=np.attribute("tooltip");

		 // qDebug("    Reading Param with name %s : %s", qUtf8Printable(name), qUtf8Printable(type));

	bool corrconv = false;
	if(type=="RichBool") {
		QString val = np.attribute("value").toLower();
		if ((val != QString("true")) && (val != QString("false")))
			return false;
		par = new RichBool(name,np.attribute("value")!=QString("false"),desc,tooltip);
		return true;
	}

	if(type=="RichInt") {
		int val = np.attribute("value").toInt(&corrconv);
		if (!corrconv)
			return false;
		par = new RichInt(name,val,desc,tooltip);
		return true;
	}

	if(type=="RichFloat") {
		float val = np.attribute("value").toFloat(&corrconv);
		if (!corrconv)
			return false;
		par = new RichFloat(name,val,desc,tooltip);
		return true;
	}

	if(type=="RichString") {
		par = new RichString(name,np.attribute("value"),desc,tooltip);
		return true;
	}

	if(type=="RichAbsPerc") {
		float val = np.attribute("value").toFloat(&corrconv);
		if ((!corrconv) && (val >= 0.0f) && (val <= 100.0f))
			return false;
		float min = np.attribute("min").toFloat(&corrconv);
		if (!corrconv)
			return false;
		float max = np.attribute("max").toFloat(&corrconv);
		if (!corrconv)
			return false;
		par = new RichPercentage(name,val,min,max,desc,tooltip);
		return true;
	}

	if(type=="RichColor") {
		unsigned int r = np.attribute("r").toUInt(&corrconv);
		if ((!corrconv) && (r <= 255))
			return false;
		unsigned int g = np.attribute("g").toUInt(&corrconv);
		if ((!corrconv) && (g <= 255))
			return false;
		unsigned int b = np.attribute("b").toUInt(&corrconv);
		if ((!corrconv) && (b <= 255))
			return false;
		unsigned int a = np.attribute("a").toUInt(&corrconv);
		if ((!corrconv) && (a <= 255))
			return false;
		QColor col(r,g,b,a);
		par= new RichColor(name,col,desc,tooltip);
		return true;
	}

	if(type=="RichMatrix44f") {
		Matrix44m mm;
		for(int i=0;i<16;++i)
		{
			Scalarm val = np.attribute(QString("val")+QString::number(i)).toFloat(&corrconv);
			if (!corrconv)
				return false;
			mm.V()[i]=val;
		}
		par = new RichMatrix44(name,mm,desc,tooltip);
		return true;
	}

	if(type=="RichEnum") {
		QStringList list;
		int enum_card = np.attribute(QString("enum_cardinality")).toUInt(&corrconv);
		if (!corrconv)
			return false;

		for(int i=0;i<enum_card;++i)
			list<<np.attribute(QString("enum_val")+QString::number(i));

		int val = np.attribute("value").toInt(&corrconv);
		if ((!corrconv) && (val >=0) && (val < enum_card))
			return false;
		par = new RichEnum(name,val,list,desc,tooltip);
		return true;
	}

	if(type == "RichMesh") {
		int val = np.attribute("value").toInt(&corrconv);

		if (!corrconv)
			return false;

		par = new RichMesh(name, val, nullptr, desc,tooltip);
		return true;
	}

	if(type == "RichDynamicFloat") {
		float min = np.attribute("min").toFloat(&corrconv);
		if (!corrconv)
			return false;
		float max = np.attribute("max").toFloat(&corrconv);
		if (!corrconv)
			return false;

		float val = np.attribute("value").toFloat(&corrconv);
		if ((!corrconv) && (val >= min) && (val <= max))
			return false;

		par = new RichDynamicFloat(name, val, min, max, desc, tooltip);
		return true;
	}

	if(type == "RichOpenFile") {
		QStringList list;
		int exts_card = np.attribute(QString("exts_cardinality")).toUInt(&corrconv);
		if (!corrconv)
			return false;

		for(int i=0;i<exts_card;++i)
			list<<np.attribute(QString("exts_val")+QString::number(i));

		QString defdir = np.attribute("value");

		par = new RichFileOpen(name,defdir,list,desc,tooltip);
		return true;
	}

	if(type == "RichSaveFile") {
		QString deffile = np.attribute("value");
		QString ext = np.attribute("ext");
		par = new RichFileSave(name,deffile,ext,desc,tooltip);
		return true;
	}

	if(type=="RichPoint3f") { // for backward compatibility
		vcg::Point3f val;
		val[0]=np.attribute("x").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[1]=np.attribute("y").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[2]=np.attribute("z").toFloat(&corrconv);
		if (!corrconv)
			return false;

		par = new RichPosition(name, val,desc,tooltip);
		return true;
	}
	if(type=="RichPosition") {
		vcg::Point3f val;
		val[0]=np.attribute("x").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[1]=np.attribute("y").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[2]=np.attribute("z").toFloat(&corrconv);
		if (!corrconv)
			return false;

		par = new RichPosition(name, val,desc,tooltip);
		return true;
	}
	if(type=="RichDirection") {
		vcg::Point3f val;
		val[0]=np.attribute("x").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[1]=np.attribute("y").toFloat(&corrconv);
		if (!corrconv)
			return false;
		val[2]=np.attribute("z").toFloat(&corrconv);
		if (!corrconv)
			return false;

		par = new RichDirection(name, val,desc,tooltip);
		return true;
	}
	if(type=="RichShotf") {
		Shotm val;
		assert(0); //TODO!!!!
		par = new RichShot(name, val,desc,tooltip);
		return true;
	}

	return false;
}
