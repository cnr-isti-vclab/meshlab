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

#include "muParser.h"

//--- Standard includes ------------------------------------------------------------------------
#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
#include <deque>
#include <sstream>
#include <locale>

using namespace std;

/** \file
    \brief This file contains the basic implementation of the muparser engine.
*/

namespace mu
{
  //------------------------------------------------------------------------------
  /** \brief Identifiers for built in binary operators. 

      When defining custom binary operators with #AddOprt(...) make sure not to choose 
      names conflicting with these definitions. 
  */
  const char_type* ParserBase::c_DefaultOprt[] = 
  { 
    _T("<="), _T(">="),  _T("!="), 
    _T("=="), _T("<"),   _T(">"), 
    _T("+"),  _T("-"),   _T("*"), 
    _T("/"),  _T("^"),   _T("and"), 
    _T("or"), _T("xor"), _T("="), 
    _T("("),  _T(")"), 0 
  };

  //------------------------------------------------------------------------------
  /** \brief Constructor.
      \param a_szFormula the formula to interpret.
      \throw ParserException if a_szFormula is null.
  */
  ParserBase::ParserBase()
    :m_pParseFormula(&ParserBase::ParseString)
    ,m_pCmdCode(0)
    ,m_vByteCode()
    ,m_vStringBuf()
    ,m_pTokenReader()
    ,m_FunDef()
    ,m_PostOprtDef()
    ,m_InfixOprtDef()
    ,m_OprtDef()
    ,m_ConstDef()
    ,m_StrVarDef()
    ,m_VarDef()
    ,m_bOptimize(true)
    ,m_bUseByteCode(true)
    ,m_bBuiltInOp(true)
    ,m_sNameChars()
    ,m_sOprtChars()
    ,m_sInfixOprtChars()
    ,m_vStackBuffer()
  {
    InitTokenReader();
  }

  //---------------------------------------------------------------------------
  /** \brief Copy constructor. 

    Tha parser can be safely copy constructed but the bytecode is reset during
    copy construction.
  */
  ParserBase::ParserBase(const ParserBase &a_Parser)
    :m_pParseFormula(&ParserBase::ParseString)
    ,m_pCmdCode(0)
    ,m_vByteCode()
    ,m_vStringBuf()
    ,m_pTokenReader()
    ,m_FunDef()
    ,m_PostOprtDef()
    ,m_InfixOprtDef()
    ,m_OprtDef()
    ,m_ConstDef()
    ,m_StrVarDef()
    ,m_VarDef()
    ,m_bOptimize(true)
    ,m_bUseByteCode(true)
    ,m_bBuiltInOp(true)
  {
    m_pTokenReader.reset(new token_reader_type(this));
    Assign(a_Parser);
  }

  //---------------------------------------------------------------------------
  ParserBase::~ParserBase()
  {}

  //---------------------------------------------------------------------------
  /** \brief Assignement operator. 

    Implemented by calling Assign(a_Parser). Self assignement is suppressed.
    \param a_Parser Object to copy to this.
    \return *this
    \throw nothrow
  */
  ParserBase& ParserBase::operator=(const ParserBase &a_Parser)
  {
    Assign(a_Parser);
    return *this;
  }

  //---------------------------------------------------------------------------
  /** \brief Copy state of a parser object to this. 

    Clears Variables and Functions of this parser.
    Copies the states of all internal variables.
    Resets parse function to string parse mode.

    \param a_Parser the source object.
  */
  void ParserBase::Assign(const ParserBase &a_Parser)
  {
    if (&a_Parser==this)
      return;

    // Don't copy bytecode instead cause the parser to create new bytecode
    // by resetting the parse function.
    ReInit();

    m_ConstDef = a_Parser.m_ConstDef;         // Copy user define constants
    m_VarDef = a_Parser.m_VarDef;             // Copy user defined variables
    m_bOptimize  = a_Parser.m_bOptimize;
    m_bUseByteCode = a_Parser.m_bUseByteCode;
    m_bBuiltInOp = a_Parser.m_bBuiltInOp;
    m_vStringBuf = a_Parser.m_vStringBuf;
    m_pTokenReader.reset(a_Parser.m_pTokenReader->Clone(this));
    m_StrVarDef = a_Parser.m_StrVarDef;
    m_vStringVarBuf = a_Parser.m_vStringVarBuf;

    // Copy function and operator callbacks
    m_FunDef = a_Parser.m_FunDef;             // Copy function definitions
    m_PostOprtDef = a_Parser.m_PostOprtDef;   // post value unary operators
    m_InfixOprtDef = a_Parser.m_InfixOprtDef; // unary operators for infix notation

    m_sNameChars = a_Parser.m_sNameChars;
    m_sOprtChars = a_Parser.m_sOprtChars;
    m_sInfixOprtChars = a_Parser.m_sInfixOprtChars;
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize the token reader. 

    Create new token reader object and submit pointers to function, operator,
    constant and variable definitions.

    \post m_pTokenReader.get()!=0
    \throw nothrow
  */
  void ParserBase::InitTokenReader()
  {
    m_pTokenReader.reset(new token_reader_type(this));
  }

  //---------------------------------------------------------------------------
  /** \brief Reset parser to string parsing mode and clear internal buffers.

      Clear bytecode, reset the token reader.
      \throw nothrow
  */
  void ParserBase::ReInit() const
  {
    m_pParseFormula = &ParserBase::ParseString;
    m_vStringBuf.clear();
    m_vByteCode.clear();
    m_pTokenReader->ReInit();
  }

  //---------------------------------------------------------------------------
  void ParserBase::OnDetectVar(string_type * /*pExpr*/, int & /*nStart*/, int & /*nEnd*/)
  {}

  //---------------------------------------------------------------------------
  /** \brief Returns the version of muparser. 
  
    Format is as follows: "MAJOR.MINOR (OPTIONAL TEXT)"
  */
  string_type ParserBase::GetVersion() const
  {
    return MUP_VERSION;
  }

  //---------------------------------------------------------------------------
  /** \brief Add a value parsing function. 
      
      When parsing an expression muParser tries to detect values in the expression
      string using different valident callbacks. Thuis it's possible to parse
      for hex values, binary values and floating point values. 
  */
  void ParserBase::AddValIdent(identfun_type a_pCallback)
  {
    m_pTokenReader->AddValIdent(a_pCallback);
  }

  //---------------------------------------------------------------------------
  /** \brief Set a function that can create variable pointer for unknown expression variables. 
      \param a_pFactory A pointer to the variable factory.
      \param pUserData A user defined context pointer.
  */
  void ParserBase::SetVarFactory(facfun_type a_pFactory, void *pUserData)
  {
    m_pTokenReader->SetVarCreator(a_pFactory, pUserData);  
  }

  //---------------------------------------------------------------------------
  /** \brief Add a function or operator callback to the parser. */
  void ParserBase::AddCallback( const string_type &a_strName,
                                const ParserCallback &a_Callback, 
                                funmap_type &a_Storage,
                                const char_type *a_szCharSet )
  {
    if (a_Callback.GetAddr()==0)
        Error(ecINVALID_FUN_PTR);

    const funmap_type *pFunMap = &a_Storage;

    // Check for conflicting operator or function names
    if ( pFunMap!=&m_FunDef && m_FunDef.find(a_strName)!=m_FunDef.end() )
      Error(ecNAME_CONFLICT);

    if ( pFunMap!=&m_PostOprtDef && m_PostOprtDef.find(a_strName)!=m_PostOprtDef.end() )
      Error(ecNAME_CONFLICT);

    if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_InfixOprtDef.find(a_strName)!=m_InfixOprtDef.end() )
      Error(ecNAME_CONFLICT);

    if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_OprtDef.find(a_strName)!=m_OprtDef.end() )
      Error(ecNAME_CONFLICT);

