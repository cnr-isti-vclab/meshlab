#ifndef SCRIPT_SYNTAX_H
#define SCRIPT_SYNTAX_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QVector>
#include <QList>
#include <QString>
#include <QStringList>
#include <QRegExp>

class SyntaxTreeNode
{
public:
	SyntaxTreeNode(const QVector<QVariant> &data, SyntaxTreeNode *parent = 0);
	~SyntaxTreeNode();

	SyntaxTreeNode *child(int number);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	SyntaxTreeNode *parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);
	SyntaxTreeNode* findChild(const QVector<QVariant>& data);

private:
	QList<SyntaxTreeNode*> childItems;
	QVector<QVariant> itemData;
	SyntaxTreeNode *parentItem;
};

class SyntaxTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	SyntaxTreeModel(SyntaxTreeNode* root,QObject *parent = 0);
	~SyntaxTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation,const QVariant &value, int role = Qt::EditRole);

	bool insertColumns(int position, int columns,const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns,const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows,const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,const QModelIndex &parent = QModelIndex());
	//void populateModel(const QStringList &signatures,);

	SyntaxTreeNode *getItem(const QModelIndex &index) const;
	//void createAndAppendBranch(QString& st,SyntaxTree* parent);
private:
	SyntaxTreeNode *rootItem;
};

struct LibraryElementInfo
{
	QString completename;
	QString help;
};

class ExternalLib
{
public:
	ExternalLib(const QString& filename);
	QString name;
	virtual QList<LibraryElementInfo> libraryMembersInfo() const = 0;
	QString libCode() const;
};

class SGLMathLib : public ExternalLib
{
public:
	SGLMathLib();
	QList<LibraryElementInfo> libraryMembersInfo() const;
};

class MLScriptLanguage
{
public:
	enum LANG_TOKEN {RESERVED,NAMESPACE,FUNCTION,MEMBERFIELD};

	MLScriptLanguage();
	~MLScriptLanguage();
	//a Library of functions is a list of functions expressed as NameSpace0.NameSpace1.----.NameSpaceN.fun(par0,par1,....,park) (if sep = "." openpar = "(") with NameSpace0 common to all the functions of a single library 
	//the function will generate a SyntaxTree in order to easy manage SyntaxHighlighting and Auto-Completing.
	void addLibrary(const QList<LibraryElementInfo>& funsigns);
	const SyntaxTreeModel* functionsLibrary() const;
	SyntaxTreeModel* functionsLibrary();
	QStringList reserved;
	QStringList langfuncs;

	//Everything used in the language syntax dividing two different "words" for example a=b or a|b or a,b or a<b or a+b or a;b or a b (whitespace) or a\nb (carriage return) etc
	QRegExp worddelimiter;

	QRegExp wordsjoiner;
	QRegExp openpar;
	QRegExp closepar;
	//For instance in C++ a qualifier is . or ->
	QRegExp qualifier;

	QRegExp matchIdentifier() const;
	QRegExp matchIdentifiersButNotReservedWords() const;
	QRegExp matchOnlyReservedWords() const;
	
	virtual const QList<ExternalLib*> scriptLibraryFiles() const = 0;
	QString getExternalLibrariesCode();
	QList<LibraryElementInfo> getExternalLibrariesMembersInfo() const;
	//QStringList splitTextInWords(const QString& st) const;
	
private:
	void initLibrary();
	void addBranch(const LibraryElementInfo& mi,SyntaxTreeNode* parent);
	SyntaxTreeModel* libraries;
};

class JavaScriptLanguage : public MLScriptLanguage
{
public:
	JavaScriptLanguage();
	const QList<ExternalLib*> scriptLibraryFiles() const;
};

#endif