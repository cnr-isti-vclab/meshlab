

#include "ocmebox.h"

OcmeBox::OcmeBox(QWidget * parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
//	setupUi(this);
//
//	stacks = new QUndoGroup(this);
//
//	QIcon undo = undo_button->icon();
//	undo_button->setDefaultAction(stacks->createUndoAction(undo_button));
//	undo_button->defaultAction()->setIcon(undo);
//
//	QIcon redo = redo_button->icon();
//	redo_button->setDefaultAction(stacks->createRedoAction(redo_button));
//	redo_button->defaultAction()->setIcon(redo);
//
//	setUndoStack(parent);
//
//	active = COLOR_PAINT;
//	gradient_frame->setHidden(true);
//	pick_frame->setHidden(true);
//	smooth_frame->setHidden(true);
//	mesh_displacement_frame->setHidden(true);
//	clone_source_frame->setHidden(true);
//	noise_frame->setHidden(true);
//
//	brush_viewer->setScene(new QGraphicsScene());
//
//	clone_source_view->setScene(new QGraphicsScene());
//	clone_source_view->centerOn(0, 0);
//
//	item = NULL;
//	pixmap_available = false;
//
//	//******QT 4.3 Workaround**********
//	QScrollArea * scrollArea = new QScrollArea(this);
//	gridLayout->removeWidget(widget);
//	scrollArea->setWidget(widget);
//	static_cast<QGridLayout * >(widget->layout())->addItem(new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), 11, 0, 1, 2);
//	widget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));
//	scrollArea->setFrameStyle(QFrame::NoFrame);
//	scrollArea->setWidgetResizable(true);
//	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	scrollArea->adjustSize();
//	gridLayout->addWidget(scrollArea, 2, 1, 1, 1);
//	//**********************************
//
//	QObject::connect(clone_source_view, SIGNAL(positionChanged(double, double)), this, SLOT(movePixmapDelta(double, double)));
//
//	refreshBrushPreview();
}
