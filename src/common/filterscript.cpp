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
/****************************************************************************
History

$Log$
Revision 1.7  2008/01/22 14:22:24  sherholz
Changed type checking, when saving the script file, from "fieldVal.type() == Variant::" to "fieldType == FilterParameter::"; Now parameter of type AbsPerc, Color or Enum can be saved and loaded.

Revision 1.6  2007/10/02 07:59:42  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.5  2006/06/27 08:07:42  cignoni
Restructured plugins interface for simplifying the server

Revision 1.4  2006/06/18 20:40:06  cignoni
Completed Open/Save of scripts

Revision 1.3  2006/06/16 07:28:21  zifnab1974
changed call to dom.save because gcc didn't like a reference to a variable created inside the function call

Revision 1.2  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.1  2006/06/15 13:05:57  cignoni
added Filter History Dialogs


****************************************************************************/

#include <GL/glew.h>
#include <algorithm>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QPair>

#include "filterscript.h"

using namespace vcg; 
bool FilterScript::save(QString filename)
{

  QDomDocument doc("FilterScript");
  QDomElement root = doc.createElement("FilterScript");
  doc.appendChild(root);

  FilterScript::iterator ii;
  for(ii=actionList.begin();ii!= actionList.end();++ii)
  {
		QDomElement tag = doc.createElement("filter");
    tag.setAttribute(QString("name"),(*ii).first);
    RichParameterSet &par=(*ii).second;
    QList<RichParameter*>::iterator jj;
		RichParameterXMLVisitor v(doc);
    for(jj=par.paramList.begin();jj!=par.paramList.end();++jj)
    {
			(*jj)->accept(v);
      tag.appendChild(v.parElem);
    }
    root.appendChild(tag);
  }
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
	actionList.clear();
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug("Failure in opening Script %s",qPrintable(filename));
		qDebug("Current dir is %s",qPrintable(QDir::currentPath()));
		return false;
	}
	QString errorMsg; int errorLine,errorColumn;
	if(!doc.setContent(&file,false,&errorMsg,&errorLine,&errorColumn))
		{
		qDebug("Failure in setting Content line %i column %i \nError'%s'",errorLine,errorColumn,qPrintable(errorMsg));
		return false;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if(root.nodeName() != "FilterScript") 
	{
		qDebug("Failure in parsing script %s\nNo root node with name FilterScript\n",qPrintable(filename));
		qDebug("Current rootname is %s",qPrintable(root.nodeName()));
		return false;
	}
          
	qDebug("FilterScript");
	for(QDomElement nf = root.firstChildElement("filter"); !nf.isNull(); nf = nf.nextSiblingElement("filter"))
		{
			RichParameterSet par;
			QString name=nf.attribute("name");
			qDebug("Reading filter with name %s",qPrintable(name));
			for(QDomElement np = nf.firstChildElement("Param"); !np.isNull(); np = np.nextSiblingElement("Param"))
					{
						RichParameter* rp = NULL;
						RichParameterFactory::create(np,&rp);
						//FilterParameter::addQDomElement(par,np);
						par.paramList.push_back(rp);
					 }
			 actionList.append(qMakePair(name,par));
		}
          
  return true;
}
