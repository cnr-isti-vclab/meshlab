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

#include "muParserComplex.h"
#include "muParserFixes.h"

#include <cmath>
#include <complex>
#include <algorithm>
#include <numeric>

using namespace std;

/** \file 
    \brief This file contains the implementation of a parser using complex numbers.
*/


/** \brief Namespace for mathematical applications. */
namespace mu
{
  //---------------------------------------------------------------------------
  value_type ParserComplex::Sin(value_type v)
  {
    complex_type c = UnpackToComplex(v);
    return PackToDouble(sin(c));
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Cos(value_type v)
  {
    complex_type c = UnpackToComplex(v);
    return PackToDouble(cos(c));
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Tan(value_type v)
  {
    complex_type c = UnpackToComplex(v);
    return PackToDouble(tan(c));
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Sqrt(value_type v)
  {
    complex_type c = UnpackToComplex(v);
    return PackToDouble(sqrt(c));
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Add(value_type v1, value_type v2) 
  { 
    complex_type c1 = UnpackToComplex(v1), 
                 c2 = UnpackToComplex(v2);
    return PackToDouble(c1+c2); 
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Sub(value_type v1, value_type v2) 
  { 
    complex_type c1 = UnpackToComplex(v1), 
                 c2 = UnpackToComplex(v2);
    return PackToDouble(c1-c2); 
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Mul(value_type v1, value_type v2) 
  { 
    complex_type c1 = UnpackToComplex(v1), 
                 c2 = UnpackToComplex(v2);
    return PackToDouble(c1*c2); 
  }

  //---------------------------------------------------------------------------
  value_type ParserComplex::Div(value_type v1, value_type v2) 
  { 
    complex_type c1 = UnpackToComplex(v1), 
                 c2 = UnpackToComplex(v2);
    return PackToDouble(c1/c2); 
  }

  //---------------------------------------------------------------------------
  // Unary operator Callbacks: Infix operators
  value_type ParserComplex::UnaryMinus(value_type v) 
  { 
    return -v; 
  }

  //---------------------------------------------------------------------------
  // Default value recognition callback
  int ParserComplex::IsVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal)
  {
    string_type buf(a_szExpr);

    float real, imag;
    int stat, len;

#if defined (_UNICODE)
    stat = swscanf(buf.c_str(), _T("{%f,%f}%n"), &real, &imag, &len);
#else
    stat = sscanf(buf.c_str(), "{%f,%f}%n", &real, &imag, &len);
#endif

    if (stat!=2)
      return 0;

    *a_iPos += len;
    *a_fVal = PackToDouble(complex_type(real, imag));
    return 1;
  }

  //---------------------------------------------------------------------------
  /** \brief Constructor. 

    Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
  */
  ParserComplex::ParserComplex()
    :ParserBase()
  {
    AddValIdent(IsVal);

    InitCharSets();
    InitFun();
    InitOprt();
  }

  //---------------------------------------------------------------------------
  void ParserComplex::InitCharSets()
  {
    DefineNameChars( _T("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") );
    DefineOprtChars( _T("+-*^/?<>=!%&|~'_") );
    DefineInfixOprtChars( _T("/+-*^?<>=!%&|~'_") );
  }

  //---------------------------------------------------------------------------
  /** \brief Initialize operators. */
  void ParserComplex::InitOprt()
  {
    // disable all built in operators, not all of them usefull for integer numbers
    // (they don't do rounding of values)
    EnableBuiltInOprt(false);

    // Disable all built in operators, they wont work with integer numbers
    // since they are designed for floating point numbers
    DefineInfixOprt( _T("-"), UnaryMinus);

    DefineOprt( _T("+"), Add, prADD_SUB);
    DefineOprt( _T("-"), Sub, prADD_SUB);
    DefineOprt( _T("*"), Mul, prMUL_DIV);
    DefineOprt( _T("/"), Div, prMUL_DIV);
  }

  //---------------------------------------------------------------------------
  void ParserComplex::InitFun()
  {
    DefineFun( _T("sin"), Sin);
    DefineFun( _T("cos"), Cos);
    DefineFun( _T("tan"), Tan);
    DefineFun( _T("sqrt"), Sqrt);
  }

  //---------------------------------------------------------------------------
  void ParserComplex::InitConst()
  {}

  //---------------------------------------------------------------------------
  ParserComplex::complex_type ParserComplex::UnpackToComplex(double val)
  {
    float imag(0), real(0);

    real = *((float*)(&val));
    imag = *((float*)(&val)+1);

    return complex_type(real, imag);
  }

  //---------------------------------------------------------------------------
  double ParserComplex::PackToDouble(const complex_type &val)
  {
    double packed = 0;
    float *real = (float*)(&packed),
          *imag = (float*)(&packed)+1;

    *real = val.real();
    *imag = val.imag();

    return packed;
  }

  //---------------------------------------------------------------------------
  ParserComplex::complex_type ParserComplex::Eval()
  {
    return UnpackToComplex(ParserBase::Eval());
  }

} // namespace mu
