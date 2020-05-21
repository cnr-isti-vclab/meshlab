#include "additionalgui.h"
#include "../common/mlexception.h"

#include <QStylePainter>
#include <QHBoxLayout>
#include <QToolTip>
#include <QWidgetAction>
#include <QApplication>
#include <QScrollBar>
#include <QStyle>
#include <QDebug>
#include <QMetaEnum>
#include <qgl.h>

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
        view()->setItemDelegate(new CheckBoxListItemDelegate(this));
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
    sel.removeAll(defaultval);
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

void PrimitiveButton::paintEvent( QPaintEvent * /*event*/ )
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

SearchMenu::SearchMenu(const WordActionsMapAccessor& wm,const int max,QWidget* parent,const int fixedwidth)
:MenuWithToolTip(QString(),parent),searchline(NULL),wama(wm),maxres(max),fixedwidthsize(fixedwidth)
{
    searchline = new MenuLineEdit(this);
    //searchline->resize(fixedwidth,searchline->height());
    QWidgetAction* searchact = new QWidgetAction(this);
    searchact->setDefaultWidget(searchline);
    addAction(searchact);
    connect(searchline,SIGNAL(textEdited( const QString&)),this,SLOT(edited( const QString&)));
    connect(searchline,SIGNAL(arrowPressed(const int)),this,SLOT(changeFocus(const int)));
    connect(this,SIGNAL(aboutToShow()),this,SLOT(onAboutToShowEvent()));
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void SearchMenu::getResults(const QString& text,QList<QAction*>& result)
{
    try
    {
        RankedMatches rm;
        int ii = wama.rankedMatchesPerInputString(text,rm);
        int inserted = 0;
        while(ii > 0)
        {
            QList<QAction*> myacts;
            rm.getActionsWithNMatches(ii,myacts);
            if (inserted + myacts.size() > maxres)
                myacts = myacts.mid(0,myacts.size() - (inserted + myacts.size() - maxres));
            result.append(myacts);
            QAction* sep = new QAction(this);
            sep->setSeparator(true);
            result.append(sep);
            inserted += myacts.size();
            --ii;
        }
    }
    catch(InvalidInvariantException& e)
    {
        qDebug() << "WARNING!!!!!!!!!!!!!!!!!!!" << e.what() << "\n";
    }
}

void SearchMenu::updateGUI( const QList<QAction*>& results )
{
    QList<QAction*> old = actions();
    //list of separators to be deleted and/or menu items (actions) that are no more in the results
    QList<QAction*> delsepremact;
    bool maybeallequal = (old.size() - 1) == results.size();
    //tt start from 1 because the first one is the QWidgetAction containing the QLineEdit
    for(int tt = 1;tt < old.size();++tt)
    {
        QAction* oldact = old[tt];
        if (oldact != NULL)
        {
            if (maybeallequal && (results.size() > 0))
            {
                QAction* resit = results[tt-1];
                maybeallequal = (resit != NULL) && ((oldact->isSeparator() && resit->isSeparator()) || (oldact == resit));
            }

            if (oldact->isSeparator() || !results.contains(oldact))
            {
                delsepremact.push_back(oldact);
            }
        }
    }
    if (!maybeallequal)
    {
#ifdef Q_OS_MAC
      this->hide();
#endif
      for(int jj = 0;jj < delsepremact.size();++jj)
        {
            QAction*  todel = delsepremact[jj];
            if ((todel != NULL) && (todel->isSeparator()))
            {
                delete todel;
                delsepremact[jj] = NULL;
            }
            else
            {
                removeAction(todel);
            }
        }
        addActions(results);
#ifdef Q_OS_MAC
        this->show();
#endif
        //if (results.size() > 0 && (results[0] != NULL))
        //	setActiveAction(results[0]);
        alignToParentGeometry();
    }
}

void SearchMenu::edited( const QString& text)
{
  QList<QAction*> results;
  getResults(text,results);
  updateGUI(results);
}

void SearchMenu::clearResults()
{
    QList<QAction*> actlst = actions();
    foreach(QAction* act,actlst)
    {
        //QLineEdit MUST NOT be deleted!
        if (qobject_cast<QWidgetAction*>(act) == 0)
            removeAction(act);
    }
}

void SearchMenu::setLineEditFocus()
{
    searchline->setFocus();
//	const QList<QAction*>& acts = actions();
    //if (acts.size() > 1 && acts[1] != NULL)
    //	setActiveAction(acts[1]);
}

void SearchMenu::alignToParentGeometry()
{
    if (parentWidget() != NULL)
    {
        QPoint p = parentWidget()->mapToGlobal(QPoint(0,0));
        int borderx = p.x() + parentWidget()->frameGeometry().width();
        QSize sz = sizeHint();
        move(borderx - sz.width(),y());
    }
}

void SearchMenu::changeFocus( const int k )
{
    setFocus();
    QAction* act = NULL;
    int next = nextEnabledAction(k,0,actions(),act);
    if (next != -1)
            setActiveAction(act);
}

int SearchMenu::nextEnabledAction( const int k,const int currentind,const QList<QAction*>& acts,QAction*& nextact) const
{
    const int errorind = -1;
    if ((currentind < 0) || (currentind >= acts.size()))
        return errorind;
    QAction* current = acts[currentind];
    if (acts.size() > 0)
    {
        if (current == NULL)
            return errorind;
        int ind = currentind;
        do
        {
            if (k == Qt::Key_Up)
                ind = (acts.size() + (ind - 1)) % acts.size();
            else
                ind = (ind + 1) % acts.size();
            QAction* curract = acts[ind];
            if ((curract != NULL) && (curract->isEnabled()) && !curract->isSeparator())
            {
                nextact = curract;
                return ind;
            }
        } while (ind != currentind);
        return errorind;
    }
    return errorind;
}

void SearchMenu::keyPressEvent( QKeyEvent * event )
{
    int k = event->key();
    if ((k != Qt::Key_Up) && (k != Qt::Key_Down))
        QMenu::keyPressEvent(event);
    else
    {
        const QList<QAction*>& acts = actions();
        QAction* current = activeAction();
        if (current != NULL)
        {
            int currentind = acts.indexOf(current);
            if (currentind > -1)
            {
                QAction* act = NULL;
                int next = nextEnabledAction(k,currentind,actions(),act);
                if (next != -1)
                {
                    if (next == 0)
                    {
                        searchline->setFocus();
                        selectTextIfNotEmpty();
                    }
                    setActiveAction(act);
                }
            }
        }
    }
}

void SearchMenu::selectTextIfNotEmpty()
{
    if (!searchline->text().isEmpty())
        searchline->selectAll();
}

QSize SearchMenu::sizeHint() const
{
    if (fixedwidthsize == -1)
        return QMenu::sizeHint();
    int borderx = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
    return QSize(fixedwidthsize + borderx * 2,QMenu::sizeHint().height());
}

void SearchMenu::onAboutToShowEvent()
{
    setLineEditFocus();
    selectTextIfNotEmpty();
    //resizeGUI();
    alignToParentGeometry();
}

void SearchMenu::resizeEvent( QResizeEvent * event )
{
    if (fixedwidthsize != -1)
    {
        searchline->setMinimumWidth(fixedwidthsize);
        searchline->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    }
    QMenu::resizeEvent(event);
    alignToParentGeometry();
}

int& SearchMenu::searchLineWidth()
{
    return fixedwidthsize;
}
//MyToolButton class has been introduced to overcome the "always on screen small down arrow visualization problem" officially recognized qt bug.
MyToolButton::MyToolButton( int msecdelay,QWidget * parent /*= 0 */ ) 
    : QToolButton( parent )
{
    if (msecdelay != 0)
    {
        setPopupMode(QToolButton::DelayedPopup);
        DelayedToolButtonPopUpStyle* delstyle = new DelayedToolButtonPopUpStyle(msecdelay);
        setStyle(delstyle);
    }
    else
        setPopupMode(QToolButton::InstantPopup);
}

MyToolButton::MyToolButton( QAction* act, int msecdelay /*= 0*/,QWidget * parent /*= 0*/ )
    :QToolButton(parent)
{
    if (msecdelay != 0)
    {
        setPopupMode(QToolButton::DelayedPopup);
        DelayedToolButtonPopUpStyle* delstyle = new DelayedToolButtonPopUpStyle(msecdelay);
        setStyle(delstyle);
    }
    else
        setPopupMode(QToolButton::InstantPopup);
    setDefaultAction(act);
}

void MyToolButton::paintEvent( QPaintEvent * )
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption( & opt ); 
    opt.features &= (~ QStyleOptionToolButton::HasMenu);
    p.drawComplexControl( QStyle::CC_ToolButton, opt );
}

