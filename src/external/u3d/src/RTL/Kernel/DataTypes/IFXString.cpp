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

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXString.h"
#include "IFXMemory.h"
#include "IFXOSUtilities.h"
#include "wcmatch.h"

#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>

#if __MINGW32__
#define vswprintf _vsnwprintf
#endif

//***************************************************************************
//	Constants
//***************************************************************************

const U32 MAX_INT_LENGTH = 40;

/// @todo: resolve the problem with new/delete methods. They were called from different libraries!!!

IFXINLINE void IFXString::NewBuffer(U32 size)
{
	DeleteBuffer();
	if(size)
	{
		//m_Buffer = new IFXCHAR[size];
		m_Buffer = (IFXCHAR*)IFXAllocate(size * sizeof(IFXCHAR));
		if( NULL != m_Buffer )
			m_BufferLength = size;
	}
}

IFXINLINE void IFXString::DeleteBuffer(void)
{
	//IFXDELETE_ARRAY( m_Buffer );

	if(m_Buffer)
	{
		IFXDeallocate(m_Buffer);
		m_Buffer = NULL;
	}
	m_BufferLength = 0;
}


IFXString::IFXString(void)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
}


IFXString::IFXString(const IFXString& operand)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	if (operand.m_BufferLength > 0) 
	{
		NewBuffer(operand.m_BufferLength);
		if (m_Buffer)
			wcscpy(m_Buffer, operand.m_Buffer);
	}
}


IFXString::IFXString(const IFXString* operand)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	if(!operand)
	{
		DeleteBuffer();
	}
	else
	{
		if (operand->m_BufferLength > 0) 
		{
			NewBuffer((U32)operand->m_BufferLength);
			if (m_Buffer)
				wcscpy(m_Buffer, operand->m_Buffer);
		}
	}
}


IFXString::IFXString(const U8* operand)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	if(!operand)
	{
		DeleteBuffer();
	}
	else
	{
		U32 size = 0;
		IFXRESULT result = IFXOSGetWideCharStrSize( operand, &size );
		size = size + 1; // null terminator

		if( IFXSUCCESS(result) && (size > 0) )
		{
			NewBuffer(size);
			if (m_Buffer)
				IFXOSConvertUtf8StrToWideChar( operand, m_Buffer, size );
		}
	}
}


IFXString::IFXString(const char* operand)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	if(!operand)
	{
		DeleteBuffer();
	}
	else
	{
		U32 size = 0;
		IFXRESULT result = IFXOSGetWideCharStrSize( (const U8*)operand, &size );
		size = size + 1; // null terminator

		if( IFXSUCCESS(result) && (size > 0) )
		{
			NewBuffer(size);
			if (m_Buffer)
				IFXOSConvertUtf8StrToWideChar( (const U8*)operand, m_Buffer, size );
		}
	}
}


IFXString::IFXString(const IFXCHAR* operand)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	if(!operand)
	{
		DeleteBuffer();
	}
	else
	{
		NewBuffer((U32)wcslen(operand)+1);
		if (m_Buffer)
			wcscpy(m_Buffer, operand);
	}
}

//---------------------------------------------------------------------------
//  IFXString::IFXString
//---------------------------------------------------------------------------
IFXString::IFXString(U32 size)
: 	m_Buffer(NULL),
	m_BufferLength(0)
{
	NewBuffer(size+1);
	if(m_Buffer)
		m_Buffer[0] = 0;
}

//---------------------------------------------------------------------------
//  IFXString::~IFXString
//---------------------------------------------------------------------------
IFXString::~IFXString(void)
{
	DeleteBuffer();
}


//---------------------------------------------------------------------------
//  IFXString::Clear
//---------------------------------------------------------------------------
IFXRESULT IFXString::Clear()
{
	DeleteBuffer();

	return IFX_OK;
}


U32 IFXString::LengthU8(void) const
{
	U32 len = 0;

	if (m_Buffer)
		IFXOSGetUtf8StrSize( m_Buffer, &len );

	return len;
}


U32 IFXString::Length(void) const
{
	U32 len = 0;

	if (m_Buffer)
		len = wcslen( m_Buffer );

	return len;
}


//---------------------------------------------------------------------------
//  IFXString::GetAt
//
//  This method returns the ICHAR at the specified index
//---------------------------------------------------------------------------
IFXRESULT IFXString::GetAt(U32 uIndex, IFXCHAR* pChar)
{
	IFXRESULT result = IFX_OK;

	if(uIndex>m_BufferLength)
		result = IFX_E_INVALID_RANGE;

	if(pChar==NULL)
		result = IFX_E_INVALID_POINTER;

	if(m_Buffer==NULL)
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
		*pChar=m_Buffer[uIndex];

	return result;
}


