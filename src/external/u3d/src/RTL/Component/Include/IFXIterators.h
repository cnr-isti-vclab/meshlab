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
	@file	IFXIterators.h
*/

#ifndef IFX_ITERATORS_H
#define IFX_ITERATORS_H

#include "IFXEnums.h"
#include "IFXVertexAttributes.h"
#include "IFXFace.h"
#include "IFXLine.h"
#include "IFXVector2.h"
#include "IFXVector3.h"
#include "IFXVector4.h"

/**
 *  This is the abstract base class for all iterators.  There is nothing more
 *  to it at this point than a pointer to a data buffer and a stride between
 *  data elements in the buffer.
 */
class IFXIterator
{
public:
  /**
   *  Constructor.
   *
   *  @param  uDataSize The size in bytes of the data elements
   *            iterated through.
   */
  IFXIterator(U32 uDataSize=0)
  {
    m_pBaseData = 0;
    m_pData = 0;
    m_uStride = 0;
    m_uDataSize = uDataSize;
  }

  /**
   *  Sets the stride in bytes between sequential data elements.
   *
   *  @param  uStride Distance in bytes between data elements.
   */
  void SetStride(U32 uStride)
  {
    m_uStride = uStride;
  }

  /**
   *  Sets the data buffer pointer.  This will a buffer of data
   *  to iterate over.  This original address is remembered for calls to
   *  PointAt().  The current data pointer is set to this address as well.
   *
   *  @param  pData Pointer to the beginning of a buffer of data to
   *          iterate through.
   */
  void SetData(U8* pData)
  {
    m_pBaseData = m_pData = pData;
  }

  /**
   *  Gets the stride in bytes between data elements.
   *
   *  @return The data stride in bytes.
   */
  U32 GetStride() const { return m_uStride; }

  /**
   *  Gets the size of the data elements iterated over.
   *
   *  @return The size in bytes of the data elements we are iterating through.
   */
  U32 GetDataSize() const { return m_uDataSize; }

  /**
   *  Configures this iterator to point to the same data as @a pSrcIter.
   *  @param  pSrcIter  Pointer to another Iterator instance that we
   *            will copy.
   */
  void Clone(const IFXIterator* pSrcIter) { *this = *pSrcIter; }

  /**
   *  Points at the data element @ uIndex elements away from the
   *  address set by SetData().
   *
   *  @param  uIndex  The index of the data element (relative to the
   *          address specified with SetData()) to point to.
   */
  void PointAt(U32 uIndex) { m_pData = m_pBaseData + uIndex*m_uStride; }

protected:
  U8* m_pBaseData;  ///< Data buffer start
  U8* m_pData;    ///< Current location in the data buffer.
  U32 m_uStride;    ///< Stride between data elements.
  U32 m_uDataSize;  ///< Size of data elements.
};

/**
 *  Generic implementation of IFXIterator that can be
 *  specialized for specific data types.
 */
template <class T>
class IFXIteratorT : virtual public IFXIterator
{
public:
  /// Constructor
  IFXIteratorT() : IFXIterator((U32)sizeof(T)) {}

  /**
   *  Gets a pointer to the data at the current location.
   *
   *  @return A Pointer to the data at the current location.
   */
  T* Get() { return (T*)m_pData; }

  /**
   *  Gets a pointer to the data element @a uIndex positions ahead
   *  of the current location.
   *
   *  @param  uIndex  The index relative to the current location
   *          to fetch the address of.
   *
   *  @return The address of the data element requested.
   */
  T* Index(U32 uIndex) const { return (T*)(m_pData + (uIndex*m_uStride)); }

  /**
   *  Retrieves the address of the current data element, then increments
   *  the current data pointer to the next data element.
   *
   *  @return The address of the current data element.
   */
  T* Next() { T* pRet = (T*)m_pData; m_pData += m_uStride; return pRet; }
};

