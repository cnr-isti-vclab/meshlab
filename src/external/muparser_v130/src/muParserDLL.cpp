/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2004-2008 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#if defined(MUPARSER_DLL) 

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include "muParserDLL.h"
#include "muParser.h"
#include "muParserError.h"


#define MU_TRY  \
        try     \
        {

#define MU_CATCH                                                 \
        }                                                        \
        catch(muError_t &e)                                      \
        {                                                        \
          ParserTag *pTag = static_cast<ParserTag*>(a_hParser);  \
          pTag->exc = e;                                         \
          pTag->bError = true;                                   \
          if (pTag->errHandler)                                  \
            (pTag->errHandler)(a_hParser);                       \
        }                                                        \
        catch(...)                                               \
        {                                                        \
          ParserTag *pTag = static_cast<ParserTag*>(a_hParser);  \
          pTag->exc = muError_t(mu::ecINTERNAL_ERROR);           \
          pTag->bError = true;                                   \
          if (pTag->errHandler)                                  \
            (pTag->errHandler)(a_hParser);                       \
        }

/** \file 
    \brief This file contains the implementation of the DLL interface of muparser.
*/

//---------------------------------------------------------------------------
class ParserTag
{
public:
  ParserTag()
    :pParser(new mu::Parser())
    ,errHandler(NULL)
    ,bError(false)
  {}
 
 ~ParserTag()
  {
    delete pParser;
  }

  mu::Parser *pParser;
  mu::ParserBase::exception_type exc;
  muErrorHandler_t errHandler;
  bool bError;

private:
  ParserTag(const ParserTag &ref);
  ParserTag& operator=(const ParserTag &ref);
};

//---------------------------------------------------------------------------
// private types
typedef mu::ParserBase::exception_type muError_t;
typedef mu::Parser muParser_t;

//---------------------------------------------------------------------------
//
//
//  unexported functions
//
//
//---------------------------------------------------------------------------

muParser_t* AsParser(muParserHandle_t a_hParser)
{
  return static_cast<ParserTag*>(a_hParser)->pParser;
}

//---------------------------------------------------------------------------
ParserTag* AsParserTag(muParserHandle_t a_hParser)
{
  return static_cast<ParserTag*>(a_hParser);
}

//---------------------------------------------------------------------------
#if defined(_WIN32)
  #define _CRT_SECURE_NO_DEPRECATE

  BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                         DWORD ul_reason_for_call, 
                         LPVOID /*lpReserved*/ )
  {
	  switch (ul_reason_for_call)
	  {
	  case  DLL_PROCESS_ATTACH:
          break;

    case  DLL_THREAD_ATTACH:
    case  DLL_THREAD_DETACH:
    case  DLL_PROCESS_DETACH:
    		  break;
	  }

    return TRUE;
  }

#endif

//---------------------------------------------------------------------------
//
//
//  exported functions
//
//
//---------------------------------------------------------------------------