//---------------------------------------------------------------------------
//  IFXString::GetLength
//
//---------------------------------------------------------------------------
IFXRESULT IFXString::GetLengthU8(U32* pLength)
{
	IFXRESULT result = IFX_OK;

	if(pLength == NULL)
		result = IFX_E_INVALID_POINTER;

	if(m_Buffer == NULL)
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
		result = IFXOSGetUtf8StrSize( m_Buffer, pLength );

	return result;
}

//---------------------------------------------------------------------------
//  IFXString::IsEmpty
//
//  This method returns a bool based on if the buffer is empty or not
//---------------------------------------------------------------------------
BOOL IFXString::IsEmpty()
{
	if(NULL == m_Buffer)
		return IFX_TRUE;

	if(m_Buffer[0]==0)
		return IFX_TRUE;

	return IFX_FALSE;
}

// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Buffer Accessors Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&

//---------------------------------------------------------------------------
//  IFXString::SetAt
//
//  This method sets a character at the specified location
//---------------------------------------------------------------------------
IFXRESULT IFXString::SetAt(U32 uIndex, IFXCHAR* pChar)
{
	IFXRESULT iResult=IFX_OK;

	if(uIndex>m_BufferLength)
		iResult=IFX_E_INVALID_RANGE;
	if(pChar==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_Buffer==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult))
		m_Buffer[uIndex]=*pChar;

	return iResult;
}

