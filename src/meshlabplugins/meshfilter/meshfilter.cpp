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

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>
using namespace vcg;

QStringList ExtraMeshFilterPlugin::filters() const
{ 
    return QStringList() << tr("Unify Vertexes")
                         << tr("Remove Unreferenced Vertexes")
                         ;
}

bool ExtraMeshFilterPlugin::applyFilter(const QString &filter, MeshModel &m, QWidget *parent) 
{
       QMessageBox::information(parent, tr("Filter Plugins"),
                                     tr("Trying to apply filter 2'%1.'").arg(filter));

       if(filter == tr("Remove Unreferenced Vertexes")){
         int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
         QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
       }
  return true;
}

Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
