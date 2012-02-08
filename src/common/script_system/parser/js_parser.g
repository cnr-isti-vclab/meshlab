%parser js_grammar
%decl js_parser.h
%impl js_parser.cpp

%token T_AND
%token T_AND_AND
%token T_AND_EQ
%token T_BREAK
%token T_CASE
%token T_CATCH
%token T_COLON
%token T_COMMA
%token T_CONTINUE
%token T_DEFAULT
%token T_DELETE
%token T_DIVIDE_
%token T_DIVIDE_EQ
%token T_DO
%token T_DOT
%token T_ELSE
%token T_EQ
%token T_EQ_EQ
%token T_EQ_EQ_EQ
%token T_FINALLY
%token T_FOR
%token T_FUNCTION
%token T_GE
%token T_GT
%token T_GT_GT
%token T_GT_GT_EQ
%token T_GT_GT_GT
%token T_GT_GT_GT_EQ
%token T_IDENTIFIER
%token T_IF
%token T_IN
%token T_INSTANCEOF
%token T_LBRACE
%token T_LBRACKET
%token T_LE
%token T_LPAREN
%token T_LT
%token T_LT_LT
%token T_LT_LT_EQ
%token T_MINUS
%token T_MINUS_EQ
%token T_MINUS_MINUS
%token T_NEW
%token T_NOT
%token T_NOT_EQ 
%token T_NOT_EQ_EQ
%token T_OR
%token T_OR_EQ
%token T_OR_OR
%token T_PLUS
%token T_PLUS_EQ
%token T_PLUS_PLUS
%token T_QUESTION
%token T_RBRACE
%token T_RBRACKET
%token T_REMAINDER
%token T_REMAINDER_EQ
%token T_RETURN
%token T_RPAREN
%token T_SEMICOLON
%token T_STAR
%token T_STAR_EQ
%token T_STRING_LITERAL
%token T_SWITCH
%token T_THIS
%token T_THROW
%token T_TILDE
%token T_TRY
%token T_TYPEOF
%token T_VAR
%token T_VOID
%token T_WHILE
%token T_WITH
%token T_XOR
%token T_XOR_EQ
%token T_NULL
%token T_TRUE
%token T_FALSE	
%token T_CONST
%token T_DEBUGGER
%token T_NUMERIC_LITERAL
%token T_RESERVED_WORD

%start Program

/:
#ifndef JS_PARSER_H
#define JS_PARSER_H

#include "qparser.h"
#include <QtCore>
#include "js_grammar_p.h"

union Value 
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

class JSParser: public QParser<JSParser, $table,Value>
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

PrimaryExpression: T_THIS ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_IDENTIFIER ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_NULL ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_TRUE ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_FALSE ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_STRING_LITERAL ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_DIVIDE_ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE1 $rule_number
:/
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_DIVIDE_EQ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE2 $rule_number
:/
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_LBRACKET ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_RBRACKET ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_COMMA ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
} break;
./

-- PrimaryExpression: T_LBRACE T_RBRACE ;
-- /.
-- case $rule_number: {
-- } break;
-- ./

PrimaryExpression: T_LBRACE PropertyNameAndValueListOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_LBRACE PropertyNameAndValueList T_COMMA T_RBRACE ;
/.
case $rule_number: {
} break;
./

PrimaryExpression: T_LPAREN Expression T_RPAREN ;
/.
case $rule_number: {
} break;
./

ElementList: ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
} break;
./

ElementList: ElementList T_COMMA ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
} break;
./

Elision: T_COMMA ;
/.
case $rule_number: {
} break;
./

Elision: Elision T_COMMA ;
/.
case $rule_number: {
} break;
./

ElisionOpt: ;
/.
case $rule_number: {

} break;
./

ElisionOpt: Elision ;
/.
case $rule_number: {
} break;
./

PropertyNameAndValueList: PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
} break;
./

PropertyNameAndValueList: PropertyNameAndValueList T_COMMA PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
} break;
./

PropertyName: T_IDENTIFIER ;
/.
case $rule_number: {
} break;
./

