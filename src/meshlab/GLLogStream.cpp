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
/****************************************************************************
  History
$Log$
Revision 1.3  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include <qfont.h>
#include <stdio.h>

#include "GLLogStream.h"

using namespace std;
void GLLogStream::Log(int Level, const char * f, ... )
{
	char buf[4096];
	va_list marker;
	va_start( marker, f );     

	vsprintf(buf,f,marker);
	va_end( marker );              

	S.push_back(make_pair<int,string>(Level,buf));
}

void GLLogStream::Save(int Level, const char * filename )
{
	FILE *fp=fopen(filename,"wb");
	list<pair <int,string> > ::iterator li;
	for(li=S.begin();li!=S.end();++li)
		fprintf(fp,(*li).second.c_str());
}


void  GLLogStream::glDraw(QGLWidget *qgl, int Level, int nlines)
{
	static QFont qf("Helvetica",8);
	const int LineWidth=15;

	list<pair <int,string> > ::iterator li;
	li=S.end();

	advance(li,-nlines); 

	if(li==S.end())
		li=S.begin();

	int StartLine = qgl->height() - (nlines+1) * LineWidth;

	for(;li!=S.end();++li)
	{
		qgl->renderText(20,StartLine,(*li).second.c_str(),qf);

		StartLine+=LineWidth;
	}
}
