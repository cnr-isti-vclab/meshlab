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
	@file	IFXAuthorLineSet.h
*/

#ifndef __IFXAUTHORLINESET_H__
#define __IFXAUTHORLINESET_H__

#include "IFXUnknown.h"
#include "IFXString.h"
#include "IFXVector4.h"
#include "IFXAuthor.h"
#include "IFXLine.h"


/// The interface ID (IFXIID) for the IFXAuthorLineSet interface.
// {EE58FC2F-652A-425a-8097-DEC798CCD987}
IFXDEFINE_GUID(IID_IFXAuthorLineSet, 
0xee58fc2f, 0x652a, 0x425a, 0x80, 0x97, 0xde, 0xc7, 0x98, 0xcc, 0xd9, 0x87);


enum IFXLINESET_FLAGS
{	
	IFXLINESET_UNDEFINED			= 0x00000000,
	IFXLINESET_ALLOCATED			= 0x00000004,
	IFXLINESET_WANTATTRIBUTEMAPS	= 0x00000010,
	IFXLINESET_FORCEDWORD			= 0x7FFFFFFF
};

#define IFX_E_AUTHOR_LINESET_LOCKED \
				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_AUTHORGEOM, 0x01)
#define IFX_E_AUTHOR_LINESET_NOT_LOCKED \
				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_AUTHORGEOM, 0x02)

struct IFXAuthorLineSetDesc
{
	U32	m_numLines;			///< Number of lines in the set	
	U32	m_numPositions;		///< Number of positions
	U32	m_numNormals;		///< Number of Normals
	U32	m_numDiffuseColors;	///< Number of Diffuse Colors
	U32	m_numSpecularColors;///< Number of Specular Colors
	U32	m_numTexCoords;		///< Number of Texture Coordinates.
	U32	m_numMaterials;		///< Number of Materials  

	IFXAuthorLineSetDesc() 
	{
		m_numLines  		= U32(-1);
		m_numPositions		= U32(-1);
		m_numNormals		= U32(-1);
		m_numDiffuseColors	= U32(-1);
		m_numSpecularColors	= U32(-1);
		m_numTexCoords		= U32(-1);
		m_numMaterials		= U32(-1);		
	}
};


/**
	This is the main interface for IFXAuthorLineSet.

	Provides high-level information about a line set; namely, what attributes
	are present in the LineSet and how many elements can be found in each
	attribute array.

	@note	The associated IID is named IID_IFXAuthorLineSet.
*/
class IFXAuthorLineSet : virtual public IFXUnknown
{
public:

	/**
	*  Allocates all the necessary space required by the fully-populated
	*  IFXAuthorLineSet.
	*  
	*  @param pLineDesc  The IFXAuthorLineSet data structure describing
	*                 the properties of the line set.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pLineDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficiant memory to allocate
	*     the line set.
	*/
	virtual IFXRESULT IFXAPI  Allocate(const IFXAuthorLineSetDesc* pLineDesc)	= 0;

	/**
	*  Reallocates the LineSet based on the new LineSet description
	*
	*  @param  pLineDesc  The new properties of the LineSet.
    *
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pLineDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficient memory to allocate
	*     the LineSet.
	*/
	virtual IFXRESULT IFXAPI  Reallocate(const IFXAuthorLineSetDesc* pLineDesc) = 0;

	/**
	*  Frees all data associated with the LineSet.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI  Deallocate()  = 0;

	/**
	*  Creates a complete copy of the IFXAuthorLineSet and returns it with
	*  the specified interface.
	*
	*  @param  interfaceID  The IID of the desired LineSet interface
	*  @param  ppNewAuthorLineSet  - a pointer to the new AuthorLineSet copy.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI  Copy(IFXREFIID interfaceID, void** ppNewAuthorLineSet)= 0;

	/**
	*  Returns the current IFXLineSetDesc description.
	*
	*  @return A pointer to an IFXAuthorLineSetDesc structure describing the
	*     AuthorLineSet.
	*/
	virtual IFXAuthorLineSetDesc* IFXAPI GetLineSetDesc()	= 0;

