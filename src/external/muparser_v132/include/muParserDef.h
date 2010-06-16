/*
                 __________                                      
    _____   __ __\______   \_____  _______  ______  ____ _______ 
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|   
        \/                       \/            \/      \/        
  Copyright (C) 2010 Ingo Berg

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
#ifndef MUP_DEF_H
#define MUP_DEF_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include "muParserFixes.h"

/** \file
    \brief This file contains standard definitions used by the parser.
*/

#define MUP_VERSION _T("1.32")

/** \brief Define the base datatype for values.

  This datatype must be a built in value type. You can not use custom classes.
  It has been tested with float, double and long double types, int should
  work as well.
*/
#define MUP_BASETYPE double

/** \brief Definition of the basic bytecode datatype.

  This defines the smalles entity used in the bytecode.
*/
#define MUP_BYTECODE_TYPE long

#if defined(_UNICODE)
  /** \brief Definition of the basic parser string type. */
  #define MUP_STRING_TYPE std::wstring

  #if !defined(_T)
    #define _T(x) L##x
  #endif // not defined _T
#else
  #ifndef _T
  #define _T
  #endif
  
  /** \brief Definition of the basic parser string type. */
  #define MUP_STRING_TYPE std::string
#endif

#if defined(_DEBUG)
  /** \brief Debug macro to force an abortion of the programm with a certain message.
  */
  #define MUP_FAIL(MSG)    \
          bool MSG=false;  \
          assert(MSG);

  #ifndef _UNICODE
    /** \brief An assertion that does not kill the program.

        This macro is neutralised in UNICODE builds. It's
        too difficult to translate.
    */
    #define MUP_ASSERT(COND)                         \
            if (!(COND))                             \
            {                                        \
              stringstream_type ss;                  \
              ss << "Assertion \""#COND"\" failed: " \
                 << __FILE__ << " line "             \
                 << __LINE__ << ".";                 \
              throw ParserError( ss.str() );         \
            }
  #else
    #define MUP_ASSERT(COND)
  #endif // _UNICODE
#else
  #define MUP_FAIL(MSG)
  #define MUP_ASSERT(COND)
#endif

//------------------------------------------------------------------------------
//
// do not change anything beyond this point...
//
// !!! This section is devoted to macros that are used for debugging
// !!! or for features that are not fully implemented yet.
//
//#define MUP_DUMP_STACK
//#define MUP_DUMP_CMDCODE


namespace mu
{
#if defined(_UNICODE)

  //------------------------------------------------------------------------------
  /** \brief Encapsulate wcout. */
  inline std::wostream& console()
  {
    return std::wcout;
  }

  /** \brief Encapsulate cin. */
  inline std::wistream& console_in()
  {
    return std::wcin;
  }

#else

  /** \brief Encapsulate cout. 
  
    Used for supporting UNICODE more easily.
  */
  inline std::ostream& console()
  {
    return std::cout;
  }

  /** \brief Encapsulate cin. 

    Used for supporting UNICODE more easily.
  */
  inline std::istream& console_in()
  {
    return std::cin;
  }

#endif

  //------------------------------------------------------------------------------
  /** \brief Bytecode values.

      \attention The order of the operator entries must match the order in ParserBase::c_DefaultOprt!
  */
  enum ECmdCode
  {
    // The following are codes for built in binary operators
    // apart from built in operators the user has the opportunity to
    // add user defined operators.
    cmLE            = 0,   ///< Operator item:  less or equal
    cmGE            = 1,   ///< Operator item:  greater or equal
    cmNEQ           = 2,   ///< Operator item:  not equal
    cmEQ            = 3,   ///< Operator item:  equals
    cmLT            = 4,   ///< Operator item:  less than
    cmGT            = 5,   ///< Operator item:  greater than
    cmADD           = 6,   ///< Operator item:  add
    cmSUB           = 7,   ///< Operator item:  subtract
    cmMUL           = 8,   ///< Operator item:  multiply
    cmDIV           = 9,   ///< Operator item:  division
    cmPOW           = 10,  ///< Operator item:  y to the power of ...
    cmAND           = 11,  ///< Operator item:  logical and
    cmOR            = 12,  ///< Operator item:  logical or
    cmXOR           = 13,  ///< Operator item:  logical xor
    cmASSIGN        = 14,  ///< Operator item:  Assignment operator
    cmBO            = 15,  ///< Operator item:  opening bracket
    cmBC            = 16,  ///< Operator item:  closing bracket
    cmARG_SEP,             ///< function argument separator
    cmVAR,                 ///< variable item
    cmVAL,                 ///< value item
    cmFUNC,                ///< Code for a function item
    cmFUNC_STR,            ///< Code for a function with a string parameter
    cmSTRING,              ///< Code for a string token
    cmOPRT_BIN,            ///< user defined binary operator
    cmOPRT_POSTFIX,        ///< code for postfix operators
    cmOPRT_INFIX,          ///< code for infix operators
    cmEND,                 ///< end of formula
    cmUNKNOWN,             ///< uninitialized item
  };

