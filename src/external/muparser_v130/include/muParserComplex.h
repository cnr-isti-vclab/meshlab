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

#ifndef MU_PARSER_COMPLEX_H
#define MU_PARSER_COMPLEX_H

#include "muParserBase.h"
#include <vector>
#include <complex>

/** \file 
    \brief This file contains a definition of a parser using complex numbers.
*/


namespace mu
{
  /** \brief Mathematical expressions parser.
    
    This version of the parser handles only complex numbers. It disables the built in operators 
    thus it is slower than muParser. 
  */
  class ParserComplex : private ParserBase
  {
  public:

    typedef std::complex<float> complex_type;

    ParserComplex();

    using ParserBase::SetExpr;

    complex_type Eval();

  private:

    // !! The unary Minus is a MUST, otherwise you cant use negative signs !!
    static value_type  UnaryMinus(value_type);
    // binary operator callbacks
    static value_type  Add(value_type v1, value_type v2);
    static value_type  Sub(value_type v1, value_type v2);
    static value_type  Mul(value_type v1, value_type v2);
    static value_type  Div(value_type v1, value_type v2);

    static value_type  Sin(value_type);
    static value_type  Cos(value_type);
    static value_type  Tan(value_type);
    static value_type  Sqrt(value_type);

    static int IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_iVal);

    static complex_type UnpackToComplex(double val);
    static double PackToDouble(const complex_type &val);

    virtual void InitOprt();
    virtual void InitCharSets();
    virtual void InitFun();
    virtual void InitConst();
  };
} // namespace mu

#endif