	/**
	*  Sets the current IFXLineSetDesc description (as opposed to the "MaxLineSetDesc,"
	*  which is set when the LineSet is allocated).
	*
	*  @param pLineSetDesc  The new IFXLineSetDesc description
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE One of the values in pLineSetDesc is
	*     larger than the IFXLineSetDesc used to allocate the LineSet,
	*     and/or the number of materials is not the same as when the LineSet
	*     was allocated.
	*/
	virtual IFXRESULT IFXAPI  SetLineSetDesc(const IFXAuthorLineSetDesc* pLineSetDesc)= 0;

	/**
	*  Returns the maximum resolution LineSet description 
	*
	*  @return A pointer to an IFXAuthorLineSetDesc structure describing the
	*     full-resolution representation of the LineSet.
	*/
	virtual IFXAuthorLineSetDesc* IFXAPI GetMaxLineSetDesc()				= 0;

	/** 
	*  Returns the number of texline layers, where texLines layers are 
	*  allocated on demand to save memory.
	*
	*  @param pLayers  The number of layers allocated
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pLayers was NULL
	*/
	virtual IFXRESULT IFXAPI  GetNumAllocatedTexLineLayers(U32* pLayers) = 0;


	// get & set indexes array
	/** 
	*  Returns the pointer to the Line Position index array. 
	*
	*  @param ppPositionLine  The array of IFXLine specifying
	*     the indices of the positions for each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppPositionLine was NULL.
	*/	
	virtual IFXRESULT IFXAPI  GetPositionLines(IFXU32Line** ppPositionLine)	= 0;
	/**
	*  Returns the pointer to the LineSet Normal index array. 
	*
	*  @param ppNormalLine  The array of IFXLine specifying
	*     the indices of the normals used by each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER   ppNormalLine was NULL
	*  @retval IFX_E_INVALID_RANGE     The LineSet does not contain this 
	*      attribute. NumNormals in the IFXAuthorLineSetDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetNormalLines(IFXU32Line** ppNormalLine)		= 0;
	/**
	*  Returns the pointer to the LineSet Diffuse index array. 
	*
	*  @param ppDiffuseLine  The array of IFXLine specifying
	*     the indices of the diffuse colors used by each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppDiffuseLine was NULL.
	*  @retval IFX_E_INVALID_RANGE   The LineSet does not contain this attribute.
	*      NumDiffuseColors in the IFXLine is equal to zero(0).
	*/
	
	virtual IFXRESULT IFXAPI  GetDiffuseLines(IFXU32Line** ppDiffuseLine)		= 0;

	/**
	*  Returns the pointer to the LineSet Specular index array 
	*
	*  @param ppSpecularFaces  The array of IFXLine specifying
	*     the indices of the specular colors used by each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppSpecularFaces was NULL.
	*  @retval IFX_E_INVALID_RANGE   The LineSet does not contain this attribute.
	*      NumSpecularColors in the IFXLine is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetSpecularLines(IFXU32Line** ppSpecularLine)	= 0;


	/**
	*  Returns the pointer to the LineSet Texture Coordinates index array
	*
	*  @param ppTexCoordLines  The array of IFXLine specifying
	*     the indices of the texture coords used by each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppTexCoordLines was NULL.
	*  @retval IFX_E_INVALID_RANGE   The LineSet does not contain this attribute.
	*      NumTextureCoords in the IFXLine is equal to zero(0).
	*/
	virtual	IFXRESULT IFXAPI  GetTexCoordsLines (IFXU32Line** ppTexCoordLines) = 0;

	/**
	*  Returns the pointer to the LineSet Materials index array
	*
	*  @param ppMaterialLines  The array of IFXLine specifying
	*     the indices of the materials used by each vertex on each line. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppTexCoordLines was NULL.
	*  @retval IFX_E_INVALID_RANGE   The LineSet does not contain this attribute.
	*      NumMaterials in the IFXLine is equal to zero(0).
	*/
	virtual	IFXRESULT IFXAPI  GetMaterialsLines (U32** ppMaterialLines) = 0; 

