%language "C++"
%defines /* generates .h */
%locations
%define parser_class_name "JSCacheParser"

%code requires 
{
	#include <QtCore>
	#include "../../scriptsyntax.h"
}

%parse-param {JSSymbolsCache& symb} /*add a parameter member to the parser class*/
%parse-param {QList<JSVarDescriptor>& tmp} /*add a parameter member to the parser class*/

%union {
	int i;
	unsigned u;
	unsigned long ul;
	unsigned long long ull;
	long l;
	double d;
	float f;
	QString *s;
	JSFunctionTypeDescriptor* ftd;
	JSVarDescriptor* vd;
};

%token T_COLON
%token T_COMMA
%token T_DOT
%token T_EQ
%token T_FUNCTION
%token <s> T_IDENTIFIER
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

%type <ftd> Expression NewExpr Assignment InitOpt;
%type <s> IdTok OptIdTok;

%{
	extern int yylex(yy::JSCacheParser::semantic_type *yylval,yy::JSCacheParser::location_type *yylloc);	
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
			{
				qDebug() << "Parsing Error Recovery!!!\n";
			}
			;
			
Block: OpenBlock StatementList CloseBlock;

OpenBlock:	T_LBRACKET 
			{
				symb.pushFrame();
			}
			; 

CloseBlock: T_RBRACKET
			{
				symb.popFrame();
			}
			;

VariableDeclStatement:	DeclTok IdTok InitOpt OtherDeclOpt T_SEMICOLON
						{
							JSVarDescriptor vd(*$2,$3);
							symb.currentFrame()[*$2] = vd;
						}
						;

AssignmentStatement: 	MemberExpr Assignment T_SEMICOLON
						{
							
						}
						| ThisExpr Assignment T_SEMICOLON
						{
							
						}
						;

OtherDeclOpt:
				| OtherDeclOpt T_COMMA IdTok InitOpt
				;
				
DeclTok: 	T_VAR
			| T_CONST
			;

Assignment: 	T_EQ Expression
				{
					$$ = $2;
				}
				;
			
InitOpt: 	
			{
				$$ = NULL;
			}
			| Assignment
			;

/*divided in two rules in order to add formal parameters to current frame in the varsstack*/
FunctionDecl: StartFun StatementList CloseBlock;
StartFun:	T_FUNCTION IdTok OpenBracket OptParamList CloseBracket OpenBlock
			{
				JSFunctionTypeDescriptor ftd(*$2);
				symb.globalFunType()[*$2] = ftd;
			}
			;
			
StartOptFun :	T_FUNCTION OptIdTok OpenBracket OptParamList CloseBracket
				{
					QString nm;
					if (*$2 == "")
						nm = QUuid::createUuid().toString();
					JSFunctionTypeDescriptor ftd(nm);
				}
				;

OptParamList:  
				| IdTok OtherOptParamList
				{
						symb.currentFrame()[*$1] = JSVarDescriptor(*$1,NULL);
				}
				;
				
OtherOptParamList:
					| OtherOptParamList T_COMMA IdTok
					{
						symb.currentFrame()[*$3] = JSVarDescriptor(*$3,NULL);
					}
					;
					
Expression: MemberExpr
			{
				$$ = NULL;
			}
			| ThisExpr
			{
				$$ = NULL;
			}
			| NewExpr
			{
			
				$$ = $1;
			}
			| FunctionExpr
			{
				$$ = NULL;
			}
			| FunctionCall
			{
				$$ = NULL;
			}
			| ObjConstExpr
			{
				JSFunctionTypeDescriptor ftd(QUuid::createUuid().toString());
				for(int ii = 0;ii < tmp.size();++ii)
					ftd.varmember[tmp[ii].name] = tmp[ii];
				tmp.clear();
				symb.globalFunType()[ftd.name] = ftd;
				$$ = &symb.globalFunType()[ftd.name];
			}
			| ArrayExpr
			{
				$$ = NULL;
			}
			| T_NULL
			{
				$$ = NULL;
			}
			| T_STRING_LITERAL
			{
				$$ = NULL;
			}
			| T_NUMERIC_LITERAL
			{
				$$ = NULL;
			}
			;
			
OptIdTok:
			{
				$$ = NULL;
			}
			| IdTok
			;

MemberExpr: IdTok
			| MemberExpr T_DOT IdTok
			;

ThisExpr: ThisTok T_DOT MemberExpr;

ThisTok: T_THIS;

IdTok: T_IDENTIFIER;

NewTok: T_NEW;
 
NewExpr:	NewTok IdTok OpenBracket OptArgumentList CloseBracket
			{
				$$ = symb.getFunctionType(*$2);
			};



FunctionExpr:	StartOptFun StatementList CloseBlock
				{
					
					
				}
				;

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

ObjConstExpr: 	OpenBlock CloseBlock
				| OpenBlock IdTok T_COLON Expression OtherOptFields CloseBlock
				{
					JSVarDescriptor vd(*$2,$4);
					tmp.push_back(vd);
				}
				;

OtherOptFields:
				| OtherOptFields T_COMMA IdTok T_COLON Expression
				{
					JSVarDescriptor vd(*$3,$5);
					tmp.push_back(vd);
				}
				;

ArrayExpr: OpenArrBracket Expression OtherOptExpressionList CloseArrBracket;

OpenBracket: T_LPAREN;

CloseBracket: T_RPAREN;

OpenArrBracket: T_LBRACE;

CloseArrBracket: T_RBRACE;

%%

namespace yy 
{
	void JSCacheParser::error(location const& loc, const std::string& s)
	{
		//std::cerr << "error at " << loc << ": " << s << "\n";
	}
}
