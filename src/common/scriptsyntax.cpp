#include "scriptsyntax.h"
#include <QFile>

SyntaxTreeNode::SyntaxTreeNode(const QVector<QVariant> &data, SyntaxTreeNode *parent)
{
	parentItem = parent;
	itemData = data;
}

SyntaxTreeNode::~SyntaxTreeNode()
{
	qDeleteAll(childItems);
}

SyntaxTreeNode *SyntaxTreeNode::child(int number)
{
	return childItems.value(number);
}

int SyntaxTreeNode::childCount() const
{
	return childItems.count();
}

int SyntaxTreeNode::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<SyntaxTreeNode*>(this));

	return 0;
}

int SyntaxTreeNode::columnCount() const
{
	return itemData.count();
}

QVariant SyntaxTreeNode::data(int column) const
{
	return itemData.value(column);
}

bool SyntaxTreeNode::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) 
	{
		QVector<QVariant> dt(columns);
		for(int ii = 0;ii < columns;++ii)
			dt[ii] = QVariant(this->data(ii).type());
		SyntaxTreeNode *item = new SyntaxTreeNode(dt, this);
		childItems.insert(position, item);
	}

	return true;
}

bool SyntaxTreeNode::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	foreach (SyntaxTreeNode *child, childItems)
		child->insertColumns(position, columns);

	return true;
}

SyntaxTreeNode *SyntaxTreeNode::parent()
{
	return parentItem;
}

bool SyntaxTreeNode::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

bool SyntaxTreeNode::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.remove(position);

	foreach (SyntaxTreeNode *child, childItems)
		child->removeColumns(position, columns);

	return true;
}

bool SyntaxTreeNode::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}

SyntaxTreeNode* SyntaxTreeNode::findChild( const QVector<QVariant>& data )
{
	SyntaxTreeNode* ch = NULL;
	int ind = 0;
	while ((ch == NULL) && (ind < childCount()))
	{
		SyntaxTreeNode* tmp = child(ind);
		bool equal = true;
		int ii = 0;
		while(equal && (ii < tmp->columnCount()))
		{
			equal = (tmp->data(ii) == data[ii]);
			++ii;
		}
		if (equal)
			ch = tmp;
		else
			++ind;
	}
	return ch;
}

SyntaxTreeModel::SyntaxTreeModel(SyntaxTreeNode* root,QObject *parent)
:QAbstractItemModel(parent),rootItem(root)
{
}

SyntaxTreeModel::~SyntaxTreeModel()
{
	delete rootItem;
}

int SyntaxTreeModel::columnCount(const QModelIndex & /* parent */) const
{
	return rootItem->columnCount();
}

QVariant SyntaxTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	SyntaxTreeNode *item = getItem(index);

	return item->data(index.column());
}

Qt::ItemFlags SyntaxTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

SyntaxTreeNode *SyntaxTreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		SyntaxTreeNode *item = static_cast<SyntaxTreeNode*>(index.internalPointer());
		if (item) return item;
	}
	return rootItem;
}

QVariant SyntaxTreeModel::headerData(int section, Qt::Orientation orientation,
									 int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex SyntaxTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	SyntaxTreeNode *parentItem = getItem(parent);

	SyntaxTreeNode *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

bool SyntaxTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginInsertColumns(parent, position, position + columns - 1);
	success = rootItem->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool SyntaxTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	SyntaxTreeNode *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}

QModelIndex SyntaxTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	SyntaxTreeNode *childItem = getItem(index);
	SyntaxTreeNode *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool SyntaxTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginRemoveColumns(parent, position, position + columns - 1);
	success = rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return success;
}

bool SyntaxTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	SyntaxTreeNode *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int SyntaxTreeModel::rowCount(const QModelIndex &parent) const
{
	SyntaxTreeNode *parentItem = getItem(parent);

	return parentItem->childCount();
}

