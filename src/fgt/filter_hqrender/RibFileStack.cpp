#include "RibFileStack.h"

RibFileStack::RibFileStack(QString dir) {
	ribProc = ribParser::initHash(); //hash of rib procedure
	stack = new QStack< QPair<QFile*, QTextStream*>* >();
	templateDir = dir;
	subDir.append("."); //to search in the same dir
};

RibFileStack::~RibFileStack() {
	delete ribProc;	
	while(!stack->isEmpty())
		popFile(); //close all file
	delete stack; //it's enough?
};

//open file and push the text stream to stack
bool RibFileStack::pushFile(QString path) {
  if(QFile::exists(path)) {
    QFile* file = new QFile(path);
		return pushFile(file);
	} else {
		return false;
	}
};

bool RibFileStack::pushFile(QFile* file) {
	//add file to stack and open it
	if(!(*file).open(QIODevice::ReadOnly)) {
		return false;
	}
	QTextStream* stream = new QTextStream(file);
	QPair<QFile*, QTextStream*>* p = new QPair<QFile*, QTextStream*>(file, stream);
	stack->push(p);
	if(stack->count() == 1) { //if it's the first prepare next line
		bool end;
		nextLine = topNextLine(&end);
		nextLineType = isRibProcedure(nextLine);
	}
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
QString RibFileStack::topNextLine(bool* end) {
	if(stack->isEmpty()) {
		*end = true;
		return "";
	}
	//take the top element from stack
	QPair<QFile*, QTextStream*>* p = stack->top();
	//the stream of file
	QTextStream* s = (p->second);
	if(!s->atEnd()) {
		*end = false;
		QString str = s->readLine();
		if(str == "")
			return topNextLine(end); //ignore empty line
		else
			return str;
	} else {
		//close and remove from stack the file
		popFile();
		//recursive call (the top is changed)
		return topNextLine(end);
	}
};

//return the next statement of file stack (maybe on more line)
QString RibFileStack::nextStatement(int* type, bool manageReadArchive) {	
	if(nextLine == "") {
		bool end = false;
		nextLine = topNextLine(&end);
		if(end) {
			*type = ribParser::NOMORESTATEMENT;
			return nextLine; //no more line to be readable
		}
		else
			*type = isRibProcedure(nextLine);
	}
	QString str = nextLine; //nextline is a rib procedure	
	*type = nextLineType;

	if(*type == ribParser::READARCHIVE) {
		//qDebug("is a ReadArchive");
		QStringList token = ribParser::splitStatement(&str);
		//0: ReadArchive,
		//1: " ,
		//2: <string> ,
		//3: "
		bool end;
		while(token.size() < 4 && !end) {
			QString line = topNextLine(&end);
			str += " " + line;
			token = ribParser::splitStatement(&str);
		}
		if(manageReadArchive) {
			if(readArchive(str)) { //add the new file to stack
				nextLine = topNextLine(&end);
				nextLineType = isRibProcedure(nextLine);
				//qDebug("Readed: %s Type: %d",qPrintable(str), *type);
				//qDebug("nextline: %s type: %d",qPrintable(nextLine),nextLineType);
				return nextStatement(type, manageReadArchive); //read the first line of new file, if found it
			}
      else  {
			  nextLine = "";
        nextLineType = ribParser::NOMORESTATEMENT;
        return str;
      }
		}
    else {
      nextLine = "";
      nextLineType = ribParser::NOMORESTATEMENT;  		
      return str;
    }
	}
	else { //it isn't a ReadArchive procedure
		bool end;
		nextLine = topNextLine(&end); //take next line
		if(!inString(str))
			nextLineType = isRibProcedure(nextLine);
		else //maybe a string with a procedure name
			nextLineType = ribParser::NOTAPROCEDURE;
		while(nextLine != "" && nextLineType == ribParser::NOTAPROCEDURE ) {
			//qDebug("nextline: %s type: %d",qPrintable(nextLine),nextLineType);	
			str += "\n" + nextLine; //if isn't a rib procedure append to str
			nextLine = topNextLine(&end);
			if(!inString(str))
				nextLineType = isRibProcedure(nextLine); //if there's a string opened, don't test if it's a procedure
		}
		//qDebug("Readed: %s Type: %d",qPrintable(str), *type);
		//qDebug("nextline: %s type: %d",qPrintable(nextLine),nextLineType);
		return str;
	}
}

//parse the statement, search and add the new file to stack
bool RibFileStack::readArchive(QString line) {
	QStringList token = ribParser::splitStatement(&line);
	//test if it's a statement to open a new file
	if(ribProc->value(token[0]) == ribParser::READARCHIVE) {
		//if it's a ReadArchive statement search the next file to parse
		QString filename = token[2]; //token[2] is the file name
		//search in subdir list
    return searchFile(filename);		
	}
	return false;
}

//search file in subdir and add to stack
bool RibFileStack::searchFile(QString filename) {
  //search in subdir list
	for(int i=0; i<subDir.size(); i++) {
		QString str(templateDir + QDir::separator() + (subDir)[i] + QDir::separator() + filename);
		//qDebug("looking for: %s",qPrintable(str));
    if(pushFile(str)) {
			return true;
		}
	}
  return false;
}

//true if in line there's a string opened and not closed
bool RibFileStack::inString(QString line) const {
	if(!line.contains('\"'))
		return false;
	line = line.trimmed();
	int a = 0;
	bool slash = false;
	for(int i=0; i<line.size(); i++) {
		if(!slash && line[i] == '\"') {
			a++;
			slash = false;
		}
		else {
			if(!slash && line[i] == '\\')
				slash = true;
			else
				slash = false;
		}
	}
	return a%2 != 0;
}

//check if is't a rib procedure (in ribProc table)
int RibFileStack::isRibProcedure(QString line) const {
	if(line == "")
		return ribParser::NOTAPROCEDURE;
	if(line.trimmed().startsWith('#')) //RibProcedure " ...\n#..." how does appen?
		return ribParser::COMMENT;
	QString token = line.left(line.indexOf(' ')).trimmed(); 
	return ribProc->value(token);
	
}

bool RibFileStack::hasNext() const {
	return !stack->isEmpty();
};

//append a directory list to the list of stack
bool RibFileStack::addSubDirs(QStringList dirs) {
	foreach(QString dir,dirs)
		subDir.append(dir);
	return true;
};

