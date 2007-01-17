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

#ifndef EDITPAINT_H
#define EDITPAINT_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>
#include <QHash>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/math/matrix44.h>
#include "ui_painttoolbox.h"
#include "colorwid.h"

class EditPaintPlugin;
class PaintToolbox;
class Penn;

typedef enum {Pen, Fill, Pick} PaintThing;

class Penn {
public:
	QPoint width;
	QPoint pos;
};

class EditPaintPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	QList <QAction *> actionList;

public:
	EditPaintPlugin();
	
	virtual ~EditPaintPlugin();
	
	virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();
	
	virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	//	  virtual void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );
	virtual QList<QAction *> actions() const ;
	QPoint start;
	QPoint cur;
	QPoint prev;
	bool isDragging;
	vector<CMeshO::FacePointer> LastSel;

private:
	typedef enum {SMAdd, SMClear,SMSub} SelMode;
	SelMode selMode;

	vector<CMeshO::FacePointer> curSel;
	QHash<CVertexO *,Color4b> temporaneo;
	Penn pen;
	//QHash <long,face::Color4bOcf> dipendendo;
	QHash <long,CFaceO *> painting_at;
	
	PaintToolbox* paintbox;
	Qt::MouseButton curr_mouse;

	int paintType();
	void DrawXORRect(GLArea * gla, bool doubleDraw);
	void getInternFaces(vector<CMeshO::FacePointer> *actual,vector<CMeshO::VertexPointer> * risult, GLArea * gla);
	int isIn(QPointF p0,QPointF p1,float dx,float dy,float raduis);
};

class PaintToolbox : public QWidget {
Q_OBJECT
public:
	float diag;
	PaintToolbox ( /*const QString & title,*/ QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	Color4b getColor(Qt::MouseButton);
	double getRadius() { return ui.pen_radius->value(); }
	int paintType() { if (ui.pen_type->currentText()=="pixel") return 1; return 2; }
	int getOpacity() { return ui.deck_slider->value(); }
private:
	Ui::PaintToolbox ui;
private slots:
	void on_pen_type_currentIndexChanged(QString value);
	void on_pen_radius_valueChanged(double value);
	void on_switch_me_clicked();
	void on_set_bw_clicked();
	void on_deck_slider_valueChanged(int value);
	void on_deck_box_valueChanged(int value);
};

#endif
