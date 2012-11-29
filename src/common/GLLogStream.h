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

#ifndef GLLOGSTREAM_H
#define GLLOGSTREAM_H

#include <list>
#include <utility>
#include <QMultiMap>
#include <QPair>
#include <QString>
/**
  This is the logging class.
  One for each document. Responsible of getting an history of the logging message printed out by filters.
  */
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
	
   GLLogStream ();
   ~GLLogStream (){}
  void print(QStringList &list);		// Fills a QStringList with the log entries
  void Save(int Level, const char *filename);
  void Clear() 
  {
	  S.clear();
  }
	void Logf(int Level, const char * f, ... );
  void Log(int Level, const char * buf )
	{
		QString tmp(buf);
		S.push_back(std::make_pair<int,QString>(Level,tmp));
    qDebug("LOG: %i %s",Level,buf);
	}

  void SetBookmark();
  void ClearBookmark();
  void BackToBookmark();

//private:
  QList<std::pair<int,QString> > S;

  // The list of strings used in realtime display of info over the mesh.
  // Each box is identified by the title, name of the mesh and text.
  // the name of the mesh is shown only if two or more box with the same title are shown.
  QMultiMap<QString,QPair<QString,QString> > RealTimeLogText;

  void RealTimeLogf(const QString& Id, const QString &meshName, const char * f, ... );
  void RealTimeLog(const QString& Id, const QString &meshName,const QString& text);


private:
  int bookmark; /// this field is used to place a bookmark for restoring the log. Useful for previeweing

};

#endif //GLLOGSTREAM_H
