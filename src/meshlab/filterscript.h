#ifndef FILTERSCRIPT_H
#define FILTERSCRIPT_H

#include <QAction>
#include <QList>

#include "meshmodel.h"
//#include "glarea.h"

class FilterScript 
{
public:
  bool Open(QString filename);
  bool Save(QString filename);
  
  QList< QPair< QAction *, FilterParameter> > actionList;
  typedef QList< QPair<QAction*, FilterParameter> >::iterator iterator;
};

#endif
