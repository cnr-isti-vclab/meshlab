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
	@file	IFXAuthorPointSet.h
		
			Declaration of the IFXAuthorPointSet interface.
*/

#ifndef IFXAuthorPointSet_H
#define IFXAuthorPointSet_H

#include "IFXUnknown.h"
#include "IFXVector4.h"
#include "IFXAuthor.h"

/// The interface ID (IFXIID) for the IFXAuthorPointSet interface.
// {3ABEA6BB-FA68-43d3-B5D3-E00AA389A909}
IFXDEFINE_GUID(IID_IFXAuthorPointSet, 
0x3abea6bb, 0xfa68, 0x43d3, 0xb5, 0xd3, 0xe0, 0xa, 0xa3, 0x89, 0xa9, 0x9);


struct IFXAuthorPointSetDesc
{
	U32	m_numPoints;		///< Number of Points in the set	
	U32	m_numPositions;		///< Number of positions
	U32	m_numNormals;		///< Number of Normals
	U32	m_numDiffuseColors;	///< Number of Diffuse Colors
	U32	m_numSpecularColors;///< Number of Specular Colors
	U32	m_numTexCoords;		///< Number of Texture Coordinates.
	U32	m_numMaterials;		///< Number of Materials  
     
	IFXAuthorPointSetDesc() 
	{
		m_numPoints  		= U32(-1);
		m_numPositions		= U32(-1);
		m_numNormals		= U32(-1);
		m_numDiffuseColors	= U32(-1);
		m_numSpecularColors	= U32(-1);
		m_numTexCoords		= U32(-1);
		m_numMaterials		= U32(-1);		
	}
};


/**
	This is the main interface for IFXAuthorPointSet.

	Provides high-level information about a Point set; namely, what attributes
	are present in the PointSet and how many elements can be found in each
	attribute array.

	@note	The associated IID is named IID_IFXAuthorPointSet.
*/
class IFXAuthorPointSet : virtual public IFXUnknown  
{
public:

	/**
	*  Allocates all the necessary space required by the fully-populated
	*  IFXAuthorPointSet.
	*  
	*  @param pPointDesc  The IFXAuthorPointSet data structure describing
	*                 the properties of the Point set.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pPointDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficiant memory to allocate
	*     the Point set.
	*/
	virtual IFXRESULT IFXAPI Allocate(const IFXAuthorPointSetDesc* pPointDesc) = 0;

	/**
	*  Reallocates the PointSet based on the new PointSet description
	*
	*  @param  pPointDesc  The new properties of the PointSet.
    *
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pPointDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficient memory to allocate
	*     the PointSet.
	*/
	virtual IFXRESULT IFXAPI Reallocate(const IFXAuthorPointSetDesc* pPointDesc) = 0;

	/**
	*  Frees all data associated with the PointSet.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI Deallocate()  = 0;

	/**
	*  Creates a complete copy of the IFXAuthorPointSet and returns it with
	*  the specified interface.
	*
	*  @param  interfaceID  The IID of the desired PointSet interface
	*  @param  ppNewAuthorPointSet  - a pointer to the new AuthorPointSet copy.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI Copy(IFXREFIID interfaceID, void** ppNewAuthorPointSet)= 0;

	/**
	*  Returns the current IFXPointSetDesc description.
	*
	*  @return A pointer to an IFXAuthorPointSetDesc structure describing the
	*     AuthorPointSet.
	*/
	virtual IFXAuthorPointSetDesc* IFXAPI GetPointSetDesc()	= 0;

	/**
	*  Sets the current IFXPointSetDesc description (as opposed to the MaxPointSetDesc
	*  which is set when the PointSet is allocated).
	*
	*  @param pPointSetDesc  The new IFXPointSetDesc description
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE One of the values in pPointSetDesc is
	*     larger than the IFXPointSetDesc used to allocate the PointSet,
	*     and/or the number of materials is not the same as when the PointSet
	*     was allocated.
	*/
	virtual IFXRESULT IFXAPI SetPointSetDesc(
									const IFXAuthorPointSetDesc* pPointSetDesc)= 0;

	/**
	*  Returns the maximum resolution PointSet description 
	*
	*  @return A pointer to an IFXAuthorPointSetDesc structure describing the
	*     full-resolution representation of the PointSet.
	*/
	virtual IFXAuthorPointSetDesc* IFXAPI GetMaxPointSetDesc()				= 0;

