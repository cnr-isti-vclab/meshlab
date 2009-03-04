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

#ifndef MU_PARSER_FIXES_H
#define MU_PARSER_FIXES_H

/** \file
    \brief This file contains compatibility fixes for some platforms.
*/

//
// Compatibility fixes
//

//---------------------------------------------------------------------------
//
// Intel Compiler
//
//---------------------------------------------------------------------------

#ifdef __INTEL_COMPILER

// remark #981: operands are evaluated in unspecified order
// disabled -> completely pointless if the functions do not have side effects
//
#pragma warning(disable:981)

// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable:383)

// remark #1572: floating-point equality and inequality comparisons are unreliable
// disabled -> everyone knows it, the parser passes this problem
//             deliberately to the user
#pragma warning(disable:1572)

#endif


//---------------------------------------------------------------------------
//
// MSVC6
//
//---------------------------------------------------------------------------


#if defined(_MSC_VER) && _MSC_VER==1200

/** \brief Macro to replace the MSVC6 auto_ptr with the _my_auto_ptr class.

    Hijack auto_ptr and replace it with a version that actually does
    what an auto_ptr normally does. If you use std::auto_ptr in your other code
    might either explode or work much better. The original crap created
    by Microsoft, called auto_ptr and bundled with MSVC6 is not standard compliant.
*/
#define auto_ptr _my_auto_ptr

// This is another stupidity that needs to be undone in order to de-pollute
// the global namespace!
#undef min
#undef max


namespace std
{
  typedef ::size_t size_t;

  //---------------------------------------------------------------------------
  /** \brief MSVC6 fix: Dummy function to put rand into namespace std.

    This is a hack for MSVC6 only. It's dirty, it's ugly and it works, provided
    inlining is enabled. Necessary because I will not pollute or change my
    code in order to adopt it to MSVC6 interpretation of how C++ should look like!
  */
  inline int rand(void)
  {
    return ::rand();
  }

  //---------------------------------------------------------------------------
  /** \brief MSVC6 fix: Dummy function to put strlen into namespace std.

    This is a hack for MSVC6 only. It's dirty, it's ugly and it works, provided
    inlining is enabled. Necessary because I will not pollute or change my
    code in order to adopt it to MSVC6 interpretation of how C++ should look like!
  */
  inline size_t strlen(const char *szMsg)
  {
    return ::strlen(szMsg);
  }

  //---------------------------------------------------------------------------
  /** \brief MSVC6 fix: Dummy function to put strncmp into namespace std.

    This is a hack for MSVC6 only. It's dirty, it's ugly and it works, provided
    inlining is enabled. Necessary because I will not pollute or change my
    code in order to adopt it to MSVC6 interpretation of how C++ should look like!
  */
  inline int strncmp(const char *a, const char *b, size_t len)
  {
    return ::strncmp(a,b,len);
  }

  //---------------------------------------------------------------------------
  template<typename T>
  T max(T a, T b)
  {
    return (a>b) ? a : b;
  }

  //---------------------------------------------------------------------------
  template<typename T>
  T min(T a, T b)
  {
    return (a<b) ? a : b;
  }

  //---------------------------------------------------------------------------
  /** Standard compliant auto_ptr redefinition for MSVC6.

    The code is taken from VS.NET 2003, slightly modified to reduce
    it's dependencies from other classes.
  */
  template<class _Ty>
  class _my_auto_ptr
  {
  public:
    typedef _Ty element_type;

	  explicit _my_auto_ptr(_Ty *_Ptr = 0)
	    :_Myptr(_Ptr)
	  {}

	  _my_auto_ptr(_my_auto_ptr<_Ty>& _Right)
	    :_Myptr(_Right.release())
	  {}

	  template<class _Other>
    operator _my_auto_ptr<_Other>()
	  {
      return (_my_auto_ptr<_Other>(*this));
	  }

	  template<class _Other>
	  _my_auto_ptr<_Ty>& operator=(_my_auto_ptr<_Other>& _Right)
	  {
      reset(_Right.release());
	    return (*this);
	  }

	 ~auto_ptr()              { delete _Myptr;    }
	  _Ty& operator*() const  { return (*_Myptr); }
	  _Ty *operator->() const	{ return (&**this);	}
	  _Ty *get() const        { return (_Myptr);	}

    _Ty *release()
    {
		  _Ty *_Tmp = _Myptr;
		  _Myptr = 0;
		  return (_Tmp);
		}

	  void reset(_Ty* _Ptr = 0)
		{
		  if (_Ptr != _Myptr)
			  delete _Myptr;
		  _Myptr = _Ptr;
    }

  private:
	    _Ty *_Myptr;
	}; // class _my_auto_ptr
} // namespace std

#endif // Microsoft Visual Studio Version 6.0

#endif // include guard