	/**
	*  Retrieves a specific Position line index. 
	*
	*  @param index  The index of the position line to return
	*  @param pPositionLine  The returned line containing position indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pPositionLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position line segm in the lineset.
	*/
	virtual IFXRESULT IFXAPI  GetPositionLine(U32 index, IFXU32Line* pPositionLine)	= 0;
	/**
	*  Sets a specific Position Line index.
	*
	*  @param index  The index of the position line to set
	*  @param pPositionLine  The position Line containing the position 
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pPositionLien was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position lines in the lineset.
	*/
	virtual IFXRESULT IFXAPI  SetPositionLine(
										U32 index, 
										const IFXU32Line* pPositionLine) = 0;

	/**
	*  Retrieves a specific Normal Line index. 
	*
	*  @param index  The index of the normal Line to return
	*  @param pNormalLine  The returned line containing normal indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pNormalLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal Line in the lineset.
	*/	
	virtual IFXRESULT IFXAPI  GetNormalLine(U32 index, IFXU32Line* pNormalLine)	= 0;
	
	/**
	*  Set a specific Normal Line index.
	*
	*  @param index  The index of the normal line to return
	*  @param pNormalLine  The normal Line containing the normal 
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pNormalLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal Line in the LineSet.
	*/
	virtual IFXRESULT IFXAPI  SetNormalLine(U32 index, const IFXU32Line* pNormalLine) = 0;

	/**
	*  Retrieves a specific Diffuse vertex color Line index. 
    *
	*  @param index  The index of the diffuse Line to return
	*  @param pDiffuseFace  The diffuse line containing the diffuse vertex
	*                           color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pDiffuseLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color Line in the LineSet.
	*/
	virtual IFXRESULT IFXAPI  GetDiffuseLine(U32 index,	IFXU32Line* pDiffuseLine) = 0;

	/**
	*  Set a specific Diffuse Line index.
    *
	*  @param index  The index of the diffuse Line to set
	*  @param pDiffuseFace  The diffuse Line containing the diffuse vertex
	*                           color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pDiffuseLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color Line in the LineSet.
	*
	*/
	virtual IFXRESULT IFXAPI  SetDiffuseLine(U32 index,	const IFXU32Line* pDiffuseLine)	= 0;
	
	/**
	*  Retrieves a specific Specular line index. 
	*
	*  @param index  The index of the specular Line to return
	*  @param pSpecularFace  The specular Line containing the specular 
	*                           vertex color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pSpecularLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color Line in the LineSet.
	*/
	virtual IFXRESULT IFXAPI  GetSpecularLine(U32 index, IFXU32Line* pSpecularLine)	= 0;

	/**
	*  Set a specific Specular Line index.
    *
	*  @param index  The index of the specular Line to set
	*  @param pSpecularFace  The specular line containing the specular 
	*                           vertex color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pSpecularLine was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color Line in the LineSet.
	*/
	virtual IFXRESULT IFXAPI  SetSpecularLine(U32 index, const IFXU32Line* pSpecularLine) = 0;

	/** 
	*  Returns the pointer to the Material Definition Array. 
	*
	*  @param  ppMaterials  The pointer to the material definition array
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER   ppMaterials was NULL.
	*/
	virtual IFXRESULT IFXAPI  GetMaterials(IFXAuthorMaterial** ppMaterials) = 0;

	/**
	*  Returns the pointer to the Line Material index array. 
	*
	*  @param ppLineMaterials  The array of U32es specifying
	*     the material index to use on each line of the lineset
	*	
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppFaceMaterials was NULL.
	*  @retval IFX_E_INVALID_RANGE   The lineset does not contain this attribute.
	*      NumMaterials in the IFXAuthorLineDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetLineMaterials(U32** ppLineMaterials) = 0;

	/**
	*  Retrieves a specific material by index. 
	*
	*  @param index  The index of the material to return
	*  @param pMaterial  A pointer to the material.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER pMaterial was NULL.
	*  @retval IFX_E_INVALID_RANGE The index is invalid
	*/
	virtual IFXRESULT IFXAPI GetMaterial( U32 index, IFXAuthorMaterial* pMaterial)	= 0;


