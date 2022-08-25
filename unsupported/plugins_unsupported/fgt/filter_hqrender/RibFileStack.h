#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStack>
#include <QPair>
#include <QString>
#include <QStringList>
#include "ribProcedure.h"

class RibFileStack {
	
public:
	RibFileStack(QString dir);
	~RibFileStack();
  bool pushFile(QString path);
	bool pushFile(QFile* file);
	bool hasNext() const;
	bool addSubDirs(QStringList dirs);
	QString nextStatement(int* type, bool manageReadArchive = true);
	bool readArchive(QString line);
  bool searchFile(QString filename);
private:
	QStack< QPair<QFile*, QTextStream*>* >* stack;
	QString templateDir;
	QStringList subDir;
	bool popFile();
	int isRibProcedure(QString line) const;
	bool inString(QString line) const;
	QString nextLine;
	int nextLineType;
	QString topNextLine(bool* end);

	QHash<QString, int>* ribProc;


};