/// Specialization of IFXIteratorT for F32 values.
typedef IFXIteratorT<F32>    IFXFloatIter;
/// Specialization of IFXIteratorT for F32 values.
typedef IFXIteratorT<F32>    IFXF32Iter;
/// Specialization of IFXIteratorT for IFXVector2 values.
typedef IFXIteratorT<IFXVector2> IFXVector2Iter;
/// Specialization of IFXIteratorT for IFXVector3 values.
typedef IFXIteratorT<IFXVector3> IFXVector3Iter;
/// Specialization of IFXIteratorT for IFXVector4 values.
typedef IFXIteratorT<IFXVector4> IFXVector4Iter;
/// Specialization of IFXIteratorT for U32 values.
typedef IFXIteratorT<U32>    IFXU32Iter;
/// Specialization of IFXIteratorT for U16 values.
typedef IFXIteratorT<U32>    IFXU16Iter;
/// Specialization of IFXIteratorT for IFXU16Face values.
typedef IFXIteratorT<IFXU16Face> IFXU16FaceIter;
/// Specialization of IFXIteratorT for IFXU32Face values.
typedef IFXIteratorT<IFXU32Face> IFXU32FaceIter;
/// Specialization of IFXIteratorT for IFXU16Line values.
typedef IFXIteratorT<IFXU16Line> IFXU16LineIter;
/// Specialization of IFXIteratorT for IFXU32Line values.
typedef IFXIteratorT<IFXU32Line> IFXU32LineIter;


/**
 *  Implementation of IFXIterator to iterate over the polymorphic
 *  IFXFace data type.  Because IFXFace is itself a pointer to
 *  to the actual data, the generic implementation (IFXIteratorT)
 *  will not work.
 */
class IFXFaceIter : virtual public IFXIterator
{
public:
  /// Constructor
  IFXFaceIter() : IFXIterator((U32)sizeof(IFXU32Face))
  {
    m_pFace =0;
    Set32Bit(TRUE);
  }

  /// Destructor
  ~IFXFaceIter()
  {
    IFXDELETE(m_pFace);
  }

  /**
   *  Sets the size of the indices iterated over.
   *
   *  @param  b32 If @b TRUE, the indices that make up the
   *        face are 32 bit.  If @b FALSE, the indices
   *        are 16 bit.
   */
  void Set32Bit(BOOL b32)
  {
    IFXDELETE(m_pFace);
    if(b32)
    {
      m_pFace = new IFXGenFaceU32;
      m_uDataSize = (U32)sizeof(IFXU32Face);
    }
    else
    {
      m_pFace = new IFXGenFaceU16;
      m_uDataSize = (U32)sizeof(IFXU16Face);
    }
  }

  /**
   *  Returns the address of an IFXFace object that is pointing at
   *  the current data element.
   *
   *  @return The address of an IFXFace object.
   */
  IFXFace* Get()
  {
    m_pFace->SetDataPtr(m_pData);
    return m_pFace;
  }

  /**
   *  Returns the address of an IFXFace object that is pointing
   *  at the face @a uIndex positions ahead of the current
   *  face.
   *
   *  @param  uIndex  The index of the desired face data element
   *          relative to the current data pointer.
   *
   *  @return The address of an IFXFace object pointing to the
   *      desired face data element.
   */
  IFXFace* Index(U32 uIndex)
  {
    m_pFace->SetDataPtr(m_pData + (uIndex*m_uStride));
    return m_pFace;
  }

  /**
   *  Retrieves the current face, increment the data pointer to point
   *  to the next face data element.
   *
   *  @return The address of an IFXFace object that points to the current
   *      (before incrementing) face data element.
   */
  IFXFace* Next()
  {
    m_pFace->SetDataPtr(m_pData);
    m_pData += m_uStride;
    return m_pFace;
  }

