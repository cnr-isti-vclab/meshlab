
#line 90 "js_parser.g"

#ifndef JS_PARSER_H
#define JS_PARSER_H

#include "qparser.h"
#include <QtCore>
#include "js_grammar_p.h"

union UnionValue 
{
	int i;
	unsigned u;
	unsigned long ul;
	unsigned long long ull;
	long l;
	double d;
	float f;
	const QString *s;
};

class JSParser: public QParser<JSParser, js_grammar,UnionValue>
{
public:
	JSParser();
	int nextToken();
	void consumeRule(int ruleno);
	const QString *insert(const QString &s);
    
protected:
	QSet<QString> string_repository;
    QSet<const QString*> types;
	
};

#endif // JS_PARSER_H

#line 192 "js_parser.g"

#define Q_SCRIPT_REGEXPLITERAL_RULE1 7

#line 201 "js_parser.g"

#define Q_SCRIPT_REGEXPLITERAL_RULE2 8