	/** 
	*  Returns the number of texPoint layers, where texPoints layers are 
	*  allocated on demand to save memory.
	*
	*  @param pLayers  The number of layers allocated
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pLayers was NULL
	*/
	virtual IFXRESULT IFXAPI GetNumAllocatedTexPointLayers(U32* pLayers) = 0;


	// get & set indexes array
	/** 
	*  Returns the pointer to the Point Position index array. 
	*
	*  @param ppPositionPoint  The array of IFXPoint specifying
	*     the indices of the positions for each vertex on each Point. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppPositionPoint was NULL.
	*/	
	virtual IFXRESULT IFXAPI GetPositionPoints(U32** ppPositionPoint)	= 0;

	/**
	*  Returns the pointer to the PointSet Normal index array. 
	*
	*  @param ppNormalPoint  The array of IFXPoint specifying
	*     the indices of the normals used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER   ppNormalPoint was NULL
	*  @retval IFX_E_INVALID_RANGE     The PointSet does not contain this 
	*      attribute. NumNormals in the IFXAuthorPointSetDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetNormalPoints(U32** ppNormalPoint)		= 0;

	/**
	*  Returns the pointer to the PointSet Diffuse Colors index array. 
	*
	*  @param ppDiffusePoint  The array of IFXPoint specifying
	*     the indices of the diffuse colors used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppDiffusePoint was NULL.
	*  @retval IFX_E_INVALID_RANGE   The PointSet does not contain this attribute.
	*      NumDiffuseColors in the IFXPoint is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetDiffusePoints(U32** ppDiffusePoint)		= 0;

	/**
	*  Returns the pointer to the PointSet Specular Colors index array 
	*
	*  @param ppSpecularFaces  The array of IFXPoint specifying
	*     the indices of the specular colors used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppSpecularFaces was NULL.
	*  @retval IFX_E_INVALID_RANGE   The PointSet does not contain this attribute.
	*      NumSpecularColors in the IFXPoint is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetSpecularPoints(U32** ppSpecularPoint)	= 0;

	/**
	*  Returns the pointer to the PointSet Texture Coordinates index array 
	*
	*  @param ppTexCoordPoints  The array of IFXPoint specifying
	*     the indices of the texture coordinates used by each vertex on each face.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppTexCoordPoints was NULL.
	*  @retval IFX_E_INVALID_RANGE   The PointSet does not contain this attribute.
	*      NumTexCoords in the IFXPoint is equal to zero(0).
	*/
	virtual	IFXRESULT IFXAPI GetTexCoordsPoints (U32** ppTexCoordPoints) = 0;

	/**
	*  Returns the pointer to the PointSet Materials index array 
	*
	*  @param ppMaterialPoints  The array of IFXPoint specifying
	*     the indices of the meterials used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppMaterialPoints was NULL.
	*  @retval IFX_E_INVALID_RANGE   The PointSet does not contain this attribute.
	*      NumMaterials in the IFXPoint is equal to zero(0).
	*/
	virtual	IFXRESULT IFXAPI GetMaterialsPoints (U32** ppMaterialPoints) = 0; 

	/**
	*  Retrieves a specific Position face index. 
	*
	*  @param index  The index of the position Point to return
	*  @param pPositionPoint  The returned Point containing position indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pPositionPoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position Point segm in the Pointset.
	*/
	virtual IFXRESULT IFXAPI GetPositionPoint(U32 index, U32* pPositionPoint) = 0;

	/**
	*  Sets a specific Position Point index.
	*
	*  @param index  The index of the position face to set
	*  @param pPositionPoint  The position Point containing the position 
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pPositionLien was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position faces in the Pointset.
	*/
	virtual IFXRESULT IFXAPI SetPositionPoint(U32 index, const U32* pPositionPoint)	= 0;

	/**
	*  Retrieves a specific Normal Point index. 
	*
	*  @param index  The index of the normal Point to return
	*  @param pNormalPoint  The returned face containing normal indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pNormalPoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal Point in the Pointset.
	*/	
	virtual IFXRESULT IFXAPI GetNormalPoint(U32 index, U32* pNormalPoint) = 0;
	
	/**
	*  Set a specific Normal Point index.
	*
	*  @param index  The index of the normal face to return
	*  @param pNormalPoint  The normal Point containing the normal 
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pNormalPoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal Point in the PointSet.
	*/
	virtual IFXRESULT IFXAPI SetNormalPoint(U32 index, const U32* pNormalPoint)	= 0;

