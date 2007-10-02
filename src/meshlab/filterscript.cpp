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
#include <QtGui>
#include <algorithm>

// widget capace di fare rendering di un Modello
#include <vcg/space/box3.h>
#include <wrap/gl/space.h>
#include "meshmodel.h"
#include "interfaces.h"
#include "filterscript.h"
//#include "glarea.h"
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QPair>

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
    FilterParameterSet &par=(*ii).second;
    QList<FilterParameter>::iterator jj;
    for(jj=par.paramList.begin();jj!=par.paramList.end();++jj)
    {
      QDomElement parElem = doc.createElement("Param");
      parElem.setAttribute("name",(*jj).fieldName);

      if((*jj).fieldVal.type()==QVariant::Bool) { 
        parElem.setAttribute("type","Bool");
        parElem.setAttribute("value",(*jj).fieldVal.toString());
      }
      if((*jj).fieldVal.type()==QVariant::String) {
        parElem.setAttribute("type","String");
        parElem.setAttribute("value",(*jj).fieldVal.toString());
      }
      if((*jj).fieldVal.type()==QVariant::Int) {
        parElem.setAttribute("type","Int");
        parElem.setAttribute("value",(*jj).fieldVal.toInt());
      }
      if((*jj).fieldVal.type()==QVariant::Double) {
        parElem.setAttribute("type","Float");
        parElem.setAttribute("value",(*jj).fieldVal.toString());
      }

      if((*jj).fieldVal.type()==QVariant::List) {
        parElem.setAttribute("type","Matrix44");
        QList<QVariant> matrixVals = (*jj).fieldVal.toList();
        for(int i=0;i<16;++i)
          parElem.setAttribute(QString("val")+QString::number(i),matrixVals[i].toString());
      }
      tag.appendChild(parElem);
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
	//if(filename.endsWith(".mlx"))
	{
  		QFile file(filename);
			if (file.open(QIODevice::ReadOnly) && doc.setContent(&file)) 
        {
					file.close();
					QDomElement root = doc.documentElement();
					if (root.nodeName() == "FilterScript") 
          {
             qDebug("FilterScript");
              for(QDomElement nf = root.firstChildElement("filter"); !nf.isNull(); nf = nf.nextSiblingElement("filter"))
              {
                  FilterParameterSet par;
                  QString name=nf.attribute("name");
                  qDebug("Reading filter with name %s",qPrintable(name));
                      for(QDomElement np = nf.firstChildElement("Param"); !np.isNull(); np = np.nextSiblingElement("Param"))
                      {
                        QString name=np.attribute("name");
                        QString type=np.attribute("type");

                        qDebug("    Reading Param with name %s : %s",qPrintable(name),qPrintable(type));
                        if(type=="Bool")    par.addBool(name,np.attribute("value")!=QString("false"));
                        if(type=="Int")     par.addInt(name,np.attribute("value").toInt());
                        if(type=="Float")   par.addFloat(name,np.attribute("value").toDouble());
                        if(type=="String")  par.addString(name,np.attribute("value"));
                        if(type=="Matrix44")par.addMatrix44(name,getMatrix(&np));                        
                      }
                   actionList.append(qMakePair(name,par));
             }
          }
        }
    }
  return true;
}

Matrix44f FilterScript::getMatrix(QDomElement *n)
{
  Matrix44f mm;
  for(int i=0;i<16;++i)
    mm.V()[i]=n->attribute(QString("val")+QString::number(i)).toDouble();
  return mm;
}
