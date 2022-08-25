//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

#ifndef IFXSTRING_H
#define IFXSTRING_H

#include <stdarg.h>

#include "IFXDataTypes.h"
#include "IFXResult.h"

// remove MS warning: unreferenced inline function has been removed
#ifdef _MSC_VER
#pragma warning( disable : 4514 )
#endif

enum MIB_enum
{
  MIB_UTF8 = 106
};

/**
  @note The private virtual allocation functions exist to allow a DLL to
      assign a value, even if the object was created in the application (or vica versa).
      Otherwise, newing in the DLL and deleting in the application would cause a fault.
      The virtual functions force the allocation in the same heap that the
      object was created.  The IFXString itself still has to be created and
      destructed in the same heap.
*/
class IFXString
{
public:
  IFXString(void);

  /**
    Constructs a string as a copy of another string.
  */
  IFXString(const IFXString& operand);

  /**
    Constructs a string as a copy of another string.
  */
  IFXString(const IFXString* operand);

  /**
    Constructs a string as a copy of another string performing the
    conversion from UTF-8 to internal string representation.
  */
  IFXString(const char* operand);

  /**
    Constructs a string as a copy of another string performing the
    conversion from UTF-8 to internal string representation.
  */
  IFXString(const U8* operand);

  /**
    Constructs a string as a copy of another string.
  */
  IFXString(const IFXCHAR* operand);

  /**
    Constructs an empty string with predefined size. The buffer allocated
    will be at least the size specified. The new buffer is initialized with
    a leading null so as to be a 0 length string.
  */
  IFXString(U32 size);

  virtual ~IFXString(void);

  operator const IFXCHAR* (void) const;
  operator const U8* (void) const;

  /// Delete string buffer.
  IFXRESULT Clear();

  /**
    Returns the length of the string in characters excluding
    null terminator.

    @return The length of string in characters.
  */
  U32 Length(void) const;

  /**
    Returns the size of buffer needed to convert this string to utf8
    excluding null terminator.

    @warning  Depricated method, which does not return error code. Use
          GetLengthU8() instead.

    @return The number of bytes to represent this string in utf8.
  */
  U32 LengthU8(void) const;

  /**
    Determines the size of buffer needed to convert this string to utf8
    excluding null terminator.

    @param  pLength The number of bytes to represent this string in utf8.
  */
  IFXRESULT GetLengthU8(U32* pLength);

  /// True/False if string is empty.
  BOOL IsEmpty();

  /// This method copies the source characters to the destination buffer.
  IFXRESULT Assign(const IFXString* pSource);

  /// This method copies the source characters to the destination buffer.
  IFXRESULT Assign(const U8* pSource);

  /// This method copies the source characters to the destination buffer.
  IFXRESULT Assign(const IFXCHAR* pSource);

  /// This method copies the source characters to the destination buffer.
  IFXString& operator=(const IFXString& operand);

  /// This method copies the source characters to the destination buffer.
  IFXString& operator=(const U8* operand);

  /** This method copies the source characters to the destination buffer
    performing and conversion necessary. */
  IFXString& operator=(const IFXCHAR* operand);

  /** This method compares the two strings and returns following result:
    0 if this buffer == null and input buffer == null,
    1 if this buffer != null and input buffer == null,
     -1 if this buffer == null and input buffer != null,
    in other cases it return such result as wcscmp:
     <0 if this string less than input string,
    0 if this string identical to input string,
     >0 if this string greater than input string. */
  I32 Compare(const IFXString& operand) const;

  /** This method compares the two strings and returns following result:
    0 if this buffer == null and input buffer == null,
    1 if input string pointer == null,
    1 if this buffer != null and input buffer == null,
     -1 if this buffer == null and input buffer != null,
    in other cases it return such result as wcscmp:
     <0 if this string less than input string,
    0 if this string identical to input string,
     >0 if this string greater than input string. */
  I32 Compare(IFXString* pSource) const;

  /** This method compares the two strings and returns following result:
    0 if this buffer == null and input buffer == null,
    1 if this buffer != null and input buffer == null,
     -1 if this buffer == null and input buffer != null,
    in other cases it return such result as wcscmp:
     <0 if this string less than input string,
    0 if this string identical to input string,
     >0 if this string greater than input string. */
  I32 Compare(const IFXCHAR* pSource) const;    // mutlibyte/char input

  /** This method compares the two strings and returns following result:
    0 if this buffer == null and input buffer == null,
    1 if input string pointer == null,
    1 if this buffer != null and input buffer == null,
     -1 if this buffer == null and input buffer != null,
    in other cases it return such result as _wcsicmp:
     <0 if this string less than input string,
    0 if this string identical to input string,
     >0 if this string greater than input string.
    It does not consider case, so "dog" and "DoG" will be equal. */
  I32 CompareNoCase(const IFXString* pSource)  const;