PropertyName: T_STRING_LITERAL ;
/.
case $rule_number: {
} break;
./

PropertyName: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
} break;
./

PropertyName: ReservedIdentifier ;
/.
case $rule_number: {
} break;
./

ReservedIdentifier: T_BREAK ;
/.
case $rule_number:
./
ReservedIdentifier: T_CASE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CATCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONTINUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEFAULT ;
/.
case $rule_number:
./
ReservedIdentifier: T_DELETE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DO ;
/.
case $rule_number:
./
ReservedIdentifier: T_ELSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FALSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FINALLY ;
/.
case $rule_number:
./
ReservedIdentifier: T_FOR ;
/.
case $rule_number:
./
ReservedIdentifier: T_FUNCTION ;
/.
case $rule_number:
./
ReservedIdentifier: T_IF ;
/.
case $rule_number:
./
ReservedIdentifier: T_IN ;
/.
case $rule_number:
./
ReservedIdentifier: T_INSTANCEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_NEW ;
/.
case $rule_number:
./
ReservedIdentifier: T_NULL ;
/.
case $rule_number:
./
ReservedIdentifier: T_RETURN ;
/.
case $rule_number:
./
ReservedIdentifier: T_SWITCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_THIS ;
/.
case $rule_number:
./
ReservedIdentifier: T_THROW ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRY ;
/.
case $rule_number:
./
ReservedIdentifier: T_TYPEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_VAR ;
/.
case $rule_number:
./
ReservedIdentifier: T_VOID ;
/.
case $rule_number:
./
ReservedIdentifier: T_WHILE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONST ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEBUGGER ;
/.
case $rule_number:
./
ReservedIdentifier: T_RESERVED_WORD ;
/.
case $rule_number:
./
ReservedIdentifier: T_WITH ;
/.
case $rule_number:
{
} break;
./

PropertyIdentifier: T_IDENTIFIER ;
PropertyIdentifier: ReservedIdentifier ;

MemberExpression: PrimaryExpression ;
MemberExpression: FunctionExpression ;

MemberExpression: MemberExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
} break;
./

MemberExpression: MemberExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
} break;
./

MemberExpression: T_NEW MemberExpression Arguments ;
/.
case $rule_number: {
} break;
./

NewExpression: MemberExpression ;

NewExpression: T_NEW NewExpression ;
/.
case $rule_number: {
} break;
./

CallExpression: MemberExpression Arguments ;
/.
case $rule_number: {
} break;
./

CallExpression: CallExpression Arguments ;
/.
case $rule_number: {
} break;
./

CallExpression: CallExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
} break;
./

CallExpression: CallExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
} break;
./

Arguments: T_LPAREN T_RPAREN ;
/.
case $rule_number: {
} break;
./

Arguments: T_LPAREN ArgumentList T_RPAREN ;
/.
case $rule_number: {
} break;
./

ArgumentList: AssignmentExpression ;
/.
case $rule_number: {
} break;
./

ArgumentList: ArgumentList T_COMMA AssignmentExpression ;
/.
case $rule_number: {
} break;
./

LeftHandSideExpression: NewExpression ;
LeftHandSideExpression: CallExpression ;
PostfixExpression: LeftHandSideExpression ;

PostfixExpression: LeftHandSideExpression T_PLUS_PLUS ;
/.
case $rule_number: {
} break;
./

PostfixExpression: LeftHandSideExpression T_MINUS_MINUS ;
/.
case $rule_number: {

} break;
./

UnaryExpression: PostfixExpression ;

UnaryExpression: T_DELETE UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_VOID UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_TYPEOF UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_PLUS_PLUS UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_MINUS_MINUS UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_PLUS UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_MINUS UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_TILDE UnaryExpression ;
/.
case $rule_number: {
} break;
./

UnaryExpression: T_NOT UnaryExpression ;
/.
case $rule_number: {
} break;
./

MultiplicativeExpression: UnaryExpression ;