  //------------------------------------------------------------------------------
  /** \brief Types internally used by the parser.
  */
  enum ETypeCode
  {
    tpSTR  = 0,     ///< String type (Function arguments and constants only, no string variables)
    tpDBL  = 1,     ///< Floating point variables
    tpVOID = 2      ///< Undefined type.
  };

  //------------------------------------------------------------------------------
  /** \brief Parser operator precedence values. */
  enum EPrec
  {
    // binary operators
    prLOGIC   = 1,  ///< logic operators
    prCMP     = 2,  ///< comparsion operators
    prADD_SUB = 3,  ///< addition
    prMUL_DIV = 4,  ///< multiplication/division
    prPOW     = 5,  ///< power operator priority (highest)

    // infix operators
    prINFIX    = 4, ///< Signs have a higher priority than ADD_SUB, but lower than power operator
    prPOSTFIX  = 4  ///< Postfix operator priority (currently unused)
  };

  //------------------------------------------------------------------------------
  // basic types

  /** \brief The numeric datatype used by the parser. 
  
    Normally this is a floating point type either single or double precision.
  */
  typedef MUP_BASETYPE value_type;

  /** \brief The stringtype used by the parser. 

    Depends on wether UNICODE is used or not.
  */
  typedef MUP_STRING_TYPE string_type;

  /** \brief The bytecode type used by the parser. 
  
    The bytecode type depends on the value_type.
  */
  typedef MUP_BYTECODE_TYPE bytecode_type;

  /** \brief The character type used by the parser. 
  
    Depends on wether UNICODE is used or not.
  */
  typedef string_type::value_type char_type;

  /** \brief Typedef for easily using stringstream that respect the parser stringtype. */
  typedef std::basic_stringstream<char_type,
                                  std::char_traits<char_type>,
                                  std::allocator<char_type> > stringstream_type;

  // Data container types

  /** \brief Type used for storing variables. */
  typedef std::map<string_type, value_type*> varmap_type;
  
  /** \brief Type used for storing constants. */
  typedef std::map<string_type, value_type> valmap_type;
  
  /** \brief Type for assigning a string name to an index in the internal string table. */
  typedef std::map<string_type, std::size_t> strmap_type;

  // Parser callbacks
  
  /** \brief Callback type used for functions without arguments. */
  typedef value_type (*fun_type0)();

  /** \brief Callback type used for functions with a single arguments. */
  typedef value_type (*fun_type1)(value_type);

  /** \brief Callback type used for functions with two arguments. */
  typedef value_type (*fun_type2)(value_type, value_type);

  /** \brief Callback type used for functions with three arguments. */
  typedef value_type (*fun_type3)(value_type, value_type, value_type);

  /** \brief Callback type used for functions with four arguments. */
  typedef value_type (*fun_type4)(value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with five arguments. */
  typedef value_type (*fun_type5)(value_type, value_type, value_type, value_type, value_type);

  /** \brief Callback type used for functions with a variable argument list. */
  typedef value_type (*multfun_type)(const value_type*, int);

  /** \brief Callback type used for functions taking a string as an argument. */
  typedef value_type (*strfun_type1)(const char_type*);

  /** \brief Callback type used for functions taking a string and a value as arguments. */
  typedef value_type (*strfun_type2)(const char_type*, value_type);

  /** \brief Callback type used for functions taking a string and two values as arguments. */
  typedef value_type (*strfun_type3)(const char_type*, value_type, value_type);

  /** \brief Callback used for functions that identify values in a string. */
  typedef int (*identfun_type)(const char_type *sExpr, int *nPos, value_type *fVal);

  /** \brief Callback used for variable creation factory functions. */
  typedef value_type* (*facfun_type)(const char_type*, void*);

  //------------------------------------------------------------------------------
  /** \brief Static type checks
    
     I took the static assert from boost, but did not want to 
     add boost as a dependency to this project. For the original go to:
  
     http://www.boost.org/doc/html/boost_staticassert.html
  */
  template <bool> struct STATIC_ASSERTION_FAILURE;
  template <> struct STATIC_ASSERTION_FAILURE<true> {};

  /** \brief This is a static typecheck.
      
      If you get a compiler error here you tried to use an unsigned bytecode map type!
  */
  typedef char MAP_TYPE_CANT_BE_UNSIGNED[ sizeof( STATIC_ASSERTION_FAILURE< bytecode_type(-1)<0 >) ];
} // end fo namespace

#endif