	/**
	*  Sets a specific material line.
	*
	*  @param index  The index of the material to set
	*  @param pMaterial  A pointer to the material to assign to this index.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER pMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number of
	*      materials used by the lineset.
	*/
	virtual IFXRESULT IFXAPI SetMaterial( 
									U32 index, 
									const IFXAuthorMaterial* pMaterial) = 0;

	/**
	*  Returns the pointer to the Face Texture index array. 
	*
	*  @param index   The index of the texture layer to recover
	*  @param ppTexLines  The array of IFXU32Lines specifying
	*     the indices of the texture coordinates used by each vertex 
	*     on each line for the specified texture layer.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppTexLines was NULL.
	*  @retval IFX_E_INVALID_RANGE	 The Texture Layer index is invalid. The 
	*        value specified for index may be out of range.  This parameter 
	*        cannot be less than zero(0) or greater than
	*        IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      m_numTexCoords in the IFXAuthorLineSetDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetTexLines(U32 index, IFXU32Line** ppTexLines) = 0;
	
	/**
	*  Returns the pointer to the Position Array for this line set.
	*
	*  @param ppPositions  The array of IFXVector3s containing all the
	*     vertex positions used in this line set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppPositions was NULL.
	*  @retval IFX_E_INVALID_RANGE   The line set does not contain this attribute.
	*      m_numPositions in the IFXAuthorLineSetDesc is equal to zero(0).
	*/	
	virtual IFXRESULT IFXAPI  GetPositions (IFXVector3** ppPositions) = 0;
	
	/**
	*  Returns the pointer to the Normal array for this line set.
	*
	*  @param ppNormals  The array of IFXVector3s containing all the
	*     normals used in this line set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppNormals was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      m_numNormals in the IFXAuthorLineSetDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetNormals(IFXVector3** ppNormals) = 0 ;
	
	/**
	*  Returns the pointer to the array of Texture Coordinates for  this line set.
	*
	*  @param ppTexCoords  The array of IFXVector4s containing the
	*     texture coordinates used by this line set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppTexCoords was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      m_numTexCoords in the IFXAuthorLineSetDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI  GetTexCoords(IFXVector4** ppTexCoords) = 0;
	
	/**
	*  Returns the pointer to the array of Diffuse Vertex Colors for this line set.
	*
	*  @param ppDiffuseColors  The array of IFXVector4s containing the
	*     diffuse vertex colors used by this line set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppDiffuseColors was NULL.
	*  @retval IFX_E_INVALID_RANGE   The line set does not contain this attribute.
	*      m_numDiffuseColors in the IFXAuthorLineSetDesc is equal to zero(0).
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  GetDiffuseColors(IFXVector4** ppDiffuseColors) = 0;

	/**
	*  Returns the pointer to the array of Specular Vertex Colors for this line set.
	*
	*  @param ppSpecularColor  The array of IFXVector4s containing the
	*     specular vertex colors used by this line set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppSpecularColors was NULL.
	*  @retval IFX_E_INVALID_RANGE   The line set does not contain this attribute.
	*      m_numSpecularColors in the IFXAuthorLineSetDesc is equal to zero(0).
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  GetSpecularColors(IFXVector4** ppSpecularColor) = 0;

	/**
	*  Retrieves a specific vertex position from the Line Set.
	*
	*  @param index   The index of the vertex position to return
	*  @param pVector3  The position of the specified vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       vertex positions stored in the line set.
	*/
	virtual IFXRESULT IFXAPI  GetPosition(U32 index,IFXVector3* pVector3)= 0;

	/**
	*  Sets a specific vertex position for this Lineset.
	*
	*  @param index   The index of the vertex position to set
	*  @param pVector3  The new position value for this vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       vertex positions stored in the lineset.
	*/
	virtual IFXRESULT IFXAPI  SetPosition(U32 index,const IFXVector3* pVector3)= 0;