  /** This method compares the two strings and returns following result:
    0 if this buffer == null and input buffer == null,
    1 if this buffer != null and input buffer == null,
     -1 if this buffer == null and input buffer != null,
    in other cases it return such result as _wcsicmp:
     <0 if this string less than input string,
    0 if this string identical to input string,
     >0 if this string greater than input string.
    It does not consider case, so "dog" and "DoG" will be equal. */
  I32 CompareNoCase(const IFXCHAR* pSource)  const; // mutlibyte/char input

  /** This method compares the two strings and decides if they are equal,
    returning a bool. */
  BOOL  operator==(const IFXString& operand) const;

  /** This method compares the two strings and decides if they are equal,
    returning a bool. */
  BOOL  operator==(const IFXCHAR* operand) const;

  /** This method compares the two strings and decides if they are not equal,
    returning a bool. */
  BOOL  operator!=(const IFXString& operand) const;

  /** This method compares the two strings and decides if they are not equal,
    returning a bool. */
  BOOL  operator!=(const IFXCHAR* operand) const;

  /// This method converts the string to all upper case.
  void  ForceUppercase();

  /// This method converts the string to all lower case.
  void  ForceLowercase();

  /// This method converts the string to all lower case.
  IFXRESULT MakeLower();

  /// This method converts the string to all upper case.
  IFXRESULT MakeUpper();

  /// Returns the raw string buffer.
  const IFXCHAR* Raw() const;

  /**
    This method converts from internal string representation to multi-byte string.

    @param pString  milti-byte string buffer

    @param destSize size of milti-byte string buffer

    @note It is up to the allocator to make sure he/she does a string.GetLengthU8()
        to properly allocate a buffer of the proper length. This means the buffer
        should be of size:

    @code
    sString.GetLengthU8( &length );
    U8* pString = new U8[length*sizeof(U8)];
    @endcode

    @warning  This method will crash if the size of pString is less then destSize.

    @note   Output depends on current locale setting. On Windows multi-byte code page
          will be used to produce output buffer, which is not UTF-8.
          On Linux it can produce UTF-8, if locale is set to use UTF-8.
  */
  IFXRESULT ConvertToRawU8(U8* pString, U32 destSize);

  /**
    This method converts from internal string representation to UTF-8 string.

    @param pString  milti-byte string buffer

    @param destSize size of milti-byte string buffer

    @note It is up to the allocator to make sure he/she does a string.GetLengthU8()
        to properly allocate a buffer of the proper length. This means the buffer
        should be of size:

    @code
    sString.GetLengthU8(&length);
    U8* pString=new U8[(length*sizeof(U8))];
    @endcode
  */
  //IFXRESULT ConvertToUTF8(I8* pUtf8String, U32 stringSize);

  /** This method extracts a substring of the source strings and puts it into
    the buffers pointed to by pDest.  It allocates a new buffer if needed
    returning a bool. */
  IFXRESULT Substring(IFXString* pDest, U32 uStart, U32 uEnd) const;

  /** This method extracts a substring of the source strings and puts it into
    the buffers pointed to by pDest.  It allocates a new buffer if needed
    returning a bool. */
  IFXRESULT Substring(IFXCHAR* pDest, U32 uDestSize, U32 uStart, U32 uEnd) const;

  /** This method finds the first matching string named pKey starting after
    the pStartIndex index.  It returns the next instance in pStartIndex. */
  IFXRESULT FindSubstring(IFXString* pKey, U32* pStartIndex) const;

  /** This method finds the first matching string named pKey starting after
    the pStartIndex index.  It returns the next instance in pStartIndex. */
  IFXRESULT FindSubstring(U8* pKey, U32* pStartIndex) const;

  /** This method finds the first matching string named pKey starting after
    the pStartIndex index.  It returns the next instance in pStartIndex. */
  IFXRESULT FindSubstring(const IFXCHAR* pKey, U32* pStartIndex) const;

  /**
    Returns the number of bytes in this string.
  */
  U32 ByteCount() const;

  /**
    To speed up hashing and other operations that traverse the string
    as though it's binary data, returns the byte at offset i ...

    @note Does not range check, so be sure 0 <= i < ByteCount().
  */
  U8 GetByte(U32 i) const;

  //--------------------------------------------------------------------------
  IFXRESULT SetAt(U32 uIndex, IFXCHAR* pChar);
  IFXRESULT GetAt(U32 uIndex, IFXCHAR* pChar);


  IFXRESULT IsDigit(U32 uIndex);

  IFXRESULT ToString(U32 uValue, I8 iRadix);
  IFXRESULT ToValue(U32* pValue, I8 iRadix);

  /// This method appends the source characters to the destination buffer.
  IFXRESULT Concatenate(const IFXString& pSource);
  /// This method appends the source characters to the destination buffer.
  IFXRESULT Concatenate(const IFXString* pSource);
  /// This method appends the source characters to the destination buffer.
  IFXRESULT Concatenate(const IFXCHAR* pSource);  // mutlibyte/char input
  /// This method appends the source characters to the destination buffer.
  IFXRESULT Concatenate(const U8* pSource); // mutlibyte/char input

