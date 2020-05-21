#ifndef ADDITIONALGUI_H
#define ADDITIONALGUI_H

#include <QString>
#include <QModelIndex>
#include <QListView>
#include <QCompleter>
#include <QStyledItemDelegate>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QLineEdit>
#include <QMenu>
#include <QProgressBar>
#include <QShortcut>
#include <QTreeWidget>
#include <QFontInfo>
#include <QFileInfo>
#include <QDir>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include "../common/searcher.h"
#include <QToolTip>
#include <QSyntaxHighlighter>
#include <QProxyStyle>

class CheckBoxListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckBoxListItemDelegate(QObject *parent);
    void paint(QPainter *painter, const QStyleOptionViewItem&option,const QModelIndex &index) const;
};

//WARNING!!!!DON'T USE QComboBox currentText() to get selected elements! Instead use selectedItemsString.
class CheckBoxList: public QComboBox
{
    Q_OBJECT;

public:
    CheckBoxList(QWidget *widget = 0);
    CheckBoxList(const QString& defaultValue,QWidget *widget = 0);
    ~CheckBoxList();
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *object, QEvent *event);
    QStringList getSelected() const;
    void insertCheckableItem(const int pos,const QString& lab,const bool checked);
    void insertCheckableItem(const QString& lab,const bool checked );
    void updateSelected(int ind);
    QString selectedItemsString(const QString& sep = QString(" | ")) const;
    QStringList selectedItemsNames() const;
    void setDefaultValue(const QString& defaultValue);
protected:
    void	focusOutEvent ( QFocusEvent * e );
private slots:
    void currentHighlighted(int high);
public slots:
    void setCurrentValue(const QStringList& st);
private:
    QStringList sel;
    int highli;
    QString defaultval;
    bool popupopen;
};

class PrimitiveButton : public QPushButton
{
    Q_OBJECT
public:
    PrimitiveButton(QWidget* parent);
    PrimitiveButton(const QStyle::PrimitiveElement el,QWidget* parent);
    ~PrimitiveButton();
    void setPrimitiveElement(const QStyle::PrimitiveElement el);
protected:
    void paintEvent(QPaintEvent * event);
private:
    QStyle::PrimitiveElement elem;
};

class ExpandButtonWidget : public QWidget
{
    Q_OBJECT
public:
    ExpandButtonWidget(QWidget* parent);
    ~ExpandButtonWidget();
private slots:
    void changeIcon();
signals:
    void expandView(bool exp);
private:
    PrimitiveButton* exp;
    bool isExpanded;
};

class TreeWidgetWithMenu : public QTreeWidget
{
    Q_OBJECT
public:
    TreeWidgetWithMenu(QWidget* parent = NULL);
    ~TreeWidgetWithMenu();

    void insertInMenu(const QString& st,const QVariant& data);
protected:
    void contextMenuEvent( QContextMenuEvent * event );
private:
    QMenu* menu;

signals:
    void selectedAction(QAction* act);
};

class DelayedToolButtonPopUpStyle : public QProxyStyle
{
public:
    DelayedToolButtonPopUpStyle(int msec);

    int styleHint(QStyle::StyleHint sh, const QStyleOption * opt = 0, const QWidget * widget = 0, QStyleHintReturn * hret = 0) const;
private:
    int _msec;
};

//MyToolButton class has been introduced to overcome the "always on screen small down arrow visualization problem" officially recognized qt bug.
class MyToolButton : public QToolButton
{
    Q_OBJECT
public:
    MyToolButton( int msecdelay = 0,QWidget * parent = 0 );
    MyToolButton(QAction* act, int msecdelay = 0,QWidget * parent = 0);
public slots:
    void openMenu();
protected:
    void paintEvent( QPaintEvent * );
    //void mouseReleaseEvent(QMouseEvent * ev);
    //void mousePressEvent(QMouseEvent * ev);
};

class MenuLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MenuLineEdit(QWidget* parent);
protected:
    void keyPressEvent(QKeyEvent * event);
signals:
    void arrowPressed(const int k);
};


class MenuWithToolTip : public QMenu
{
    Q_OBJECT
public:
    MenuWithToolTip(const QString& name,QWidget* par);
    bool event(QEvent * e);
};

class SearchMenu : public MenuWithToolTip
{
    Q_OBJECT
public:
    SearchMenu(const WordActionsMapAccessor& wm,const int max,QWidget* parent,const int fixedwidth = -1);
    int& searchLineWidth();
    void clearResults();
    QSize sizeHint () const;
protected:
    void keyPressEvent(QKeyEvent * event);
    void resizeEvent ( QResizeEvent * event);
private:
    MenuLineEdit* searchline;
    const WordActionsMapAccessor& wama;
    int maxres;
    int fixedwidthsize;

    void getResults(const QString& text,QList<QAction*>& results);
    void updateGUI(const QList<QAction*>& results);
    void alignToParentGeometry();
    void selectTextIfNotEmpty();
    int nextEnabledAction( const int k,const int currentind,const QList<QAction*>& acts,QAction*& nextact) const;
private slots:
    void edited(const QString& text);
    void setLineEditFocus();
    void onAboutToShowEvent();
    void changeFocus(const int k);
};

class MLFloatSlider : public QSlider 
{
    Q_OBJECT

public:
    MLFloatSlider(QWidget *parent = 0);

    void setValue(float val);

signals:
    void floatValueChanged(float value);

public slots:
    void notifyValueChanged(int value);
};

#endif
