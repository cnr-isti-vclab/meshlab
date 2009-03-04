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
#ifndef MU_PARSER_DLL_H
#define MU_PARSER_DLL_H

#if defined(_WIN32)
    #ifdef MUPARSERLIB_EXPORTS
        #define API_EXPORT(TYPE) extern "C" __declspec(dllexport) TYPE __stdcall
    #else
        #define API_EXPORT(TYPE) extern __declspec(dllimport) TYPE __stdcall
    #endif
#else
    #define API_EXPORT(TYPE) TYPE
#endif

/** \file 
    \brief This file contains the DLL interface of muparser.
*/

// Basic types
typedef void*  muParserHandle_t;    // parser handle
typedef char   muChar_t;            // character type
typedef int    muBool_t;            // boolean type
typedef int    muInt_t;             // integer type 
typedef double muFloat_t;           // floating point type

// function types for calculation
typedef muFloat_t (*muFun0_t)(); 
typedef muFloat_t (*muFun1_t)(muFloat_t); 
typedef muFloat_t (*muFun2_t)(muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun3_t)(muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun4_t)(muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muFun5_t)(muFloat_t, muFloat_t, muFloat_t, muFloat_t, muFloat_t); 
typedef muFloat_t (*muMultFun_t)(const muFloat_t*, muInt_t);
typedef muFloat_t (*muStrFun1_t)(const muChar_t*);
typedef muFloat_t (*muStrFun2_t)(const muChar_t*, muFloat_t);
typedef muFloat_t (*muStrFun3_t)(const muChar_t*, muFloat_t, muFloat_t);

// Functions for parser management
typedef void (*muErrorHandler_t)(muParserHandle_t a_hParser);           // [optional] callback to an error handler
typedef muFloat_t* (*muFacFun_t)(const muChar_t*, void*);               // [optional] callback for creating new variables
typedef muInt_t (*muIdentFun_t)(const muChar_t*, muInt_t*, muFloat_t*); // [optional] value identification callbacks

//-----------------------------------------------------------------------------------------------------
//
//
// muParser C compatible bindings
//
//
//-----------------------------------------------------------------------------------------------------

// Basic operations / initialization  
API_EXPORT(muParserHandle_t) mupCreate();
API_EXPORT(void) mupRelease(muParserHandle_t a_hParser);
API_EXPORT(const muChar_t*) mupGetExpr(muParserHandle_t a_hParser);
API_EXPORT(void) mupSetExpr(muParserHandle_t a_hParser, const muChar_t *a_szExpr);
API_EXPORT(void) mupSetVarFactory(muParserHandle_t a_hParser, muFacFun_t a_pFactory, void* pUserData);

API_EXPORT(muFloat_t) mupEval(muParserHandle_t a_hParser);

// Defining callbacks / variables / constants
API_EXPORT(void) mupDefineFun0(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun0_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun1(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun1_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun2(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun2_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun3(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun3_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun4(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun4_t a_pFun, muBool_t a_bOptimize);
API_EXPORT(void) mupDefineFun5(muParserHandle_t a_hParser, const muChar_t *a_szName, muFun5_t a_pFun, muBool_t a_bOptimize);
// string functions
API_EXPORT(void) mupDefineStrFun1(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun1_t a_pFun);
API_EXPORT(void) mupDefineStrFun2(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun2_t a_pFun);
API_EXPORT(void) mupDefineStrFun3(muParserHandle_t a_hParser, const muChar_t *a_szName, muStrFun3_t a_pFun);

API_EXPORT(void) mupDefineMultFun( muParserHandle_t a_hParser, 
                                   const muChar_t* a_szName, 
                                   muMultFun_t a_pFun, 
                                   muBool_t a_bOptimize);

API_EXPORT(void) mupDefineOprt( muParserHandle_t a_hParser, 
                                const muChar_t* a_szName, 
                                muFun2_t a_pFun, 
                                muInt_t a_iPri1, 
                                muBool_t a_bOptimize);

API_EXPORT(void) mupDefineConst( muParserHandle_t a_hParser, 
                                 const muChar_t* a_szName, 
                                 muFloat_t a_fVal );

API_EXPORT(void) mupDefineStrConst( muParserHandle_t a_hParser, 
                                    const muChar_t* a_szName, 
                                    const muChar_t *a_sVal );

API_EXPORT(void) mupDefineVar( muParserHandle_t a_hParser, 
                               const muChar_t* a_szName, 
                               muFloat_t *a_fVar);

API_EXPORT(void) mupDefinePostfixOprt( muParserHandle_t a_hParser, 
                                       const muChar_t* a_szName, 
                                       muFun1_t a_pOprt, 
                                       muBool_t a_bOptimize);


API_EXPORT(void) mupDefineInfixOprt( muParserHandle_t a_hParser, 
                                     const muChar_t* a_szName, 
                                     muFun1_t a_pOprt, 
                                     muBool_t a_bOptimize);

// Define character sets for identifiers
API_EXPORT(void) mupDefineNameChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);
API_EXPORT(void) mupDefineOprtChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);
API_EXPORT(void) mupDefineInfixOprtChars(muParserHandle_t a_hParser, const muChar_t* a_szCharset);

// Remove all / single variables
API_EXPORT(void) mupRemoveVar(muParserHandle_t a_hParser, const muChar_t* a_szName);
API_EXPORT(void) mupClearVar(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearConst(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearOprt(muParserHandle_t a_hParser);
API_EXPORT(void) mupClearFun(muParserHandle_t a_hParser);

// Querying variables / expression variables / constants
API_EXPORT(int) mupGetExprVarNum(muParserHandle_t a_hParser);
API_EXPORT(int) mupGetVarNum(muParserHandle_t a_hParser);
API_EXPORT(int) mupGetConstNum(muParserHandle_t a_hParser);
API_EXPORT(void) mupGetExprVar(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t** a_pVar);
API_EXPORT(void) mupGetVar(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t** a_pVar);
API_EXPORT(void) mupGetConst(muParserHandle_t a_hParser, unsigned a_iVar, const muChar_t** a_pszName, muFloat_t* a_pVar);
API_EXPORT(void) mupSetArgSep(muParserHandle_t a_hParser, const muChar_t cArgSep);
API_EXPORT(void) mupSetDecSep(muParserHandle_t a_hParser, const muChar_t cArgSep);
API_EXPORT(void) mupSetThousandsSep(muParserHandle_t a_hParser, const muChar_t cArgSep);

// Add value recognition callbacks
API_EXPORT(void) mupAddValIdent(muParserHandle_t a_hParser, muIdentFun_t);

// Error handling
API_EXPORT(muBool_t) mupError(muParserHandle_t a_hParser);
API_EXPORT(void) mupErrorReset(muParserHandle_t a_hParser);
API_EXPORT(void) mupSetErrorHandler(muParserHandle_t a_hParser, muErrorHandler_t a_pErrHandler);
API_EXPORT(const muChar_t*) mupGetErrorMsg(muParserHandle_t a_hParser);
API_EXPORT(muInt_t) mupGetErrorCode(muParserHandle_t a_hParser);
API_EXPORT(muInt_t) mupGetErrorPos(muParserHandle_t a_hParser);
API_EXPORT(const muChar_t*) mupGetErrorToken(muParserHandle_t a_hParser);
API_EXPORT(const muChar_t*) mupGetErrorExpr(muParserHandle_t a_hParser);

// This is used for .NET only. It creates a new variable allowing the dll to
// manage the variable rather than the .NET garbage collector.
API_EXPORT(muFloat_t*) mupCreateVar();
API_EXPORT(void) mupReleaseVar(muFloat_t*);


#endif // include guard
