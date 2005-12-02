/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
/****************************************************************************
  History

 $Log$
 Revision 1.8  2005/12/02 17:41:33  fmazzant
 added support obj dialog exporter

 Revision 1.7  2005/11/30 16:26:56  cignoni
 All the modification, restructuring seen during the 30/12 lesson...

 Revision 1.6  2005/11/30 01:06:59  fmazzant
 added support Export OBJ(base)
 added comment line history
 deleted bug-fix in meshio.pro in unix{ ... }

   

 *****************************************************************************/
#ifndef EXTRAIOPLUGIN_H
#define EXTRAIOPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class ExtraMeshIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
public:
  QStringList formats() const;

	 bool open(
      const QString &format,
			QString &fileName,
      MeshModel &m, 
      int& mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent=0);
    
   bool save(
      const QString &format,
			QString &fileName,
      MeshModel &m, 
      int mask,
      vcg::CallBackPos *cb=0,
      QWidget *parent= 0);
};

#endif