  /**
   *  Retrieves a direct pointer to the face element at @u uIndex
   *  data elements ahead of the current data pointer.
   *
   *  @return A pointer to the raw face index data buffer.
   */
  U8* IndexPtr(U32 uIndex)
  {
    return m_pData = m_pBaseData + (uIndex * m_uStride);
  }

  /**
   *  Retrieves a direct face data element pointer.
   *
   *  @return A pointer to the raw face index data buffer.
   */
  U8* GetPtr()
  {
    return m_pData;
  }

  /**
   *  Implemention of the '=' operator that takes the
   *  pointer data members into account.
   */
  IFXFaceIter& operator=(const IFXFaceIter& operand)
  {
    if(operand.m_uDataSize == sizeof(IFXU32Face))
    {
      Set32Bit(TRUE);
    }
    else
    {
      Set32Bit(FALSE);
    }
    m_pFace->Clone(*operand.m_pFace);
    m_pData = operand.m_pData;
    m_uStride = operand.m_uStride;
    m_uDataSize = operand.m_uDataSize;

    return *this;
  }

protected:
  IFXFace*  m_pFace;
};





/**
 *  Implementation of IFXIterator to iterate over the polymorphic
 *  IFXLine data type.  Because IFXLine is itself a pointer to
 *  to the actual data, the generic implementation (IFXIteratorT)
 *  will not work.
 */
class IFXLineIter : virtual public IFXIterator
{
public:
  /// Constructor
  IFXLineIter() : IFXIterator((U32)sizeof(IFXU32Line))
  {
    m_pLine =0;
    Set32Bit(TRUE);
  }

  /// Destructor
  ~IFXLineIter()
  {
    IFXDELETE(m_pLine);
  }

  /**
   *  Sets the size of the indices iterated over.
   *
   *  @param  b32 If @b TRUE, the indices that make up the
   *        line are 32 bit.  If @b FALSE, the indices
   *        are 16 bit.
   */
  void Set32Bit(BOOL b32)
  {
    IFXDELETE(m_pLine);
    if(b32)
    {
      m_pLine = new IFXGenLineU32;
      m_uDataSize = (U32)sizeof(IFXU32Line);
    }
    else
    {
      m_pLine = new IFXGenLineU16;
      m_uDataSize = (U32)sizeof(IFXU16Line);
    }
  }

  /**
   *  Returns the address of an IFXLine object that is pointing at
   *  the current data element.
   *
   *  @return The address of an IFXLine object.
   */
  IFXLine* Get()
  {
    m_pLine->SetDataPtr(m_pData);
    return m_pLine;
  }

  /**
   *  Returns the address of an IFXLine object that is pointing
   *  at the line @a uIndex positions ahead of the current
   *  line.
   *
   *  @param  uIndex  The index of the desired line data element
   *          relative to the current data pointer.
   *
   *  @return The address of an IFXLine object pointing to the
   *      desired face data element.
   */
  IFXLine* Index(U32 uIndex)
  {
    m_pLine->SetDataPtr(m_pData + (uIndex*m_uStride));
    return m_pLine;
  }

  /**
   *  Retrieves the current line, increments the data pointer to point
   *  to the next line data element.
   *
   *  @return The address of an IFXLine object that points to the current
   *      (before incrementing) line data element.
   */
  IFXLine* Next()
  {
    m_pLine->SetDataPtr(m_pData);
    m_pData += m_uStride;
    return m_pLine;
  }

  /**
   *  Retrieves a direct pointer to the line element at @u uIndex
   *  data elements ahead of the current data pointer.
   *
   *  @return A pointer to the raw line index data buffer.
   */
  U8* IndexPtr(U32 uIndex)
  {
    return m_pData = m_pBaseData + (uIndex * m_uStride);
  }

  /**
   *  Retrieves a direct line data element pointer.
   *
   *  @return A pointer to the raw line index data buffer.
   */
  U8* GetPtr()
  {
    return m_pData;
  }

