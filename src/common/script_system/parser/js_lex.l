
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

"&&"							{return T_AND_AND;}
"&="							{return T_AND_EQ;}
"&" 							{return T_AND;}
"break"							{return T_BREAK;}
"case"							{return T_CASE;}
"catch"							{return T_CATCH;}
":"								{return T_COLON;}
","								{return T_COMMA;}
"continue"						{return T_CONTINUE;}
"default" 						{return T_DEFAULT;}
"delete"						{return T_DELETE;}
"/="							{return T_DIVIDE_EQ;}
"/"								{return T_DIVIDE_;}
"do"							{return T_DO;}
"."								{return T_DOT;}
"else"							{return T_ELSE;}
"==="							{return T_EQ_EQ_EQ;}
"=="							{return T_EQ_EQ;}
"="								{return T_EQ;}
"finally"						{return T_FINALLY;}
"for"							{return T_FOR;}
"function"						{return T_FUNCTION;}
">>>="							{return T_GT_GT_GT_EQ;}
">>="							{return T_GT_GT_EQ;}
">="							{return T_GE;}
">>"							{return T_GT_GT;}
">"								{return T_GT;}
"if"							{return T_IF;}
"in"							{return T_IN;}
"instanceof"					{return T_INSTANCEOF;}
"{"								{return T_LBRACE;}
"["								{return T_LBRACKET;}
"<<="							{return T_LT_LT_EQ;}
"<<"							{return T_LT_LT;}
"<="							{return T_LE;}
"<"								{return T_LT;}
"("								{return T_LPAREN;}
"--"							{return T_MINUS_MINUS;}
"-="							{return T_MINUS_EQ;}
"-"								{return T_MINUS;}
"new"							{return T_NEW;}
"!=="							{return T_NOT_EQ_EQ;}
"!="							{return T_NOT_EQ;} 
"!"								{return T_NOT;}
"|="							{return T_OR_EQ;}
"||"							{return T_OR_OR;}
"|"								{return T_OR;}
"+="							{return T_PLUS_EQ;}
"++"							{return T_PLUS_PLUS;}
"+"								{return T_PLUS;}
"?"								{return T_QUESTION;}
"}"								{return T_RBRACE;}
"]"								{return T_RBRACKET;}
"%="							{return T_REMAINDER_EQ;}
"%"								{return T_REMAINDER;}
"return"						{return T_RETURN;}
")"								{return T_RPAREN;}
";"								{return T_SEMICOLON;}
"*="							{return T_STAR_EQ;}
"*"								{return T_STAR;}
"/"([^"\\]|\\.)*"/"				{return T_STRING_LITERAL;}
"switch"						{return T_SWITCH;}
"this"							{return T_THIS;}
"throw"							{return T_THROW;}
"~"								{return T_TILDE;}
"try"							{return T_TRY;}
"typeof"						{return T_TYPEOF;}
"var"							{return T_VAR;}
"void"							{return T_VOID;}
"while"							{return T_WHILE;}
"with"							{return T_WITH;}
"^="							{return T_XOR_EQ;}
"^"								{return T_XOR;}
"null"							{return T_NULL;}
"true"							{return T_TRUE;}
"false"							{return T_FALSE;}	
"const"							{return T_CONST;}
"debugger"						{return T_DEBUGGER;}
"reserved"						{return T_RESERVED_WORD;}
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
