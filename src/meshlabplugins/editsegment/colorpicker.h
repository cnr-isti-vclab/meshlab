/***************************************************************************
first version: 0.1 
autor: Gfrei Andreas 
date:  07/02/2007 
email: gfrei.andreas@gmx.net
****************************************************************************/

#ifndef COLORPICKER_H
#define COLORPICKER_H


#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QColorDialog>

class ColorPicker : public QWidget {
protected:
	QColor color;
public:
	ColorPicker ( QWidget * parent = 0, Qt::WindowFlags f = 0 ) :QWidget(parent,f) {}
	void setColor(QColor c);
	inline QColor getColor() { return color; }
	virtual void paintEvent ( QPaintEvent * event );
	virtual void mousePressEvent ( QMouseEvent * event );
};

#endif
