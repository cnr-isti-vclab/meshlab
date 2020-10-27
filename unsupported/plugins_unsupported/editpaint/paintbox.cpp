/***************************************************************************
first version: 0.1 
autor: Gfrei Andreas 
date:  07/02/2007 
email: gfrei.andreas@gmx.net
****************************************************************************/

#include "editpaint.h"
#include <QPainter>
#include <QColorDialog>

using namespace std;
using namespace vcg;

Color4b to4b(QColor c) {
	return Color4b(c.red(),c.green(),c.blue(),255);
}

QColor toQcolor(Color4b c) {
	return QColor(c[0],c[1],c[2]);
}

void ColorWid::setColor(QColor co) {
	bg=co;
	//repaint();
	update();
}

void ColorWid::paintEvent(QPaintEvent * event ) {
	QPainter painter(this);
	painter.fillRect(QRect(0,0,width(),height()),bg);
	int col=bg.red()+bg.green()+bg.blue();
	if (col<150) painter.setPen(Qt::white);
	else painter.setPen(Qt::black);
	painter.drawRect(QRect(1,1,width()-3,height()-3));
}

void ColorWid::mousePressEvent ( QMouseEvent * event ) {
	QColor temp=QColorDialog::getColor(bg);
	if (temp.isValid()) bg=temp;
	update();
}

PaintToolbox::PaintToolbox(/*const QString & title,*/ QWidget * parent, Qt::WindowFlags flags) : QWidget(/*parent*/0,flags/* | Qt::WindowStaysOnTopHint*/) {
	ui.setupUi(this);
	ui.front->setColor(Qt::black);
	ui.back->setColor(Qt::white);
	ui.front->setGeometry(10,10,40,35);
	ui.back->setGeometry(30,25,40,35);
	ui.switch_me->setGeometry(55,10,15,15);
	ui.set_bw->setGeometry(10,45,15,15);
	QPoint p=parent->mapToGlobal(QPoint(0,0));

	ui.label_penmodus->setVisible(false); // since at the moment there is only one modus it is useless
	ui.pen_modus->setVisible(false); // same as above
	//QDesktopWidget::screenGeometry();
	//ui.fill_button->setEnabled(false);
	ui.pick_frame->setVisible(false);
	//ui.advanced_frame->setVisible(false);
	ui.pen_button->setChecked(true);
	ui.undo_button->setEnabled(false);
	ui.redo_button->setEnabled(false);
	ui.gradient_frame->setVisible(false);
	ui.smooth_frame->setVisible(false);
	//ui.select_widget->setVisible(false);
	ui.tabWidget->widget(0)->layout()->setMargin(2);
	ui.tabWidget->widget(0)->layout()->setSpacing(2);
	ui.tabWidget->widget(1)->layout()->setMargin(2);
	ui.tabWidget->widget(1)->layout()->setSpacing(2);
	//####################
	setGeometry(p.x()+parent->width()-width(),p.y(),/*width()*/width(),/*height()*/400);
	ui.select_button->setChecked(true);
	//ui.pen_select_frame->setEnabled(true);
	ui.poly_smooth_frame->setVisible(false);
	paint_utensil[0]=PEN;
	paint_utensil[1]=SELECT;
	//this->setWidget(wi);
}

Color4b PaintToolbox::getColor(Qt::MouseButton mouse) {
	switch (mouse) {
		case Qt::LeftButton: return to4b(ui.front->getColor());
		case Qt::RightButton: return to4b(ui.back->getColor());
		default: return to4b(ui.front->getColor());
	}
}

void PaintToolbox::on_pen_type_currentIndexChanged(QString value) {
	on_pen_radius_valueChanged( ui.pen_radius->value());
}
void PaintToolbox::on_pen_radius_valueChanged(double value) {
	static double oldval=-1;
	if (ui.pen_type->currentText()=="pixel") {
		if ((double)((int)value)!=value) {
			if (oldval<value)
			ui.pen_radius->setValue((double)((int)value)+1);
			else ui.pen_radius->setValue((double)((int)value));
		}
	} else {
	}
	oldval=ui.pen_radius->value();
}

void PaintToolbox::on_switch_me_clicked() {
	QColor temp=ui.front->getColor();
	ui.front->setColor(ui.back->getColor());
	ui.back->setColor(temp);
	//ui.front->update();
	//ui.back->update();
}
void PaintToolbox::on_set_bw_clicked() {
	ui.front->setColor(Qt::black);
	ui.back->setColor(Qt::white);
	//ui.front->update();
	//ui.back->update();
}


void PaintToolbox::on_deck_slider_valueChanged(int value) {
	if (value!=ui.deck_box->value()) ui.deck_box->setValue(value);
}

void PaintToolbox::on_deck_box_valueChanged(int value) {
	if (value!=ui.deck_slider->value()) ui.deck_box->setValue((int)value);
}

void PaintToolbox::on_percentual_slider_valueChanged(int value) {
	if (value!=ui.percentual_box->value()) ui.percentual_box->setValue(value);
}

void PaintToolbox::on_percentual_box_valueChanged(int value) {
	if (value!=ui.percentual_slider->value()) ui.percentual_box->setValue((int)value);
}

