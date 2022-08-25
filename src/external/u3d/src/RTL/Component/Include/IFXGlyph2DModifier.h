//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXGlyph2DModifier.h

		    Inteface file for the 2D glyph modifier.
*/

#ifndef __IFXGLYPH2DMODIFIER_INTERFACES_H__
#define __IFXGLYPH2DMODIFIER_INTERFACES_H__

#include "IFXUnknown.h"
#include "IFXMatrix4x4.h"
#include "IFXSceneGraph.h"
#include "IFXModifier.h"
#include "IFXSimpleList.h"
#include "IFXGlyph2DCommands.h"

// {51F08BB2-1BC0-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyph2DModifier,
			   0x51f08bb2, 0x1bc0, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

/**
This class provides an interface to construct a 3D extruded model from a set of 2D contours.
It can be used to generate a string of 3D Text.  The contour drawing interface matches up
with the true type font spec.  There are also member functions to control the appearance of the
generated models.   The depth of the extrusion and beveled faces can be specified.
*/
class IFXGlyph2DModifier : virtual public IFXModifier
{
public:
	enum EIFXAttributes
	{
		BILLBOARD = 1,
		SINGLE_SHADER = 1 << 1
	};

	virtual IFXRESULT IFXAPI Initialize( F64 width, F64 spacing, F64 height )=0;

	/**
	This method gets attributes of modifier.

	@param  glyphAttributes desired value of the attributes.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  GetAttributes(U32* pGlyphAttributes)    = 0;

	/**
	This method sets attributes of modifier.

	@param  glyphAttributes desired value of the attributes.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  SetAttributes(U32 glyphAttributes)    = 0;

	// exposure of contour generator fuctions
	/**
	Begin a contour path.
	@return IFXRESLUT
	*/
	virtual IFXRESULT IFXAPI StartPath()=0;
	/**
	Add a line to the current contour from the present location to [fX,fY]
	@param fX,fY  end point of the line.
	*/
	virtual IFXRESULT IFXAPI LineTo(F64 fX, F64 fY)=0;
	/**
	Called after start path to begin a contour.
	@param fX,fY  starting point of a contour.
	*/
	virtual IFXRESULT IFXAPI MoveTo(F64 fX, F64 fY)=0;
	/**
	Draw a Bézier Curve starting at lastt position given by MoveTo,CurveTo or LineTo
	@param  fCx1,fCy1 the first control point
	@param  fCx2,fCy2 the second control point
	@param  fAx,fAy   endpoint of the curve
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy)=0;
	/**
	End a contour.  A line will connect the first point and the last point of the contour if they are not identical.
	@return IFXREULT
	*/
	virtual IFXRESULT IFXAPI EndPath()=0;
	/**
	Start a string of glyphs.
	*/
	virtual IFXRESULT IFXAPI StartGlyphString()=0;
	/**
	Start a contour set for a glyph.
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI StartGlyph()=0;
	/**
	End drawing a contour set for a glyph.
	@return IFXRESLUT
	*/
	virtual IFXRESULT IFXAPI EndGlyph(F64 fWidth, F64 fHeight)=0;
	/**
	End the string.
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI EndGlyphString()=0;
	/**
	Get the bounding box of the glyph string
	@param pMin X,Y and Z of the min box coordinate
	@param pMax X,Y and Z of the max box coordinate
	*/
	virtual IFXRESULT IFXAPI GetBoundingBox(F64 pMin[3], F64 pMax[3])=0;
	/**
	Convert the mesh index to a string index.  Used to tell which character in a string was picked.
	@param uPickedMeshIndex a valid mesh index.
	@param pCharIndex *pCharIndex is set to the index of the character piced in the string.
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI GetCharIndex(U32 uPickedMeshIndex, I32 *pcharIndex)=0;

	/**
	Get the list of contour commands.  The api commands used to generate the contour set are stored in a list.
	@param  ppGlyphList handle to the list of contour commands used to generate the extruded text
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI GetGlyphCommandList(IFXSimpleList** ppGlyphList)=0;
	/**
	Set the contour commands list.
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI SetGlyphCommandList(IFXSimpleList* pGlyphList)=0;
	/**
	Query the current bounding sphere
	@return IFXVector4  set to the bounding sphere of the whole extruded string.
	*/
	virtual const IFXVector4& IFXAPI GetBoundingSphere()=0;
	/**
	Set the bounding sphere
	@param vInBoundingSphere  the desired bounding sphere of the whole extruded string.
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI SetBoundingSphere(const IFXVector4& vInBoundingSphere)=0;

	/**
	Query the current offset transformation
	@return IFXMatrix4x4&  the current offset transformation matrix
	*/
	virtual const IFXMatrix4x4& IFXAPI GetTransform()=0;
	/**
	Set the glyph offset transformation
	@param  offset the desired transform
	@return IFXRESULT
	*/
	virtual IFXRESULT IFXAPI SetTransform(const IFXMatrix4x4& offset)=0;
 
	/**
	Returns the current view transform matrix.
	@return IFXMatrix4x4	Current view transform matrix.\n
	*/
	virtual IFXMatrix4x4* IFXAPI GetViewTransform( void )=0;

	/**
	Sets the current view transform matrix.
	@param transform	Current view transform matrix.\n
	@return void
	*/
	virtual IFXRESULT IFXAPI SetViewTransform( IFXMatrix4x4 transform )=0;
};

#endif
