#include "additionalgui.h"

CheckBoxListItemDelegate::CheckBoxListItemDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{
}

void CheckBoxListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
	bool value = index.data(Qt::CheckStateRole).toBool();
	QString text = index.data(Qt::DisplayRole).toString();

	// fill style options with item data
	const QStyle *style = QApplication::style();
	QStyleOptionButton opt;
	opt.state |= value ? QStyle::State_On : QStyle::State_Off;
	opt.state |= QStyle::State_Enabled;
	opt.text = text;
	opt.rect = QRect(option.rect.x(),option.rect.y(),16,16);

	QRect textrect(option.rect.x() + 16,option.rect.y(),option.rect.width() - 16,option.rect.height());
	style->drawPrimitive(QStyle::PE_IndicatorCheckBox,&opt,painter);
	style->drawItemText(painter,textrect,Qt::AlignLeft,opt.palette,true,text);
}

CheckBoxList::CheckBoxList(const QString& defaultValue,QWidget *widget )
:QComboBox(widget),highli(0),defaultval(defaultValue),popupopen(false)
{
	view()->viewport()->installEventFilter(this);
	view()->setItemDelegate(new CheckBoxListItemDelegate(this));
	connect(this,SIGNAL(highlighted(int)),this,SLOT(currentHighlighted(int)));
}

CheckBoxList::CheckBoxList( QWidget *widget /*= 0*/ )
:QComboBox(widget),highli(0),defaultval(),popupopen(false)
{
	view()->viewport()->installEventFilter(this);
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
	opt.currentText = "";
	if (selectedItemsNames().empty())
		opt.currentText = defaultval;
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

bool CheckBoxList::eventFilter(QObject *object, QEvent * event)
{
	if ((event->type() == QEvent::MouseButtonPress) && (!popupopen))
	{
		popupopen = true;
		return true;
	}
	if((event->type() == QEvent::MouseButtonRelease) &&
		(object==view()->viewport()) && popupopen)
	{
		updateSelected(highli);
		repaint();
		popupopen = true;
		return true;
	}
	return QComboBox::eventFilter(object,event);
}

void CheckBoxList::focusOutEvent ( QFocusEvent * /*e*/ )
{
	if (popupopen)
		popupopen = false;
}

QStringList CheckBoxList::getSelected() const
{
	return sel;
}

void CheckBoxList::updateSelected(const int ind)
{
	bool checked = itemData(ind,Qt::CheckStateRole).toBool();
	QString text = itemText(highli);
	if (checked)
		sel.removeAll(text);
	else
		sel.push_back(text);
}

void CheckBoxList::insertCheckableItem( const int pos,const QString&
									   lab,const bool checked )
{
	insertItem(pos,lab);
	if (checked)
		sel.push_back(lab);
}

void CheckBoxList::insertCheckableItem(const QString& lab,const bool
									   checked )
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
	if (ll.isEmpty())
		return defaultval;
	return ll.join(sep);
}

void CheckBoxList::setDefaultValue( const QString& defaultValue )
{
	defaultval = defaultValue;
}

void CheckBoxList::setCurrentValue( const QStringList& st )
{
	sel = st;
}

QPixmap UsefulGUIFunctions::pixmapGeneratorFromQtPrimitiveElement(const QSize& pixmapsize,const QStyle::PrimitiveElement primitive, QStyle *style,const QStyleOption& opt)
{
	QPixmap pix(pixmapsize);
	pix.fill(Qt::transparent);
	QPainter p;
	p.begin(&pix);
	style->drawPrimitive(primitive, &opt, &p);
	p.end();
	return pix;
}

QString UsefulGUIFunctions::generateUniqueDefaultName( const QString& basename,const QStringList& namelist)
{
	int max = INT_MIN;
	QString regexp(basename + "_(\\d)+");
	QStringList items = namelist.filter(QRegExp(regexp));
	for(int ii = 0;ii < items.size();++ii)
	{
		QRegExp reg("(\\d)+");
		items[ii].indexOf(reg);
		int index = reg.cap().toInt();
		if (index > max)
			max = index;
	}
	QString tmpname = basename + "_";
	if (items.size() == 0)
		tmpname += QString::number(namelist.size());
	else
		tmpname += QString::number(max + 1);
	return tmpname;
}

