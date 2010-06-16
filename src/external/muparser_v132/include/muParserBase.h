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
#ifndef MU_PARSER_BASE_H
#define MU_PARSER_BASE_H

//--- Standard includes ------------------------------------------------------------------------
#include <cmath>
#include <string>
#include <iostream>
#include <map>
#include <memory>

//--- Parser includes --------------------------------------------------------------------------
#include "muParserDef.h"
#include "muParserStack.h"
#include "muParserTokenReader.h"
#include "muParserBytecode.h"
#include "muParserError.h"


namespace mu
{
/** \file
    \brief This file contains the class definition of the muparser engine.
*/

//--------------------------------------------------------------------------------------------------
/** \brief Mathematical expressions parser (base parser engine).
  
  Version 1.32 (20100113)

  This is the implementation of a bytecode based mathematical expressions parser. 
  The formula will be parsed from string and converted into a bytecode. 
  Future calculations will be done with the bytecode instead the formula string
  resulting in a significant performance increase. 
  Complementary to a set of internally implemented functions the parser is able to handle 
  user defined functions and variables. 

  \author (C) 2010 Ingo Berg
*/
class ParserBase 
{
friend class ParserTokenReader;

private:

    /** \brief Typedef for the parse functions. 
    
      The parse function do the actual work. The parser exchanges
      the function pointer to the parser function depending on 
      which state it is in. (i.e. bytecode parser vs. string parser)
    */
    typedef value_type (ParserBase::*ParseFunction)() const;  

    /** \brief Type used for storing an array of values. */
    typedef std::vector<value_type> valbuf_type;

    /** \brief Type for a vector of strings. */
    typedef std::vector<string_type> stringbuf_type;

    /** \brief Typedef for the token reader. */
    typedef ParserTokenReader token_reader_type;
    
    /** \brief Type used for parser tokens. */
    typedef ParserToken<value_type, string_type> token_type;

 public:

    /** \brief Type of the error class. 
    
      Included for backwards compatibility.
    */
    typedef ParserError exception_type;

    ParserBase(); 
    ParserBase(const ParserBase &a_Parser);
    ParserBase& operator=(const ParserBase &a_Parser);

    virtual ~ParserBase();
    
    //---------------------------------------------------------------------------
    /** \brief Calculate the result.

      A note on const correctness: 
      I consider it important that Calc is a const function.
      Due to caching operations Calc changes only the state of internal variables with one exception
      m_UsedVar this is reset during string parsing and accessible from the outside. Instead of making
      Calc non const GetUsedVar is non const because it explicitely calls Eval() forcing this update. 

      \pre A formula must be set.
      \pre Variables must have been set (if needed)
  
      \sa #m_pParseFormula
      \return The evaluation result
      \throw ParseException if no Formula is set or in case of any other error related to the formula.
    */
	  inline value_type Eval() const
    {
      return (this->*m_pParseFormula)(); 
    }

    void SetExpr(const string_type &a_sExpr);
    void SetVarFactory(facfun_type a_pFactory, void *pUserData = NULL);

    void EnableOptimizer(bool a_bIsOn=true);
    void EnableByteCode(bool a_bIsOn=true);
    void EnableBuiltInOprt(bool a_bIsOn=true);

    bool HasBuiltInOprt() const;
    void AddValIdent(identfun_type a_pCallback);

/** \fn void mu::ParserBase::DefineFun(const string_type &a_strName, fun_type0 a_pFun, bool a_bAllowOpt = true) 
    \brief Define a parser function without arguments.
    \param a_strName Name of the function
    \param a_pFun Pointer to the callback function
    \param a_bAllowOpt A flag indicating this function may be optimized
*/

#define MUP_DEFINE_FUNC(TYPE)                                                                  \
    inline void DefineFun(const string_type &a_strName, TYPE a_pFun, bool a_bAllowOpt = true)  \
    {                                                                                          \
      AddCallback( a_strName, ParserCallback(a_pFun, a_bAllowOpt),                             \
                   m_FunDef, ValidNameChars() );                                               \
    }

    MUP_DEFINE_FUNC(fun_type0)
    MUP_DEFINE_FUNC(fun_type1)
    MUP_DEFINE_FUNC(fun_type2)
    MUP_DEFINE_FUNC(fun_type3)
    MUP_DEFINE_FUNC(fun_type4)
    MUP_DEFINE_FUNC(fun_type5)
    MUP_DEFINE_FUNC(multfun_type)
    MUP_DEFINE_FUNC(strfun_type1)
    MUP_DEFINE_FUNC(strfun_type2)
    MUP_DEFINE_FUNC(strfun_type3)
#undef MUP_DEFINE_FUNC

