#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "muParserDLL.h"

#define PARSER_CONST_PI  3.141592653589793238462643
#define PARSER_CONST_E   2.718281828459045235360287
#define PARSER_MAXVARS		10


//---------------------------------------------------------------------------
// Callbacks for postfix operators
double Mega(double a_fVal) 
{ 
  return a_fVal * 1.0e6; 
}

double Milli(double a_fVal) 
{
  return a_fVal / 1.0e3; 
}

double ZeroArg()
{
  printf("i'm a function without arguments.\n");
  return 123;
}

//---------------------------------------------------------------------------
// Callbacks for infix operators
double Not(double v) { return v==0; }

//---------------------------------------------------------------------------
// Function callbacks
double Rnd(double v) { return v * rand() / (double)(RAND_MAX+1.0); }

double SampleQuery(const char *szMsg) 
{
  if (szMsg) 
    printf("%s\n", szMsg);

  return 999;
}

double Sum(const double *a_afArg, int a_iArgc)
{ 
  double fRes=0;
  int i=0;

  for (i=0; i<a_iArgc; ++i) 
    fRes += a_afArg[i];

  return fRes;
}

//---------------------------------------------------------------------------
// Binarty operator callbacks
muFloat_t Add(muFloat_t v1, muFloat_t v2) 
{ 
  return v1+v2; 
}

muFloat_t Mul(muFloat_t v1, muFloat_t v2) 
{ 
  return v1*v2; 
}

//---------------------------------------------------------------------------
// Factory function for creating new parser variables
// This could as well be a function performing database queries.
muFloat_t* AddVariable(const muChar_t* a_szName, void *pUserData)
{
  static double afValBuf[PARSER_MAXVARS];  // I don't want dynamic allocation here
  static int iVal = 0;                     // so i used this buffer

  printf("Generating new variable \"%s\" (slots left: %d)\n", a_szName, PARSER_MAXVARS-iVal);

  afValBuf[iVal] = 0;
  if (iVal>=PARSER_MAXVARS-1) 
  {
     printf("Variable buffer overflow.");
     return NULL;
  }

  return &afValBuf[iVal++];
}

//---------------------------------------------------------------------------
void Intro()
{
  printf("---------------------------------------\n");
  printf("\n");
  printf("  Math Parser sample application\n");
  printf("  (DLL version)\n");
  printf("\n");
  printf("---------------------------------------\n");
  printf("Commands:\n");
  printf("  list var     - list parser variables\n");
  printf("  list exprvar - list expression variables\n");
  printf("  list const   - list all numeric parser constants\n");
  printf("  exit         - exits the parser\n");
  printf("Constants:\n");
  printf("  \"_e\"   2.718281828459045235360287\n");
  printf("  \"_pi\"  3.141592653589793238462643\n");
  printf("---------------------------------------\n");
  printf("Please enter a formula:\n");
}

//---------------------------------------------------------------------------
// Callback function for parser errors
void OnError(muParserHandle_t hParser)
{
  printf("\nError:\n");
  printf("------\n");
  printf("Message:  \"%s\"\n", mupGetErrorMsg(hParser));
  printf("Token:    \"%s\"\n", mupGetErrorToken(hParser));
  printf("Position: %d\n", mupGetErrorPos(hParser));
  printf("Errc:     %d\n", mupGetErrorCode(hParser));
}

//---------------------------------------------------------------------------
void ListVar(muParserHandle_t a_hParser)
{
  int iNumVar = mupGetVarNum(a_hParser);
  int i = 0;

  if (iNumVar==0)
  {
    printf("No variables defined\n");
    return;
  }

  printf("\nExpression variables:\n");
  printf("---------------------\n");
  printf("Number: %d\n", iNumVar);
  
  for (i=0; i<iNumVar; ++i)
  {
    const muChar_t* szName = 0;
    muFloat_t* pVar = 0;

    mupGetVar(a_hParser, i, &szName, &pVar);
    printf("Name: %s    Address: [0x%x]\n", szName, (long long)pVar);
  }
}

//---------------------------------------------------------------------------
void ListExprVar(muParserHandle_t a_hParser)
{
  muInt_t iNumVar = mupGetExprVarNum(a_hParser),
          i = 0;

  if (iNumVar==0)
  {
    printf("Expression dos not contain variables\n");
    return;
  }

  printf("\nExpression variables:\n");
  printf("---------------------\n");
  printf("Expression: %s\n", mupGetExpr(a_hParser) );
  printf("Number: %d\n", iNumVar);
  
  for (i=0; i<iNumVar; ++i)
  {
    const muChar_t* szName = 0;
    muFloat_t* pVar = 0;

    mupGetExprVar(a_hParser, i, &szName, &pVar);
    printf("Name: %s   Address: [0x%x]\n", szName, (long long)pVar);
  }
}