	/**
	*  Retrieves a specific Diffuse vertex color Point index. 
    *
	*  @param index  The index of the diffuse Point to return
	*  @param pDiffuseFace  The diffuse face containing the diffuse vertex
	*                           color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pDiffusePoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color Point in the PointSet.
	*/
	virtual IFXRESULT IFXAPI GetDiffusePoint(U32 index,	U32* pDiffusePoint)	= 0;

	/**
	*  Set a specific Diffuse Point index.
    *
	*  @param index  The index of the diffuse Point to set
	*  @param pDiffuseFace  The diffuse Point containing the diffuse vertex
	*                           color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pDiffusePoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color Point in the PointSet.
	*
	*/
	virtual IFXRESULT IFXAPI SetDiffusePoint(U32 index,	const U32* pDiffusePoint) = 0;
	
	/**
	*  Retrieves a specific Specular face index. 
	*
	*  @param index  The index of the specular Point to return
	*  @param pSpecularFace  The specular Point containing the specular 
	*                           vertex color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pSpecularPoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color Point in the PointSet.
	*/
	virtual IFXRESULT IFXAPI GetSpecularPoint(U32 index, U32* pSpecularPoint) = 0;

	/**
	*  Set a specific Specular Point index.
    *
	*  @param index  The index of the specular Point to set
	*  @param pSpecularFace  The specular face containing the specular 
	*                           vertex color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pSpecularPoint was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color Point in the PointSet.
	*/
	virtual IFXRESULT IFXAPI SetSpecularPoint(U32 index, const U32* pSpecularPoint)	= 0;

	/** 
	*  Returns the pointer to the Material Definition Array. 
	*
	*  @param  ppMaterials  The pointer to the material definition array
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER   ppMaterials was NULL.
	*/
	virtual IFXRESULT IFXAPI GetMaterials(IFXAuthorMaterial** ppMaterials)= 0;

	/**
	*  Returns the pointer to the Point Material index array. 
	*
	*  @param ppPointMaterials  The array of U32es specifying
	*     the material index to use on each face of the Pointset
	*	
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER ppFaceMaterials was NULL.
	*  @retval IFX_E_INVALID_RANGE   The Pointset does not contain this attribute.
	*      NumMaterials in the IFXAuthorPointDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetPointMaterials(U32** ppPointMaterials)= 0;

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
	virtual IFXRESULT IFXAPI GetMaterial(	U32 index, 
									IFXAuthorMaterial* pMaterial)	= 0;

	/**
	*  Sets a specific material Point.
	*
	*  @param index  The index of the material to set
	*  @param pMaterial  A pointer to the material to assign to this index.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER pMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number of
	*      materials used by the Pointset.
	*/
	virtual IFXRESULT IFXAPI SetMaterial(	U32 index, 
									const IFXAuthorMaterial* pMaterial) = 0;		

	virtual IFXRESULT IFXAPI GetTexPoints(U32, U32**) = 0;
	virtual IFXRESULT IFXAPI GetPositions (IFXVector3** ) = 0;
	virtual IFXRESULT IFXAPI GetNormals(IFXVector3** ) = 0 ;
	virtual IFXRESULT IFXAPI GetTexCoords(IFXVector4** ) = 0;
	virtual IFXRESULT IFXAPI GetDiffuseColors(IFXVector4**) = 0;
	virtual IFXRESULT IFXAPI GetSpecularColors(IFXVector4** ) = 0;

	/**
	*  Retrieves a specific vertex position from the Point Set.
	*
	*  @param index   The index of the vertex position to return
	*  @param pVector3  The position of the specified vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       vertex positions stored in the Point set.
	*/
	virtual IFXRESULT IFXAPI GetPosition(U32 index,IFXVector3* pVector3)= 0;

	/**
	*  Sets a specific vertex position for this Pointset.
	*
	*  @param index   The index of the vertex position to set
	*  @param pVector3  The new position value for this vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       vertex positions stored in the Pointset.
	*/
	virtual IFXRESULT IFXAPI SetPosition(U32 index,const IFXVector3* pVector3)= 0;

