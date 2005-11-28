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

#include "meshrender.h"

using namespace vcg;


QStringList ExtraMeshRenderPlugin::modes() const
{ 
	return QStringList() << tr("Show Normals");
}
void ExtraMeshRenderPlugin::Render(const QString &mode, MeshModel &m, RenderMode &rm, QWidget *parent) 
{
	if(mode == tr("Show Normals"))
	{
    float LineLen = m.cm.bbox.Diag()/20.0;
    CMeshO::VertexIterator vi;
    glPushAttrib(GL_ENABLE_BIT );
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
    {
      glVertex((*vi).P());
      glVertex((*vi).P()+(*vi).N()*LineLen);
    }
    glEnd();
    glPopAttrib();
	}
}

Q_EXPORT_PLUGIN(ExtraMeshRenderPlugin)
