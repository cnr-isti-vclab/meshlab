
%option noyywrap 
%option nounistd
%{
#include "js_parser.h"
#include <cstdlib>
#include <io.h>

#define YY_DECL int JSParser::nextToken()
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

"&&"							{}
"&="							{}
"&" 							{}
"break"							{}
"case"							{}
"catch"							{}
":"								{return T_COLON;}
","								{return T_COMMA;}
"continue"						{}
"default" 						{}
"delete"						{}
"/="							{}
"/"								{}
"do"							{}
"."								{return T_DOT;}
"else"							{}
"==="							{}
"=="							{}
"="								{return T_EQ;}
"finally"						{}
"for"							{}
"function"						{return T_FUNCTION;}
">>>="							{}
">>="							{}
">="							{}
">>"							{}
">"								{}
"if"							{}
"in"							{}
"instanceof"					{}
"{"								{return T_LBRACKET;}
"["								{return T_LBRACE;}
"<<="							{}
"<<"							{}
"<="							{}
"<"								{}
"("								{return T_LPAREN;}
"--"							{}
"-="							{}
"-"								{}
"new"							{return T_NEW;}
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
"}"								{return T_RBRACKET;}
"]"								{return T_RBRACE;}
"%="							{}
"%"								{}
"return"						{}
")"								{return T_RPAREN;}
";"								{return T_SEMICOLON;}
"*="							{}
"*"								{}
"/"([^"\\]|\\.)*"/"				{return T_STRING_LITERAL;}
"switch"						{}
"this"							{return T_THIS;}
"throw"							{}
"~"								{}
"try"							{}
"typeof"						{}
"var"							{return T_VAR;}
"void"							{}
"while"							{}
"with"							{}
"^="							{}
"^"								{}
"null"							{return T_NULL;}
"true"							{}
"false"							{}	
"const"							{return T_CONST;}

{icst}							{yyval.i = QString(yytext).toInt();return T_NUMERIC_LITERAL;}
{icst}[uU]						{yyval.u = QString(yytext).toUInt();return T_NUMERIC_LITERAL;}
{icst}[uU][lL]					{yyval.ul = QString(yytext).toULong();return T_NUMERIC_LITERAL;}
{icst}[lL][uU]					{yyval.ul = QString(yytext).toULong();return T_NUMERIC_LITERAL;}
{icst}[lL]						{yyval.l = QString(yytext).toLong();return T_NUMERIC_LITERAL;}
{icst}[uU](ll|LL)				{yyval.l = QString(yytext).toULongLong();return T_NUMERIC_LITERAL;}
{icst}(ll|LL)					{yyval.l = QString(yytext).toLongLong();return T_NUMERIC_LITERAL;}
{icst}(ll|LL)[uU]				{yyval.l = QString(yytext).toULongLong();return T_NUMERIC_LITERAL;}
{fract}{exp}?[flFL]?			{yyval.f = QString(yytext).toFloat();return T_NUMERIC_LITERAL;}
{digit}+{exp}[flFL]?			{yyval.f = QString(yytext).toFloat();return T_NUMERIC_LITERAL;}
0[xX]{hexfract}{binexp}[flFL]?	{yyval.f = QString(yytext).toFloat();return T_NUMERIC_LITERAL;}
0[xX]{hex}+{binexp}[flFL]?		{yyval.f = QString(yytext).toFloat();return T_NUMERIC_LITERAL;}
({letter}|"_")+({letter}|{digit}|"_")* {yyval.s = insert(yytext);return T_IDENTIFIER;}
[ \t\n]     					{}

%%
