#include "additionalgui.h"

CheckBoxList::CheckBoxList(const QString& defaultValue,QWidget *widget )
:QComboBox(widget),highli(0),default(defaultValue)
{
	view()->viewport()->installEventFilter(this);
	view()->setAlternatingRowColors(true);
	connect(this,SIGNAL(highlighted(int)),this,SLOT(currentHighlighted(int)));
}

CheckBoxList::~CheckBoxList()
{
}

void CheckBoxList::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));

	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	if (selectedItemsNames().empty())
		opt.currentText = default;
	else
		opt.currentText = selectedItemsString(QString(" | "));
	for (int ii=0;ii<count();++ii) 
	{
		Qt::CheckState v;
		if (sel.contains(itemText(ii)))
			v = Qt::Checked;
		else
			v = Qt::Unchecked;
		setItemData(ii,QVariant(v),Qt::CheckStateRole);
	}
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);

}

bool CheckBoxList::eventFilter(QObject *object, QEvent *event)
{
	if(event->type() == QEvent::MouseButtonRelease && object==view()->viewport())
	{
		updateSelected(highli);
		repaint();
		return true;
	}
	return QComboBox::eventFilter(object,event);
}

QStringList CheckBoxList::getSelected() const
{
	return sel;
}

void CheckBoxList::updateSelected(const int ind)
{
	bool checked = itemData(ind,Qt::CheckStateRole).toBool();
	QString text = itemText(highli);
	QItemSelectionModel::SelectionFlag flag;
	if (checked)
		sel.removeAll(text);
	else
		sel.push_back(text);
}

void CheckBoxList::insertCheckableItem( const int pos,const QString& lab,const bool checked )
{
	insertItem(pos,lab);
	if (checked)
		sel.push_back(lab);
}

void CheckBoxList::insertCheckableItem(const QString& lab,const bool checked )
{
	addItem(lab);
	if (checked)
		sel.push_back(lab);
}

void CheckBoxList::currentHighlighted( int high )
{
	highli = high;
}

QStringList CheckBoxList::selectedItemsNames() const
{
	return sel;
}

QString CheckBoxList::selectedItemsString(const QString& sep) const
{
	QStringList ll = selectedItemsNames();
	return ll.join(sep);
}