void PaintToolbox::on_decrease_slider_valueChanged(int value) {
	if (value!=ui.decrease_box->value()) ui.decrease_box->setValue(value);
}

void PaintToolbox::on_decrease_box_valueChanged(int value) {
	if (value!=ui.decrease_slider->value()) ui.decrease_box->setValue((int)value);
}


void PaintToolbox::on_pen_button_clicked() {
	paint_utensil[0]=PEN;
	ui.pen_frame->setVisible(true);
	ui.pen_extra_frame->setVisible(true);
	ui.pick_frame->setVisible(false);
	//ui.advanced_frame->setVisible(false);
	ui.gradient_frame->setVisible(false);
	ui.smooth_frame->setVisible(false);
}

void PaintToolbox::on_fill_button_clicked() {
	paint_utensil[0]=FILL;
	ui.pen_frame->setVisible(true);
	ui.pen_extra_frame->setVisible(false);
	ui.pick_frame->setVisible(false);
	//ui.advanced_frame->setVisible(false);
	ui.gradient_frame->setVisible(false);
	ui.smooth_frame->setVisible(false);
}

void PaintToolbox::on_pick_button_clicked() {
	paint_utensil[0]=PICK;
	ui.pen_frame->setVisible(false);
	ui.pen_extra_frame->setVisible(false);
	ui.pick_frame->setVisible(true);
	//ui.advanced_frame->setVisible(false);
	ui.gradient_frame->setVisible(false);
	ui.smooth_frame->setVisible(false);
}

void PaintToolbox::on_gradient_button_clicked() {
	paint_utensil[0]=GRADIENT;
	ui.pen_frame->setVisible(true);
	ui.pen_extra_frame->setVisible(false);
	ui.pick_frame->setVisible(false);
	//ui.advanced_frame->setVisible(false);
	ui.gradient_frame->setVisible(true);
	ui.smooth_frame->setVisible(false);
}

void PaintToolbox::on_smooth_button_clicked() {
	paint_utensil[0]=SMOOTH;
	ui.pen_frame->setVisible(false);
	ui.pen_extra_frame->setVisible(true);
	ui.pick_frame->setVisible(false);
	//ui.advanced_frame->setVisible(false);
	ui.gradient_frame->setVisible(false);
	ui.smooth_frame->setVisible(true);
}

void PaintToolbox::on_tabWidget_currentChanged ( int index ) {}

void PaintToolbox::on_select_button_clicked() {
	paint_utensil[1]=SELECT;
	ui.pen_select_frame->setVisible(true);
	ui.poly_smooth_frame->setVisible(false);
}
void PaintToolbox::on_poly_smooth_button_clicked() {
	paint_utensil[1]=POLY_SMOOTH;
	ui.pen_select_frame->setVisible(true);
	ui.poly_smooth_frame->setVisible(true);
}

void PaintToolbox::on_percentual_slider_2_valueChanged(int value) {
	if (value!=ui.percentual_box_2->value()) ui.percentual_box_2->setValue(value);
}

void PaintToolbox::on_percentual_box_2_valueChanged(int value) {
	if (value!=ui.percentual_slider_2->value()) ui.percentual_box_2->setValue((int)value);
}

void PaintToolbox::on_decrease_slider_2_valueChanged(int value) {
	if (value!=ui.decrease_box_2->value()) ui.decrease_box_2->setValue(value);
}

void PaintToolbox::on_decrease_box_2_valueChanged(int value) {
	if (value!=ui.decrease_slider_2->value()) ui.decrease_box_2->setValue((int)value);
}

void PaintToolbox::on_pen_type_2_currentIndexChanged(QString value) {
	on_pen_radius_2_valueChanged( ui.pen_radius_2->value());
}

void PaintToolbox::on_pen_radius_2_valueChanged(double value) {
	static double oldval=-1;
	if (ui.pen_type_2->currentText()=="pixel") {
		if ((double)((int)value)!=value) {
			if (oldval<value)
			ui.pen_radius_2->setValue((double)((int)value)+1);
			else ui.pen_radius_2->setValue((double)((int)value));
		}
	} else {
	}
	oldval=ui.pen_radius_2->value();
}


void PaintToolbox::setColor(Color4b newcol,Qt::MouseButton mouse) {
	switch (mouse) {
		case Qt::LeftButton: {ui.front->setColor(toQcolor(newcol)); return; }
		case Qt::RightButton: {ui.back->setColor(toQcolor(newcol)); return;}
		default: {ui.front->setColor(toQcolor(newcol)); return;}
	}
}

void PaintToolbox::setColor(int r,int g,int b,Qt::MouseButton mouse) {
	switch (mouse) {
		case Qt::LeftButton: {ui.front->setColor(QColor(r,g,b)); return; }
		case Qt::RightButton: {ui.back->setColor(QColor(r,g,b)); return;}
		default: {ui.front->setColor(QColor(r,g,b)); return;}
	}
}

void PaintToolbox::on_backface_culling_stateChanged(int value) {}

void PaintToolbox::on_invisible_painting_stateChanged(int value) {}

void PaintToolbox::on_undo_button_clicked() {
	emit undo_redo(1);
}
void PaintToolbox::on_redo_button_clicked() {
	emit undo_redo(2);
}
