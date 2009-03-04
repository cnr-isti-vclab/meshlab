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
#include "muParser.h"

//--- Standard includes ------------------------------------------------------------------------
#include <cmath>
#include <algorithm>
#include <numeric>

/** \brief Pi (what else?). */
#define PARSER_CONST_PI  3.141592653589793238462643

/** \brief The eulerian number. */
#define PARSER_CONST_E   2.718281828459045235360287

using namespace std;

/** \file
    \brief Implementation of the standard floating point parser.
*/


/** \brief Namespace for mathematical applications. */
namespace mu
{
  std::locale Parser::s_locale = std::locale("C");
  
  //---------------------------------------------------------------------------
  // Trigonometric function
  value_type Parser::Sin(value_type v)   { return sin(v);  }
  value_type Parser::Cos(value_type v)   { return cos(v);  }
  value_type Parser::Tan(value_type v)   { return tan(v);  }
  value_type Parser::ASin(value_type v)  { return asin(v); }
  value_type Parser::ACos(value_type v)  { return acos(v); }
  value_type Parser::ATan(value_type v)  { return atan(v); }
  value_type Parser::Sinh(value_type v)  { return sinh(v); }
  value_type Parser::Cosh(value_type v)  { return cosh(v); }
  value_type Parser::Tanh(value_type v)  { return tanh(v); }
  value_type Parser::ASinh(value_type v) { return log(v + sqrt(v * v + 1)); }
  value_type Parser::ACosh(value_type v) { return log(v + sqrt(v * v - 1)); }
  value_type Parser::ATanh(value_type v) { return ((value_type)0.5 * log((1 + v) / (1 - v))); }

  //---------------------------------------------------------------------------
  // Logarithm functions
  value_type Parser::Log2(value_type v)  { return log(v)/log((value_type)2); } // Logarithm base 2
  value_type Parser::Log10(value_type v) { return log10(v); } // Logarithm base 10
  value_type Parser::Ln(value_type v)    { return log(v);   } // Logarithm base e (natural logarithm)

  //---------------------------------------------------------------------------
  //  misc
  value_type Parser::Exp(value_type v)  { return exp(v);   }
  value_type Parser::Abs(value_type v)  { return fabs(v);  }
  value_type Parser::Sqrt(value_type v) { return sqrt(v);  }
  value_type Parser::Rint(value_type v) { return floor(v + (value_type)0.5); }
  value_type Parser::Sign(value_type v) { return (value_type)((v<0) ? -1 : (v>0) ? 1 : 0); }

