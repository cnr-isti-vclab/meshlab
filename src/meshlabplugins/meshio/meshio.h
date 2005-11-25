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

#ifndef EXTRAIOPLUGIN_H
#define EXTRAIOPLUGIN_H

#include <Qt/QObject>
#include <Qt/QStringList>
#include <Qt/QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class ExtraMeshIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
  QStringList format() const;
  bool open(QString &filter, MeshModel &m, int mask, CallBackPos *cb=0, QWidget *parent=0);  
  // prima istanza il dialogo di opzioni viene sempre.
  bool save(QString &filter, MeshModel &m, int mask, CallBackPos *cb=0, QWidget *parent=0); 
};

#endif