    void DefineOprt(const string_type &a_strName, fun_type2 a_pFun, unsigned a_iPri=0, bool a_bAllowOpt = false);
    void DefineConst(const string_type &a_sName, value_type a_fVal);
    void DefineStrConst(const string_type &a_sName, const string_type &a_strVal);
    void DefineVar(const string_type &a_sName, value_type *a_fVar);
    void DefinePostfixOprt(const string_type &a_strFun, fun_type1 a_pOprt, bool a_bAllowOpt=true);
    void DefineInfixOprt(const string_type &a_strName, fun_type1 a_pOprt, int a_iPrec=prINFIX, bool a_bAllowOpt=true);

    // Clear user defined variables, constants or functions
    void ClearVar();
    void ClearFun();
    void ClearConst();
    void ClearInfixOprt();
    void ClearPostfixOprt();
    void ClearOprt();
    
    void RemoveVar(const string_type &a_strVarName);
    const varmap_type& GetUsedVar() const;
    const varmap_type& GetVar() const;
    const valmap_type& GetConst() const;
    const string_type& GetExpr() const;
    const funmap_type& GetFunDef() const;
    string_type GetVersion() const;

    const char_type ** GetOprtDef() const;
    void DefineNameChars(const char_type *a_szCharset);
    void DefineOprtChars(const char_type *a_szCharset);
    void DefineInfixOprtChars(const char_type *a_szCharset);

    const char_type* ValidNameChars() const;
    const char_type* ValidOprtChars() const;
    const char_type* ValidInfixOprtChars() const;

    void SetArgSep(char_type cArgSep);
    char_type GetArgSep() const;
    
    void  Error(EErrorCodes a_iErrc, 
                int a_iPos = (int)mu::string_type::npos, 
                const string_type &a_strTok = string_type() ) const;

 protected:
	  
    void Init();

    virtual void InitCharSets() = 0;
    virtual void InitFun() = 0;
    virtual void InitConst() = 0;
    virtual void InitOprt() = 0; 

    virtual void OnDetectVar(string_type *pExpr, int &nStart, int &nEnd);
    static const char_type *c_DefaultOprt[]; 

 private:

    void Assign(const ParserBase &a_Parser);
    void InitTokenReader();
    void ReInit() const;

    void AddCallback( const string_type &a_strName, 
                      const ParserCallback &a_Callback, 
                      funmap_type &a_Storage,
                      const char_type *a_szCharSet );

    void ApplyBinOprt(ParserStack<token_type> &a_stOpt,
                      ParserStack<token_type> &a_stVal) const;

    void ApplyFunc(ParserStack<token_type> &a_stOpt,
                   ParserStack<token_type> &a_stVal, 
                   int iArgCount) const; 

    token_type ApplyNumFunc(const token_type &a_FunTok,
                            const std::vector<token_type> &a_vArg) const;

    token_type ApplyStrFunc(const token_type &a_FunTok,
                            const std::vector<token_type> &a_vArg) const;

    int GetOprtPri(const token_type &a_Tok) const;

    value_type ParseString() const; 
    value_type ParseCmdCode() const;
    value_type ParseValue() const;

    void  ClearFormula();
    void  CheckName(const string_type &a_strName, const string_type &a_CharSet) const;

#if defined(MUP_DUMP_STACK) | defined(MUP_DUMP_CMDCODE)
    void StackDump(const ParserStack<token_type > &a_stVal, 
                   const ParserStack<token_type > &a_stOprt) const;
#endif

    /** \brief Pointer to the parser function. 
    
      Eval() calls the function whose address is stored there.
    */
    mutable ParseFunction  m_pParseFormula;
    mutable const ParserByteCode::map_type *m_pCmdCode; ///< Formula converted to bytecode, points to the data of the bytecode class.
    mutable ParserByteCode m_vByteCode;   ///< The Bytecode class.
    mutable stringbuf_type  m_vStringBuf; ///< String buffer, used for storing string function arguments
    stringbuf_type  m_vStringVarBuf;

    std::auto_ptr<token_reader_type> m_pTokenReader; ///< Managed pointer to the token reader object.

    funmap_type  m_FunDef;        ///< Map of function names and pointers.
    funmap_type  m_PostOprtDef;   ///< Postfix operator callbacks
    funmap_type  m_InfixOprtDef;  ///< unary infix operator.
    funmap_type  m_OprtDef;       ///< Binary operator callbacks
    valmap_type  m_ConstDef;      ///< user constants.
    strmap_type  m_StrVarDef;     ///< user defined string constants
    varmap_type  m_VarDef;        ///< user defind variables.

    bool m_bOptimize;             ///< Flag that indicates if the optimizer is on or off.
    bool m_bUseByteCode;          ///< Flag that indicates if bytecode parsing is on or off.
    bool m_bBuiltInOp;            ///< Flag that can be used for switching built in operators on and off

    string_type m_sNameChars;      ///< Charset for names
    string_type m_sOprtChars;      ///< Charset for postfix/ binary operator tokens
    string_type m_sInfixOprtChars; ///< Charset for infix operator tokens

    mutable valbuf_type m_vStackBuffer; ///< This is merely a buffer used for the stack in the cmd parsing routine
};

} // namespace mu

#endif

