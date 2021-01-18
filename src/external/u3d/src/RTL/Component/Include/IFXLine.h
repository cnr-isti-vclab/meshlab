//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

/**
	@file	IFXLine.h
*/

#ifndef __IFXLINE_H__
#define __IFXLINE_H__

#include "IFXDebug.h"

/**
 *	Template class for line primitives. Indices for lines are usually
 *	either 16 bit or 32 bit values.  There are template specializations
 *	for both of these cases (IFXU16Line and IFXU32Line).
 */
template<class INDEX>
class IFXLineT
{
public:
	/**
	 *	Sets the two vertex indices for this line.
	 *
	 *	@param	a	Start vertex index
	 *	@param	b	End vertex index
	 */
	void IFXAPI Set(INDEX a, INDEX b)
	{
		m_pData[0] = a;
		m_pData[1] = b;
	}
	
	/**
	 *	Sets the start vertex index of this line.
	 *	@param	a	Start vertex index
	 */
	void IFXAPI SetA(INDEX a)
	{
		m_pData[0] = a;
	}

	/**
	 *	Sets the end vertex index of this line.
	 *	@param	b	Start vertex index
	 */
	void IFXAPI SetB(INDEX b)
	{
		m_pData[1] = b;
	}

	/**
	 *	Sets a designated vertex index of this line.
	 *	@param	idx	Which vertex index to set, must be either 0 or 1.
	 *	@param	val	The new vertex index.
	 */
	void IFXAPI SetAtIndex(U32 idx, INDEX val)
	{
		IFXASSERTBOX(idx < 2, "Line corner index out of range!");
		m_pData[idx] = val;
	}
	
	/**
	 *	Retrieves the vertex index for the start of the line.
	 *	@return	The start vertex index.
	 */
	INDEX IFXAPI VertexA() const { return m_pData[0]; }

	/**
	 *	Retrieves the vertex index for the end of the line.
	 *	@return	The end vertex index.
	 */
	INDEX IFXAPI VertexB() const { return m_pData[1]; }

	/**
	 *	Retrieves the vertex index for the specified end of the line.
	 *	@param	idx	Which vertex index to retrieve, must be either 0 or 1.
	 *	@return	The specified vertex index.
	 */
	INDEX IFXAPI Vertex(U32 idx) const
	{
		IFXASSERTBOX(idx < 2, "Line corner index out of range!");
		return m_pData[idx]; 
	}
protected:
	INDEX m_pData[2];
};
typedef IFXLineT<U16> IFXU16Line;
typedef IFXLineT<U32> IFXU32Line;

/**
 *	Abstract base class representation of a line primitive that can handle either
 *	16 or 32 bit indices.
 *
 *	Unlike IFXLineT (where the instance of the line IS the data), this class holds
 *	a pointer to a buffer that contains the vertex indices.
 */
class IFXLine
{
public:
	IFXLine() {}			///< Constructor
	virtual ~IFXLine() {}	///< Destrutor

	/**
	 *	Gets a pointer to the raw line indices.
	 *	@return	A void* pointer to the line indices.
	 */
	virtual void* IFXAPI GetDataPtr() const=0;
	
	/**
	 *	Sets the location of the line indices for this line.
	 *	@param	pData	A void* pointer to the new line indices.
	 */
	virtual void IFXAPI SetDataPtr(void* pData)=0;
	
	/**
	 *	Sets the two vertex indices for this line.
	 *
	 *	@param	a	Start vertex index
	 *	@param	b	End vertex index
	 */
	virtual void IFXAPI Set(U32 a, U32 b)=0;
	/**
	 *	Sets the start vertex index of this line.
	 *	@param	a	Start vertex index
	 */
	virtual void IFXAPI SetA(U32 a)=0;
	/**
	 *	Sets the end vertex index of this line.
	 *	@param	b	Start vertex index
	 */
	virtual void IFXAPI SetB(U32 b)=0;

	/**
	 *	Sets a designated vertex index of this line.
	 *	@param	idx	Which vertex index to set, must be either 0 or 1.
	 *	@param	val	The new vertex index.
	 */
	virtual void IFXAPI SetAtIndex(U32 idx, U32 val)=0;
	