bool SyntaxTreeModel::setData(const QModelIndex &index, const QVariant &value,
							  int role)
{
	if (role != Qt::EditRole)
		return false;

	SyntaxTreeNode *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index);

	return result;
}

bool SyntaxTreeModel::setHeaderData(int section, Qt::Orientation orientation,
									const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	bool result = rootItem->setData(section, value);

	if (result)
		emit headerDataChanged(orientation, section, section);

	return result;
}

//void SyntaxTreeModel::addCompleteSubTree(const QStringList &signatures)
//{
//	QStringList signs = signatures;
//	foreach(QString sg,signs)
//		createAndAppendBranch(sg,rootItem);
//}

//void SyntaxTreeModel::createAndAppendBranch( QString& st,SyntaxTree* parent )
//{
//	if (st.isEmpty() || (parent == NULL))
//		return;
//	QVector<QVariant> dt;
//	int indexpoint = st.indexOf(".");
//	if (indexpoint == -1)
//	{
//		int indexpar = st.indexOf("(");
//		if (indexpar == -1)
//		{
//			//is a member
//			dt << st;
//		}
//		else 
//		{
//			//is a function. I will add the name of the function and the signature for the tooltip
//			dt << st.left(indexpar)/* << st*/;
//		}
//		AutoCompleterItem* ch = parent->findChild(dt);
//		//Search if the node is already in the tree
//		if (ch == NULL)
//		{
//			ch = new AutoCompleterItem(dt,parent);
//			parent->appendChild(ch);
//		}
//		return;
//	}
//	else
//	{
//		//+1 so I will take also the .
//		QString tmp  = st.left(indexpoint + 1);
//		st.remove(tmp);
//		tmp.remove(".");
//		dt << tmp;
//		int ind = 0;
//
//		AutoCompleterItem* ch = parent->findChild(dt);
//		//Search if the node is already in the tree
//		if (ch == NULL)
//		{
//			ch = new AutoCompleterItem(dt,parent);
//			parent->appendChild(ch);
//		}
//		createAndAppendBranch(st,ch);
//	}
//}

MLScriptLanguage::MLScriptLanguage()
:reserved(),langfuncs(),worddelimiter(),wordsjoiner(),openpar(),closepar(),libraries()
{
	initLibrary();
}



const SyntaxTreeModel* MLScriptLanguage::functionsLibrary() const
{
	return libraries;
}

SyntaxTreeModel* MLScriptLanguage::functionsLibrary()
{
	return libraries;
}

void MLScriptLanguage::addLibrary( const QList<LibraryElementInfo>& funsigns )
{
	if (libraries != NULL)
	{
		SyntaxTreeNode* root = libraries->getItem(QModelIndex());
		foreach(LibraryElementInfo st,funsigns)
			addBranch(st,root);
	}
}

void MLScriptLanguage::addBranch( const LibraryElementInfo& mi,SyntaxTreeNode* parent )
{
	if (mi.completename.isEmpty() || (parent == NULL))
		return;
	QString st = mi.completename;
	QVector<QVariant> dt(5,QVariant(QString()));
	int indexpoint = st.indexOf(wordsjoiner);
	if (indexpoint == -1)
	{
		int indexpar = st.indexOf(openpar);
		if (indexpar == -1)
		{
			//is a member
			dt[0] = st;
			dt[1] = mi.help;
			dt[4] = QString::number(MLScriptLanguage::MEMBERFIELD);
		}
		else 
		{
			//is a function. I will add the name of the function and the signature for the tooltip
			dt[0] = st.left(indexpar);
			dt[1] = mi.help;
			dt[3] = st;
			dt[4] = QString::number(MLScriptLanguage::FUNCTION);
		}
		SyntaxTreeNode* ch = parent->findChild(dt);

		//Search if the node is already in the tree
		if (ch == NULL)
		{
			parent->insertChildren(parent->childCount(),1,parent->columnCount());
			ch = parent->child(parent->childCount() - 1);
			for(int ii = 0;ii < 5;++ii)
				ch->setData(ii,dt[ii]);
		}
		return;
	}
	else
	{
		//+1 so I will take also the sep
		QString tmp  = st.left(indexpoint + 1);
		st.remove(tmp);
		QString specificsep = wordsjoiner.cap();
		tmp.remove(wordsjoiner);
		dt[0] = tmp;
		dt[2] = specificsep;
		dt[4] = QString::number(MLScriptLanguage::NAMESPACE);
		SyntaxTreeNode* ch = parent->findChild(dt);
		//Search if the node is already in the tree
		if (ch == NULL)
		{
			parent->insertChildren(parent->childCount(),1,parent->columnCount());
			ch = parent->child(parent->childCount() - 1);
			for(int ii = 0;ii < 5;++ii)
				ch->setData(ii,dt[ii]);
		}
		LibraryElementInfo minext;
		minext.completename = st;
		minext.help = mi.help; 
		addBranch(minext,ch);
	}
}