MultiplicativeExpression: MultiplicativeExpression T_STAR UnaryExpression ;
/.
case $rule_number: {
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_DIVIDE_ UnaryExpression ;
/.
case $rule_number: {
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_REMAINDER UnaryExpression ;
/.
case $rule_number: {
} break;
./

AdditiveExpression: MultiplicativeExpression ;

AdditiveExpression: AdditiveExpression T_PLUS MultiplicativeExpression ;
/.
case $rule_number: {
} break;
./

AdditiveExpression: AdditiveExpression T_MINUS MultiplicativeExpression ;
/.
case $rule_number: {
} break;
./

ShiftExpression: AdditiveExpression ;

ShiftExpression: ShiftExpression T_LT_LT AdditiveExpression ;
/.
case $rule_number: {
} break;
./

ShiftExpression: ShiftExpression T_GT_GT AdditiveExpression ;
/.
case $rule_number: {
} break;
./

ShiftExpression: ShiftExpression T_GT_GT_GT AdditiveExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: ShiftExpression ;

RelationalExpression: RelationalExpression T_LT ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: RelationalExpression T_GT ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: RelationalExpression T_LE ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: RelationalExpression T_GE ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: RelationalExpression T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpression: RelationalExpression T_IN ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpressionNotIn: ShiftExpression ;

RelationalExpressionNotIn: RelationalExpressionNotIn T_LT ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GT ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_LE ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GE ShiftExpression ;
/.
case $rule_number: {
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
} break;
./

EqualityExpression: RelationalExpression ;

EqualityExpression: EqualityExpression T_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ RelationalExpression ;
/.
case $rule_number: {
} break;
./

EqualityExpression: EqualityExpression T_EQ_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
} break;
./

EqualityExpressionNotIn: RelationalExpressionNotIn ;

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ RelationalExpressionNotIn;
/.
case $rule_number: {
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
} break;
./

BitwiseANDExpression: EqualityExpression ;

BitwiseANDExpression: BitwiseANDExpression T_AND EqualityExpression ;
/.
case $rule_number: {
} break;
./

BitwiseANDExpressionNotIn: EqualityExpressionNotIn ;

BitwiseANDExpressionNotIn: BitwiseANDExpressionNotIn T_AND EqualityExpressionNotIn ;
/.
case $rule_number: {
} break;
./

BitwiseXORExpression: BitwiseANDExpression ;

BitwiseXORExpression: BitwiseXORExpression T_XOR BitwiseANDExpression ;
/.
case $rule_number: {
} break;
./

BitwiseXORExpressionNotIn: BitwiseANDExpressionNotIn ;

BitwiseXORExpressionNotIn: BitwiseXORExpressionNotIn T_XOR BitwiseANDExpressionNotIn ;
/.
case $rule_number: {
} break;
./

BitwiseORExpression: BitwiseXORExpression ;

BitwiseORExpression: BitwiseORExpression T_OR BitwiseXORExpression ;
/.
case $rule_number: {
} break;
./

BitwiseORExpressionNotIn: BitwiseXORExpressionNotIn ;

BitwiseORExpressionNotIn: BitwiseORExpressionNotIn T_OR BitwiseXORExpressionNotIn ;
/.
case $rule_number: {
} break;
./

LogicalANDExpression: BitwiseORExpression ;

LogicalANDExpression: LogicalANDExpression T_AND_AND BitwiseORExpression ;
/.
case $rule_number: {
} break;
./

LogicalANDExpressionNotIn: BitwiseORExpressionNotIn ;

LogicalANDExpressionNotIn: LogicalANDExpressionNotIn T_AND_AND BitwiseORExpressionNotIn ;
/.
case $rule_number: {
} break;
./

LogicalORExpression: LogicalANDExpression ;

LogicalORExpression: LogicalORExpression T_OR_OR LogicalANDExpression ;
/.
case $rule_number: {
} break;
./

LogicalORExpressionNotIn: LogicalANDExpressionNotIn ;

LogicalORExpressionNotIn: LogicalORExpressionNotIn T_OR_OR LogicalANDExpressionNotIn ;
/.
case $rule_number: {
} break;
./

ConditionalExpression: LogicalORExpression ;

ConditionalExpression: LogicalORExpression T_QUESTION AssignmentExpression T_COLON AssignmentExpression ;
/.
case $rule_number: {
} break;
./

ConditionalExpressionNotIn: LogicalORExpressionNotIn ;

ConditionalExpressionNotIn: LogicalORExpressionNotIn T_QUESTION AssignmentExpressionNotIn T_COLON AssignmentExpressionNotIn ;
/.
case $rule_number: {
} break;
./

AssignmentExpression: ConditionalExpression ;

AssignmentExpression: LeftHandSideExpression AssignmentOperator AssignmentExpression ;
/.
case $rule_number: {
} break;
./

AssignmentExpressionNotIn: ConditionalExpressionNotIn ;

AssignmentExpressionNotIn: LeftHandSideExpression AssignmentOperator AssignmentExpressionNotIn ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_STAR_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_DIVIDE_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_REMAINDER_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_PLUS_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_MINUS_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_LT_LT_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_GT_GT_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_GT_GT_GT_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_AND_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_XOR_EQ ;
/.
case $rule_number: {
} break;
./

AssignmentOperator: T_OR_EQ ;
/.
case $rule_number: {
} break;
./

Expression: AssignmentExpression ;

Expression: Expression T_COMMA AssignmentExpression ;
/.
case $rule_number: {
} break;
./

ExpressionOpt: ;
/.
case $rule_number: {
} break;
./

ExpressionOpt: Expression ;

ExpressionNotIn: AssignmentExpressionNotIn ;

ExpressionNotIn: ExpressionNotIn T_COMMA AssignmentExpressionNotIn ;
/.
case $rule_number: {
} break;
./

ExpressionNotInOpt: ;
/.
case $rule_number: {
} break;
./

ExpressionNotInOpt: ExpressionNotIn ;

Statement: Block ;
Statement: VariableStatement ;
Statement: EmptyStatement ;
Statement: ExpressionStatement ;
Statement: IfStatement ;
Statement: IterationStatement ;
Statement: ContinueStatement ;
Statement: BreakStatement ;
Statement: ReturnStatement ;
Statement: WithStatement ;
Statement: LabelledStatement ;
Statement: SwitchStatement ;
Statement: ThrowStatement ;
Statement: TryStatement ;
Statement: DebuggerStatement ;


Block: T_LBRACE StatementListOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

StatementList: Statement ;
/.
case $rule_number: {
} break;
./

StatementList: StatementList Statement ;
/.
case $rule_number: {
} break;
./

StatementListOpt: ;
/.
case $rule_number: {
} break;
./

StatementListOpt: StatementList ;
/.
case $rule_number: {
} break;
./

VariableStatement: VariableDeclarationKind VariableDeclarationList T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

VariableDeclarationKind: T_CONST ;
/.
case $rule_number: {
} break;
./

VariableDeclarationKind: T_VAR ;
/.
case $rule_number: {
} break;
./

VariableDeclarationList: VariableDeclaration ;
/.
case $rule_number: {
} break;
./

VariableDeclarationList: VariableDeclarationList T_COMMA VariableDeclaration ;
/.
case $rule_number: {
} break;
./

VariableDeclarationListNotIn: VariableDeclarationNotIn ;
/.
case $rule_number: {
} break;
./

VariableDeclarationListNotIn: VariableDeclarationListNotIn T_COMMA VariableDeclarationNotIn ;
/.
case $rule_number: {
} break;
./

VariableDeclaration: T_IDENTIFIER InitialiserOpt ;
/.
case $rule_number: {
} break;
./

VariableDeclarationNotIn: T_IDENTIFIER InitialiserNotInOpt ;
/.
case $rule_number: {
} break;
./

Initialiser: T_EQ AssignmentExpression ;
/.
case $rule_number: {
} break;
./

InitialiserOpt: ;
/.
case $rule_number: {
} break;
./

InitialiserOpt: Initialiser ;

InitialiserNotIn: T_EQ AssignmentExpressionNotIn ;
/.
case $rule_number: {
} break;
./

InitialiserNotInOpt: ;
/.
case $rule_number: {
} break;
./

InitialiserNotInOpt: InitialiserNotIn ;

EmptyStatement: T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

ExpressionStatement: Expression T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement T_ELSE Statement ;
/.
case $rule_number: {
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./


IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

IterationStatement: T_WHILE T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

IterationStatement: T_FOR T_LPAREN ExpressionNotInOpt T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationListNotIn T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

IterationStatement: T_FOR T_LPAREN LeftHandSideExpression T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationNotIn T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

ContinueStatement: T_CONTINUE T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

ContinueStatement: T_CONTINUE T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

BreakStatement: T_BREAK T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

BreakStatement: T_BREAK T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

ReturnStatement: T_RETURN ExpressionOpt T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

WithStatement: T_WITH T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
} break;
./

SwitchStatement: T_SWITCH T_LPAREN Expression T_RPAREN CaseBlock ;
/.
case $rule_number: {
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt DefaultClause CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

CaseClauses: CaseClause ;
/.
case $rule_number: {
} break;
./

CaseClauses: CaseClauses CaseClause ;
/.
case $rule_number: {
} break;
./

CaseClausesOpt: ;
/.
case $rule_number: {
} break;
./

CaseClausesOpt: CaseClauses ;
/.
case $rule_number: {
} break;
./

CaseClause: T_CASE Expression T_COLON StatementListOpt ;
/.
case $rule_number: {
} break;
./

DefaultClause: T_DEFAULT T_COLON StatementListOpt ;
/.
case $rule_number: {
} break;
./

LabelledStatement: T_IDENTIFIER T_COLON Statement ;
/.
case $rule_number: {
} break;
./

ThrowStatement: T_THROW Expression T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

TryStatement: T_TRY Block Catch ;
/.
case $rule_number: {
} break;
./

TryStatement: T_TRY Block Finally ;
/.
case $rule_number: {
} break;
./

TryStatement: T_TRY Block Catch Finally ;
/.
case $rule_number: {
} break;
./

Catch: T_CATCH T_LPAREN T_IDENTIFIER T_RPAREN Block ;
/.
case $rule_number: {
} break;
./

Finally: T_FINALLY Block ;
/.
case $rule_number: {
} break;
./

DebuggerStatement: T_DEBUGGER T_SEMICOLON ;
/.
case $rule_number: {
} break;
./

FunctionDeclaration: T_FUNCTION T_IDENTIFIER T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

FunctionExpression: T_FUNCTION IdentifierOpt T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
} break;
./

FormalParameterList: T_IDENTIFIER ;
/.
case $rule_number: {
} break;
./

FormalParameterList: FormalParameterList T_COMMA T_IDENTIFIER ;
/.
case $rule_number: {
} break;
./

FormalParameterListOpt: ;
/.
case $rule_number: {
} break;
./

FormalParameterListOpt: FormalParameterList ;
/.
case $rule_number: {
} break;
./

FunctionBodyOpt: ;
/.
case $rule_number: {
} break;
./

FunctionBodyOpt: FunctionBody ;

FunctionBody: SourceElements ;
/.
case $rule_number: {
} break;
./

Program: SourceElements ;
/.
case $rule_number: {
} break;
./

SourceElements: SourceElement ;
/.
case $rule_number: {
} break;
./

SourceElements: SourceElements SourceElement ;
/.
case $rule_number: {
} break;
./

SourceElement: Statement ;
/.
case $rule_number: {
} break;
./

SourceElement: FunctionDeclaration ;
/.
case $rule_number: {
} break;
./

IdentifierOpt: ;
/.
case $rule_number: {
} break;
./

IdentifierOpt: T_IDENTIFIER ;

PropertyNameAndValueListOpt: ;
/.
case $rule_number: {
} break;
./

PropertyNameAndValueListOpt: PropertyNameAndValueList ;

/.
    } // switch
}
./