	/**
	 *	Retrieves the vertex index for the start of the line.
	 *	@return	The start vertex index.
	 */
	virtual U32 IFXAPI VertexA() const=0;
	/**
	 *	Retrieves the vertex index for the end of the line.
	 *	@return	The end vertex index.
	 */
	virtual U32 IFXAPI VertexB() const=0;
	
	/**
	 *	Retrieves the vertex index for the specified end of the line.
	 *	@param	idx	Which vertex index to retrieve, must be either 0 or 1.
	 *	@return	The specified vertex index.
	 */
	virtual U32 IFXAPI Vertex(U32 idx) const=0;
	
	/**
	 *	Implementation of the '=' (assignment) operator to account for 
	 *	pointers.
	 */
	const IFXLine& IFXAPI operator=(const IFXLine& operand);

	/**
	 *	Makes this instance point to the same vertex indices as @a operand.
	 *	After this method completes, changes to the line indices in one
	 *	instance will affect the other instance.
	 *
	 *	@param	operand	Another IFXLine instance that we want to duplicate.
	 */
	void IFXAPI Clone(const IFXLine& operand);
};

/**
 *	Generic implentation of IFXLine that will be specialized to support 
 *	either 16 or 32 bit indices (IFXGenLineU16 and IFXGenLineU32)
 */
template <class INDEX>
class IFXGenLineT : public IFXLine
{
public: 
	/// Constructor.
	IFXGenLineT() {SetDataPtr(0);}
	virtual ~IFXGenLineT() {} ///< Destructor.

	/**
	 *	Gets a pointer to the raw line indices.
	 *	@return	A void* pointer to the line indices.
	 */
	void* IFXAPI GetDataPtr() const
	{
		return (void*)m_pLine;
	}
	/**
	 *	Sets the location of the line indices for this line.
	 *	@param	pData	A void* pointer to the new line indices.
	 */
	void IFXAPI SetDataPtr(void* pData)
	{
		m_pLine = (IFXLineT<INDEX>*)pData;
	}
	/**
	 *	Sets the two vertex indices for this line.
	 *
	 *	@param	a	Start vertex index
	 *	@param	b	End vertex index
	 */
	void IFXAPI Set(U32 a, U32 b)
	{
		m_pLine->Set((INDEX)a, (INDEX)b );
	}
	/**
	 *	Sets the start vertex index of this line.
	 *	@param	a	Start vertex index
	 */
	void IFXAPI SetA(U32 a)
	{
		m_pLine->SetA((INDEX)a);
	}
	/**
	 *	Sets the end vertex index of this line.
	 *	@param	b	Start vertex index
	 */
	void IFXAPI SetB(U32 b)
	{
		m_pLine->SetB((INDEX)b);
	}

	/**
	 *	Sets a designated vertex index of this line.
	 *	@param	idx	Which vertex index to set, must be either 0 or 1.
	 *	@param	val	The new vertex index.
	 */
	void IFXAPI SetAtIndex(U32 idx, U32 val)
	{
		m_pLine->SetAtIndex(idx, (INDEX)val);
	}
	
	/**
	 *	Retrieves the vertex index for the start of the line.
	 *	@return	The start vertex index.
	 */
	U32 IFXAPI VertexA() const
	{
		return (U32)m_pLine->VertexA();
	}
	/**
	 *	Retrieves the vertex index for the end of the line.
	 *	@return	The end vertex index.
	 */
	U32 IFXAPI VertexB() const
	{
		return (U32)m_pLine->VertexB();
	}
	
	/**
	 *	Retrieves the vertex index for the specified end of the line.
	 *	@param	idx	Which vertex index to retrieve, must be either 0 or 1.
	 *	@return	The specified vertex index.
	 */
	U32 IFXAPI Vertex(U32 idx) const
	{
		return (U32)m_pLine->Vertex(idx);
	}

private:
	IFXLineT<INDEX>* m_pLine;	///< Pointer to line index buffer.
};
typedef IFXGenLineT<U16> IFXGenLineU16;
typedef IFXGenLineT<U32> IFXGenLineU32;

IFXINLINE const IFXLine& IFXLine::operator=(const IFXLine& operand)
{
	Set(operand.VertexA(), operand.VertexB());

	return *this;
}

IFXINLINE void IFXLine::Clone(const IFXLine& operand)
{
	SetDataPtr(operand.GetDataPtr());
}

#endif
