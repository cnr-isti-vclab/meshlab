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
#include "alignDialog.h" 
#include "align/AlignPair.h" 
#include "align/OccupancyGrid.h" 
#include <wrap/gui/trackball.h>

class MeshNode
{
public:
	bool glued;
	Matrix44d Tr;
  MeshModel *m;
};

class MeshTree
{
	public:
  QList<MeshNode> NL;
	vcg::OccupancyGrid OG;
	vector<AlignPair::Result> ResVec(/* OG.SVA.size()*/ );
	vector<AlignPair::Result *> ResVecPtr;

	void Process();
};


class EditAlignPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
  QList <QAction *> actionList;
	
	enum 
	{
		ALIGN_IDLE = 0x01,
		ALIGN_PICK = 0x02,
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
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/){};
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) ;
    virtual void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		virtual QList<QAction *> actions() const ;

    QPoint cur;
		QFont qFont;
    bool haveToPick;
		CMeshO::FacePointer curFacePtr;
  	int mode;			

    AlignDialog *alignDialog; //the widget with the list of 
		void updateButtons();

		Trackball trackball;
		
		MeshDocument *md; 
		GLArea * gla;
		inline MeshModel* MM(int id)
			{
				return md->meshList.at(id);
			}

public:
// The Alignment Data;
	OccupancyGrid OG;
	AlignPair::Param ap;
	vector<AlignPair::Result> ResVec;
	vector<AlignPair::Result *> ResVecPtr;
		

public slots:

	  //void Align();
		void Process();
		void GlueHere();
		void GlueManual();
		void EditAlignPlugin::DrawArc(/* ArcPtr a, bool relative, const Point3d &Center, double Size*/);

};

#endif