MLScriptLanguage::~MLScriptLanguage()
{

}


QString MLScriptLanguage::getExternalLibrariesCode()
{
	QString code;
	QList<ExternalLib*> liblist = this->scriptLibraryFiles();
	for(int ii = 0;ii <liblist.size();++ii)
		code += liblist[ii]->libCode();
	return code;
}

void MLScriptLanguage::initLibrary()
{
	delete libraries;
	QVector<QVariant> v;
	v.push_back("partial function ID");
	v.push_back("help");
	v.push_back("separator");
	v.push_back("signature");
	v.push_back("token");
	SyntaxTreeNode* root = new SyntaxTreeNode(v,NULL);
	libraries = new SyntaxTreeModel(root,NULL);
}

QList<LibraryElementInfo> MLScriptLanguage::getExternalLibrariesMembersInfo() const
{
	QList<LibraryElementInfo> res;
	QList<ExternalLib*> liblist = this->scriptLibraryFiles();
	for(int ii = 0;ii <liblist.size();++ii)
		res.append(liblist[ii]->libraryMembersInfo());
	return res;
}

QRegExp MLScriptLanguage::matchIdentifiersButNotReservedWords() const
{
	QString res = reserved.join("|");
	QRegExp name("([a-z]|[A-Z])+\\w*");
	QRegExp nokey("\\b(?!(?:" + res + ")\\b)");
	return QRegExp(nokey.pattern() + name.pattern() + "\\b(\\s*" + wordsjoiner.pattern() + "\\s*" + nokey.pattern() + name.pattern() + "\\b)*");
}

QRegExp MLScriptLanguage::matchOnlyReservedWords() const
{
	QString res = reserved.join("|");
	return QRegExp("\\b(" + res + ")\\b"); 
}

QRegExp MLScriptLanguage::joinedWordExpression() const
{
	//*\\*/(\\(\\w*\\))?
	QRegExp parameter("(\\w*|\\d*(\\.\\d+)*)");
	QRegExp parameterlist(parameter.pattern() + "(\\s*,\\s*" + parameter.pattern() + ")*");
	QRegExp name("([a-z]|[A-Z])+\\w*(\\(" + parameterlist.pattern() + "\\))?");
	QString st(name.pattern() + "(\\s*" + wordsjoiner.pattern() +"\\s*|\\s*" + wordsjoiner.pattern() +"\\s*"  + name.pattern() + ")*");
	return QRegExp(st);
	
}

