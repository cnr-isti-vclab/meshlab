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
Revision 1.2  2006/06/18 20:40:06  cignoni
Completed Open/Save of scripts

Revision 1.1  2006/06/15 21:24:35  cignoni
First ver


****************************************************************************/

#ifndef MESHLAB_FILTERPARAMETER_H
#define MESHLAB_FILTERPARAMETER_H
#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>

/*
The FilterParameter class abstracts the whole set of parameters that are necessary to a given filter
It is a map from string (the name of the parameter) to QVariant (their values)
*/

class FilterParameter
{
public:

  FilterParameter(){}

  inline QString getString(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    assert(ii!=paramMap.end());
    assert(ii.value().type()==QVariant::String);
    return ii.value().toString();
  }

  inline bool getBool(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    assert(ii!=paramMap.end());
    assert(ii.value().type()==QVariant::Bool);
    return ii.value().toBool();
  }
  
  inline int getInt(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::Int);
    return ii.value().toInt();
  }

  inline float getFloat(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::Double);
    return float(ii.value().toDouble());
  }
  
  inline Matrix44f getMatrix44(QString name) { 
    QMap<QString,QVariant>::iterator ii=paramMap.find(name);
    if(ii==paramMap.end()) assert(0);
    assert(ii.value().type()==QVariant::List);
    Matrix44f matrix;
    QList<QVariant> matrixVals = ii.value().toList();
    assert(matrixVals.size()==16);
    for(int i=0;i<16;++i)
      matrix.V()[i]=matrixVals[i].toDouble();
    
    return matrix;
  }

  inline void addFloat(QString name,float val)  { paramMap.insert(name, QVariant( double(val)) ); }
  inline void addInt  (QString name,float val)  { paramMap.insert(name, QVariant(    int(val)) ); }
  inline void addBool (QString name,bool val)   { paramMap.insert(name, QVariant(        val )  );  }
  inline void addString(QString name,QString val){ paramMap.insert(name,QVariant(        val )  );  }
  
  inline void addMatrix44(QString name,Matrix44f val) { 
    QList<QVariant> matrixVals;
    for(int i=0;i<16;++i)
        matrixVals.append(val.V()[i]);
    paramMap.insert(name, QVariant(matrixVals)  );  
  }

  inline void clear() { paramMap.clear(); }


  // The data is just a list of Parameters
  QMap<QString,QVariant> paramMap;  

};

#endif
