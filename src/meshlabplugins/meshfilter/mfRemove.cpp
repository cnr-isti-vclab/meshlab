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

#include <vcg/complex/trimesh/clean.h>

#include "mfRemove.h"

using namespace vcg;

// ****************** Remove Ureferenced Vertexes ******************

QString MFPlugin_RemoveUnref::filterName() const {return name;}

bool MFPlugin_RemoveUnref::apply(MeshModel &m, QWidget *parent) 
{
	int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
  return true;
}