JavaScriptLanguage::JavaScriptLanguage()
:MLScriptLanguage()
{
	//HighlightingRule res;
	//res.format.setForeground(Qt::darkBlue);
	//res.format.setFontWeight(QFont::Bold);
	wordsjoiner.setPattern("\\.");
	openpar.setPattern("\\(");
	closepar.setPattern("\\)");
	reserved << "break";
	reserved << "case";	
	reserved << "catch";	
	reserved << "continue";
	reserved << "default";
	reserved << "delete";
	reserved << "do";
	reserved << "else";
	reserved << "finally";
	reserved << "for";
	reserved << "function";
	reserved << "if";
	reserved << "in";
	reserved << "instanceof";
	reserved << "new";
	reserved << "return";
	reserved << "switch";
	reserved << "this";
	reserved << "throw";
	reserved << "try";
	reserved << "typeof";
	reserved << "var";
	reserved << "void";
	reserved << "while";
	reserved << "with";
	reserved << "true";
	reserved << "false";
	reserved << "null";

	worddelimiter.setPattern("[\\s|\\t|\\n|\\r|=|;|,|\\(|\\)|{|}|\\[|\\]|\\||\\&|\\?|\\!|\\+|\\*|\\\\|\\-|%|\"|<|>]");

	//foreach(QString st,reserved)
	//{
	//	res.pattern = QRegExp(st);
	//	highlightingRules << res;
	//}

	//autoCompleteModel(mod);
	////comp.setModel(&ls);
	//comp.setCaseSensitivity(Qt::CaseSensitive);
	//comp.setWidget(parent);
	//comp.setCompletionMode(QCompleter::PopupCompletion);
	//comp.setModel(&mod);
}

const QList<ExternalLib*> JavaScriptLanguage::scriptLibraryFiles() const
{
	QList<ExternalLib*> res;
	SGLMathLib* lib = new SGLMathLib();
	res << lib;
	return res;
}

//QStringList JavaScriptLanguage::splitLineInWords( const QStringList& line )
//{
//	//Regular expression matching all the identifiers but not keywords
//	QRegExp exp = matchIdentifiersButNotReservedWords();
//	int index = 0;
//	while (index >= 0)
//	{
//		line 
//	}
//}

QString ExternalLib::libCode() const
{
	QFile lib(name);
	if (!lib.open(QFile::ReadOnly))
		qDebug("Warning: Library %s has not been loaded.",qPrintable(name));
	QByteArray libcode = lib.readAll();
		/*QScriptValue res = env.evaluate(QString(libcode));
		if (res.isError())
		throw JavaScriptException("Library " + liblist[ii] + " generated a JavaScript Error: " + res.toString() + "\n");*/
	return QString(libcode);
}

ExternalLib::ExternalLib( const QString& filename )
:name(filename)
{
}

SGLMathLib::SGLMathLib()
:ExternalLib(":/script_system/math.js")
{
	
}

QList<LibraryElementInfo> SGLMathLib::libraryMembersInfo() const
{
	QString code = libCode();
	QList<LibraryElementInfo> res;
	int index = 0;
	QRegExp parameter("\\w*");
	QRegExp parameterlist(parameter.pattern() + "(\\s*,\\s*" + parameter.pattern() + ")*");
	QRegExp namespacelist(parameter.pattern() + "(\\s*\\.\\s*" + parameter.pattern() + ")*\\$?");
	QRegExp help("/\\*([^*]|[\\r\\n]|(\\*+([^*/]|[\\r\\n])))*\\*+/");
	QRegExp exp("(" + help.pattern() + ")?" + "\\s*" + namespacelist.pattern() + "\\s*=\\s*function\\s*\\(" + parameterlist.pattern() + "\\)");
	int ii = 0;
	do
	{
		index = code.indexOf(exp,index) + exp.matchedLength();
		if (index >= 0)
		{
			QString fun = exp.cap();
			LibraryElementInfo mi; 
			int helpind = fun.indexOf(help);
			if (helpind >= 0)
			{
				mi.help = help.cap();
				mi.help.remove(QRegExp("\\*\\s+\\n"));
				fun.remove(help.cap());
			}
			fun.remove(QRegExp("\\s*")).remove("=").remove("function");
			mi.completename = fun;
			res << mi;
			++ii;
		}
	}while(index >= 0);
	return res;
}