//---------------------------------------------------------------------------
//  IFXString::ConvertToRawU8
//---------------------------------------------------------------------------
IFXRESULT IFXString::ConvertToRawU8(U8* pString, U32 destSize)
{
	IFXRESULT result = IFX_OK;

	if( NULL == pString )
		result = IFX_E_INVALID_POINTER;

	if( 0 == destSize )
		result = IFX_E_BAD_PARAM;

	if( IFXSUCCESS( result ) )
	{
		if( NULL != m_Buffer && 0 != Length() )
		{
			result =  IFXOSConvertWideCharStrToUtf8( 
								m_Buffer, pString, destSize );
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}

	return result;
}


// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Assignment Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
//---------------------------------------------------------------------------
//  IFXString::Assign
//
//  This method copies the source characters to the destination buffer
//---------------------------------------------------------------------------
IFXRESULT IFXString::Assign(const IFXString* pSource)
{
	IFXRESULT result = IFX_OK;

	if(!pSource)
	{
		DeleteBuffer();
	}
	else
	{
		if (pSource->m_BufferLength > 0) 
		{
			NewBuffer(pSource->m_BufferLength);
			if (m_Buffer)
				wcscpy(m_Buffer, pSource->m_Buffer);
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}

	return result;
}


//---------------------------------------------------------------------------
//  IFXString::Assign
//
//  This method copies the source UTF-8 characters to the destination wide characters.
//---------------------------------------------------------------------------
IFXRESULT IFXString::Assign(const U8* pSource)
{
	IFXRESULT result = IFX_OK;

	if(!pSource)
	{
		DeleteBuffer();
	}
	else
	{
		U32 size = 0;
		result = IFXOSGetWideCharStrSize( pSource, &size );
		size = size + 1; //null terminator

		if( IFXSUCCESS( result) && (size > 0) )
		{
			NewBuffer(size);
			if (m_Buffer)
				result = IFXOSConvertUtf8StrToWideChar( pSource, m_Buffer, size );
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
		else
			result = IFX_E_UNDEFINED;
	}

	return result;
}

//---------------------------------------------------------------------------
//  IFXString::Assign converter
//
//  This method copies the source characters to the destination making any
//  conversions that are necessary
//---------------------------------------------------------------------------
IFXRESULT IFXString::Assign(const IFXCHAR* pSource)
{
	IFXRESULT result = IFX_OK;

	if(!pSource)
	{
		DeleteBuffer();
	}
	else
	{
		NewBuffer((U32)wcslen(pSource)+1);
		if (m_Buffer)
			wcscpy(m_Buffer, pSource);
		else
			result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}


// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Comparison Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&

I32 IFXString::Compare(const IFXCHAR* pSource) const
{
	if(m_Buffer==NULL && pSource==NULL)
		return 0;
	if(m_Buffer==NULL)
		return -1;
	if(pSource==NULL)
		return 1;

	return wcscmp(m_Buffer, pSource);
}

I32 IFXString::CompareNoCase(const IFXCHAR* pSource)  const
{
	if(m_Buffer==NULL && pSource==NULL)
		return 0;
	else if(m_Buffer==NULL)
		return -1;
	else if(pSource==NULL)
		return 1;

	IFXString str1(this), str2(pSource);

	str1.ForceLowercase();
	str2.ForceLowercase();

	return wcscmp(str1.Raw(), str2.Raw());
}

// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Case support Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&


//---------------------------------------------------------------------------
//  IFXString::ForceUpperCase
//
//  This method converts the string to all upper case
//---------------------------------------------------------------------------
void IFXString::ForceUppercase()
{
	if(m_Buffer)
	{
		U32 i;
		for(i = 0; m_Buffer[i] != L'\0'; ++i)
			m_Buffer[i] = towupper(m_Buffer[i]);
	}
}
//---------------------------------------------------------------------------
//  IFXString::ForceLowerCase
//
//  This method converts the string to all lower case
//---------------------------------------------------------------------------
void IFXString::ForceLowercase()
{
	if(m_Buffer)
	{
		U32 i;
		for(i = 0; m_Buffer[i] != L'\0'; ++i)
			m_Buffer[i] = towlower(m_Buffer[i]);
	}
}

// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Concatenation Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&

//---------------------------------------------------------------------------
//  IFXString::Concatenate
//
//  This method appends the source characters to the destination buffer
//---------------------------------------------------------------------------
IFXRESULT IFXString::Concatenate(const IFXCHAR* pSource)
{
	IFXRESULT result = IFX_OK;

	if(pSource==NULL)
		result = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result))
	{
		if (m_Buffer) 
		{
			IFXString tmpString(this);
			NewBuffer(m_BufferLength+(U32)wcslen(pSource));
			if (m_Buffer)
			{
				wcscpy(m_Buffer, tmpString.m_Buffer);
				wcscat(m_Buffer, pSource);
			}
			else
				result = IFX_E_INVALID_POINTER;
		} 
		else
			result = Assign(pSource);
	}

	return result;
}


//---------------------------------------------------------------------------
//  IFXString::operator +
//
// appends the arg string to this string and returns the newly formaed string
//---------------------------------------------------------------------------
IFXString IFXString::operator+( const IFXString& rString ) const
{
	IFXString Temp(*this);
	Temp.Concatenate(rString.m_Buffer);
	return Temp;
}

//---------------------------------------------------------------------------
//  IFXString::operator +
//
// appends the arg string to this string and returns the newly formaed string
//---------------------------------------------------------------------------
IFXString IFXString::operator+( const U8* rString ) const
{
	IFXString Temp(*this);
	Temp.Concatenate(rString);
	return Temp;
}

//---------------------------------------------------------------------------
//  IFXString::operator +
//
// appends the arg string to this string and returns the newly formaed string
//---------------------------------------------------------------------------
IFXString IFXString::operator+( const IFXCHAR* rString ) const
{
	IFXString Temp(*this);
	Temp.Concatenate((const IFXCHAR*)rString);
	return Temp;
}



// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
// Substring Support
// &**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&**&
//---------------------------------------------------------------------------
//  IFXString::Substring
//
//  This method extracts a substring of the source strings and puts it into
//  the buffers pointed to by pDest.  It allocates a new buffer if needed
//  returning a bool
// uEnd is a misnomer it is really Num chars to get
//---------------------------------------------------------------------------
IFXRESULT IFXString::Substring(IFXString* pDest, U32 uStart, U32 uEnd) const
{
	IFXRESULT result = IFX_OK;
	if(pDest==NULL)
		result = IFX_E_INVALID_POINTER;
	
	if(IFXSUCCESS(result))
	{
		pDest->NewBuffer(uEnd-uStart+1);
		if(pDest->m_Buffer==NULL)
			result = IFX_E_OUT_OF_MEMORY;
	}

	if (IFXSUCCESS(result))
		result = Substring(pDest->m_Buffer, pDest->m_BufferLength, uStart, uEnd);

	return result;
}

//---------------------------------------------------------------------------
//  IFXString::Substring
//
//  This method extracts a substring of the source strings and puts it into
//  the buffers pointed to by pDest.
//---------------------------------------------------------------------------
IFXRESULT IFXString::Substring(IFXCHAR* pDest, U32 uDestSize, U32 uStart, U32 uEnd) const
{
	if(m_Buffer==NULL)
		return IFX_E_NOT_INITIALIZED;
	if(pDest==NULL)
		return IFX_E_INVALID_POINTER;
	if(uStart > m_BufferLength || uDestSize < uEnd)
		return IFX_E_INVALID_RANGE;

	wcsncpy((IFXCHAR*)pDest, m_Buffer + uStart, uEnd);
	pDest[uEnd] = 0;

	return IFX_OK;
}


//---------------------------------------------------------------------------
//  IFXString::FindSubstring
//
//  This method finds the first matching string named pKey starting after the pStartIndex
//  index.  It returns the next instance in pStartIndex
//---------------------------------------------------------------------------
IFXRESULT IFXString::FindSubstring(const IFXCHAR* pKey, U32* pStartIndex) const
{
	IFXRESULT result = IFX_OK;
	const IFXCHAR* pcharSource;
	U32 uLength, uIndex, uKeyLength;

	if(m_Buffer==NULL)
		return IFX_E_NOT_INITIALIZED;
	if(pKey==NULL || pStartIndex==NULL)
		return IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result))
		if(*pStartIndex > m_BufferLength)
			result = IFX_E_INVALID_RANGE;

	if (IFXSUCCESS(result))
		if(m_Buffer==NULL || pKey==NULL || pStartIndex==NULL)
			result = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result)) {
		// pcharSource = (const IFXCHAR*)m_Buffer;
		// uLength = 0;
		// uIndex = 0;
		// uKeyLength = 0;

		pcharSource=(IFXCHAR*)m_Buffer;

		uLength = (U32)wcslen(pcharSource);
		uKeyLength = (U32)wcslen(pKey);

		if (uLength == 0)
			result = IFX_E_CANNOT_FIND;
	}

	if (IFXSUCCESS(result)) {
		uIndex=*pStartIndex;
		while(uIndex<uLength ) {
			if(wcsncmp(&pcharSource[uIndex], pKey, uKeyLength) ==0) {
				*pStartIndex = uIndex;
				return IFX_OK;
			} else
				uIndex++;
		}
	}


	return IFX_E_CANNOT_FIND;

}


