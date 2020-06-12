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
// IFXInterleavedData.h
#ifndef IFX_INTERLEAVED_DATA_H
#define IFX_INTERLEAVED_DATA_H

#include "IFXUnknown.h"
#include "IFXIterators.h"

IFXDEFINE_GUID(IID_IFXInterleavedData,
0x28d9d676, 0x9ad, 0x4415, 0x80, 0x7, 0xf8, 0x5e, 0xc5, 0xc6, 0x65, 0xc);

/**
 *  The IFXInterleavedData objects represents an interleaved array of
 *  separate data elements.  This object can keep track of what data
 *  elements are in the interleaved array, and provide iterators over
 *  specific data types within the interleaved array.  The array
 *  contains a number of vertices, with each vertex containing a
 *  number of interleaved vectors.
 */
class IFXInterleavedData : virtual public IFXUnknown
{
public:
  /**
   *  Allocates memory for the array.  The useable allocated memory
   *  begins on the start of a cache line.  The array consists
   *  of a number of vertices.  Each vertex has a number of
   *  vectors that comprise the vertex.
   *
   *  @param  uNumVectors Input U32 specifying the number of separate
   *            vectors that make up each vertex.
   *  @param  puVectorSizes Input array of U32 values.  Each entry
   *              in the array specifies the size in bytes
   *              of that corresponding vector entry in each
   *              vertex.  The array @b MUST be at least
   *              @a uNumVectors in length, otherwise a
   *              memory corruption error can occur.
   *  @param  uNumVertices  Input U32 specifying how many "vertices"
   *              the interleaved array should contain.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_OUT_OF_MEMORY There was a memory allocation error.
   *  @retval IFX_E_INVALID_POINTER The value specified for
   *                  @a puVectorSizes was invalid (NULL).
   *  @retval IFX_E_INVALID_RANGE The value specified for @uNumVectors was 0.
   */
  virtual IFXRESULT IFXAPI Allocate(  U32 uNumVectors,
                U32* puVectorSizes,
                U32 uNumVertices)=0;

  /**
   *  Configures an IFXIterator to iterate over the @a uVectorNum vector
   *  within each vertex.
   *
   *  @param  uVectorNum  Input U32 specifying which vector in each vertex
   *            to iterate over.
   *
   *  @param  iter  Reference to an IFXIterator that will be configured
   *          to iterate over the specified vector.
   *
   *  @return An IFXRESULT
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE Either @a uVectorNum was not a valid value,
   *                or @a iter is of a type that iterates over data
   *                larger than the vector to be iterated over.
   */
  virtual IFXRESULT IFXAPI GetVectorIter(U32 uVectorNum, IFXIterator& iter)=0;

  /**
   *  Configures an IFXIterator to iterate over all vertex data (treated as
   *  on large vector).
   *
   *  @param  iter  Reference to an IFXIterator that will be configured
   *          to iterate over the vertex data.
   *
   *  @return An IFXRESULT
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a iter is of a type that iterates over data
   *                larger than the total vertex size.
   */
  virtual IFXRESULT IFXAPI GetVertexIter(IFXIterator& iter)=0;

  /**
   *  Gets the version word of a specific vector within the interleaved array.
   *  Version words start at random values and are incremented with
   *  UpdateVersionWord().
   *
   *  @param  uVectorNum  Input U32 specifying which vector to retrieve the
   *            version word for.  If this value is invalid, the
   *            method returns 0.
   *
   *  @return A U32 value containing the version word for the specified vector.
   */
  virtual U32 IFXAPI GetVersionWord(U32 uVectorNum)=0;

  /**
   *  Updates the version word for a specified vector in the interleaved array.
   *
   *  @param  uVectorNum  Input U32 specifying which vector to update.  If
   *            this value is invalid, no version numbers are updated
   *            and the method returns 0.
   *
   *  @return A U32 value containing the new version word for the
   *      specified vector.
   */
  virtual U32 IFXAPI UpdateVersionWord(U32 uVectorNum)=0;

  /**
   *  Gets a pointer to the beginning of the data within the array.
   *  @warning  This is dangerous to do without knowing
   *        what format the data is in!
   *
   *  @return A U8* pointing to the beginning of the data memory.
   */
  virtual U8* IFXAPI GetDataPtr()=0;

