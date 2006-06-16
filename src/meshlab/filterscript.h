#ifndef FILTERSCRIPT_H
#define FILTERSCRIPT_H

#include <QAction>
#include <QList>

#include "meshmodel.h"
#include "filterparameter.h"
//#include "glarea.h"

class FilterScript 
{
public:
  bool open(QString filename);
  bool save(QString filename);
  
  QList< QPair< QString , FilterParameter> > actionList;
  typedef QList< QPair<QString, FilterParameter> >::iterator iterator;
};

#endif
