#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStack>
#include <QPair>
#include <QString>
#include <QStringList>

class RibFileStack {
	
public:
	//RibFileStack() : this(QString("")) {};
	RibFileStack(QString* name, QString* dir);
	~RibFileStack();
	bool pushFile(QString* path);
	bool pushFile(QFile* file);
	QString topNextLine();
	bool isEmpty();
private:
	QStack< QPair<QFile*, QTextStream*>* >* stack;
	QString templateName;
	QString templateDir;
	QStringList subDir;
	bool popFile();
};
