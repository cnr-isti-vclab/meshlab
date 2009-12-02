/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                      
\/)\/    *
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
Revision 1.6  2007/03/27 12:20:09  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.5  2006/05/25 09:46:37  cignoni
missing std and and all the other gcc detected syntax errors

Revision 1.4  2006/01/25 15:37:15  glvertex
Added variable vertical spacing

Revision 1.3  2006/01/17 16:35:27  glvertex
Added Scalable fonts

Revision 1.2  2005/11/26 18:24:00  glvertex
Added method [print] that writes the log entries ina QStringList

Revision 1.1  2005/11/16 23:19:22  cignoni
Initial Draft release; still to be adapted to our needs.

Revision 1.1  2005/04/01 15:43:42  fiorin
Initial commit

Revision 1.2  2005/03/18 13:45:32  fiorin
*** empty log message ***

Revision 1.1  2005/03/15 20:26:43  fiorin
*** empty log message ***

Revision 1.2  2005/02/22 16:28:48  fiorin
*** empty log message ***

Revision 1.1  2004/07/12 14:02:14  cignoni
Initial Commit


****************************************************************************/

#ifndef GLLOGSTREAM_H
#define GLLOGSTREAM_H

#include <list>
#include <utility>
#include <QFont>

class QGLWidget;
class GLArea;

class GLLogStream 
{
public:
	enum Levels
	{
		SYSTEM = 0,
		WARNING = 1,
		FILTER = 2,
		DEBUG = 3
	};
	
	virtual ~GLLogStream () {}
  void print(QStringList &list);		// Fills a QStringList with the log entries 
  void glDraw(QGLWidget *qgl, int Level, int nlines,float vSpacing,QFont font);
  void Save(int Level, const char *filename);
  void Clear() {S.clear();}
	void Logf(int Level, const char * f, ... );
  void Log(int Level, const char * buf )
	{
		S.push_back(std::make_pair<int,QString>(Level,buf));
        qDebug("LOG: %i %s",Level,buf);
	}

//private:
  QList<std::pair<int,QString> > S;
};

#endif //GLLOGSTREAM_H