  /** This method appends the arg string to this string and returns
    this string. */
  IFXString& operator+=( const IFXString& rString );
  /** This method appends the arg string to this string and returns
    this string. */
  IFXString& operator+=( const U8* rString );
  /** This method appends the arg string to this string and returns
    this string. */
  IFXString& operator+=( const IFXCHAR* rString );

  IFXString operator+( const IFXString& rString ) const;
  IFXString operator+( const U8* rString ) const;
  IFXString operator+( const IFXCHAR* rString ) const;

  IFXString &VSPrintf(const IFXCHAR* fmt,va_list ap);

  U32 Match(const IFXCHAR *wildcard, U32 icase);

private:
  IFXCHAR*  m_Buffer;
  U32     m_BufferLength;
  void  NewBuffer(U32 size);
  void  DeleteBuffer(void);

};

IFXINLINE const IFXCHAR* IFXString::Raw() const
{
  return m_Buffer;
}

IFXINLINE U32 IFXString::ByteCount() const
{
  return ( Length() * (U32)sizeof(IFXCHAR) );
}

IFXINLINE U8 IFXString::GetByte(U32 i) const
{
  return ((U8 *)m_Buffer)[i];
}


IFXINLINE IFXString::operator const IFXCHAR* () const
{
  return Raw();
}

IFXINLINE IFXString& IFXString::operator=(const IFXCHAR* operand)
{
  Assign(operand);
  return *this;
}


IFXINLINE IFXString& IFXString::operator=(const U8* operand)
{
  Assign(operand);
  return *this;
}

IFXINLINE IFXString& IFXString::operator=(const IFXString& operand)
{
  Assign(&operand);
  return *this;
}


IFXINLINE BOOL IFXString::operator==(const IFXString& operand) const
{
  return 0 == Compare(operand.m_Buffer);
}


IFXINLINE BOOL  IFXString::operator==(const IFXCHAR* operand) const
{
  return 0 == Compare((IFXCHAR*)operand);
}


IFXINLINE BOOL IFXString::operator!=(const IFXString& operand) const
{
  return 0 != Compare(operand.m_Buffer);
}


IFXINLINE BOOL  IFXString::operator!=(const IFXCHAR* operand) const
{
  return 0 != Compare((IFXCHAR*)operand);
}


IFXINLINE I32 IFXString::Compare(const IFXString& operand) const
{
  return Compare(operand.m_Buffer);
}


IFXINLINE I32 IFXString::Compare(IFXString* pSource) const
{
  if(pSource==NULL) // return that this string is greater then the input string
    return 1;

  return Compare(pSource->m_Buffer);
}

IFXINLINE I32 IFXString::CompareNoCase(const IFXString* pSource) const
{
  if(pSource==NULL)
    return 1;

  return CompareNoCase(pSource->m_Buffer);
}

IFXINLINE IFXRESULT IFXString::MakeLower()
{
  ForceLowercase();
  return IFX_OK;
}


IFXINLINE IFXRESULT IFXString::MakeUpper()
{
  ForceUppercase();
  return IFX_OK;
}

//---------------------------------------------------------------------------
//  IFXString::Concatenate
//
//  This method appends the source characters to the destination buffer
//---------------------------------------------------------------------------
IFXINLINE IFXRESULT IFXString::Concatenate(const U8* pSource)
{
  if(pSource==NULL)
    return IFX_E_INVALID_POINTER;

  IFXString TempString(pSource);
  Concatenate(&TempString);

  return IFX_OK;
}

IFXINLINE IFXRESULT IFXString::Concatenate(const IFXString* pSource)
{
  if(pSource==NULL)
    return IFX_E_INVALID_POINTER;

  IFXRESULT ir = Concatenate(pSource->m_Buffer);

  return ir;
}

IFXINLINE IFXRESULT IFXString::Concatenate(const IFXString& pSource)
{
  return Concatenate(&pSource);
}

IFXINLINE IFXString& IFXString::operator+=( const IFXString& rString )
{
  Concatenate(&rString);
  return *this;
}


IFXINLINE IFXString& IFXString::operator+=( const U8* rString )
{
  Concatenate(rString);
  return *this;
}


IFXINLINE IFXString& IFXString::operator+=( const IFXCHAR* rString )
{
  Concatenate(rString);
  return *this;
}

IFXINLINE IFXRESULT IFXString::FindSubstring(U8* pKey, U32* pStartIndex) const
{
  if(pKey==NULL)
    return IFX_E_INVALID_POINTER;

  IFXString tempKey(pKey);
  return FindSubstring(tempKey.m_Buffer, pStartIndex);
}

IFXINLINE IFXRESULT IFXString::FindSubstring(IFXString* pKey, U32* pStartIndex) const
{
  if(pKey==NULL)
    return IFX_E_INVALID_POINTER;

  return FindSubstring(pKey->m_Buffer, pStartIndex);
}

#endif