//---------------------------------------------------------------------------
void ListConst(muParserHandle_t a_hParser)
{
  muInt_t iNumVar = mupGetConstNum(a_hParser),
          i = 0;

  if (iNumVar==0)
  {
    printf("No constants defined\n");
    return;
  }

  printf("\nParser constants:\n");
  printf("---------------------\n");
  printf("Number: %d", iNumVar);

  for (i=0; i<iNumVar; ++i)
  {
    const muChar_t* szName = 0;
    muFloat_t fVal = 0;

    mupGetConst(a_hParser, i, &szName, &fVal);
    printf("  %s = %f\n", szName, fVal);
  }
}

//---------------------------------------------------------------------------
/** \brief Check for external keywords.
*/
muBool_t CheckKeywords(const char *a_szLine, muParserHandle_t a_hParser)
{
  if (!strcmp(a_szLine, "quit"))
  {
    exit(0);
  }
  else if (!strcmp(a_szLine,"list var"))
  {
    ListVar(a_hParser);
    return 1;
  }
  else if (!strcmp(a_szLine, "list exprvar"))
  {
    ListExprVar(a_hParser);
    return 1;
  }
  else if (!strcmp(a_szLine, "list const"))
  {
    ListConst(a_hParser);
    return 1;
  }

  return 0;
}

//---------------------------------------------------------------------------
void Calc()
{
  muChar_t szLine[100];
  muFloat_t fVal = 0,
            afVarVal[] = { 1, 2 }; // Values of the parser variables
  muParserHandle_t hParser;

  hParser = mupCreate();              // initialize the parser

  // Set an error handler [optional]
  // the only function that does not take a parser instance handle
  mupSetErrorHandler(hParser, OnError);

//#define GERMAN_LOCALS
#ifdef GERMAN_LOCALS
  mupSetArgSep(hParser, ';');
  mupSetDecSep(hParser, ',');
  mupSetThousandsSep(hParser, '.');
#else
  mupSetArgSep(hParser, ',');
  mupSetDecSep(hParser, '.');
#endif

  // Set a variable factory
  mupSetVarFactory(hParser, AddVariable, NULL);

  // Define parser variables and bind them to C++ variables [optional]
  mupDefineConst(hParser, "const1", 1);  
  mupDefineConst(hParser, "const2", 2);
  mupDefineStrConst(hParser, "strBuf", "Hallo welt");

  // Define parser variables and bind them to C++ variables [optional]
  mupDefineVar(hParser, "a", &afVarVal[0]);  
  mupDefineVar(hParser, "b", &afVarVal[1]);

  // Define postfix operators [optional]
  mupDefinePostfixOprt(hParser, "M", Mega, 0);
  mupDefinePostfixOprt(hParser, "m", Milli, 0);

  // Define infix operator [optional]
  mupDefineInfixOprt(hParser, "!", Not, 0);

  // Define functions [optional]
//  mupDefineStrFun(hParser, "query", SampleQuery, 0); // Add an unoptimizeable function 
  mupDefineFun0(hParser, "zero", ZeroArg, 0);
  mupDefineFun1(hParser, "rnd", Rnd, 0);             // Add an unoptimizeable function
  mupDefineFun1(hParser, "rnd2", Rnd, 1); 
  mupDefineMultFun(hParser, "_sum", Sum, 0);  // "sum" is already a default function

  // Define binary operators [optional]
  mupDefineOprt(hParser, "add", Add, 0, 0);
  mupDefineOprt(hParser, "mul", Mul, 1, 0);

  while ( fgets(szLine, 99, stdin) )
  {
    szLine[strlen(szLine)-1] = 0; // overwrite the newline

    if (CheckKeywords(szLine, hParser)) 
      continue;

    mupSetExpr(hParser, szLine);

    fVal = mupEval(hParser);

  
    // Without an Error handler function 
    // you must use this for error treatment:
    //if (mupError(hParser))
    //{
    //  printf("\nError:\n");
    //  printf("------\n");
    //  printf("Message:  %s\n", mupGetErrorMsg(hParser) );
    //  printf("Token:    %s\n", mupGetErrorToken(hParser) );
    //  printf("Position: %s\n", mupGetErrorPos(hParser) );
    //  printf("Errc:     %s\n", mupGetErrorCode(hParser) );
    //  continue;
    //}

    if (!mupError(hParser))
      printf("%f\n", fVal);

  } // while 

  // finalle free the parser ressources
  mupRelease(hParser);
}

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Intro();
  Calc();
}
