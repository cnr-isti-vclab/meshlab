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

#ifndef EditAlignPLUGIN_H
#define EditAlignPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "align/AlignPair.h" 
#include "align/OccupancyGrid.h" 
#include "meshtree.h"
#include <wrap/gui/trackball.h>
#include "alignDialog.h"

class EditAlignPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
  QList <QAction *> actionList;
	
	enum 
	{
		ALIGN_IDLE = 0x01,
		ALIGN_INSPECT_ARC = 0x02,
		ALIGN_MOVE = 0x03
	};
		
	void Pick( MeshModel &m, GLArea * gla);

public:
    EditAlignPlugin();
		virtual ~EditAlignPlugin() {
			mode=ALIGN_IDLE;
		}

    virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();

    virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		virtual QList<QAction *> actions() const ;

		QFont qFont;
  	int mode;			

    AlignDialog *alignDialog; //the widget with the list of the meshes. 
		
		void toggleButtons();

		Trackball trackball;
		
		MeshDocument *md; 
		MeshNode *currentNode() {return meshTree.find(md->mm());}
		MeshTree meshTree;
		GLArea * gla;

public:
	AlignPair::Param ap;  // default alignment parameters
	

	void retrieveParameterSet(FilterParameterSet &fps , AlignPair::Param &app);
	void buildParameterSet(FilterParameterSet &fps , AlignPair::Param &app);

public slots:
		void process();
		void recalcCurrentArc();
		void glueHere();
		void glueHereAll();
		void glueManual();
		void glueByPicking();
		void alignParam();
		
		void DrawArc( AlignPair::Result *A );
signals:
	void suspendEditToggle();

};

#endif
