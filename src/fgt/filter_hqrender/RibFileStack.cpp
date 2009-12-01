#include "RibFileStack.h"

RibFileStack::RibFileStack(QString dir) {
	stack = new QStack< QPair<QFile*, QTextStream*>* >();
	this->templateDir = dir;
	subDir.append("."); //to search in the same dir
};

RibFileStack::~RibFileStack() {
	//close all file
	while(!stack->isEmpty())
		popFile();
	delete stack; //it's enough?
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
			//search in subdir list
			bool found = false;
			//token[1] is the file name
			for(int i=0; i<subDir.size() && !found; i++) {
				token[1] = token[1].trimmed();
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

bool RibFileStack::addSubDirs(QStringList dirs) {
	foreach(QString dir,dirs)
		subDir.append(dir);
	return true;
};
