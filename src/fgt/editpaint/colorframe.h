#ifndef COLORFRAME_H_
#define COLORFRAME_H_

#include <QtGui>

/*
 *A simple widget that displays a color and allows the
 * user to choose a new one with a QColorDialog
 */ 
class Colorframe : public QFrame
{
	Q_OBJECT
	
public:
	Colorframe(QWidget * parent, Qt::WindowFlags flags = 0) : QFrame(parent, flags){} 

	virtual void mousePressEvent ( QMouseEvent *)
	{
		QPalette palette = Colorframe::palette();
		QColor temp = palette.color(QPalette::Normal, QPalette::Window);
		temp = QColorDialog::getColor(temp);
		if (temp.isValid()){
			setColor(temp);
			update();
		}
	}

	QColor getColor(){return Colorframe::palette().color(QPalette::Normal, QPalette::Window);}
	
public slots:

	void setColor(QColor c)
	{
		QPalette palette = Colorframe::palette();
		palette.setColor(QPalette::Normal, QPalette::Window, c);
		palette.setColor(QPalette::Disabled, QPalette::Window, c);
		palette.setColor(QPalette::Inactive, QPalette::Window, c);
		setPalette(palette);
		update();
		emit colorChanged(c);
	}
	
signals:
	void colorChanged(QColor c);
	
};

#endif /*COLORFRAME_H_*/