void MyToolButton::openMenu()
{
    if (menu()->isHidden())
        showMenu();
}

MenuLineEdit::MenuLineEdit( QWidget* parent )
:QLineEdit(parent)
{

}

void MenuLineEdit::keyPressEvent( QKeyEvent * event )
{
    int k = event->key();
    if ((k != Qt::Key_Up) && (k != Qt::Key_Down))
        QLineEdit::keyPressEvent(event);
    else
        emit arrowPressed(k);
}


MenuWithToolTip::MenuWithToolTip( const QString& name,QWidget* par )
    :QMenu(name,par)
{

}

bool MenuWithToolTip::event(QEvent * e)
{
//  QString pippo = QEvent::staticMetaObject.enumerator(QEvent::staticMetaObject.indexOfEnumerator("Type")).valueToKey(e->type());

  const QHelpEvent *helpEvent = static_cast <QHelpEvent *>(e);
  if ((helpEvent->type() == QEvent::ToolTip)  && (activeAction() != 0))
    QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
  /*else
        QToolTip::hideText();*/
  return QMenu::event(e);
}


DelayedToolButtonPopUpStyle::DelayedToolButtonPopUpStyle( int msec )
    :QProxyStyle()
{
    _msec = msec;
}

int DelayedToolButtonPopUpStyle::styleHint(QStyle::StyleHint sh, const QStyleOption * opt /*= 0*/, const QWidget * widget /*= 0*/, QStyleHintReturn * hret /*= 0*/ ) const
{
    if (sh == QProxyStyle::SH_ToolButton_PopupDelay)
        return _msec;
    return QProxyStyle::styleHint(sh,opt,widget,hret);
}

MLFloatSlider::MLFloatSlider( QWidget *parent /*= 0*/ ) 
    : QSlider(parent)
{
    connect(this, SIGNAL(valueChanged(int)),
        this, SLOT(notifyValueChanged(int)));
}

void MLFloatSlider::notifyValueChanged( int value )
{
    emit floatValueChanged(float(value));
}

void MLFloatSlider::setValue( float val )
{
    setSliderPosition(int(val));
}