  /**
   *  Implemention of the '=' operator that takes the
   *  pointer data members into account.
   */
  IFXLineIter& operator=(const IFXLineIter& operand)
  {
    if(operand.m_uDataSize == (U32)sizeof(IFXU32Line))
    {
      Set32Bit(TRUE);
    }
    else
    {
      Set32Bit(FALSE);
    }
    m_pLine->Clone(*operand.m_pLine);
    m_pData = operand.m_pData;
    m_uStride = operand.m_uStride;
    m_uDataSize = operand.m_uDataSize;

    return *this;
  }

protected:
  IFXLine*  m_pLine;
};



/**
 *  Implementation of IFXIterator that can be used to iterate over
 *  parallel arrays of vertex data simultaneously.  Since this is
 *  not iterating over a single simple data type, the generic
 *  (IFXIteratorT) implementation will not work.
 */
 class IFXVertexIter : virtual public IFXIterator
{
public:
  /// Constructor
  IFXVertexIter() : IFXIterator(0) {}

  /**
   *  Gets a pointer to the start of the current vertex.  When vertex
   *  data is pooled, this is typically a pointer to the position of
   *  the vertex.
   *
   *  @return A void* pointer to the beginning of the current vertex
   *      data.
   */
  void* Get();

  /**
   *  Retrieves a raw data pointer to the beginning of the vertex
   *  that is @a uIndex vertices ahead of the current vertex.  When vertex
   *  data is pooled, this is typically a pointer to the position of
   *  the vertex.
   *
   *  @param  uIndex  The index of the desired vertex data relative to
   *          the current vertex.
   *
   *  @return A void* pointer to the beginning of the desired vertex
   *      data.
   */
  void* Index(U32 uIndex) const;

  /**
   *  Retreives a raw pointer to the beginning of the current vertex and
   *  increment all internal pointers to the next vertex element.
   *
   *  @return A void* pointer to the beginning of the current vertex
   *      data.
   */
  void* Next();

  /**
   *  Sets all internal data pointers to point at the vertex @a uIndex
   *  elements ahead of the start of the data buffer.
   *
   *  @param  uIndex  The index of the vertex relative to the start
   *          of the data buffer that we are interested in.
   */
  void PointAt(U32 uIndex);

  /**
   *  Tells the iterator what data is present in the vertex data.
   *
   *  @param  vaAttribus  The IFXVertexAttributes instance that describes
   *            what data is in each vertex.
   */
  void SetVertexAttributes(IFXVertexAttributes vaAttribs);

  /**
   *  Sets the beginning data pointer for the vertex data specified
   *  by @a eMeshAttribute.
   *
   *  @param  eMeshAttribute  An IFXenum descriptor for what vertex data
   *              @a pData will point to.  Valid values are:
   *              - @b IFX_MESH_POSITION
   *              - @b IFX_MESH_NORMAL
   *              - @b IFX_MESH_DIFFUSE_COLOR
   *              - @b IFX_MESH_SPECULAR_COLOR
   *              - @b IFX_MESH_TC0
   *              - @b IFX_MESH_TC1
   *              - @b IFX_MESH_TC2
   *              - @b IFX_MESH_TC3
   *              - @b IFX_MESH_TC4
   *              - @b IFX_MESH_TC5
   *              - @b IFX_MESH_TC6
   *              - @b IFX_MESH_TC7
   *  @param  pData Data pointer to the vertex data specified by @a eMeshAttribute.
   */
  void SetDataPtr(IFXenum eMeshAttribute, U8* pData);

