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

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "refinedialog.h"
#include "decimatordialog.h"
#include "transformdialog.h"

#include "../../meshlab/GLLogStream.h"

class ExtraMeshFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)
		
		public:
	/* naming convention : 
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum FilterType { FP_LOOP_SS, FP_BUTTERFLY_SS, FP_REMOVE_UNREFERENCED_VERTEX, 
										FP_REMOVE_DUPLICATED_VERTEX, FP_REMOVE_NULL_FACES, 
										FP_LAPLACIAN_SMOOTH, FP_DECIMATOR, FP_MIDPOINT, FP_REORIENT ,FP_INVERT_FACES,
										FP_TRANSFORM	} ;
	const QString ST(FilterType filter);


	ExtraMeshFilterPlugin();
	~ExtraMeshFilterPlugin();
	virtual const ActionInfo &Info(QAction *);
	virtual const PluginInfo &Info();
	
	virtual QList<QAction *> actions() const;
	bool applyFilter(QAction *filter, MeshModel &m, QWidget *parent, vcg::CallBackPos * cb) ;
	void setLog(GLLogStream *log) { this->log = log ; }
protected:
	GLLogStream *log;
	ActionInfo *ai;
	QList <QAction *> actionList;
	RefineDialog *refineDialog;
	DecimatorDialog *decimatorDialog;
	TransformDialog *transformDialog;
};

#endif