API_EXPORT(void) mupSetVarFactory(muParserHandle_t a_hParser, muFacFun_t a_pFactory, void *pUserData)
{
  MU_TRY
    muParser_t* p(AsParser(a_hParser));
    p->SetVarFactory(a_pFactory, pUserData);
  MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Create a new Parser instance and return its handle.
*/
API_EXPORT(muParserHandle_t) mupCreate()
{
  return (void*)(new ParserTag());
}

//---------------------------------------------------------------------------
/** \brief Release the parser instance related with a parser handle.
*/
API_EXPORT(void) mupRelease(muParserHandle_t a_hParser)
{
  MU_TRY
    ParserTag* p = static_cast<ParserTag*>(a_hParser);
    delete p;
  MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Evaluate the expression.
*/
API_EXPORT(double) mupEval(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    return p->Eval();
  MU_CATCH

  return 0;
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupSetExpr(muParserHandle_t a_hParser, const muChar_t* a_szExpr)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->SetExpr(a_szExpr);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupRemoveVar(muParserHandle_t a_hParser, const muChar_t* a_szName)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->RemoveVar( a_szName );
  MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
*/
API_EXPORT(void) mupClearVar(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->ClearVar();
  MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
*/
API_EXPORT(void) mupClearConst(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->ClearConst();
  MU_CATCH
}

//---------------------------------------------------------------------------
/** \brief Clear all user defined operators.
    \param a_hParser Handle to the parser instance.
*/
API_EXPORT(void) mupClearOprt(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->ClearOprt();
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupClearFun(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->ClearFun();
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun0( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun0_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun1( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun1_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun2( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun2_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun3( muParserHandle_t a_hParser, 
                                const muChar_t *a_szName, 
                                muFun3_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun4( muParserHandle_t a_hParser, 
                                const muChar_t *a_szName, 
                                muFun4_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineFun5( muParserHandle_t a_hParser, 
                                const muChar_t *a_szName, 
                                muFun5_t a_pFun, 
                                muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun1( muParserHandle_t a_hParser, 
                                   const muChar_t *a_szName, 
                                   muStrFun1_t a_pFun )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun2( muParserHandle_t a_hParser, 
                                   const muChar_t* a_szName, 
                                   muStrFun2_t a_pFun )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrFun3( muParserHandle_t a_hParser, 
                                   const muChar_t* a_szName, 
                                   muStrFun3_t a_pFun )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, false);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineMultFun( muParserHandle_t a_hParser, 
                                   const muChar_t *a_szName, 
                                   muMultFun_t a_pFun, 
                                   muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineOprt( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun2_t a_pFun, 
                                muInt_t a_iPri, 
                                muBool_t a_bAllowOpt)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineOprt(a_szName, a_pFun, a_iPri, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineVar(muParserHandle_t a_hParser, const char *a_szName, double *a_pVar)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineVar(a_szName, a_pVar);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineConst(muParserHandle_t a_hParser, const char *a_szName, double a_fVal)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineConst(a_szName, a_fVal);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineStrConst(muParserHandle_t a_hParser, const char *a_szName, const char *a_szVal)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineStrConst(a_szName, a_szVal);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(const muChar_t*) mupGetExpr(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    return p->GetExpr().c_str();
  MU_CATCH

  return "";
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefinePostfixOprt( muParserHandle_t a_hParser,
                                       const muChar_t* a_szName,
                                       muFun1_t a_pOprt,
                                       muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefinePostfixOprt(a_szName, a_pOprt, a_bAllowOpt!=0);
  MU_CATCH
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineInfixOprt( muParserHandle_t a_hParser,
                                     const muChar_t* a_szName,
                                     muFun1_t a_pOprt,
                                     muBool_t a_bAllowOpt )
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    p->DefineInfixOprt(a_szName, a_pOprt, a_bAllowOpt!=0);
  MU_CATCH
}

// Define character sets for identifiers
//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineNameChars( muParserHandle_t a_hParser, 
                                     const muChar_t* a_szCharset )
{
    muParser_t* const p(AsParser(a_hParser));
  p->DefineNameChars(a_szCharset);
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineOprtChars( muParserHandle_t a_hParser, 
                                     const muChar_t* a_szCharset )
{
  muParser_t* const p(AsParser(a_hParser));
  p->DefineOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupDefineInfixOprtChars(muParserHandle_t a_hParser, const char *a_szCharset)
{
  muParser_t* const p(AsParser(a_hParser));
  p->DefineInfixOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables defined in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
*/
API_EXPORT(int) mupGetVarNum(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();
    return (int)VarMap.size();
  MU_CATCH

  return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.
    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.
*/
API_EXPORT(void) mupGetVar(muParserHandle_t a_hParser, unsigned a_iVar, const char **a_szName, double **a_pVar)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();

    if (a_iVar>=VarMap.size())
    {
     *a_szName = 0;
     *a_pVar = 0;
      return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

     strncpy(szName, item->first.c_str(), sizeof(szName));
     szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
     return;

  MU_CATCH

  *a_szName = 0;
  *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables used in the expression currently set in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
*/
API_EXPORT(int) mupGetExprVarNum(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();
    return (int)VarMap.size();
  MU_CATCH

  return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.

    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow
*/
API_EXPORT(void) mupGetExprVar(muParserHandle_t a_hParser, unsigned a_iVar, const char **a_szName, double **a_pVar)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();

    if (a_iVar>=VarMap.size())
    {
     *a_szName = 0;
     *a_pVar = 0;
      return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

     strncpy(szName, item->first.c_str(), sizeof(szName));
     szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
     return;

  MU_CATCH

  *a_szName = 0;
  *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Return the number of constants defined in a parser. */
API_EXPORT(int) mupGetConstNum(muParserHandle_t a_hParser)
{
  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();
    return (int)ValMap.size();
  MU_CATCH

  return 0; // never reached
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetArgSep(muParserHandle_t a_hParser, const muChar_t cArgSep)
{
  muParser_t* const p(AsParser(a_hParser));
  p->SetArgSep(cArgSep);
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetDecSep(muParserHandle_t a_hParser, const muChar_t cDecSep)
{
  muParser_t* const p(AsParser(a_hParser));
  p->SetDecSep(cDecSep);
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupSetThousandsSep(muParserHandle_t a_hParser, const muChar_t cThousandsSep)
{
  muParser_t* const p(AsParser(a_hParser));
  p->SetThousandsSep(cThousandsSep);
}

//---------------------------------------------------------------------------
/** \brief Retrieve name and value of a single parser constant.
    \param a_hParser [in] a valid parser handle
    \param a_iVar [in] Index of the constant to query
    \param a_pszName [out] pointer to a null terminated string with the constant name
    \param [out] The constant value
*/
API_EXPORT(void) mupGetConst( muParserHandle_t a_hParser, 
                              unsigned a_iVar,
                              const muChar_t **a_pszName, 
                              muFloat_t *a_fVal)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_TRY
    muParser_t* const p(AsParser(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();

    if (a_iVar>=ValMap.size())
    {
     *a_pszName = 0;
     *a_fVal = 0;
      return;
    }

    mu::valmap_type::const_iterator item;
    item = ValMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

    strncpy(szName, item->first.c_str(), sizeof(szName));
    szName[sizeof(szName)-1] = 0;

    *a_pszName = &szName[0];
    *a_fVal = item->second;
     return;

  MU_CATCH

  *a_pszName = 0;
  *a_fVal = 0;
}

//---------------------------------------------------------------------------
/** \brief Add a custom value regognition function.
*/
/*
MU_PARSER_API void mupAddValIdent(muParserHandle_t a_hParser, identfun_type a_pFun)
{
  MU_TRY
    parser_type p(AsParser(a_hParser));
    p->AddValIdent(a_pFun);
  MU_CATCH
}
*/

//---------------------------------------------------------------------------
/** \brief Query if an error occured.

    After querying the internal error bit will be reset. So a consecutive call
    will return false.
*/
API_EXPORT(muBool_t) mupError(muParserHandle_t a_hParser)
{
  bool bError( AsParserTag(a_hParser)->bError );
  AsParserTag(a_hParser)->bError = false;
  return bError;
}

//---------------------------------------------------------------------------
/** \brief Reset the internal error flag.
*/
API_EXPORT(void) mupErrorReset(muParserHandle_t a_hParser)
{
  AsParserTag(a_hParser)->bError = false;
}

//---------------------------------------------------------------------------
API_EXPORT(void) mupSetErrorHandler(muParserHandle_t a_hParser, muErrorHandler_t a_pHandler)
{
  AsParserTag(a_hParser)->errHandler = a_pHandler;
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
API_EXPORT(const char*) mupGetErrorMsg(muParserHandle_t a_hParser)
{
  return AsParserTag(a_hParser)->exc.GetMsg().c_str();
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
API_EXPORT(const char*) mupGetErrorToken(muParserHandle_t a_hParser)
{
  return AsParserTag(a_hParser)->exc.GetToken().c_str();
}

//---------------------------------------------------------------------------
/** \brief Return the code associated with the last error.
*/
API_EXPORT(int) mupGetErrorCode(muParserHandle_t a_hParser)
{
  return AsParserTag(a_hParser)->exc.GetCode();
}

//---------------------------------------------------------------------------
/** \brief Return the postion associated with the last error. */
API_EXPORT(int) mupGetErrorPos(muParserHandle_t a_hParser)
{
  return (int)AsParserTag(a_hParser)->exc.GetPos();
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(const muChar_t*) mupGetErrorExpr(muParserHandle_t a_hParser)
{
  return AsParserTag(a_hParser)->exc.GetExpr().c_str();
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(muFloat_t*) mupCreateVar()
{
  return new muFloat_t(0);
}

//-----------------------------------------------------------------------------------------------------
API_EXPORT(void) mupReleaseVar(muFloat_t *ptr)
{
  delete ptr;
}


#endif      // MUPARSER_DLL