	/**
	*  Retrieves a specific Normal from this Pointset.
	*
	*  @param index   The index of the normal to return
	*  @param pVector3  The requested normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       normals stored in the Pointset.
	*/
	virtual IFXRESULT IFXAPI GetNormal(U32 index, IFXVector3* pVector3)= 0;

	/**
	*  Sets a specific Normal for this Pointset.
	*
	*  @param index   The index of the normal to set
	*  @param pVector3  The new value of the normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       normals stored in the Pointset.
	*/
	virtual IFXRESULT IFXAPI SetNormal(U32 index, const IFXVector3* pVector3)= 0;

	/**
	*  Retrieves a specific Diffuse vertex color from this Pointset.
	*
	*  @param index   The index of the Diffuse vertex color to return
	*  @param pColor  The requested color
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       diffuse vertex colors stored in the Pointset.
	*
	*  @note  Vertex colors are stored in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetDiffuseColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Sets a specific Diffuse vertex color for this Pointset.
	*
	*  @param index   The index of the diffuse vertex color to set
	*  @param pColor  The new diffuse vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       diffuse vertex colors stored in the Pointset.
	*
	*  @note  Vertex colors are stored in BGRA order.
	*/
	virtual IFXRESULT IFXAPI SetDiffuseColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Retrieves a specific Specular vertex color from this Pointset.
	*
	*  @param index   The index of the specular vertex color to return
	*  @param pColor  The requested color value
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       specular vertex colors stored in the Pointset.
	*
	*  @note  Vertex colors are stored in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetSpecularColor(U32 index, IFXVector4* pColor)= 0;

	/**
	*  Sets a specific Specular vertex color of this Pointset.
	*
	*  @param index   The index of the specular vertex color to set
	*  @param pColor  The new specular vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       specular vertex colors stored in the Pointset.
	*
	*  @note  Vertex colors are stored in BGRA order.
	*/
	virtual IFXRESULT IFXAPI SetSpecularColor(U32 index, IFXVector4* pColor)= 0;
	
	/**
	*  Retrieves a specific Texture Coordinate from this Pointset.
	*
	*  @param index   The index of the texture coordinate to return
	*  @param pVector4  The requested texture coordinate 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       texture coordinates stored in the Pointset.
	*/
	virtual IFXRESULT IFXAPI GetTexCoord(U32 index, IFXVector4* pVector4) = 0;

	/**
	*  Sets specific Texture Coordinate for this Pointset.
	*
	*  @param index   The index of the texture coordinate to set
	*  @param pVector4  The new texture coordinate value to store 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of 
	*       texture coordinates stored in the Pointset.
	*/
	virtual IFXRESULT IFXAPI SetTexCoord(U32 index, const IFXVector4* pVector4)= 0;

	/**
	*  Retrieves a specific Material Point ID. 
	*
	*  @param index   The index of the Point for which we want the material ID
	*  @param pPointMaterial  The returned material ID.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  puFaceMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               faces in the PointSet.
	*/
	virtual IFXRESULT IFXAPI GetPointMaterial(U32 index, U32* pPointMaterial)= 0;

	/**
	*  Sets a specific Point Material ID
	*
	*  @param index   The index of the face on which we want to set the
	*           material ID.
	*  @param pointMaterialID  The material ID to set on the face.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               faces in the PointSet.
	*/
	virtual IFXRESULT IFXAPI SetPointMaterial(U32 index, U32 pointMaterialID)= 0;


 /**
	*  Retrieves a specific Texture Point index. 
	*
	*  @param layer  The texture coordinate layer this Point is from
	*  @param index  The index of the texture face to return
	*  @param pTextureLine  The texture Point containing the texture 
	*                           coordinate indices
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate Points in the Point set, the Point set has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/	
	virtual IFXRESULT IFXAPI GetTexPoint(U32 layer, U32 index, U32* pTexturePoint) = 0;

	/**
	*  Sets a specific Texture Point index.
	*
	*  @param layer  The texture coordinate layer into which to
	*               place the Point
	*  @param index  The index of the texture face to set
	*  @param pTextureLine  The texture Point containing the texture 
	*                           coordinate indices to set in the Point set
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate faces in the Point set, the Point set has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/
	virtual IFXRESULT IFXAPI SetTexPoint(
								U32 layer, U32 index, const U32* pTexturePoint) = 0;

	/**
		Calculates bounding sphere.
	*/
	virtual IFXVector4 IFXAPI CalcBoundSphere()=0;
};

#endif 
