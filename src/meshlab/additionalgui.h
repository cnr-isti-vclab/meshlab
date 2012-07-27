#ifndef ADDITIONALGUI_H
#define ADDITIONALGUI_H

#include <QtGui>
#include <QString>
#include <QModelIndex>
#include "../common/scriptsyntax.h"
#include "../common/searcher.h"

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

	static QColor editorMagicColor(){return QColor(189,215,255);}
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


class MLSyntaxHighlighter : public  QSyntaxHighlighter
{
	Q_OBJECT
public:
	MLSyntaxHighlighter(const MLScriptLanguage& synt, QWidget* parent);

protected:
	void highlightBlock (const QString& text);
	static QString addIDBoundary(const QString& st);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QMap<MLScriptLanguage::LANG_TOKEN,QTextCharFormat> tokenformat;
	bool colorTextIfInsideTree(const QString& text,SyntaxTreeNode* node,int start);
	const MLScriptLanguage& syntax;
	QList<HighlightingRule> highlightingRules;
//signals:
//	void functionRecognized(const QString& sign);

};

class MLAutoCompleterPopUp : public QListView
{
	Q_OBJECT
public:
	MLAutoCompleterPopUp(QWidget* parent);
	~MLAutoCompleterPopUp();
protected:
	bool event(QEvent *event);
};

class MLAutoCompleter : public QCompleter
{
	Q_OBJECT
public:
	MLAutoCompleter(const MLScriptLanguage& synt,QWidget* parent);
	//MLAutoCompleter(QObject* parent);

public slots:
	void changeCurrent(const QModelIndex& ind);
protected:
	QStringList splitPath(const QString &path) const;
	QString pathFromIndex(const QModelIndex &index) const;

private:
	const MLScriptLanguage& syntax;
};

class MLScriptEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	MLScriptEditor(QWidget* par = NULL);
	~MLScriptEditor();
	void lineNumberAreaPaintEvent(QPaintEvent *event,const QColor& col);
	int lineNumberAreaWidth();
	void setScriptLanguage(MLScriptLanguage* syntax);

protected:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent * e);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	//void updateCursorPos(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect& r, int dy);
	void showAutoComplete(QKeyEvent * e);
	void insertSuggestedWord(const QString& st);
	//void setToolTip(const QString& st);
private:
	QString currentLine() const;
	QString lastInsertedWord() const;
	QString wordUnderTextCursor() const;

	MLNumberArea* narea;
	QStringList regexps;

	MLScriptLanguage* synt;
	MLSyntaxHighlighter* synhigh;
	MLAutoCompleter* comp;
};

//MyToolButton class has been introduced to overcome the "always on screen small down arrow visualization problem" officially recognized qt bug.
class MyToolButton : public QToolButton
{
	Q_OBJECT
public:
	MyToolButton( QWidget * parent = 0 );
protected:
	void paintEvent( QPaintEvent * );
};

class SearchMenu : public QMenu
{
	Q_OBJECT
public:
	SearchMenu(const WordActionsMapAccessor& wm,const int max,QWidget* parent);
	inline void setMaxResultNum(const int max) {maxres = max;};
	inline int maxResultNum() const {return maxres;};
	void clearResults();
private:
	int maxres;
	QLineEdit* searchline;
	QWidgetAction* searchact;
	const WordActionsMapAccessor& wama;
private slots:
	void updateResults();
	void edited(const QString&);
signals:
	void updatedResults();
};


#endif 
