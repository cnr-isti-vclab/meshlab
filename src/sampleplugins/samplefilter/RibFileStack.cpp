#include <RibFileStack.h>

/*RibFileStack::QFileStack() {
	stack = new QStack< QPair<QFile*, QTextStream*> >();
};*/

RibFileStack::RibFileStack(QString* templateName, QString* dir) {
	stack = new QStack< QPair<QFile*, QTextStream*>* >();
	this->templateName = *templateName;
	this->templateDir = *dir;
};

RibFileStack::~RibFileStack() {
	//close all file
	while(!stack->isEmpty())
		popFile();
	delete stack; //basta?
	/*
	while(popFile()); delete stack;
	*/
};

//open file and push the text stream to stack
bool RibFileStack::pushFile(QString* path) {
	if(QFile::exists(*path)) {
		QFile* file = new QFile(*path);
		return pushFile(file);
	} else {
		return false;
	}
};

bool RibFileStack::pushFile(QFile* file) {
	if(!(*file).open(QIODevice::ReadOnly)) {
		return false;
	}
	QTextStream* stream = new QTextStream(file);
	QPair<QFile*, QTextStream*>* p = new QPair<QFile*, QTextStream*>(file, stream);
	stack->push(p);
	return true;
};

//close file and pop from stack
bool RibFileStack::popFile() {
	if(stack->isEmpty())
		return false;

	QPair<QFile*, QTextStream*>* p = stack->pop();
	(*(p->first)).close();
	return true;
};

//return the next line from file at top of stack
//if it's the end of this file, pop it and take the line of next
//if it's a ReadArchive statement, add the new file at the top of stack
QString RibFileStack::topNextLine() {
	if(stack->isEmpty())
		return QString::null;

	//take the top element from stack
	QPair<QFile*, QTextStream*>* p = stack->top();
	//the stream of file
	QTextStream* s = (p->second);
	if(!s->atEnd()) {
		QString line = s->readLine();
		QStringList token = line.split(' ');
		//test if it's a statement to open a new file
		if(token[0].trimmed() == "ReadArchive") {
			//if it's a ReadArchive statement search the next file to parse
			//QString path = QDir::currentPath();
			//search in subdir list
			bool found = false;
			//token[1] is the file name
			for(int i=0; i<subDir.size() && !found; i++) {
				token[1] = token[1].trimmed();
				//QString str(path + QDir::separator() + (subDir)[i] + QDir::separator() + token[1].remove('\"'));
				QString str(templateDir + QDir::separator() + (subDir)[i] + QDir::separator() + token[1].remove('\"'));
				if(pushFile(&str)) {
					found = true;
				}
			}
			if(found)
				//recursive call (the top is changed)
				return topNextLine();
			//else
				//return the line "ReadArchive ..."
		}
		//test if it's a statement to add a new sub-directory
		if(token[0].trimmed() == "Option") {
			if(token[1].trimmed() == "\"searchpath\"") {
				if(token[2].trimmed() == "\"string" && token[3].trimmed() == "archive\"") { //"string archive"
					//it's the subdir location of rib archives
					//assume that the format is [".:dir1:dir2"]
					QString str = token[4];
					if(token.size()>5) {
						for(int i= 5; i<token.size(); i++)
							str += " " + token[i]; //the remainig token are joined together
					}
					str = str.simplified();
					if(str.startsWith('[')) {
						if(str.endsWith(']')) {
							//remove the character [ ] "
							str.remove('[');
							str.remove(']');
							str.remove('\"');
							str = str.simplified();
							QStringList dirs = str.split(':');
							//add the found dir to the list of subdirectory
							for(int i=0; i<dirs.size(); i++) {
								//subDir.append(templateName + QDir::separator() + dirs[i]); //why did I add templateName???
								subDir.append(dirs[i]);
							}
						} else {
							//an array can be contains the \n character.... :/
						}
					} else {
						//the statement is: 'Option "string archive"' without an array =>do nothing
					}
				}
			}
		}
		return line;
	} else {
		//close and remove from stack the file
		popFile();
		//recursive call (the top is changed)
		return topNextLine();
	}
};

bool RibFileStack::isEmpty() {
	return stack->isEmpty();
};
