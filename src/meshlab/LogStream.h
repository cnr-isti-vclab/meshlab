/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004																								 \/)\/    *
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
Revision 1.5  2006/02/13 14:18:16  cignoni
cleaned up

Revision 1.4  2006/01/25 15:37:15  glvertex
Added variable vertical spacing

Revision 1.3  2006/01/17 16:35:27  glvertex
Added Scalable fonts

Revision 1.2  2005/11/24 10:35:05  mariolatronico
removed _WINDOWS define and changed <qgl.h> in <QGLWidget> to correctly compile in non windows platforms

Revision 1.1  2005/11/16 23:19:22  cignoni
Initial Draft release; still to be adapted to our needs.
****************************************************************************/

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <QGLWidget> 
#include <QFont>


class LogStream 
{
public:
  typedef enum {Error=0, Warning=1, Info=2, Debug=3, Direct=4, OnlyFileLog=5, OnlyConsole=6} Level ;
	virtual void Log(int Level, const char * f, ... ) = 0;
	virtual void glDraw(QGLWidget *qgl, int Level, int nlines,float vSpacing,QFont font) = 0;

};

#endif
