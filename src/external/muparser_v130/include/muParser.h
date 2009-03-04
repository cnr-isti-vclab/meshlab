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
#ifndef MU_PARSER_H
#define MU_PARSER_H

//--- Standard includes ------------------------------------------------------------------------
#include <vector>
#include <locale>

//--- Parser includes --------------------------------------------------------------------------
#include "muParserBase.h"


/** \file
    \brief Definition of the standard floating point parser.
*/

namespace mu
{
  /** \brief Mathematical expressions parser.
    
    Standard implementation of the mathematical expressions parser. 
    Can be used as a reference implementation for subclassing the parser.

    <small>
    (C) 2004-2008 Ingo Berg<br>
    ingo_berg(at)gmx.de
    </small>
  */
  class Parser : public ParserBase
  {
  public:

    Parser();

    virtual void InitCharSets();
    virtual void InitFun();
    virtual void InitConst();
    virtual void InitOprt();

    void SetDecSep(char_type cDecSep);
    void SetThousandsSep(char_type cThousandsSep);
    
    value_type Diff(value_type *a_Var, 
                    value_type a_fPos, 
                    value_type a_fEpsilon = 0.00074) const;

  private:

    /** \brief A facet class used to change decimal and thousands separator. */
    template<class TChar>
    class change_dec_sep : public std::numpunct<TChar>
    {
    public:
      
      explicit change_dec_sep(char_type cDecSep, char_type cThousandsSep = 0, int nGroup = 3)
        :std::numpunct<TChar>()
        ,m_cDecPoint(cDecSep)
        ,m_cThousandsSep(cThousandsSep)
        ,m_nGroup(nGroup)
      {}
      
    protected:
      
      virtual char_type do_decimal_point() const
      {
        return m_cDecPoint;
      }

      virtual char_type do_thousands_sep() const
      {
        return m_cThousandsSep;
      }

      virtual std::string do_grouping() const 
      { 
        return std::string(1, m_nGroup); 
      }

    private:

      int m_nGroup;
      char_type m_cDecPoint;  
      char_type m_cThousandsSep;
    };
     
    // Trigonometric functions
    static value_type  Sin(value_type);
    static value_type  Cos(value_type);
    static value_type  Tan(value_type);
    // arcus functions
    static value_type  ASin(value_type);
    static value_type  ACos(value_type);
    static value_type  ATan(value_type);
    // hyperbolic functions
    static value_type  Sinh(value_type);
    static value_type  Cosh(value_type);
    static value_type  Tanh(value_type);
    // arcus hyperbolic functions
    static value_type  ASinh(value_type);
    static value_type  ACosh(value_type);
    static value_type  ATanh(value_type);
    // Logarithm functions
    static value_type  Log2(value_type);  // Logarithm Base 2
    static value_type  Log10(value_type); // Logarithm Base 10
    static value_type  Ln(value_type);    // Logarithm Base e (natural logarithm)
    // misc
    static value_type  Exp(value_type);
    static value_type  Abs(value_type);
    static value_type  Sqrt(value_type);
    static value_type  Rint(value_type);
    static value_type  Sign(value_type);
    static value_type  Ite(value_type, value_type, value_type);

    // Prefix operators
    // !!! Unary Minus is a MUST if you want to use negative signs !!!
    static value_type  UnaryMinus(value_type);

    // Functions with variable number of arguments
    static value_type Sum(const value_type*, int);  // sum
    static value_type Avg(const value_type*, int);  // mean value
    static value_type Min(const value_type*, int);  // minimum
    static value_type Max(const value_type*, int);  // maximum

    static int IsVal(const char_type* a_szExpr, int *a_iPos, value_type *a_fVal);

    static std::locale s_locale;  ///< The locale used by the parser
  };
} // namespace mu

#endif

