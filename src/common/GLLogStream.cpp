/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#include <stdio.h>
#include <stdarg.h>
#include <QStringList>

#include "GLLogStream.h"

using namespace std;
GLLogStream::GLLogStream() :
	QObject(), bookmark(-1)
{
}

void GLLogStream::realTimeLog(const QString& Id, const QString &meshName, const QString& text)
{
	this->realTimeLogText.insert(Id,qMakePair(meshName,text) );
}


void GLLogStream::save(int /*Level*/, const char * filename )
{
	FILE *fp=fopen(filename,"wb");
	QList<pair <int,QString> > ::iterator li;
	for(li=logTextList.begin();li!=logTextList.end();++li)
		fprintf(fp,"%s", qUtf8Printable((*li).second));
}

void GLLogStream::clearBookmark()
{
	bookmark = -1;
}

void GLLogStream::setBookmark()
{
	bookmark=logTextList.size();
}

void GLLogStream::backToBookmark()
{
	if(bookmark<0) return;
	while(logTextList.size() > bookmark )
		logTextList.removeLast();
}

const QList<std::pair<int, QString> >& GLLogStream::logStringList() const
{
	return logTextList;
}

const QMultiMap<QString, QPair<QString, QString> >& GLLogStream::realTimeLogMultiMap() const
{
	return realTimeLogText;
}

void GLLogStream::clearRealTimeLog()
{
	realTimeLogText.clear();
}

void GLLogStream::print(QStringList &out) const
{
	out.clear();
	for (const pair <int,QString>& p : logTextList)
		out.push_back(p.second);
}

void GLLogStream::clear()
{
	logTextList.clear();
}

void GLLogStream::log(int Level, const char * buf )
{
	QString tmp(buf);
	logTextList.push_back(std::make_pair(Level,tmp));
	qDebug("LOG: %i %s",Level,buf);
	emit logUpdated();
}

void GLLogStream::log(int Level, const string& logMessage)
{
	logTextList.push_back(std::make_pair(Level, QString::fromStdString(logMessage)));
	qDebug("LOG: %i %s",Level, logMessage.c_str());
	emit logUpdated();
}

void GLLogStream::log(int Level, const QString& logMessage)
{
	logTextList.push_back(std::make_pair(Level, logMessage));
	qDebug("LOG: %i %s",Level, logMessage.toStdString().c_str());
	emit logUpdated();
}