QString UsefulGUIFunctions::generateFunctionName(const QString& originaltext)
{
	QString newname;
	if (originaltext.isEmpty())
		return newname;
	QRegExp nonchar("\\W+");
	int index = 0;
	do 
	{
		originaltext.indexOf(nonchar,index);
		QRegExp validchar("\\w+");
		int validcharind = originaltext.indexOf(validchar,index);
		if (validcharind != -1)
		{
			QString captured = validchar.cap();
			if (captured.size() > 0)
				captured[0] = captured[0].toUpper();
			newname.push_back(captured);
		}
		index = index + validchar.cap().size() + nonchar.cap().size();
	} while (index < originaltext.size());
	if (originaltext[0].isLetter() && (newname.size() > 0))
		newname[0] = originaltext[0].toLower();
	return newname;
}

QString UsefulGUIFunctions::changeNameIfAlreadyInList( const QString& name,const QStringList& allnames )
{
	QStringList ls;
	QString tmpname = name;
	do
	{
		ls = allnames.filter(tmpname);
		if (ls.size() > 1)
			tmpname = tmpname + "_" + QString::number(ls.size() - 1);
	} while(ls.size() > 1);
	return tmpname;
}

QString UsefulGUIFunctions::generateBackupName( const QFileInfo& finfo )
{
	QDir dir = finfo.absoluteDir();
	QFileInfoList list = dir.entryInfoList(QDir::Files);
	QRegExp oldexp(finfo.fileName() + "\\.old(\\d+)");
	int max = 0;
	for (int ii = 0;ii < list.size();++ii)
	{
		if (list[ii].fileName().contains(oldexp))
		{
			QRegExp num("\\d+");
			list[ii].suffix().indexOf(num);
			int ver = num.cap().toInt();
			if (ver > max)
				max = ver;
		}
	}
	return QString(finfo.absolutePath() + "/" + finfo.completeBaseName() + ".old" + QString::number(max));
}


ExpandButtonWidget::ExpandButtonWidget( QWidget* parent )
:QWidget(parent),isExpanded(false)
{
	exp = new PrimitiveButton(QStyle::PE_IndicatorArrowDown,this);
	exp->setMaximumSize(16,16);
	QHBoxLayout *hlay = new QHBoxLayout(this);
	hlay->addWidget(exp,0,Qt::AlignHCenter);
	connect(exp,SIGNAL(clicked(bool)),this,SLOT(changeIcon()));
}

ExpandButtonWidget::~ExpandButtonWidget()
{

}

void ExpandButtonWidget::changeIcon()
{

	isExpanded = !isExpanded;
	if (isExpanded)
		exp->setPrimitiveElement(QStyle::PE_IndicatorArrowUp);
	else
		exp->setPrimitiveElement(QStyle::PE_IndicatorArrowDown);
	emit expandView(isExpanded);
}

PrimitiveButton::PrimitiveButton(const QStyle::PrimitiveElement el,QWidget* parent )
:QPushButton(parent),elem(el)
{
}

PrimitiveButton::PrimitiveButton( QWidget* parent )
:QPushButton(parent),elem(QStyle::PE_CustomBase)
{

}

PrimitiveButton::~PrimitiveButton()
{

}

void PrimitiveButton::paintEvent( QPaintEvent * event )
{
	QStylePainter painter(this);
	QStyleOptionButton option;
	option.initFrom(this);
	//painter.drawControl(QStyle::CE_PushButton,option);
	painter.drawPrimitive (elem,option);
}

void PrimitiveButton::setPrimitiveElement( const QStyle::PrimitiveElement el)
{
	elem = el;
}

TreeWidgetWithMenu::TreeWidgetWithMenu( QWidget* parent /*= NULL*/ )
:QTreeWidget(parent)
{
	menu = new QMenu(this);
	connect(menu,SIGNAL(triggered(QAction*)),this,SIGNAL(selectedAction(QAction*)));
}

TreeWidgetWithMenu::~TreeWidgetWithMenu()
{

}

void TreeWidgetWithMenu::contextMenuEvent( QContextMenuEvent * event )
{
	menu->popup(event->globalPos());
}

void TreeWidgetWithMenu::insertInMenu(const QString& st,const QVariant& data)
{
	QAction* act = menu->addAction(st);
	act->setData(data);
}
