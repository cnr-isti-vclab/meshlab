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

#ifndef EDITPLUGIN_H
#define EDITPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class SelectionFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)
		
		public:
	/* naming convention : 
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { FP_SELECT_ALL, FP_SELECT_NONE, FP_SELECT_INVERT, FP_SELECT_DELETE, FP_SELECT_ERODE, FP_SELECT_DILATE} ;

	SelectionFilterPlugin();
	~SelectionFilterPlugin();
	virtual const ActionInfo &Info(QAction *);
	virtual const PluginInfo &Info();
  
  virtual const QString ST(FilterType filter);
  virtual const FilterClass getClass(QAction *) {return MeshFilterInterface::Selection;};
  virtual bool getParameters(QAction *, QWidget *, MeshModel &m, FilterParameter &par){return true;};
  virtual const int getRequirements(QAction *){return 0;};
  virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameter & /*parent*/, vcg::CallBackPos * cb) ;

protected:

	ActionInfo *ai;

};

#endif