    CheckName(a_strName, a_szCharSet);
    a_Storage[a_strName] = a_Callback;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Check if a name contains invalid characters. 

      \throw ParserException if the name contains invalid charakters.
  */
  void ParserBase::CheckName(const string_type &a_sName,
                            const string_type &a_szCharSet) const
  {
    if ( !a_sName.length() ||
        (a_sName.find_first_not_of(a_szCharSet)!=string_type::npos) ||
        (a_sName[0]>='0' && a_sName[0]<='9'))
    {
      Error(ecINVALID_NAME);
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Set the formula. 
      \param a_strFormula Formula as string_type
      \throw ParserException in case of syntax errors.

      Triggers first time calculation thus the creation of the bytecode and
      scanning of used variables.
  */
  void ParserBase::SetExpr(const string_type &a_sExpr)
  {
    // Check locale compatibility
    std::locale loc;
    if (m_pTokenReader->GetArgSep()==std::use_facet<numpunct<char_type> >(loc).decimal_point())
      Error(ecLOCALE);

    // <ibg> 20060222: Bugfix for Borland-Kylix:
    // adding a space to the expression will keep Borlands KYLIX from going wild
    // when calling tellg on a stringstream created from the expression after 
    // reading a value at the end of an expression. (mu::Parser::IsVal function)
    // (tellg returns -1 otherwise causing the parser to ignore the value)
    string_type sBuf(a_sExpr + _T(" ") );
    m_pTokenReader->SetFormula(sBuf);
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Get the default symbols used for the built in operators. 
      \sa c_DefaultOprt
  */
  const char_type** ParserBase::GetOprtDef() const
  {
    return (const char_type **)(&c_DefaultOprt[0]);
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             functions, variables, constants.
  */
  void ParserBase::DefineNameChars(const char_type *a_szCharset)
  {
    m_sNameChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             binary operators and postfix operators.
  */
  void ParserBase::DefineOprtChars(const char_type *a_szCharset)
  {
    m_sOprtChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Define the set of valid characters to be used in names of
             infix operators.
  */
  void ParserBase::DefineInfixOprtChars(const char_type *a_szCharset)
  {
    m_sInfixOprtChars = a_szCharset;
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in name identifiers. 
      \sa #ValidOprtChars, #ValidPrefixOprtChars
  */ 
  const char_type* ParserBase::ValidNameChars() const
  {
    assert(m_sNameChars.size());
    return m_sNameChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in operator definitions. 
      \sa #ValidNameChars, #ValidPrefixOprtChars
  */
  const char_type* ParserBase::ValidOprtChars() const
  {
    assert(m_sOprtChars.size());
    return m_sOprtChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Virtual function that defines the characters allowed in infix operator definitions.
      \sa #ValidNameChars, #ValidOprtChars
  */
  const char_type* ParserBase::ValidInfixOprtChars() const
  {
    assert(m_sInfixOprtChars.size());
    return m_sInfixOprtChars.c_str();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined operator. 
      \post Will reset the Parser to string parsing mode.
  */
  void ParserBase::DefinePostfixOprt(const string_type &a_sName, 
                                     fun_type1 a_pFun,
                                     bool a_bAllowOpt)
  {
    AddCallback(a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, prPOSTFIX, cmOPRT_POSTFIX),
                m_PostOprtDef, 
                ValidOprtChars() );
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize user defined functions. 
   
    Calls the virtual functions InitFun(), InitConst() and InitOprt().
  */
  void ParserBase::Init()
  {
    InitCharSets();
    InitFun();
    InitConst();
    InitOprt();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined operator. 
      \post Will reset the Parser to string parsing mode.
      \param [in] a_sName  operator Identifier 
      \param [in] a_pFun  Operator callback function
      \param [in] a_iPrec  Operator Precedence (default=prSIGN)
      \param [in] a_bAllowOpt  True if operator is volatile (default=false)
      \sa EPrec
  */
  void ParserBase::DefineInfixOprt(const string_type &a_sName, 
                                  fun_type1 a_pFun, 
                                  int a_iPrec, 
                                  bool a_bAllowOpt)
  {
    AddCallback(a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, cmOPRT_INFIX), 
                m_InfixOprtDef, 
                ValidOprtChars() );
  }


  //---------------------------------------------------------------------------
  /** \brief Define a binary operator. 
      \param [in] a_pFun Pointer to the callback function.
      \param [in] a_iPrec Precedence of the operator.
      \param [in] a_bAllowOpt If this is true the operator may be optimized away.
  */
  void ParserBase::DefineOprt( const string_type &a_sName, 
                               fun_type2 a_pFun, 
                               unsigned a_iPrec, 
                               bool a_bAllowOpt )
  {
    // Check for conflicts with built in operator names
    for (int i=0; m_bBuiltInOp && i<cmARG_SEP; ++i)
      if (a_sName == string_type(c_DefaultOprt[i]))
        Error(ecBUILTIN_OVERLOAD, -1, a_sName);

    AddCallback( a_sName, 
                ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, cmOPRT_BIN), 
                m_OprtDef, 
                ValidOprtChars() );
  }

  //---------------------------------------------------------------------------
  /** \brief Define a new string constant.
      \param [in] a_strName The name of the constant.
      \param [in] a_strVal the value of the constant. 
  */
  void ParserBase::DefineStrConst(const string_type &a_strName, const string_type &a_strVal)
  {
    // Test if a constant with that names already exists
    if (m_StrVarDef.find(a_strName)!=m_StrVarDef.end())
      Error(ecNAME_CONFLICT);

    CheckName(a_strName, ValidNameChars());
    
    m_vStringVarBuf.push_back(a_strVal);           // Store variable string in internal buffer
    m_StrVarDef[a_strName] = m_vStringBuf.size();  // bind buffer index to variable name

    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined variable. 
      \param [in] a_sName the variable name
      \param [in] a_pVar A pointer to the variable vaule.
      \post Will reset the Parser to string parsing mode.
      \pre [assert] a_fVar!=0
      \throw ParserException in case the name contains invalid signs.
  */
  void ParserBase::DefineVar(const string_type &a_sName, value_type *a_pVar)
  {
    if (a_pVar==0)
      Error(ecINVALID_VAR_PTR);

    // Test if a constant with that names already exists
    if (m_ConstDef.find(a_sName)!=m_ConstDef.end())
      Error(ecNAME_CONFLICT);

//    if (m_FunDef.find(a_sName)!=m_FunDef.end())
//      Error(ecNAME_CONFLICT);

    CheckName(a_sName, ValidNameChars());
    m_VarDef[a_sName] = a_pVar;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Add a user defined constant. 
      \param [in] a_sName The name of the constant.
      \param [in] a_fVal the value of the constant.
      \post Will reset the Parser to string parsing mode.
      \throw ParserException in case the name contains invalid signs.
  */
  void ParserBase::DefineConst(const string_type &a_sName, value_type a_fVal)
  {
    CheckName(a_sName, ValidNameChars());
    m_ConstDef[a_sName] = a_fVal;
    ReInit();
  }

  //---------------------------------------------------------------------------
  /** \brief Get operator priority.
      \throw ParserException if a_Oprt is no operator code
  */
  int ParserBase::GetOprtPri(const token_type &a_Tok) const
  {
    switch (a_Tok.GetCode())
    {
    // built in operators
    case cmEND:      return -5;
	  case cmARG_SEP:  return -4;
    case cmBO :	
    case cmBC :      return -2;
    case cmASSIGN:   return -1;               
    case cmAND:
    case cmXOR:
    case cmOR:       return  prLOGIC;  
    case cmLT:
    case cmGT:
    case cmLE:
    case cmGE:
    case cmNEQ:
    case cmEQ:       return  prCMP; 
    case cmADD:
    case cmSUB:      return  prADD_SUB;
    case cmMUL:
    case cmDIV:      return  prMUL_DIV;
    case cmPOW:      return  prPOW;

    // user defined binary operators
    case cmOPRT_INFIX: 
    case cmOPRT_BIN:   return a_Tok.GetPri();
    default:  Error(ecINTERNAL_ERROR, 5);
              return 999;
    }  
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing the used variables only. */
  const varmap_type& ParserBase::GetUsedVar() const
  {
    try
    {
      m_pTokenReader->IgnoreUndefVar(true);
      ParseString(); // implicitely create or update the map with the
                     // used variables stored in the token reader if not already done
      m_pTokenReader->IgnoreUndefVar(false);
    }
    catch(exception_type &e)
    {
      m_pTokenReader->IgnoreUndefVar(false);
      throw e;
    }
    
    // Make sure to stay in string parse mode, dont call ReInit()
    // because it deletes the array with the used variables
    m_pParseFormula = &ParserBase::ParseString;
    
    return m_pTokenReader->GetUsedVar();
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing the used variables only. */
  const varmap_type& ParserBase::GetVar() const
  {
    return m_VarDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Return a map containing all parser constants. */
  const valmap_type& ParserBase::GetConst() const
  {
    return m_ConstDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Return prototypes of all parser functions.
      \return #m_FunDef
      \sa FunProt
      \throw nothrow
      
      The return type is a map of the public type #funmap_type containing the prototype
      definitions for all numerical parser functions. String functions are not part of 
      this map. The Prototype definition is encapsulated in objects of the class FunProt
      one per parser function each associated with function names via a map construct.
  */
  const funmap_type& ParserBase::GetFunDef() const
  {
    return m_FunDef;
  }

  //---------------------------------------------------------------------------
  /** \brief Retrieve the formula. */
  const string_type& ParserBase::GetExpr() const
  {
    return m_pTokenReader->GetFormula();
  }

  //---------------------------------------------------------------------------
  ParserBase::token_type ParserBase::ApplyNumFunc(const token_type &a_FunTok,
                                                  const std::vector<token_type> &a_vArg) const
  {
    token_type  valTok;
    int  iArgCount = (unsigned)a_vArg.size();
    void  *pFunc = a_FunTok.GetFuncAddr();
    assert(pFunc);

    // Collect the function arguments from the value stack
    switch(a_FunTok.GetArgCount())
    {
      case -1:
            // Function with variable argument count
 		        // copy arguments into a vector<value_type> 
	          {
              if (iArgCount==0)
                Error(ecTOO_FEW_PARAMS, m_pTokenReader->GetPos(), a_FunTok.GetAsString());

              std::vector<value_type> vArg(iArgCount);
		          for (int i=0; i<iArgCount; ++i)
		            vArg[iArgCount-(i+1)] = a_vArg[i].GetVal();

              valTok.SetVal( ((multfun_type)a_FunTok.GetFuncAddr())(&vArg[0], (int)vArg.size()) );  
	          }
	          break;

      case 0: valTok.SetVal( ((fun_type0)pFunc)() );  break;
      case 1: valTok.SetVal( ((fun_type1)pFunc)(a_vArg[0].GetVal()) );  break;
      case 2: valTok.SetVal( ((fun_type2)pFunc)(a_vArg[1].GetVal(),
		                                            a_vArg[0].GetVal()) );  break;
      case 3: valTok.SetVal( ((fun_type3)pFunc)(a_vArg[2].GetVal(), 
		                                            a_vArg[1].GetVal(), 
														                    a_vArg[0].GetVal()) ); break;
      case 4: valTok.SetVal( ((fun_type4)pFunc)(a_vArg[3].GetVal(),
	                                              a_vArg[2].GetVal(), 
				   	  	                                a_vArg[1].GetVal(),
													                      a_vArg[0].GetVal()) );  break;
      case 5: valTok.SetVal( ((fun_type5)pFunc)(a_vArg[4].GetVal(), 
														                    a_vArg[3].GetVal(), 
	                                              a_vArg[2].GetVal(), 
				   	  	                                a_vArg[1].GetVal(),
													                      a_vArg[0].GetVal()) );  break;
      default: Error(ecINTERNAL_ERROR, 6);
    }

    // Find out if the result will depend on a variable
    /** \todo remove this loop, put content in the loop that takes the argument values.
      
        (Attention: SetVal will reset Flags.)
    */
    bool bVolatile = a_FunTok.IsFlagSet(token_type::flVOLATILE);
    for (int i=0; (bVolatile==false) && (i<iArgCount); ++i)
      bVolatile |= a_vArg[i].IsFlagSet(token_type::flVOLATILE);

    if (bVolatile)
      valTok.AddFlags(token_type::flVOLATILE);

  #if defined(_MSC_VER)
    #pragma warning( disable : 4311 ) 
  #endif

    // Formula optimization
    if ( m_bOptimize && 
        !valTok.IsFlagSet(token_type::flVOLATILE) &&
        !a_FunTok.IsFlagSet(token_type::flVOLATILE) ) 
	  {
      m_vByteCode.RemoveValEntries(iArgCount);
      m_vByteCode.AddVal( valTok.GetVal() );
	  }
	  else 
	  { 
      // operation dosnt depends on a variable or the function is flagged unoptimizeable
      // we cant optimize here...
      m_vByteCode.AddFun(pFunc, (a_FunTok.GetArgCount()==-1) ? -iArgCount : iArgCount);
	  }

    return valTok;

  #if defined(_MSC_VER)
    #pragma warning( default : 4311 )
  #endif
  }

  //---------------------------------------------------------------------------
  /** \brief Execute a function that takes a single string argument.
      \param a_FunTok Function token.
      \throw exception_type If the function token is not a string function
  */
  ParserBase::token_type ParserBase::ApplyStrFunc(const token_type &a_FunTok,
                                                  const std::vector<token_type> &a_vArg) const
  {
    if (a_vArg.back().GetCode()!=cmSTRING)
      Error(ecSTRING_EXPECTED, m_pTokenReader->GetPos(), a_FunTok.GetAsString());

    token_type  valTok;
    int  iArgCount = (unsigned)a_vArg.size();
    void  *pFunc = a_FunTok.GetFuncAddr();
    assert(pFunc);

    try
    {
      // Collect the function arguments from the value stack
      switch(a_FunTok.GetArgCount())
      {
      case 0: valTok.SetVal( ((strfun_type1)pFunc)(a_vArg[0].GetAsString().c_str()) );  break;
      case 1: valTok.SetVal( ((strfun_type2)pFunc)(a_vArg[1].GetAsString().c_str(),
		                                              a_vArg[0].GetVal()) );  break;
      case 2: valTok.SetVal( ((strfun_type3)pFunc)(a_vArg[2].GetAsString().c_str(), 
		                                              a_vArg[1].GetVal(), 
														                      a_vArg[0].GetVal()) );  break;
      default: Error(ecINTERNAL_ERROR);
      }
    }
    catch(ParserError& /*e*/)
    {
      Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), a_FunTok.GetAsString());
    }

    // Find out if the result will depend on a variable
    /** \todo remove this loop, put content in the loop that takes the argument values.
      
        (Attention: SetVal will reset Flags.)
    */
    bool bVolatile = a_FunTok.IsFlagSet(token_type::flVOLATILE);
    for (int i=0; (bVolatile==false) && (i<iArgCount); ++i)
      bVolatile |= a_vArg[i].IsFlagSet(token_type::flVOLATILE);

    if (bVolatile)
      valTok.AddFlags(token_type::flVOLATILE);

    // string functions won't be optimized
    m_vByteCode.AddStrFun((void*)pFunc, a_FunTok.GetArgCount(), a_vArg.back().GetIdx());
    
    return valTok;
  }

  //---------------------------------------------------------------------------
  /** \brief Apply a function token. 
      \param iArgCount Number of Arguments actually gathered used only for multiarg functions.
      \post The result is pushed to the value stack
      \post The function token is removed from the stack
      \throw exception_type if Argument count does not mach function requirements.
  */
  void ParserBase::ApplyFunc( ParserStack<token_type> &a_stOpt,
                              ParserStack<token_type> &a_stVal, 
                              int a_iArgCount) const
  { 
    assert(m_pTokenReader.get());

    // Operator stack empty or does not contain tokens with callback functions
    if (a_stOpt.empty() || a_stOpt.top().GetFuncAddr()==0 )
      return;

    token_type funTok = a_stOpt.pop();
    assert(funTok.GetFuncAddr());

    // Binary operators must rely on their internal operator number
    // since counting of operators relies on commas for function arguments
    // binary operators do not have commas in their expression
    int iArgCount = (funTok.GetCode()==cmOPRT_BIN) ? funTok.GetArgCount() : a_iArgCount;

    // determine how many parameters the function needs. To remember iArgCount includes the 
    // string parameter whilst GetArgCount() counts only numeric parameters.
    int iArgRequired = funTok.GetArgCount() + ((funTok.GetType()==tpSTR) ? 1 : 0);

    // Thats the number of numerical parameters
    int iArgNumerical = iArgCount - ((funTok.GetType()==tpSTR) ? 1 : 0);

    if (funTok.GetCode()==cmFUNC_STR && iArgCount-iArgNumerical>1)
        Error(ecINTERNAL_ERROR);

    if (funTok.GetArgCount()>=0 && iArgCount>iArgRequired) 
	      Error(ecTOO_MANY_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

	  if (funTok.GetCode()!=cmOPRT_BIN && iArgCount<iArgRequired )
	      Error(ecTOO_FEW_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

    if (funTok.GetCode()==cmFUNC_STR && iArgCount>iArgRequired )
	      Error(ecTOO_MANY_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

    // Collect the numeric function arguments from the value stack and store them
    // in a vector
    std::vector<token_type> stArg;  
    for (int i=0; i<iArgNumerical; ++i)
    {
      stArg.push_back( a_stVal.pop() );
      if ( stArg.back().GetType()==tpSTR && funTok.GetType()!=tpSTR )
        Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), funTok.GetAsString());
    }

    // for string functions add the string argument
    if (funTok.GetCode()==cmFUNC_STR)
    {
      stArg.push_back( a_stVal.pop() );
      if ( stArg.back().GetType()==tpSTR && funTok.GetType()!=tpSTR )
        Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), funTok.GetAsString());
    }

    // String functions accept only one parameter
    if (funTok.GetType()==tpSTR)
    {
      token_type token( ApplyStrFunc(funTok, stArg)  );
      a_stVal.push( token );
    }
    else
    {
      token_type token( ApplyNumFunc(funTok, stArg) );
      a_stVal.push( token );
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Apply a binary operator. 
      \param a_stOpt The operator stack
      \param a_stVal The value stack
  */
  void ParserBase::ApplyBinOprt( ParserStack<token_type> &a_stOpt,
                                 ParserStack<token_type> &a_stVal) const
  {
    assert(a_stOpt.size());

    // user defined binary operator
    if (a_stOpt.top().GetCode()==cmOPRT_BIN)
    {
      ApplyFunc(a_stOpt, a_stVal, 2);
    }
    else
    {
      // internal binary operator
      MUP_ASSERT(a_stVal.size()>=2);

      token_type valTok1 = a_stVal.pop(),
                valTok2 = a_stVal.pop(),
                optTok = a_stOpt.pop(),
                resTok; 

      if ( valTok1.GetType()!=valTok2.GetType() || 
          (valTok1.GetType()==tpSTR && valTok2.GetType()==tpSTR) )
        Error(ecOPRT_TYPE_CONFLICT, m_pTokenReader->GetPos(), optTok.GetAsString());

      value_type x = valTok2.GetVal(),
	               y = valTok1.GetVal();

      switch (optTok.GetCode())
      {
        // built in binary operators
        case cmAND: resTok.SetVal( (int)x & (int)y ); break;
        case cmOR:  resTok.SetVal( (int)x | (int)y ); break;
        case cmXOR: resTok.SetVal( (int)x ^ (int)y ); break;
        case cmLT:  resTok.SetVal( x < y ); break;
        case cmGT:  resTok.SetVal( x > y ); break;
        case cmLE:  resTok.SetVal( x <= y ); break;
        case cmGE:  resTok.SetVal( x >= y ); break;
        case cmNEQ: resTok.SetVal( x != y ); break;
        case cmEQ:  resTok.SetVal( x == y ); break;
        case cmADD: resTok.SetVal( x + y ); break;
        case cmSUB: resTok.SetVal( x - y ); break;
        case cmMUL: resTok.SetVal( x * y ); break;
        case cmDIV: resTok.SetVal( x / y ); break;
  	    case cmPOW: resTok.SetVal(pow(x, y)); break;

        case cmASSIGN: 
                  // The assignement operator needs special treatment
                  // it uses a different format when stored in the bytecode!
                  {
                    if (valTok2.GetCode()!=cmVAR)
                      Error(ecUNEXPECTED_OPERATOR, -1 /* m_pTokenReader->GetPos() */, _T("="));
                      
                    value_type *pVar = valTok2.GetVar();
                    resTok.SetVal( *pVar = y );
                    a_stVal.push( resTok );

                    m_vByteCode.AddAssignOp(pVar);
                    return;  // we must return since the following 
                            // stuff does not apply
                  }

        default:  Error(ecINTERNAL_ERROR, 8);
      }

      // Create the bytecode entries
      if (!m_bOptimize)
      {
        // Optimization flag is not set
        m_vByteCode.AddOp(optTok.GetCode());
      }
      else if ( valTok1.IsFlagSet(token_type::flVOLATILE) || 
                valTok2.IsFlagSet(token_type::flVOLATILE) )
      {
        // Optimization flag is not set, but one of the value
        // depends on a variable
        m_vByteCode.AddOp(optTok.GetCode());
        resTok.AddFlags(token_type::flVOLATILE);
      }
      else
      {
        // operator call can be optimized; If optimization is possible 
        // the two previous tokens must be value tokens / they will be removed
        // and replaced with the result of the pending operation.
        m_vByteCode.RemoveValEntries(2);
        m_vByteCode.AddVal(resTok.GetVal());
      }

      a_stVal.push( resTok );
    }
  }

  //---------------------------------------------------------------------------
  /** \brief Parse the command code.
      \sa ParseString(), ParseValue()

      Command code contains precalculated stack positions of the values and the
      associated operators. The Stack is filled beginning from index one the 
      value at index zero is not used at all.
  */
  value_type ParserBase::ParseCmdCode() const
  {
  #if defined(_MSC_VER)
    #pragma warning( disable : 4312 )
  #endif

    value_type *Stack = &m_vStackBuffer[0];
//    value_type Stack[99];
    ECmdCode iCode;
    bytecode_type idx(0);
    int i(0);

    __start:

    idx = m_pCmdCode[i];
    iCode = (ECmdCode)m_pCmdCode[i+1];
    i += 2;

#ifdef _DEBUG
    // Die Formelendkennung ist Wert 26 dreimal hintereinander geschrieben
    // Ich muss für den Test das Formelende filtern.
    if (idx>=(int)m_vStackBuffer.size() && iCode!=cmEND)
      throw exception_type(ecGENERIC, _T(""), m_pTokenReader->GetFormula(), -1);
#endif

    switch (iCode)
    {
      // built in binary operators
      case cmAND: Stack[idx]  = (int)Stack[idx] & (int)Stack[idx+1]; goto __start;
      case cmOR:  Stack[idx]  = (int)Stack[idx] | (int)Stack[idx+1]; goto __start;
      case cmXOR: Stack[idx]  = (int)Stack[idx] ^ (int)Stack[idx+1]; goto __start;
      case cmLE:  Stack[idx]  = Stack[idx] <= Stack[idx+1]; goto __start;
      case cmGE:  Stack[idx]  = Stack[idx] >= Stack[idx+1]; goto __start;
      case cmNEQ: Stack[idx]  = Stack[idx] != Stack[idx+1]; goto __start;
      case cmEQ:  Stack[idx]  = Stack[idx] == Stack[idx+1]; goto __start;
	    case cmLT:  Stack[idx]  = Stack[idx] < Stack[idx+1];  goto __start;
	    case cmGT:  Stack[idx]  = Stack[idx] > Stack[idx+1];  goto __start;
      case cmADD: Stack[idx] += Stack[1+idx]; goto __start;
 	    case cmSUB: Stack[idx] -= Stack[1+idx]; goto __start;
	    case cmMUL: Stack[idx] *= Stack[1+idx]; goto __start;
	    case cmDIV: Stack[idx] /= Stack[1+idx]; goto __start;
      case cmPOW: Stack[idx]  = pow(Stack[idx], Stack[1+idx]); goto __start;

      // Assignement needs special treatment
      case cmASSIGN:
            {
              // next is a pointer to the target
              value_type **pDest = (value_type**)(&m_pCmdCode[i]);

              // advance index according to pointer size
              i += m_vByteCode.GetPtrSize();
              // assign the value
              Stack[idx] = **pDest = Stack[idx+1];
            }
            goto __start;

      // user defined binary operators
      case cmOPRT_BIN:
            Stack[idx] = (**(fun_type2**)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1]);
            ++i;
            goto __start;

      // variable tokens
	    case cmVAR:
		          Stack[idx] = **(value_type**)(&m_pCmdCode[i]);
		          i += m_vByteCode.GetValSize();
		          goto __start;

      // value tokens
	    case cmVAL:
              Stack[idx] = *(value_type*)(&m_pCmdCode[i]);
 	            i += m_vByteCode.GetValSize();
              goto __start;

      // Next is treatment of string functions
      case cmFUNC_STR:
              {
		            // The function argument count
                int iArgCount = (int)m_pCmdCode[ i++ ];  

                // The index of the string argument in the string table
                int iIdxStack = (int)m_pCmdCode[ i++ ];  
                MUP_ASSERT( iIdxStack>=0 && iIdxStack<(int)m_vStringBuf.size() );

                switch(iArgCount)  // switch according to argument count
		            {
                  case 0: Stack[idx] = (*(strfun_type1*)(&m_pCmdCode[i]))(m_vStringBuf[iIdxStack].c_str()); break;
			            case 1: Stack[idx] = (*(strfun_type2*)(&m_pCmdCode[i]))(m_vStringBuf[iIdxStack].c_str(), Stack[idx]); break;
			            case 2: Stack[idx] = (*(strfun_type3*)(&m_pCmdCode[i]))(m_vStringBuf[iIdxStack].c_str(), Stack[idx], Stack[idx+1]); break;
                }
		            i += m_vByteCode.GetPtrSize();
              }
              goto __start;

      // Next is treatment of numeric functions
      case cmFUNC:
		          {
		            int iArgCount = (int)m_pCmdCode[i++];

                switch(iArgCount)  // switch according to argument count
		            {
                  case 0: Stack[idx] = (*(fun_type0*)(&m_pCmdCode[i]))(); break;
                  case 1: Stack[idx] = (*(fun_type1*)(&m_pCmdCode[i]))(Stack[idx]); break;
			            case 2: Stack[idx] = (*(fun_type2*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1]); break;
			            case 3: Stack[idx] = (*(fun_type3*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2]); break;
			            case 4: Stack[idx] = (*(fun_type4*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3]); break;
  		            case 5: Stack[idx] = (*(fun_type5*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3], Stack[idx+4]); break;
                  default:
				            if (iArgCount>0) // function with variable arguments store the number as a negative value
                      Error(ecINTERNAL_ERROR, 1);

                    Stack[idx] =(*(multfun_type*)(&m_pCmdCode[i]))(&Stack[idx], -iArgCount);
                    break;
		            }
		            i += m_vByteCode.GetPtrSize();
		          }
		          goto __start;

	    case cmEND:
		          return Stack[1];

	    default:
              Error(ecINTERNAL_ERROR, 2);
              return 0;
    }

  #if defined(_MSC_VER)
    #pragma warning( default : 4312 )
  #endif
  }

  //---------------------------------------------------------------------------
  /** \brief Return result for constant functions.

    Seems pointless, but for parser functions that are made up of only a value, which occur
    in real world applications, this speeds up things by removing the parser overhead almost
    completely.
  */
  value_type ParserBase::ParseValue() const
  {
    return *(value_type*)(&m_pCmdCode[2]);
  }

  //---------------------------------------------------------------------------
  /** \brief One of the two main parse functions.

  Parse expression from input string. Perform syntax checking and create bytecode.
  After parsing the string and creating the bytecode the function pointer 
  #m_pParseFormula will be changed to the second parse routine the uses bytecode instead of string parsing.

  \sa ParseCmdCode(), ParseValue()
  */
  value_type ParserBase::ParseString() const
  {
  #if defined(_MSC_VER)
    #pragma warning( disable : 4311 )
  #endif
    if (!m_pTokenReader->GetFormula().length())
        Error(ecUNEXPECTED_EOF, 0);

    ParserStack<token_type> stOpt, stVal;
    ParserStack<int> stArgCount;
    token_type opta, opt;  // for storing operators
    token_type val, tval;  // for storing value
    string_type strBuf;    // buffer for string function arguments

    ReInit();

    for(;;)
    {
      opt = m_pTokenReader->ReadNextToken();

      switch (opt.GetCode())
      {
        //
        // Next three are different kind of value entries
        //
        case cmSTRING:
                opt.SetIdx((int)m_vStringBuf.size());      // Assign buffer index to token 
                stVal.push(opt);
		            m_vStringBuf.push_back(opt.GetAsString()); // Store string in internal buffer
                break;
   
        case cmVAR:
                stVal.push(opt);
                m_vByteCode.AddVar( static_cast<value_type*>(opt.GetVar()) );
                break;

        case cmVAL:
		            stVal.push(opt);
                m_vByteCode.AddVal( opt.GetVal() );
                break;

        case cmARG_SEP:
                if (stArgCount.empty())
                  Error(ecUNEXPECTED_ARG_SEP, m_pTokenReader->GetPos());

                ++stArgCount.top();
                // fall through...

        case cmEND:
        case cmBC:
                {
                  // The argument count for parameterless functions is zero
                  // by default an opening bracket sets parameter count to 1
                  // in preparation of arguments to come. If the last token
                  // was an opening bracket we know better...
                  if (opta.GetCode()==cmBO)
                    --stArgCount.top();

                  while ( stOpt.size() && stOpt.top().GetCode() != cmBO)
                  {
                    if (stOpt.top().GetCode()==cmOPRT_INFIX)
                      ApplyFunc(stOpt, stVal, 1);  // infix operator
                    else
                      ApplyBinOprt(stOpt, stVal);
                  }

                  // <ibg> 20060218 infix operator treatment moved here
                  if (stOpt.size() && stOpt.top().GetCode()==cmOPRT_INFIX) 
                    ApplyFunc(stOpt, stVal, 1);  // infix operator

                  if ( opt.GetCode()!=cmBC || stOpt.size()==0 || stOpt.top().GetCode()!=cmBO )
                    break;

                  // if opt is ")" and opta is "(" the bracket has been evaluated, now its time to check
			            // if there is either a function or a sign pending
		   	          // neither the opening nor the closing bracket will be pushed back to
			            // the operator stack
			            // Check if a function is standing in front of the opening bracket, 
                  // if yes evaluate it afterwards check for infix operators
			            assert(stArgCount.size());
			            int iArgCount = stArgCount.pop();
                  
                  stOpt.pop(); // Take opening bracket from stack

                  if (iArgCount>1 && ( stOpt.size()==0 || 
                                      (stOpt.top().GetCode()!=cmFUNC && 
                                      stOpt.top().GetCode()!=cmFUNC_STR) ) )
                    Error(ecUNEXPECTED_ARG, m_pTokenReader->GetPos());
                  
                  if (stOpt.size() && stOpt.top().GetCode()!=cmOPRT_INFIX && stOpt.top().GetFuncAddr()!=0)
                    ApplyFunc(stOpt, stVal, iArgCount);
			          } // if bracket content is evaluated
                break;

        //
        // Next are the binary operator entries
        //
        case cmAND:   // built in binary operators
        case cmOR:
        case cmXOR:
        case cmLT:
        case cmGT:
        case cmLE:
        case cmGE:
        case cmNEQ:
        case cmEQ:
        case cmADD:
        case cmSUB:
        case cmMUL:
        case cmDIV:
        case cmPOW:
        case cmASSIGN:
        case cmOPRT_BIN:
                // A binary operator (user defined or built in) has been found. 
                while ( stOpt.size() && stOpt.top().GetCode() != cmBO)
                {
                  if (GetOprtPri(stOpt.top()) < GetOprtPri(opt))
                    break;

                  if (stOpt.top().GetCode()==cmOPRT_INFIX)
                    ApplyFunc(stOpt, stVal, 1);  // infix operator
                  else
                    ApplyBinOprt(stOpt, stVal);
                } // while ( ... )

    			      // The operator can't be evaluated right now, push back to the operator stack
                stOpt.push(opt);
                break;

        //
        // Last section contains functions and operators implicitely mapped to functions
        //
	      case cmBO:
                stArgCount.push(1);
                stOpt.push(opt);
		    	      break;

        case cmFUNC:
        case cmOPRT_INFIX:
        case cmFUNC_STR:  
                stOpt.push(opt);
                break;

        case cmOPRT_POSTFIX:
                stOpt.push(opt);
                ApplyFunc(stOpt, stVal, 1);  // this is the postfix operator
                break;

	      default:  Error(ecINTERNAL_ERROR, 3);
      } // end of switch operator-token

      opta = opt;

      if ( opt.GetCode() == cmEND )
      {
        m_vByteCode.Finalize();
        break;
      }

      //if (stArgCount.size())
      //  cout << "Arguments: " << stArgCount.top() << "\n";

  #if defined(MUP_DUMP_STACK)

      StackDump(stVal, stOpt);
      m_vByteCode.AsciiDump();
  #endif
    } // while (true)

    // Store pointer to start of bytecode
    m_pCmdCode = m_vByteCode.GetRawData();

  #if defined(MUP_DUMP_CMDCODE)
    m_vByteCode.AsciiDump();
  #endif

    // get the last value (= final result) from the stack
    if (stVal.size()!=1)
      Error(ecEMPTY_EXPRESSION);

    if (stVal.top().GetType()!=tpDBL)
      Error(ecSTR_RESULT);

    // no error, so change the function pointer for the main parse routine
    value_type fVal = stVal.top().GetVal();   // Result from String parsing

    if (m_bUseByteCode)
    {
      // (n is a double requires if it is stored in the bytecode)
      // pos 0:      stack index must be 0 for the final result
      // pos 1:      code signaling the next entry is a value (cmVal==20)
      // pos 2..2+n: the final result
      // pos 2+n+1:  the bytecode end marker
      // 
      // Check if the bytecode contains only a single constant, if so parsing is no longer necessary.
      std::size_t checkEnd = 2 + m_vByteCode.GetValSize();  // 
      m_pParseFormula = (m_pCmdCode[1]==cmVAL && checkEnd<m_vByteCode.GetBufSize() && m_pCmdCode[checkEnd]==cmEND) ? 
                              &ParserBase::ParseValue :
                              &ParserBase::ParseCmdCode;
      m_vStackBuffer.resize(m_vByteCode.GetMaxStackSize());
    }

    return fVal;

  #if defined(_MSC_VER)
    #pragma warning( default : 4311 )
  #endif
  }

  //---------------------------------------------------------------------------
  /** \brief Create an error containing the parse error position.

    This function will create an Parser Exception object containing the error text and
    its position.

    \param a_iErrc [in] The error code of type #EErrorCodes.
    \param a_iPos [in] The position where the error was detected.
    \param a_strTok [in] The token string representation associated with the error.
    \throw ParserException always throws thats the only purpose of this function.
  */
  void  ParserBase::Error(EErrorCodes a_iErrc, int a_iPos, const string_type &a_sTok) const
  {
    throw exception_type(a_iErrc, a_sTok, m_pTokenReader->GetFormula(), a_iPos);
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined variables.
      \throw nothrow

      Resets the parser to string parsing mode by calling #ReInit.
  */
  void ParserBase::ClearVar()
  {
    m_VarDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Remove a variable from internal storage.
      \throw nothrow

      Removes a variable if it exists. If the Variable does not exist nothing will be done.
  */
  void ParserBase::RemoveVar(const string_type &a_strVarName)
  {
    varmap_type::iterator item = m_VarDef.find(a_strVarName);
    if (item!=m_VarDef.end())
    {
      m_VarDef.erase(item);
      ReInit();
    }
  }

  //------------------------------------------------------------------------------
  /** \brief Clear the formula. 
      \post Resets the parser to string parsing mode.
      \throw nothrow

      Clear the formula and existing bytecode.
  */
  void ParserBase::ClearFormula()
  {
    m_vByteCode.clear();
    m_pCmdCode = 0;
    m_pTokenReader->SetFormula(_T(""));
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all functions.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearFun()
  {
    m_FunDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined constants.

      Both numeric and string constants will be removed from the internal storage.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearConst()
  {
    m_ConstDef.clear();
    m_StrVarDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined postfix operators.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearPostfixOprt()
  {
    m_PostOprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear all user defined binary operators.
      \post Resets the parser to string parsing mode.
      \throw nothrow
  */
  void ParserBase::ClearOprt()
  {
    m_OprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Clear the user defined Prefix operators. 
      \post Resets the parser to string parser mode.
      \throw nothrow
  */
  void ParserBase::ClearInfixOprt()
  {
    m_InfixOprtDef.clear();
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Enable or disable the formula optimization feature. 
      \post Resets the parser to string parser mode.
      \throw nothrow
  */
  void ParserBase::EnableOptimizer(bool a_bIsOn)
  {
    m_bOptimize = a_bIsOn;
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Enable or disable parsing from Bytecode. 

      \attention There is no reason to disable bytecode. It will 
                drastically decrease parsing speed.
  */
  void ParserBase::EnableByteCode(bool a_bIsOn)
  {
    m_bUseByteCode = a_bIsOn;
    if (!a_bIsOn)
      ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Enable or disable the built in binary operators.
      \throw nothrow
      \sa m_bBuiltInOp, ReInit()

    If you disable the built in binary operators there will be no binary operators
    defined. Thus you must add them manually one by one. It is not possible to
    disable built in operators selectively. This function will Reinitialize the
    parser by calling ReInit().
  */
  void ParserBase::EnableBuiltInOprt(bool a_bIsOn)
  {
    m_bBuiltInOp = a_bIsOn;
    ReInit();
  }

  //------------------------------------------------------------------------------
  /** \brief Query status of built in variables.
      \return #m_bBuiltInOp; true if built in operators are enabled.
      \throw nothrow
  */
  bool ParserBase::HasBuiltInOprt() const
  {
    return m_bBuiltInOp;
  }

  //------------------------------------------------------------------------------
  /** \brief Get the argument separator character. 
  */
  char_type ParserBase::GetArgSep() const
  {
    return m_pTokenReader->GetArgSep();
  }

  //------------------------------------------------------------------------------
  /** \brief Set argument separator. 
      \param cArgSep the argument separator character.
  */
  void ParserBase::SetArgSep(char_type cArgSep)
  {
    m_pTokenReader->SetArgSep(cArgSep);
  }

#if defined(MUP_DUMP_STACK) | defined(MUP_DUMP_CMDCODE)
  //------------------------------------------------------------------------------
  /** \brief Dump stack content. 

      This function is used for debugging only.
  */
  void ParserBase::StackDump( const ParserStack<token_type> &a_stVal, 
				             			    const ParserStack<token_type> &a_stOprt ) const
  {
    ParserStack<token_type> stOprt(a_stOprt), 
                            stVal(a_stVal);

    mu::console() << _T("\nValue stack:\n");
    while ( !stVal.empty() ) 
    {
      token_type val = stVal.pop();
      if (val.GetType()==tpSTR)
        mu::console() << _T(" \"") << val.GetAsString() << _T("\" ");
      else
        mu::console() << _T(" ") << val.GetVal() << _T(" ");
    }
    mu::console() << "\nOperator stack:\n";

    while ( !stOprt.empty() )
    {
      if (stOprt.top().GetCode()<=cmASSIGN) 
  	  {
	  	  mu::console() << _T("OPRT_INTRNL \"")
                      << ParserBase::c_DefaultOprt[stOprt.top().GetCode()] 
                      << _T("\" \n");
	    }
      else
      {
		      switch(stOprt.top().GetCode())
		      {
          case cmVAR:   mu::console() << _T("VAR\n");  break;
		      case cmVAL:   mu::console() << _T("VAL\n");  break;
		      case cmFUNC:  mu::console() << _T("FUNC_NUM \"") 
                                      << stOprt.top().GetAsString() 
                                      << _T("\"\n");   break;
		      case cmOPRT_INFIX: mu::console() << _T("OPRT_INFIX \"")
                                          << stOprt.top().GetAsString() 
                                          << _T("\"\n");   break;
          case cmOPRT_BIN:   mu::console() << _T("OPRT_BIN \"") 
                                          << stOprt.top().GetAsString() 
                                          << _T("\"\n");        break;
          case cmFUNC_STR: mu::console() << _T("FUNC_STR\n");  break;
		      case cmEND:      mu::console() << _T("END\n");       break;
		      case cmUNKNOWN:  mu::console() << _T("UNKNOWN\n");   break;
		      case cmBO:       mu::console() << _T("BRACKET \"(\"\n");  break;
		      case cmBC:       mu::console() << _T("BRACKET \")\"\n");  break;
          default:         mu::console() << stOprt.top().GetType() << _T(" ");  break;
		      }
      }	
      stOprt.pop();
    }

    mu::console() << dec << endl;
  }

#endif // defined(MUP_DUMP_STACK) | defined(MUP_DUMP_CMDCODE)
} // namespace mu