  /**
   *  Sets the stride in bytes between vertex data elements in the data buffer specified
   *  by @a eMeshAttribute.
   *
   *  @param  eMeshAttribute  An IFXenum descriptor for which vertex data buffer
   *              @a uStride refers to.  Valid values are:
   *              - @b IFX_MESH_POSITION
   *              - @b IFX_MESH_NORMAL
   *              - @b IFX_MESH_DIFFUSE_COLOR
   *              - @b IFX_MESH_SPECULAR_COLOR
   *              - @b IFX_MESH_TC0
   *              - @b IFX_MESH_TC1
   *              - @b IFX_MESH_TC2
   *              - @b IFX_MESH_TC3
   *              - @b IFX_MESH_TC4
   *              - @b IFX_MESH_TC5
   *              - @b IFX_MESH_TC6
   *              - @b IFX_MESH_TC7
   *  @param  uStride Number of bytes between data elements in the specified
   *          data buffer.
   */
  void SetDataStride(IFXenum eMeshAttribute, U32 uStride);

  /**
   *  Gets the stride between elements of the specified data buffer.
   *
   *  @param  eMeshAttribute  Which vertex data buffer to receive the stride
   *              from.  Valid values are:
   *              - @b IFX_MESH_POSITION
   *              - @b IFX_MESH_NORMAL
   *              - @b IFX_MESH_DIFFUSE_COLOR
   *              - @b IFX_MESH_SPECULAR_COLOR
   *              - @b IFX_MESH_TC0
   *              - @b IFX_MESH_TC1
   *              - @b IFX_MESH_TC2
   *              - @b IFX_MESH_TC3
   *              - @b IFX_MESH_TC4
   *              - @b IFX_MESH_TC5
   *              - @b IFX_MESH_TC6
   *              - @b IFX_MESH_TC7
   *
   *  @return The stride in bytes between data elements for the specified
   *      vertex attribute.
   */
  U32  GetDataStride(IFXenum eMeshAttribute)
  {
    return m_puStride[eMeshAttribute];
  }

  /**
   *  Gets the vertex attribute descriptor for the data we are iterating over.
   *
   *  @return An IFXVertexAttributes object specifying what data is in each vertex.
   */
  IFXVertexAttributes GetAttributes() const { return m_vaAttribs; }

  /**
   *  Gets the number of texture coordinate sets are present in each vertex.
   *  @return The number of texture coordinate layers present in the vertex data.
   */
  U32 GetNumTextureLayers() const { return m_vaAttribs.m_uData.m_uNumTexCoordLayers; }

