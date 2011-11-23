#ifndef ADDITIONALGUI_H
#define ADDITIONALGUI_H

#include <QtGui>
#include <QString>

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

class UsefulGUIFunctions
{
public:
	static QPixmap pixmapGeneratorFromQtPrimitiveElement(const QSize& pixmapsize,const QStyle::PrimitiveElement primitive, QStyle *style,const QStyleOption& opt);
	//result will be something like basename_number
	static QString generateUniqueDefaultName(const QString& basename,const QStringList& namelist);
	//called by an editingText event on a QLineEdit. Typically used when we need that the text in a QLineEdit is depending on the content of another widget.
	//Originaltext is the text contained in the pattern widget we are currently editing.
	//Example if original text is "Random vertex Displacement (really random)" we will obtain like result randomVertexDisplacementReallyRandom
	static QString generateFunctionName(const QString& originaltext);

	static QString changeNameIfAlreadyInList(const QString& name,const QStringList& allnames);

	static QString generateBackupName(const QFileInfo& finfo);
	//put <![CDATA[text]]> around text in order you can put html tag inside xml document without having problems with validation.
	static QString avoidProblemsWithHTMLTagInsideXML( const QString& text );
};

class MLScriptEditor;

class MLNumberArea : public QWidget
{
public:
	MLNumberArea(MLScriptEditor* editor);
	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent *event);

private:
	MLScriptEditor* mledit;
};

class MLScriptEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	MLScriptEditor(QWidget* par = NULL);
	~MLScriptEditor();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect& r, int dy);
private:
	MLNumberArea* narea;
};

#endif // CHECKBOXLIST_H
