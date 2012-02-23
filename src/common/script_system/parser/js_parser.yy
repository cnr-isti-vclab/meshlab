%language "C++"
%defines /* generates .h */
%locations 
%define parser_class_name "JSParser"

/* parse-param { type& param_name} lex-param { type& param_name} add a parameter to the parser constructor and to yylex*/

%code requires 
{
	#include <QtCore>
}

%union {
	int i;
	unsigned u;
	unsigned long ul;
	unsigned long long ull;
	long l;
	double d;
	float f;
	QString *s;
};

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

%{
	extern int yylex(yy::JSParser::semantic_type *yylval,yy::JSParser::location_type *yylloc);	
%}

%%
		 
Program: StatementList;

StatementList:
				| StatementList Statement
				;

Statement: 	Block
			| FunctionDecl
			| VariableDeclStatement
			| AssignmentStatement
			| error T_SEMICOLON 
			;
			
Block: OpenBlock StatementList CloseBlock;

OpenBlock: T_LBRACKET; 

CloseBlock: T_RBRACKET;

VariableDeclStatement: DeclTok IdTok InitOpt OtherDeclOpt T_SEMICOLON;

AssignmentStatement: LeftSideExpr Assignment T_SEMICOLON;

OtherDeclOpt:
				| OtherDeclOpt T_COMMA IdTok InitOpt
				;
				
DeclTok: 	T_VAR
			| T_CONST
			;

Assignment: T_EQ Expression;
			
InitOpt: 
			| Assignment
			;

FunctionDecl: T_FUNCTION IdTok OpenBracket OptParamList CloseBracket OpenBlock StatementList CloseBlock;

OptParamList:  
				| IdTok OtherOptParamList
				;
				
OtherOptParamList:
					| OtherOptParamList T_COMMA IdTok
					;
					
Expression: LeftSideExpr
			| NewExpr
			| FunctionExpr
			| FunctionCall
			| ObjConstExpr
			| ArrayExpr
			| T_NULL
			| T_STRING_LITERAL
			| T_NUMERIC_LITERAL
			;

LeftSideExpr: 	MemberExpr
				| ThisExpr
				;
			
OptIdTok:
			| IdTok
			;

MemberExpr: IdTok
			| IdTok T_DOT MemberExpr
			;

ThisExpr: ThisTok T_DOT MemberExpr;

ThisTok: T_THIS;

IdTok: T_IDENTIFIER;

NewTok: T_NEW;
 
NewExpr: NewTok IdTok OpenBracket OptArgumentList CloseBracket;

FunctionExpr: T_FUNCTION OptIdTok OpenBracket OptParamList CloseBracket OpenBlock StatementList CloseBlock;

FunctionCall: 	MemberExpr OpenBracket OptExpressionList CloseBracket
				| ThisExpr OpenBracket OptExpressionList CloseBracket
				;


OptExpressionList: 
					Expression OtherOptExpressionList
					;

OtherOptExpressionList:
						| OtherOptExpressionList T_COMMA Expression
						;


OptArgumentList: 
					| IdTok OtherOptArgumentList
					;

OtherOptArgumentList: 
						| OtherOptArgumentList T_COMMA IdTok
						;

ObjConstExpr: OpenBlock IdTok T_COLON Expression OtherOptFields CloseBlock;

OtherOptFields:
				| OtherOptFields T_COMMA IdTok T_COLON Expression
				;

ArrayExpr: OpenArrBracket Expression OtherOptExpressionList CloseArrBracket;

OpenBracket: T_LPAREN;

CloseBracket: T_RPAREN;

OpenArrBracket: T_LBRACE;

CloseArrBracket: T_RBRACE;

%%

namespace yy 
{
	void JSParser::error(location const& loc, const std::string& s)
	{
		//std::cerr << "error at " << loc << ": " << s << "\n";
	}
}
