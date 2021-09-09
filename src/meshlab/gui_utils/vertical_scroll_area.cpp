#include "vertical_scroll_area.h"

#include <QEvent>
#include <QScrollBar>
#include <QVBoxLayout>

#include "vertical_scroll_area.h"

VerticalScrollArea::VerticalScrollArea(QWidget *parent) :
	QScrollArea(parent)
{
	setWidgetResizable(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e)
{
	// This works because QScrollArea::setWidget installs an eventFilter on the widget
	if(o && o == widget() && e->type() == QEvent::Resize)
		setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->width());

	return QScrollArea::eventFilter(o, e);
}
