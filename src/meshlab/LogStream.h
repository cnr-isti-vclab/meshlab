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
Revision 1.1  2005/11/16 23:19:22  cignoni
Initial Draft release; still to be adapted to our needs.

Revision 1.2  2005/07/22 11:16:25  fiorin
out-of-core
volume partition

Revision 1.1  2005/04/01 15:43:42  fiorin
Initial commit

Revision 1.7  2005/03/31 12:03:04  fiorin
*** empty log message ***

Revision 1.6  2005/03/18 13:45:32  fiorin
*** empty log message ***

Revision 1.5  2005/03/16 22:36:45  fiorin
*** empty log message ***

Revision 1.4  2005/03/16 17:19:24  fiorin
*** empty log message ***

Revision 1.3  2005/03/15 20:23:02  fiorin
*** empty log message ***

Revision 1.2  2005/02/22 16:28:48  fiorin
*** empty log message ***

Revision 1.1  2004/07/12 14:02:14  cignoni
Initial Commit


****************************************************************************/

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#ifdef _WINDOWS
#include <GL/glew.h>
#include <qgl.h> 
#endif //_WINDOWS


class LogStream 
{
public:
  typedef enum {Error=0, Warning=1, Info=2, Debug=3, Direct=4, OnlyFileLog=5, OnlyConsole=6} Level ;
  
	virtual void Log(int Level, const char * f, ... ) = 0;

#ifdef _WINDOWS
	virtual void glDraw(QGLWidget *qgl, int Level, int nlines) = 0;
#endif
};

#endif