	/**
	*  Retrieves a specific Normal from this lineset.
	*
	*  @param index   The index of the normal to return
	*  @param pVector3  The requested normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       normals stored in the lineset.
	*/
	virtual IFXRESULT IFXAPI  GetNormal(U32 index, IFXVector3* pVector3)= 0;

	/**
	*  Sets a specific Normal for this lineset.
	*
	*  @param index   The index of the normal to set
	*  @param pVector3  The new value of the normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       normals stored in the lineset.
	*/
	virtual IFXRESULT IFXAPI  SetNormal(U32 index, const IFXVector3* pVector3)= 0;

	/**
	*  Retrieves a specific Diffuse vertex color from this lineset.
	*
	*  @param index   The index of the Diffuse vertex color to return
	*  @param pColor  The requested color
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       diffuse vertex colors stored in the lineset.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  GetDiffuseColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Sets a specific Diffuse vertex color for this lineset.
	*
	*  @param index   The index of the diffuse vertex color to set
	*  @param pColor  The new diffuse vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       diffuse vertex colors stored in the lineset.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  SetDiffuseColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Retrieves a specific Specular vertex color from this lineset.
	*
	*  @param index   The index of the specular vertex color to return
	*  @param pColor  The requested color value
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       specular vertex colors stored in the lineset.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  GetSpecularColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Sets a specific Specular vertex color of this lineset.
	*
	*  @param index   The index of the specular vertex color to set
	*  @param pColor  The new specular vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       specular vertex colors stored in the lineset.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI  SetSpecularColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Retrieves a specific Texture line index. 
	*
	*  @param layer  The texture coordinate layer this line is from
	*  @param index  The index of the texture line to return
	*  @param pTextureLine  The texture line containing the texture 
	*                           coordinate indices
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate lines in the line set, the line set has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/	
	virtual IFXRESULT IFXAPI GetTexLine( 
									U32 layer, 
									U32 index, 
									IFXU32Line* pTextureLine) = 0;

	/**
	*  Sets a specific Texture Line index.
	*
	*  @param layer  The texture coordinate layer into which to
	*               place the line
	*  @param index  The index of the texture line to set
	*  @param pTextureLine  The texture line containing the texture 
	*                           coordinate indices to set in the line set
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate lines in the line set, the line set has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/
	virtual IFXRESULT IFXAPI  SetTexLine(
								U32 layer, 
								U32 index, 
								const IFXU32Line* pTextureLine)	= 0;

	/**
	*  Retrieves a specific Texture Coordinate from this lineset.
	*
	*  @param index   The index of the texture coordinate to return
	*  @param pVector4  The requested texture coordinate 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       texture coordinates stored in the lineset.
	*/
	virtual IFXRESULT IFXAPI GetTexCoord(U32 index, IFXVector4* pVector4) = 0;

	/**
	*  Sets specific Texture Coordinate for this lineset.
	*
	*  @param index   The index of the texture coordinate to set
	*  @param pVector4  The new texture coordinate value to store 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       texture coordinates stored in the lineset.
	*/
	virtual IFXRESULT IFXAPI  SetTexCoord(U32 index, const IFXVector4* pVector4) = 0;

	/**
	*  Retrieves a specific Material line ID. 
	*
	*  @param index   The index of the Line for which we want the material ID
	*  @param puFaceMaterial  The returned material ID.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  puFaceMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               lines in the LineSet.
	*/
	virtual IFXRESULT IFXAPI GetLineMaterial(U32 index, U32* pLineMaterial) = 0;

	/**
	*  Sets a specific Line Material ID
	*
	*  @param index   The index of the line on which we want to set the
	*           material ID.
	*  @param faceMaterialID  The material ID to set on the line.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               lines in the LineSet.
	*/
	virtual IFXRESULT IFXAPI SetLineMaterial(U32 index, U32 lineMaterialID)= 0;

	/// Calculates bounding sphere.
	virtual IFXVector4 IFXAPI CalcBoundSphere()=0;
};

#endif