  /**
   *  Gets the vertex position of the current vertex.
   *
   *  @return A pointer to the vertex position of the current vertex.
   */
  IFXVector3* GetPosition() const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppData[IFX_MESH_POSITION]);
  }

  /**
   *  Gets the vertex position of a vertex @a uIndex vertices ahead of the
   *  current vertex.
   *
   *  @param  uIndex  The index the vertex relative to the current vertex
   *          for which to retreive the position.
   *  @return A pointer to the vertex position of the specified vertex.
   */
  IFXVector3* GetIndexPosition(U32 uIndex) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppBaseData[IFX_MESH_POSITION] + uIndex * m_puStride[IFX_MESH_POSITION]);
  }

  /**
   *  Gets the vertex normal of the current vertex.
   *
   *  @return A pointer to the vertex normal of the current vertex.
   */
  IFXVector3* GetNormal() const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasNormals,
      "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppData[IFX_MESH_NORMAL]);
  }

  /**
   *  Gets the vertex normal of a vertex @a uIndex vertices ahead of the
   *  current vertex.
   *
   *  @param  uIndex  The index the vertex relative to the current vertex
   *          for which to retreive the normal.
   *  @return A pointer to the vertex normal of the specified vertex.
   */
  IFXVector3* GetIndexNormal(U32 uIndex) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasNormals,
      "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppBaseData[IFX_MESH_NORMAL] + uIndex * m_puStride[IFX_MESH_NORMAL]);
  }

  /**
   *  Gets the vertex normal of the current vertex.
   *
   *  @param  uColorLayer Which color to retrieve: 0 = diffuse, 1 = specular.
   *
   *  @return A pointer to the vertex color of the current vertex.
   */
  U32* GetColor(U32 uColorLayer=0) const
  {
    IFXASSERTBOX((m_vaAttribs.m_uData.m_bHasDiffuseColors) ||
      (m_vaAttribs.m_uData.m_bHasSpecularColors && uColorLayer),
      "Accessing non-valid vertex data!");
    IFXASSERTBOX(uColorLayer < 2, "Accessing non-valid vertex data!");

    return (U32*)(m_ppData[IFX_MESH_DIFFUSE_COLOR + uColorLayer]);
  }

  /**
   *  Gets the vertex color of a vertex @a uIndex vertices ahead of the
   *  current vertex.
   *
   *  @param  uIndex  The index the vertex relative to the current vertex
   *          for which to retrieve the color.
   *  @param  uColorLayer Which color to retrieve: 0 = diffuse, 1 = specular.
   *
   *  @return A pointer to the vertex color of the specified vertex.
   */
  IFXVector3* GetIndexColor(U32 uIndex, U32 uColorLayer=0) const
  {
    IFXASSERTBOX((m_vaAttribs.m_uData.m_bHasDiffuseColors) ||
      (m_vaAttribs.m_uData.m_bHasSpecularColors && uColorLayer),
      "Accessing non-valid vertex data!");
    IFXASSERTBOX(uColorLayer < 2, "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppBaseData[IFX_MESH_DIFFUSE_COLOR + uColorLayer]
               + uIndex * m_puStride[IFX_MESH_DIFFUSE_COLOR + uColorLayer]);
  }

  /**
   *  Gets the 1D vertex texture coordinate of the current vertex.
   *
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 1D vertex texcoord of the current vertex.
   */
  F32* GetTexCoordV1(U32 uLayer = 0) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");
    IFXASSERTBOX( ValidTCAccess(1,m_vaAttribs,uLayer),
      "Accessing non-valid vertex data!");

    return (F32*)(m_ppData[IFX_MESH_TC0 + uLayer]);
  }

  /**
   *  Gets the 2D vertex texture coordinate of the current vertex.
   *
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 2D vertex texcoord of the current vertex.
   */
  IFXVector2* GetTexCoordV2(U32 uLayer = 0) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");
    IFXASSERTBOX( ValidTCAccess(2,m_vaAttribs,uLayer),
      "Accessing non-valid vertex data!");

    return (IFXVector2*)(m_ppData[IFX_MESH_TC0 + uLayer]);
  }

  /**
   *  Gets the 3D vertex texture coordinate of the current vertex.
   *
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 3D vertex texcoord of the current vertex.
   */
  IFXVector3* GetTexCoordV3(U32 uLayer = 0) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");
    IFXASSERTBOX( ValidTCAccess(3,m_vaAttribs,uLayer),
      "Accessing non-valid vertex data!");

    return (IFXVector3*)(m_ppData[IFX_MESH_TC0 + uLayer]);
  }

  /**
   *  Gets the 4D vertex texture coordinate of the current vertex.
   *
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 4D vertex texcoord of the current vertex.
   */
  IFXVector4* GetTexCoordV4(U32 uLayer = 0) const
  {
    IFXASSERTBOX( m_vaAttribs.m_uData.m_bHasPositions,
      "Accessing non-valid vertex data!");
    IFXASSERTBOX( ValidTCAccess(4,m_vaAttribs,uLayer),
      "Accessing non-valid vertex data!");

    return (IFXVector4*)(m_ppData[IFX_MESH_TC0 + uLayer]);
  }

  /**
   *  Gets the 2D vertex texture coordinate of the current vertex.
   *
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 2D vertex texcoord of the current vertex.
   */
  IFXVector2* GetTexCoord(U32 uLayer = 0) const
  {
    return GetTexCoordV2(uLayer);
  }

  /**
   *  Gets the 2D vertex texture coordinate of the vertex that lies
   *  @a uIndex vertices ahead of the current vertex.
   *
   *  @param  uIndex  The index of the desired vertex relative to the
   *          current vertex.
   *  @param  uLayer  Which texture coordinate set to retrieve from.  This
   *          must be less than the value returned from
   *          GetNumTextureLayers().
   *
   *  @return A pointer to the 2D vertex texcoord of the desired vertex.
   */
  IFXVector2* GetIndexTexCoord(U32 uIndex, U32 uLayer = 0) const
  {
    return (IFXVector2*)(m_ppBaseData[IFX_MESH_TC0 + uLayer] +
               uIndex * m_puStride[IFX_MESH_TC0 + uLayer]);
  }

  /**
   *  Makes this iterator identical in state to pIter.
   *
   *  @param  pIter Another IFXVertexIter to copy state from.
   */
  void Clone(IFXVertexIter* pIter)
  {
    *this = *pIter;
  }

