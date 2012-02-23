
%option noyywrap 
%option nounistd
%x MLCOMMENTSTATE

%{
#include "js_parser.tab.hh"
#include <cstdlib>
#include <cstdio>
#include <QtCore>
#include <io.h>


#define YY_DECL int yylex(yy::JSParser::semantic_type* yylval,yy::JSParser::location_type* yylloc)
#define YY_USER_ACTION yylloc->columns(yyleng);

typedef yy::JSParser::token token;
%}

hex [0-9a-fA-F]
dec [1-9][0-9]*
oct [0-7]
digit [0-9]
letter [a-zA-Z]

fract {digit}*\.{digit}+|{digit}+\.
exp [eE][+-]?{digit}+

hexfract {hex}*\.{hex}+|{hex}+\.
binexp [pP][+-]?{digit}+

icst ({dec}|0{oct}*|0[xX]{hex}+)

%%

%{
	yylloc->step();
%}
				
<MLCOMMENTSTATE>"*/"			{BEGIN(INITIAL);}
<MLCOMMENTSTATE>[^*\n]+   		{}
<MLCOMMENTSTATE>"*"       		{}
<MLCOMMENTSTATE>\n        		{}
"//"(.)*						{}
"/*"							{BEGIN(MLCOMMENTSTATE);}
"&&"							{}
"&="							{}
"&" 							{}
"break"							{}
"case"							{}
"catch"							{}
":"								{return token::T_COLON;}
","								{return token::T_COMMA;}
"continue"						{}
"default" 						{}
"delete"						{}
"/="							{}
"/"								{}
"do"							{}
"."								{return token::T_DOT;}
"else"							{}
"==="							{}
"=="							{}
"="								{return token::T_EQ;}
"finally"						{}
"for"							{}
"function"						{return token::T_FUNCTION;}
">>>="							{}
">>="							{}
">="							{}
">>"							{}
">"								{}
"if"							{}
"in"							{}
"instanceof"					{}
"{"								{return token::T_LBRACKET;}
"["								{return token::T_LBRACE;}
"<<="							{}
"<<"							{}
"<="							{}
"<"								{}
"("								{return token::T_LPAREN;}
"--"							{}
"-="							{}
"-"								{}
"new"							{return token::T_NEW;}
"!=="							{}
"!="							{} 
"!"								{}
"|="							{}
"||"							{}
"|"								{}
"+="							{}
"++"							{}
"+"								{}
"?"								{}
"}"								{return token::T_RBRACKET;}
"]"								{return token::T_RBRACE;}
"%="							{}
"%"								{}
"return"						{}
")"								{return token::T_RPAREN;}
";"								{return token::T_SEMICOLON;}
"*="							{}
"*"								{}
\"([^"\\]|\\.)*\"				{return token::T_STRING_LITERAL;}
"switch"						{}
"this"							{return token::T_THIS;}
"throw"							{}
"~"								{}
"try"							{}
"typeof"						{}
"var"							{return token::T_VAR;}
"void"							{}
"while"							{}
"with"							{}
"^="							{}
"^"								{}
"null"							{return token::T_NULL;}
"true"							{}
"false"							{}	
"const"							{return token::T_CONST;}
"debugger"						{}
"reserved"						{}
{icst}							{yylval->i = QString(yytext).toInt();return token::T_NUMERIC_LITERAL;}
{icst}[uU]						{yylval->u = QString(yytext).toUInt();return token::T_NUMERIC_LITERAL;}
{icst}[uU][lL]					{yylval->ul = QString(yytext).toULong();return token::T_NUMERIC_LITERAL;}
{icst}[lL][uU]					{yylval->ul = QString(yytext).toULong();return token::T_NUMERIC_LITERAL;}
{icst}[lL]						{yylval->l = QString(yytext).toLong();return token::T_NUMERIC_LITERAL;}
{icst}[uU](ll|LL)				{yylval->l = QString(yytext).toULongLong();return token::T_NUMERIC_LITERAL;}
{icst}(ll|LL)					{yylval->l = QString(yytext).toLongLong();return token::T_NUMERIC_LITERAL;}
{icst}(ll|LL)[uU]				{yylval->l = QString(yytext).toULongLong();return token::T_NUMERIC_LITERAL;}
{fract}{exp}?[flFL]?			{yylval->f = QString(yytext).toFloat();return token::T_NUMERIC_LITERAL;}
{digit}+{exp}[flFL]?			{yylval->f = QString(yytext).toFloat();return token::T_NUMERIC_LITERAL;}
0[xX]{hexfract}{binexp}[flFL]?	{yylval->f = QString(yytext).toFloat();return token::T_NUMERIC_LITERAL;}
0[xX]{hex}+{binexp}[flFL]?		{yylval->f = QString(yytext).toFloat();return token::T_NUMERIC_LITERAL;}
({letter}|"_")+({letter}|{digit}|"_")* {yylval->s = new QString(yytext);return token::T_IDENTIFIER;}
[ \t\n]     					{}

%%
