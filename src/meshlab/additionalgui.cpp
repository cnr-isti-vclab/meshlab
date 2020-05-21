#include "additionalgui.h"
#include "../common/mlexception.h"

#include <QStylePainter>

#include <QWidgetAction>

#include <QStyle>
#include <QDebug>

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