//---------------------------------------------------------------------------
//  IFXString::IsDigit
//
//  This method returns a bool based on if the specified character is a digit
//  or not
//---------------------------------------------------------------------------
IFXRESULT IFXString::IsDigit(U32 uIndex)
{
	IFXRESULT iResult=IFX_TRUE;
	if(uIndex > m_BufferLength)
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult))
	{
		iResult = iswdigit((wint_t)m_Buffer[uIndex]) != 0;
	}
	return iResult;
}


//---------------------------------------------------------------------------
//  IFXString::ToString
//
//  This method converts the decimal numerical value into a string characters.
//  Similar to itoa or other functions
//---------------------------------------------------------------------------
IFXRESULT IFXString::ToString(U32 uValue, I8 iRadix)
{
	IFXRESULT result = IFX_OK;
	U8 number[MAX_INT_LENGTH], c;
	U32 i, j, len, temp, rest;

	if ( (iRadix < 2) || (iRadix > 32) )
	{
		result = IFX_E_BAD_PARAM;
	}

	if (IFXSUCCESS(result))
	{
		i = 0;
		temp = uValue;
		do
		{
			rest = temp%iRadix;
			temp = temp/iRadix;

			if (rest > 9)
				number[i] = (U8)((I32)('a')+(rest-10));
			else
				number[i] = (U8)((I32)('0')+rest);

			i++;
		}
		while (temp > 0);
		
		len = i;
		number[len] = '\0';
		
		for (i = 0, j = len-1; i<j; i++, j--)
		{
			c = number[i];
			number[i] = number[j]; 
			number[j] = c;
		}

		result = Assign( number );
	}

	return result;
}


//---------------------------------------------------------------------------
//  IFXString::ToValue
//
//  This method converts the string characters into a numerical value. 
//  Similar to atoi or other functions
//---------------------------------------------------------------------------
IFXRESULT IFXString::ToValue(U32* pValue, I8 iRadix)
{
	IFXRESULT result = IFX_OK;

	if (NULL == pValue)
		result = IFX_E_INVALID_POINTER;

	if (NULL == m_Buffer)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(result))
	{
		IFXCHAR* pEnd = NULL;
		*pValue=(U32)wcstol( m_Buffer, &pEnd, iRadix );
	}

	return result;
}

// Somewhat brute force, but works.  Rewrite if necessary for speed.
IFXString& IFXString::VSPrintf(const IFXCHAR* fmt, va_list ap)
{
	int sz = m_BufferLength;

	if(!m_BufferLength)
	{ // start at 80 bytes, seems reasonable (1 tty line)
		NewBuffer(80);
		sz = m_BufferLength;
	}
	else
	{
		NewBuffer(sz);
	}

	while(IFX_TRUE)
	{
		int n =  vswprintf((IFXCHAR*)m_Buffer, m_BufferLength, fmt, ap);

		if(n > -1 && n < sz)
		{
			break;
		}
		if(n > -1)
		{
			sz = n+1;
		}
		else
		{
			sz *= 2;
		}
		NewBuffer(sz);
	}

	return *this;
}

U32 IFXString::Match(const IFXCHAR *wildcard, U32 icase)
{
    const IFXCHAR *beg, *end;
    return WCMatchPos((IFXCHAR*)m_Buffer,wildcard,&beg,&end,icase);
}
