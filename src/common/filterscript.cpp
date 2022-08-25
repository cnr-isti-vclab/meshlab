/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <algorithm>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QPair>

#include "filterscript.h"

using namespace vcg;


FilterScript::FilterScript() : QObject(), QList< FilterNameParameterValuesPair >()
{
}

FilterScript::~FilterScript()
{
}

QDomDocument FilterScript::xmlDoc() const
{
	QDomDocument doc("FilterScript");
	QDomElement root = doc.createElement("FilterScript");
	doc.appendChild(root);

	for(const FilterNameParameterValuesPair& oldpv : *this) {
		QDomElement tag = doc.createElement("filter");
		const QPair<QString,RichParameterList>& pair = oldpv;
		tag.setAttribute(QString("name"),pair.first);
		const RichParameterList &par=pair.second;
		for(const RichParameter& rp : par) {
			tag.appendChild(rp.fillToXMLDocument(doc));
		}
		root.appendChild(tag);
	}
	return doc;
}

bool FilterScript::save(QString filename) const
{
	QDomDocument doc = xmlDoc();
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream,1);
	file.close();
	return true;
}

bool FilterScript::open(QString filename)
{
	QDomDocument doc;
	clear();
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug("Failure in opening Script %s", qUtf8Printable(filename));
		qDebug("Current dir is %s", qUtf8Printable(QDir::currentPath()));
		return false;
	}
	QString errorMsg; int errorLine,errorColumn;
	if(!doc.setContent(&file,false,&errorMsg,&errorLine,&errorColumn)) {
		qDebug("Failure in setting Content line %i column %i \nError'%s'",errorLine,errorColumn, qUtf8Printable(errorMsg));
		return false;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if(root.nodeName() != "FilterScript") {
		qDebug("Failure in parsing script %s\nNo root node with name FilterScript\n", qUtf8Printable(filename));
		qDebug("Current rootname is %s", qUtf8Printable(root.nodeName()));
		return false;
	}

	qDebug("FilterScript");
	for(QDomElement nf = root.firstChildElement(); !nf.isNull(); nf = nf.nextSiblingElement()) {
		if (nf.tagName() == QString("filter")) {
			RichParameterList par;
			QString name=nf.attribute("name");
			qDebug("Reading filter with name %s", qUtf8Printable(name));
			for(QDomElement np = nf.firstChildElement("Param"); !np.isNull(); np = np.nextSiblingElement("Param")) {
				par.pushFromQDomElement(np);
			}
			FilterNameParameterValuesPair tmp;
			tmp.first = name; tmp.second = par;
			append(tmp);
		}
	}

	return true;
}
