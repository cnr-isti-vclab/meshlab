#ifndef FILTERSCRIPT_H
#define FILTERSCRIPT_H

#include <QAction>
#include <QList>

#include "meshmodel.h"
#include "filterparameter.h"
class QDomElement;
/*
The filterscipt class abstract the concept of history of processing.
It is simply a list of all the performed actions
Each action is a pair <filtername,parameters>
*/

class FilterScript 
{
public:
  bool open(QString filename);
  bool save(QString filename);
  vcg::Matrix44f getMatrix(QDomElement *n);
  
  QList< QPair< QString , FilterParameterSet> > actionList;
  typedef QList< QPair<QString, FilterParameterSet> >::iterator iterator;

private:
	//constant strings to make future additions to this class less error prone
	static const QString Type;
	static const QString Value;
	static const QString Item;
	
	//constants for the type names
	static const QString FloatList;
  
};

#endif
