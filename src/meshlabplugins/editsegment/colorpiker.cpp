#include <colorpicker.h>

void ColorPicker::setColor(QColor c) {
	color = c;
	update();
}

void ColorPicker::paintEvent(QPaintEvent * event ) {
	QPainter painter(this);
	painter.fillRect(QRect(0,0,width(),height()),color);
	int col=color.red()+color.green()+color.blue();
	if (col<150) painter.setPen(Qt::white);
	else painter.setPen(Qt::black);
	painter.drawRect(QRect(1,1,width()-3,height()-3));
}

void ColorPicker::mousePressEvent ( QMouseEvent * event ) {
	QColor temp = QColorDialog::getColor(color);
	if (temp.isValid()) color = temp;
	update();
}