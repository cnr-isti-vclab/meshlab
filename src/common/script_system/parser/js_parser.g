%parser js_grammar
%decl js_parser.h
%impl js_parser.cpp

%token T_COLON
%token T_COMMA
%token T_DOT
%token T_EQ
%token T_FUNCTION
%token T_IDENTIFIER
%token T_LBRACE
%token T_LBRACKET
%token T_LPAREN
%token T_NEW
%token T_RBRACE
%token T_RBRACKET
%token T_RPAREN
%token T_SEMICOLON
%token T_STRING_LITERAL
%token T_THIS
%token T_NULL
%token T_VAR
%token T_CONST
%token T_NUMERIC_LITERAL

%start Program

/:
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

class JSParser: public QParser<JSParser, $table,UnionValue>
{
public:
	JSParser();
	int nextToken();
	void consumeRule(int ruleno);
	const QString *insert(const QString &s);
    
protected:
	QSet<QString> string_repository;
    QSet<const QString*> types;
	QSet<QString> functions;
};

#endif // JS_PARSER_H
:/


/.
#include "js_parser.h"

#include <QtDebug>
#include <cstdlib>

JSParser::JSParser()
:string_repository(),types()
{
}

const QString *JSParser::insert(const QString &s)
{
	return &*string_repository.insert(s);
}

void JSParser::consumeRule(int ruleno)
  {
    switch (ruleno) {
./

Program: StatementList;
/.
case $rule_number: {
} break;
./

StatementList: ;
StatementList: StatementList Statement;
/.
case $rule_number: {
} break;
./

Statement: Block;
/.
case $rule_number: {
} break;
./

Statement: VariableDecl;
/.
case $rule_number: {
} break;
./

Statement: FunctionDecl;
/.
case $rule_number: {
} break;
./
Statement: FunctionCallStatement;
/.
case $rule_number: {
} break;
./

Block: OpenBlock StatementList CloseBlock;
/.
case $rule_number: {
} break;
./

OpenBlock: T_LBRACKET; 
/.
case $rule_number: {
} break;
./

CloseBlock: T_RBRACKET;
/.
case $rule_number: {
} break;
./

VariableDecl: DeclTok IdTok InitOpt OtherDeclOpt T_SEMICOLON;
/.
case $rule_number: {
} break;
./

FunctionCallStatement: FunctionCall T_SEMICOLON;
/.
case $rule_number: {
} break;
./

OtherDeclOpt: ;
OtherDeclOpt: OtherDeclOpt T_COMMA IdTok InitOpt;
/.
case $rule_number: {
} break;
./

DeclTok: T_VAR;
/.
case $rule_number: {
} break;
./

DeclTok: T_CONST;
/.
case $rule_number: {
} break;
./

InitOpt: ;
/.
case $rule_number: {
} break;
./

InitOpt: T_EQ Expression;
/.
case $rule_number: {
} break;
./

FunctionDecl: T_FUNCTION IdTok OpenBracket OptParamList CloseBracket OpenBlock StatementList CloseBlock;
/.
case $rule_number: {
	functions.insert(symbol(2));
} break;
./

OptParamList: ; 
OptParamList: IdTok OtherOptParamList;
/.
case $rule_number: {
} break;
./

OtherOptParamList: ;
OtherOptParamList: OtherOptParamList T_COMMA IdTok;
/.
case $rule_number: {
} break;
./

Expression: MemberExpr;
/.
case $rule_number: {
} break;
./

Expression: ThisExpr;
/.
case $rule_number: {
} break;
./

Expression: NewExpr;
/.
case $rule_number: {
} break;
./

Expression: FunctionExpr;
/.
case $rule_number: {
} break;
./

Expression: FunctionCall;
/.
case $rule_number: {
} break;
./

Expression: ObjConstExpr;
/.
case $rule_number: {
} break;
./

Expression: ArrayExpr;
/.
case $rule_number: {
} break;
./

Expression: T_NULL;
/.
case $rule_number: {
} break;
./

Expression: T_STRING_LITERAL;
/.
case $rule_number: {
} break;
./

Expression: T_NUMERIC_LITERAL;
/.
case $rule_number: {
} break;
./

OptIdTok: ;
OptIdTok: IdTok;
/.
case $rule_number: {
} break;
./

MemberExpr: IdTok;
/.
case $rule_number: {
} break;
./

MemberExpr: IdTok T_DOT MemberExpr;
/.
case $rule_number: {
} break;
./

ThisExpr: ThisTok T_DOT MemberExpr;
/.
case $rule_number: {
} break;
./

ThisTok: T_THIS;
/.
case $rule_number: {
} break;
./

IdTok: T_IDENTIFIER;
/.
case $rule_number: {
} break;
./

NewTok: T_NEW;
/.
case $rule_number: {
} break;
./
 
NewExpr: NewTok IdTok OpenBracket OptArgumentList CloseBracket;
/.
case $rule_number: {
} break;
./

FunctionExpr: T_FUNCTION OptIdTok OpenBracket OptParamList CloseBracket OpenBlock StatementList CloseBlock;
/.
case $rule_number: {
} break;
./

FunctionCall: MemberExpr OpenBracket OptExpressionList CloseBracket;
/.
case $rule_number: {
} break;
./

FunctionCall: ThisExpr OpenBracket OptExpressionList CloseBracket;
/.
case $rule_number: {
} break;
./

OptExpressionList: ;
OptExpressionList: Expression OtherOptExpressionList;
/.
case $rule_number: {
} break;
./

OtherOptExpressionList: ;
/.
case $rule_number: {
} break;
./

OtherOptExpressionList: OtherOptExpressionList T_COMMA Expression;
/.
case $rule_number: {
} break;
./

OptArgumentList: ;
OptArgumentList: IdTok OtherOptArgumentList;
/.
case $rule_number: {
} break;
./

OtherOptArgumentList: ;
OtherOptArgumentList: OtherOptArgumentList T_COMMA IdTok;
/.
case $rule_number: {
} break;
./


ObjConstExpr: OpenBlock IdTok T_COLON Expression OtherOptFields CloseBlock;
/.
case $rule_number: {
} break;
./

OtherOptFields: ;
OtherOptFields: OtherOptFields T_COMMA IdTok T_COLON Expression;
/.
case $rule_number: {
} break;
./

ArrayExpr: OpenArrBracket Expression OtherOptExpressionList CloseArrBracket;
/.
case $rule_number: {
} break;
./

OpenBracket: T_LPAREN;
/.
case $rule_number: {
} break;
./

CloseBracket: T_RPAREN;
/.
case $rule_number: {
} break;
./

OpenArrBracket: T_LBRACE;
/.
case $rule_number: {
} break;
./

CloseArrBracket: T_RBRACE;
/.
case $rule_number: {
} break;
./

/.
    } // switch
}
./