  /**
   *  Gets the number of vertices last allocated for this array.
   *
   *  @return A U32 value containing the number of vertices allocated.
   */
  virtual U32 IFXAPI GetNumVertices() const= 0;

  /**
   *  Gets information about the interleaved data array.  This method
   *  allows the user to determine the data arrangement of vectors
   *  within each vertex and the total number of vertices in the
   *  interleaved array.  If @a puVectorSizes is NULL, the function
   *  will succeed and return other information.  This will allow the
   *  correct array size to be allocated for a second call.
   *
   *  @param  uNumVectors Reference to a U32 value that will be
   *            filled in with the number of vectors
   *            within each vertex of data.
   *  @param  puVectorSizes Pointer to an array of U32 values
   *              that will be filled in with vector
   *              sizes within each vertex.  If non-NULL,
   *              this array must be at least
   *              @a uNumVectors in length.
   *  @param  uNumVertices  Reference to a U32 value that will
   *              be filled with the number of vertices
   *              in the array.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *
   */
  virtual IFXRESULT IFXAPI GetInfo( U32& uNumVectors,
                U32* puVectorSizes,
                U32& uNumVertices)=0;

  /**
   *  Gets the number of bytes between each vertex in the data array.
   *  Since all vertices are densely packed, this is also the total
   *  size of each vertex.
   *
   *  @return A U32 value containing the stride between each vertex
   *      in the array.
   */
  virtual U32 IFXAPI GetStride() const=0;

  /**
   *  Copies data from @a rSrcData into this data array.  The arrays
   *  must have been allocated with the same vertex format for the
   *  method to succeed.  Also, the number of vertices must fit within
   *  the currently allocated data array.  This will automatically
   *  update the version word for all vectors in each vertex.
   *
   *  @param  rSrcData  Reference to another IFXInterleavedData
   *            object.  Data from @a rSrcData will be
   *            copied into this data array.
   *  @param  uStartVertex  The start of the data segment to
   *              copy.  This offset is used for both the
   *              @a rSrcData array and this (the destination)
   *              array.
   *  @param  uNumVertices  The number of vertices to copy, starting
   *              from @a uStartVertex.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE Either the vertex formats do not match, or
   *                the data copy would extend beyond the
   *                allocated memory of one (or both) of the
   *                data arrays.
   */
  virtual IFXRESULT IFXAPI CopyData(  IFXInterleavedData &rSrcData,
                    U32 uStartVertex, U32 uNumVertices)=0;

  /**
   *  Gets the unique id of this instance of IFXInterleavedData
   *
   *  @return A U32 value that is unique for this allocated instance.
   */
  virtual U32 IFXAPI GetId() const=0;

  /**
   *  Each instance of IFXInterleavedData can contain a user-defined U32 value
   *  referred to as a TimeStamp.  This method will retrieve it.  It can
   *  be set with SetTimeStamp().  This method should @b NOT be used by anyone
   *  other than the rendering system.  It is used to determine when static data
   *  is repeatedly rendered.  This value is cleared to 0 when UpdateVersionWord()
   *  or Allocate() is called.
   *
   *  @return A U32 value containing the current time stamp.
   */
  virtual U32 IFXAPI GetTimeStamp() const=0;

  /**
   *  Each instance of IFXInterleavedData can contain a user-defined U32 value
   *  referred to as a TimeStamp.  This method will set it.  It can be retrieved
   *  with GetTimeStamp().  This method should @b NOT be used by anyone
   *  other than the rendering system.  It is used to determine when static data
   *  is repeatedly rendered.  This value is cleared to 0 when UpdateVersionWord()
   *  or Allocate() is called.
   *
   *  @param  uTimeStamp  The new time stamp value.
   *
   *  @return A U32 value containing the current time stamp.
   */
virtual U32 IFXAPI SetTimeStamp(U32 uTimeStamp)=0;

protected:
  IFXInterleavedData() {}       ///< Constructor - empty
  virtual ~IFXInterleavedData() {}  ///< Destructor - empty
};

/**
 *  Smart pointer type specific to IFXInterleavedData.
 */
 typedef IFXSmartPtr<IFXInterleavedData> IFXInterleavedDataPtr;

/**
 *  This value determines how the beginning of the allocated data array
 *  is aligned to the beginning of a cache line.
 */
#define IFX_CACHE_BLOCK_SIZE  32

#endif
