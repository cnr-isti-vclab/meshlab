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
// IFXRenderHelpers.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_HELPERS_H
#define IFX_RENDER_HELPERS_H

#include "IFXAPI.h"

//==============================
// IFXRectT
//==============================
/**
 *	This is a generic templated rectangle class.  The default
 *	instantiations are with signed 32-bit integers (IFXRect) and
 *	single precision floating point values (IFXF32Rect).
 */
template<class T> class IFXRectT
{
public:
	/**
	 *	Constructor - initializes to a 10x10 rectangle.
	 */
	IFXRectT()
	{
		IFXRectT<T>(0, 0, 10, 10);
	}

	/**
	 *	Constructor.
	 *
	 *	@param x	Input T value that is the initial value for m_X.
	 *	@param y	Input T value that is the initial value for m_Y.
	 *	@param width	Input T value that is the initial value for m_Width.
	 *	@param height	Input T value that is the initial value for m_Height.
	 */
	IFXRectT(T x, T y, T width, T height)
	{
		m_X = x;
		m_Y = y;
		m_Width = width;
		m_Height = height;
	}

	/**
	 *	Standard '==' operator override.
	 *
	 *	@param	rcIn	Input reference to an IFXRectT object.  This must
	 *					have the same template type as host object.
	 *
	 *	@return	A Boolean value.
	 *
	 *	@retval	TRUE	@a rcIn is identical to this object.
	 *	@retval	FALSE	@a rcIn has different properties than this object.
	 */
	BOOL IFXAPI operator==(const IFXRectT& rcIn) const
	{
		if(m_X != rcIn.m_X)
			return FALSE;
		if(m_Y != rcIn.m_Y)
			return FALSE;
		if(m_Width != rcIn.m_Width)
			return FALSE;
		if(m_Height != rcIn.m_Height)
			return FALSE;

		return TRUE;
	}

	/**
	 *	Standard '=' operator override.  After this method returns, this
	 *	object will have identical properties to @a rcIn.
	 *
	 *	@param	rcIn	Input reference to an IFXRectT object.  This must
	 *					have the same template type as host object.
	 *
	 *	@return	A reference to this object.
	 */
	IFXRectT& IFXAPI operator=(const IFXRectT& rcIn)
	{
		m_X = rcIn.m_X;
		m_Y = rcIn.m_Y;
		m_Width = rcIn.m_Width;
		m_Height = rcIn.m_Height;

		return (*this);
	}

	/**
	 *	Determines whether the rectangle represented by this
	 *	object completely envelopes the rectangle represented by @a rcIn.
	 *
	 *	@param	rcIn	Input reference to an IFXRectT object.  This must
	 *					have the same template type as host object.
	 *
	 *	@return	A Boolean value.
	 *
	 *	@retval	TRUE	The rectangle represented by this object completely
	 *					contains the rectangle represented by @a rcIn.
	 *
	 *	@retval	FALSE	This rectangle does not contain the @a rcIn rectangle.
	 */
	BOOL IFXAPI DoesContain(const IFXRectT& rcIn) const
	{
		if(m_X > rcIn.m_X)
		{
			return FALSE;
		}
		if(m_Y > rcIn.m_Y)
		{
			return FALSE;
		}
		if(Right() < rcIn.Right())
		{
			return FALSE;
		}
		if(Bottom() < rcIn.Bottom())
		{
			return FALSE;
		}

		return TRUE;
	}

	/**
	 *	Attempts change @a rcIn to lie within the bounds
	 *	of the rectangle represented by this object.
	 *
	 *	@param	rcIn	Input/Output reference to an IFXRectT object.  This
	 *					must have the same template type as host object.
	 *
	 *	@return	A Boolean value.
	 *
	 *	@retval	TRUE	The method was successfully able to modify rcIn to
	 *					lie within this rectangle.
	 *	@retval	FALSE	The rectangle passed in trough @rcIn does not intersect
	 *					this rectangle.  The final value of rcIn may have
	 *					negative values for m_Width and/or m_Height.
	 */
	BOOL IFXAPI GetIntersection(IFXRectT& rcIn) const
	{
		if(m_X > rcIn.m_X)
		{
			rcIn.m_Width -= m_X - rcIn.m_X;
			rcIn.m_X = m_X;
		}
		if(Right() < rcIn.Right())
		{
			rcIn.SetRight(Right());
		}
		if(m_Y > rcIn.m_Y)
		{
			rcIn.m_Height -= m_Y - rcIn.m_Y;
			rcIn.m_Y = m_Y;
		}
		if(Bottom() < rcIn.Bottom())
		{
			rcIn.SetBottom(Bottom());
		}

		if(rcIn.m_Width <= 0 || rcIn.m_Height <= 0)
		{
			return FALSE;
		}

		return TRUE;
	}


	/**
	 *	Modifies the m_Hieght property so that m_Y + m_Height = @a iBottom.
	 *
	 *	@param	iBottom	Input value that represents the desired value for
	 *			m_Y + m_Height.
	 */
	void IFXAPI SetBottom(T iBottom)
	{
		m_Height = iBottom - m_Y;
	}

	/**
	 *	Modifies the m_Width property so that m_X + m_Width = @a iRight.
	 *
	 *	@param	iRight Input value that represents the desired value for
	 *			m_X + m_Width.
	 */
	void IFXAPI SetRight(T iRight)
	{
		m_Width = iRight - m_X;
	}

	/**
	 *	Sets new dimensions for the rectangle.
	 *
	 *	@param	iX	Input value set into m_X.
	 *	@param	iY	Input value set into m_Y.
	 *	@param	iWidth	Input value set into m_Width.
	 *	@param	iHeight	Input value set into m_Height.
	 */
	void IFXAPI Set(T iX, T iY, T iWidth, T iHeight)
	{
		m_X = iX;
		m_Y = iY;
		m_Width = iWidth;
		m_Height = iHeight;
	}

	/**
	 *	Gets the right edge of the rectangle.  This property isn't stored
	 *	explicitly, so this accessor is required to compute it.
	 *
	 *	@return	A value of the templated type T.  This is the value computed
	 *			by m_X + m_Width.
	 */
	T IFXAPI Right() const
	{
		return m_X + m_Width;
	}

	/**
	 *	Gets the bottom edge of the rectangle.  This property isn't stored
	 *	explicitly, so this accessor is required to compute it.
	 *
	 *	@return	A value of the templated type T.  This is the value computed
	 *			by m_Y + m_Height.
	 */
	T IFXAPI Bottom() const
	{
		return m_Y + m_Height;
	}

	T m_X;			///< Left edge of rectangle.
	T m_Y;			///< Top edge of rectangle.
	T m_Width;		///< Width of rectangle.
	T m_Height;		///< Height of rectangle.
};

/**
 *	Template instantiation of IFXRectT with signed integers.
 */
typedef IFXRectT<I32> IFXRect;

/**
 *	Template instantiation of IFXRectT with single precision floats.
 */
typedef IFXRectT<F32> IFXF32Rect;


#endif
