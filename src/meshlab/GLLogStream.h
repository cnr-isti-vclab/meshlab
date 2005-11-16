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
#include <string>
#include <utility>
#include "LogStream.h"

class GLLogStream : public LogStream
{
public:
  void glDraw(QGLWidget *qgl, int Level, int nlines);
  void Save(int Level, const char *filename);
  void Clear() {S.clear();}
	void Log(int Level, const char * f, ... );
 
private:
  std::list<std::pair<int,std::string> > S;
};

#endif //GLLOGSTREAM_H
