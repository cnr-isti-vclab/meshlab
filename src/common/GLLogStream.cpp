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

#include <QFont>
#include <QGLWidget>

#include <stdio.h>

#include "GLLogStream.h"

using namespace std;
GLLogStream::GLLogStream()
{
  ClearBookmark();
}

void GLLogStream::Logf(int Level, const char * f, ... )
{
	char buf[4096];
	va_list marker;
	va_start( marker, f );     

	vsprintf(buf,f,marker);
	va_end( marker );              
	Log(Level,buf);
}

void GLLogStream::RealTimeLogf(QString Id, const char * f, ... )
{
	char buf[4096];
	va_list marker;
	va_start( marker, f );

	vsprintf(buf,f,marker);
	va_end( marker );
	RealTimeLog(Id,buf);
}

void GLLogStream::RealTimeLog(QString Id, QString text)
{
  this->RealTimeLogText[Id]=text;
}


void GLLogStream::Save(int /*Level*/, const char * filename )
{
	FILE *fp=fopen(filename,"wb");
	QList<pair <int,QString> > ::iterator li;
	for(li=S.begin();li!=S.end();++li)
        fprintf(fp,"%s",qPrintable((*li).second));
}

void GLLogStream::ClearBookmark()
{
  bookmark = -1;
}

void GLLogStream::SetBookmark()
{
  bookmark=S.size();
}

void GLLogStream::BackToBookmark()
{
  if(bookmark<0) return;
  while(S.size() > bookmark )
    S.removeLast();
}
void GLLogStream::print(QStringList &out)
{
  out.clear();
  QList<pair <int,QString> > ::iterator li;
  for(li=S.begin();li!=S.end();++li)
        out.push_back((*li).second);
}