protected:
  IFXVertexAttributes m_vaAttribs;

  U8* m_ppBaseData[IFX_MESH_FACE];
  U8* m_ppData[IFX_MESH_FACE];
  U32 m_puStride[IFX_MESH_FACE];
};

IFXINLINE void IFXVertexIter::SetDataPtr(IFXenum eMeshAttribute, U8* pData)
{
	IFXASSERTBOX(eMeshAttribute < IFX_MESH_FACE, "Invalid mesh attribute");
	if(eMeshAttribute < IFX_MESH_FACE)
	{
		m_ppBaseData[eMeshAttribute] = pData;
		m_ppData[eMeshAttribute] = pData;
	}
}

IFXINLINE void IFXVertexIter::SetDataStride(IFXenum eMeshAttribute, U32 uStride)
{
	IFXASSERTBOX(eMeshAttribute < IFX_MESH_FACE, "Invalid mesh attribute");
	if(eMeshAttribute < IFX_MESH_FACE)
	{
		m_puStride[eMeshAttribute] = uStride;
	}
}

IFXINLINE void IFXVertexIter::SetVertexAttributes(IFXVertexAttributes vaAttribs)
{
	m_vaAttribs = vaAttribs;
	U32 uStride = 0;

	if(m_vaAttribs.m_uData.m_bHasPositions)
	{
		uStride += (U32)sizeof(IFXVector3);
	}

	if(m_vaAttribs.m_uData.m_bHasNormals)
	{
		uStride += (U32)sizeof(IFXVector3);
	}

	if(m_vaAttribs.m_uData.m_bHasDiffuseColors)
	{
		uStride += (U32)sizeof(U32);
	}

	if(m_vaAttribs.m_uData.m_bHasSpecularColors)
	{
		uStride += (U32)sizeof(U32);
	}

	U32 i;
	for(i = 0; i < m_vaAttribs.m_uData.m_uNumTexCoordLayers; i++)
	{
		uStride += m_vaAttribs.GetTexCoordSize(i) * (U32)sizeof(F32);
	}

	SetStride(uStride);
}

IFXINLINE void* IFXVertexIter::Get()
{
	return (void*)m_ppData[IFX_MESH_POSITION];
}

IFXINLINE void* IFXVertexIter::Index(U32 uIndex) const
{
	return (void*)(m_ppData[IFX_MESH_POSITION] + uIndex*m_puStride[IFX_MESH_POSITION]);
}

IFXINLINE void* IFXVertexIter::Next()
{
	void* pRet = m_ppData[IFX_MESH_POSITION];

	U32 i;
	for(i = 0; i < IFX_MESH_FACE; i++)
	{
		m_ppData[i] += m_puStride[i];
	}

	return pRet;
}

IFXINLINE void IFXVertexIter::PointAt(U32 uIndex)
{
	U32 i;
	for(i=0; i < IFX_MESH_FACE; i++)
	{
		m_ppData[i] = m_ppBaseData[i] + uIndex*m_puStride[i];
	}
}

#endif
