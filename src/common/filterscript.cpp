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
#include <QPair>

#include "filterscript.h"

using namespace vcg;


FilterScript::FilterScript() :QObject()
{

}

FilterScript::~FilterScript()
{
    for(FilterScript::iterator it = filtparlist.begin();it != filtparlist.end();++it)
        delete *it;
    filtparlist.clear();
}

QDomDocument FilterScript::xmlDoc()
{
    QDomDocument doc("FilterScript");
    QDomElement root = doc.createElement("FilterScript");
    doc.appendChild(root);

    for(FilterScript::iterator ii=filtparlist.begin();ii!= filtparlist.end();++ii)
    {
        if (!(*ii)->isXMLFilter())
        {
            OldFilterNameParameterValuesPair* oldpv = reinterpret_cast<OldFilterNameParameterValuesPair*>(*ii);
            QDomElement tag = doc.createElement("filter");
            QPair<QString,RichParameterSet>& pair = oldpv->pair;
            tag.setAttribute(QString("name"),pair.first);
            RichParameterSet &par=pair.second;
            QList<RichParameter*>::iterator jj;
            RichParameterXMLVisitor v(doc);
            for(jj=par.paramList.begin();jj!=par.paramList.end();++jj)
            {
                (*jj)->accept(v);
                tag.appendChild(v.parElem);
            }
            root.appendChild(tag);
        }
        else
        {   
            XMLFilterNameParameterValuesPair* xmlpv = reinterpret_cast<XMLFilterNameParameterValuesPair*>(*ii);
            QDomElement tag = doc.createElement("xmlfilter");
            QPair<QString, QMap<QString,QString> >& pair = xmlpv->pair;
            tag.setAttribute(QString("name"),pair.first);
            QMap<QString,QString>& tmpmap = pair.second;
            for(QMap<QString,QString>::const_iterator itm = tmpmap.constBegin();itm != tmpmap.constEnd();++itm)
            {
                QDomElement partag = doc.createElement("xmlparam");
                partag.setAttribute("name",itm.key());
                partag.setAttribute("value",itm.value());
                tag.appendChild(partag);
            }
            root.appendChild(tag);
        }
    }
    return doc;
}

bool FilterScript::save(QString filename)
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
    filtparlist.clear();
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
    for(QDomElement nf = root.firstChildElement(); !nf.isNull(); nf = nf.nextSiblingElement())
    {
        if (nf.tagName() == QString("filter"))
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
            OldFilterNameParameterValuesPair* tmp = new OldFilterNameParameterValuesPair();
            tmp->pair = qMakePair(name,par);
            filtparlist.append(tmp);
        }        
        else
        {
            QString name=nf.attribute("name");
            qDebug("Reading filter with name %s",qPrintable(name));
            QMap<QString,QString> map;
            for(QDomElement np = nf.firstChildElement("xmlparam"); !np.isNull(); np = np.nextSiblingElement("xmlparam"))
                map[np.attribute("name")] = np.attribute("value");
            XMLFilterNameParameterValuesPair* tmp = new XMLFilterNameParameterValuesPair();
            tmp->pair = qMakePair(name,map);
            filtparlist.append(tmp);
        }
    }

    return true;
}

void FilterScript::addExecutedXMLFilter( const QString& name,const QMap<QString,QString>& parvalue )
{
    XMLFilterNameParameterValuesPair* tmp = new XMLFilterNameParameterValuesPair();
    tmp->pair = qMakePair(name,parvalue);
    filtparlist.append(tmp);
}



