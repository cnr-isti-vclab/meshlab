#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStack>
#include <QPair>
#include <QString>
#include <QStringList>

class RibFileStack {
	
public:
	RibFileStack(QString* name, QString* dir);
	~RibFileStack();
	bool pushFile(QString* path);
	bool pushFile(QFile* file);
	QString topNextLine();
	bool isEmpty();
	bool addSubDirs(QStringList dirs);	
private:
	QStack< QPair<QFile*, QTextStream*>* >* stack;
	QString templateName;
	QString templateDir;
	bool popFile();
	QStringList subDir;
};