  //---------------------------------------------------------------------------
  /** \brief Conditional (if then else).
      \param v1 Condition
      \param v2 First value
      \param v3 Second value
      \return v2 if v1!=0 v3 otherwise.
  */
  value_type Parser::Ite(value_type v1, value_type v2, value_type v3) 
  { 
    return (v1) ? v2 : v3; 
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for the unary minus operator.
      \param v The value to negate
      \return -v
  */
  value_type Parser::UnaryMinus(value_type v) 
  { 
    return -v; 
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for adding multiple values. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Sum(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function sum."));

    value_type fRes=0;
    for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
    return fRes;
  }

  //---------------------------------------------------------------------------
  /** \brief Callback for averaging multiple values. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Avg(const value_type *a_afArg, int a_iArgc)
  { 
    if (!a_iArgc)	
      throw exception_type(_T("too few arguments for function sum."));

    value_type fRes=0;
    for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
    return fRes/(double)a_iArgc;
  }


  //---------------------------------------------------------------------------
  /** \brief Callback for determining the minimum value out of a vector. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Min(const value_type *a_afArg, int a_iArgc)
  { 
      if (!a_iArgc)	
          throw exception_type(_T("too few arguments for function min."));

      value_type fRes=a_afArg[0];
      for (int i=0; i<a_iArgc; ++i) fRes = std::min(fRes, a_afArg[i]);

      return fRes;
  }


  //---------------------------------------------------------------------------
  /** \brief Callback for determining the maximum value out of a vector. 
      \param [in] a_afArg Vector with the function arguments
      \param [in] a_iArgc The size of a_afArg
  */
  value_type Parser::Max(const value_type *a_afArg, int a_iArgc)
  { 
      if (!a_iArgc)	
          throw exception_type(_T("too few arguments for function min."));

      value_type fRes=a_afArg[0];
      for (int i=0; i<a_iArgc; ++i) fRes = std::max(fRes, a_afArg[i]);

      return fRes;
  }


  //---------------------------------------------------------------------------
  /** \brief Default value recognition callback. 
      \param [in] a_szExpr Pointer to the expression
      \param [in, out] a_iPos Pointer to an index storing the current position within the expression
      \param [out] a_fVal Pointer where the value should be stored in case one is found.
      \return 1 if a value was found 0 otherwise.
  */
  int Parser::IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_fVal)
  {
    value_type fVal(0);

  // 20080309 commented the fix since it is not local aware
  //
  //// thanks to CodeProject member sailorickm for writing this fix:
  //// http://www.codeproject.com/cpp/FastMathParser.asp?msg=1354598#xx1354598xx
  //// i cant test it myself, if you see problems please contact me.
  ////
  //// - 20080309 ibg; support for locales wont work with this fix
  //#if defined (__hpux) || (defined __GNUC__ && (__GNUC__ == 3 && (__GNUC_MINOR__ < 3 )))
  //  int iEnd = 0;
  //  int nAssigned = sscanf(a_szExpr, "%lf%n", &fVal, &iEnd);
  //  if (nAssigned == 0)
  //  iEnd = -1;
  //#else
    stringstream_type stream(a_szExpr);
    stream.seekg(0);        // todo:  check if this really is necessary
    stream.imbue(Parser::s_locale);
    stream >> fVal;
    int iEnd = stream.tellg(); // Position after reading
  //#endif

    if (iEnd==-1)
      return 0;

    *a_iPos += iEnd;
    *a_fVal = fVal;
    return 1;
  }


  //---------------------------------------------------------------------------
  /** \brief Constructor. 

    Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
  */
  Parser::Parser()
    :ParserBase()
  {
    AddValIdent(IsVal);

    InitCharSets();
    InitFun();
    InitConst();
    InitOprt();
  }

  //---------------------------------------------------------------------------
  /** \brief Define the character sets. 
      \sa DefineNameChars, DefineOprtChars, DefineInfixOprtChars
    
    This function is used for initializing the default character sets that define
    the characters to be useable in function and variable names and operators.
  */
  void Parser::InitCharSets()
  {
    DefineNameChars( _T("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") );
    DefineOprtChars( _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_") );
    DefineInfixOprtChars( _T("/+-*^?<>=#!$%&|~'_") );
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize the default functions. */
  void Parser::InitFun()
  {
    // trigonometric functions
    DefineFun(_T("sin"), Sin);
    DefineFun(_T("cos"), Cos);
    DefineFun(_T("tan"), Tan);
    // arcus functions
    DefineFun(_T("asin"), ASin);
    DefineFun(_T("acos"), ACos);
    DefineFun(_T("atan"), ATan);
    // hyperbolic functions
    DefineFun(_T("sinh"), Sinh);
    DefineFun(_T("cosh"), Cosh);
    DefineFun(_T("tanh"), Tanh);
    // arcus hyperbolic functions
    DefineFun(_T("asinh"), ASinh);
    DefineFun(_T("acosh"), ACosh);
    DefineFun(_T("atanh"), ATanh);
    // Logarithm functions
    DefineFun(_T("log2"), Log2);
    DefineFun(_T("log10"), Log10);
    DefineFun(_T("log"), Log10);
    DefineFun(_T("ln"), Ln);
    // misc
    DefineFun(_T("exp"), Exp);
    DefineFun(_T("sqrt"), Sqrt);
    DefineFun(_T("sign"), Sign);
    DefineFun(_T("rint"), Rint);
    DefineFun(_T("abs"), Abs);
    DefineFun(_T("if"), Ite);
    // Functions with variable number of arguments
    DefineFun(_T("sum"), Sum);
    DefineFun(_T("avg"), Avg);
    DefineFun(_T("min"), Min);
    DefineFun(_T("max"), Max);
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize constants.
  
    By default the parser recognizes two constants. Pi ("pi") and the eulerian
    number ("_e").
  */
  void Parser::InitConst()
  {
    DefineConst(_T("_pi"), (value_type)PARSER_CONST_PI);
    DefineConst(_T("_e"), (value_type)PARSER_CONST_E);
  }

  //---------------------------------------------------------------------------
  /** \brief Set the decimal separator.
      \param cDecSep Decimal separator as a character value.
      \sa SetThousandsSep

      By default muparser uses the "C" locale. The decimal separator of this
      locale is overwritten by the one provided here.
  */
  void Parser::SetDecSep(char_type cDecSep)
  {
    char_type cThousandsSep = std::use_facet< change_dec_sep<char_type> >(s_locale).thousands_sep();
    s_locale = std::locale(std::locale("C"), new change_dec_sep<char_type>(cDecSep, cThousandsSep));
  }
  
  //---------------------------------------------------------------------------
  /** \brief Sets the thousands operator. 
      \param cThousandsSep The thousands separator as a character
      \sa SetDecSep

      By default muparser uses the "C" locale. The thousands separator of this
      locale is overwritten by the one provided here.
  */
  void Parser::SetThousandsSep(char_type cThousandsSep)
  {
    char_type cDecSep = std::use_facet< change_dec_sep<char_type> >(s_locale).decimal_point();
    s_locale = std::locale(std::locale("C"), new change_dec_sep<char_type>(cDecSep, cThousandsSep));
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize operators. 
  
    By default only the unary minus operator is added.
  */
  void Parser::InitOprt()
  {
    DefineInfixOprt(_T("-"), UnaryMinus);
  }


  //---------------------------------------------------------------------------
  /** \brief Numerically differentiate with regard to a variable. 
      \param [in] a_Var Pointer to the differentiation variable.
      \param [in] a_fPos Position at which the differentiation should take place.
      \param [in] a_fEpsilon Epsilon used for the numerical differentiation.

    Numerical differentiation uses a 5 point operator yielding a 4th order 
    formula. The default value for epsilon is 0.00074 which is
    numerical_limits<double>::epsilon() ^ (1/5) as suggested in the muparser
    forum:

    http://sourceforge.net/forum/forum.php?thread_id=1994611&forum_id=462843
  */
  value_type Parser::Diff(value_type *a_Var, 
                          value_type  a_fPos, 
                          value_type  a_fEpsilon) const
  {
    value_type fRes(0), 
               fBuf(*a_Var), 
               f[4] = {0,0,0,0};

    *a_Var = a_fPos+2 * a_fEpsilon;  f[0] = Eval();
    *a_Var = a_fPos+1 * a_fEpsilon;  f[1] = Eval();
    *a_Var = a_fPos-1 * a_fEpsilon;  f[2] = Eval();
    *a_Var = a_fPos-2 * a_fEpsilon;  f[3] = Eval();
    *a_Var = fBuf; // restore variable

    fRes = (-f[0] + 8*f[1] - 8*f[2] + f[3]) / (12*a_fEpsilon);
    return fRes;
  }
} // namespace mu
