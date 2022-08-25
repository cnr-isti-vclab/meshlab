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
  @file IFXFace.h

      This header defines the IFXFace class and its functionality.
*/

#ifndef IFX_FACE_H
#define IFX_FACE_H

#include "IFXDebug.h"

/**
  This is the main interface for IFXFace.
*/
template<class INDEX>
class IFXFaceT
{
public:

  /**
    This method is used to set the face indices.

    @param a A template class to the first index.
    @param b A template class to the second index.
    @param c A template class to the third index.

    @return void
  */
  void IFXAPI Set(INDEX a, INDEX b, INDEX c)
  {
    m_pData[0] = a;
    m_pData[1] = b;
    m_pData[2] = c;
  }

  /**
    This method is used to set the first face index.

    @param a A template class to the first index.

    @return void
  */
  void IFXAPI SetA(INDEX a)
  {
    m_pData[0] = a;
  }

  /**
    This method is used to set the second face index.

    @param b A template class to the second index.

    @return void
  */
  void IFXAPI SetB(INDEX b)
  {
    m_pData[1] = b;
  }

  /**
    This method is used to set the third face index.

    @param c A template class to the third index.

    @return void
  */
  void IFXAPI SetC(INDEX c)
  {
    m_pData[2] = c;
  }

  /**
    This method is used to set one of the three indices.

    @param uIndex A U32 denoting the index.
    @param uVal   A template class to the index.

    @return void
  */
  void IFXAPI Set(U32 uIndex, INDEX uVal)
  {
    IFXASSERTBOX(uIndex < 3, "Face corner index out of range!");
    m_pData[uIndex] = uVal;
  }

  /**
    This method is used to return the first index.

    @return INDEX A template class to the first index.
  */
  INDEX IFXAPI VertexA() const { return m_pData[0]; }

  /**
    This method is used to return the second index.

    @return INDEX A template class to the second index.
  */
  INDEX IFXAPI VertexB() const { return m_pData[1]; }

  /**
    This method is used to return the third index.

    @return INDEX A template class to the third index.
  */
  INDEX IFXAPI VertexC() const { return m_pData[2]; }

  /**
    This method is used to return one of the three indices.

    @param  uIndex  A U32 denoting the index.

    @return INDEX A template class to one of the indices.
  */
  INDEX IFXAPI Vertex(U32 uIndex) const
  {
    IFXASSERTBOX(uIndex < 3, "Face corner index out of range!");
    return m_pData[uIndex];
  }
protected:
  INDEX m_pData[3];
};
typedef IFXFaceT<U16> IFXU16Face;
typedef IFXFaceT<U32> IFXU32Face;

class IFXFace
{
public:
  IFXFace() {}
  virtual ~IFXFace() {}

  /**
    This method is used to return the internal data cast
    to a void pointer.

    @return void Pointer to the data.
  */
  virtual void* IFXAPI GetDataPtr() const=0;

  /**
    This method is used to set the internal data from
    a passed in void pointer.

    @param void Pointer to the data.

    @return void
  */
  virtual void IFXAPI SetDataPtr(void* pData)=0;

  /**
    This method is used to set all three indices.

    @param a A U32 denoting the new first index value.
    @param b A U32 denoting the new second index value.
    @param c A U32 denoting the new third index value.

    @return void
  */
  virtual void IFXAPI Set(U32 a, U32 b, U32 c)=0;

  /**
    This method is used to set the first index.

    @param a A U32 denoting the new index value.

    @return void
  */
  virtual void IFXAPI SetA(U32 a)=0;

  /**
    This method is used to set the second index.

    @param b A U32 denoting the new index value.

    @return void
  */
  virtual void IFXAPI SetB(U32 b)=0;

  /**
    This method is used to set the third index.

    @param c A U32 denoting the new index value.

    @return void
  */
  virtual void IFXAPI SetC(U32 c)=0;

  /**
    This method is used to set one of the three indices.

    @param uIndex A U32 denoting the desired index to be set.
    @param uVal   A U32 denoting the new index value.

    @return void
  */
  virtual void IFXAPI Set(U32 uIndex, U32 uVal)=0;

  /**
    This method is used to return the first index.

    @return A U32 denoting the first index.
  */
  virtual U32 IFXAPI VertexA() const=0;

  /**
    This method is used to return the second index.

    @return A U32 denoting the second index.
  */
  virtual U32 IFXAPI VertexB() const=0;

  /**
    This method is used to return the third index.

    @return A U32 denoting the third index.
  */
  virtual U32 IFXAPI VertexC() const=0;

  /**
    This method is used to return one of the three indices.

    @param  uIndex  A U32 denoting the desired index.

    @return A U32 denoting one of the indices.
  */
  virtual U32 IFXAPI Vertex(U32 uIndex) const=0;

  /**
    This method overloads the = operator so that one
    face can be set equal to another face.

    @param  operand An IFXFace reference.

    @return The resultant IFXFace.
  */
  const IFXFace& IFXAPI operator=(const IFXFace& operand);

  /**
    This method copies the contents of one face to another face.

    @param  operand An IFXFace reference to the input face.

    @return void.
  */
  void IFXAPI Clone(const IFXFace& operand);
};

template <class INDEX>
class IFXGenFaceT : public IFXFace
{
public:
  IFXGenFaceT() {SetDataPtr(0);}
  virtual ~IFXGenFaceT() {}

  void* IFXAPI GetDataPtr() const
  {
    return (void*)m_pFace;
  }
  void IFXAPI SetDataPtr(void* pData)
  {
    m_pFace = (IFXFaceT<INDEX>*)pData;
  }
  void IFXAPI Set(U32 a, U32 b, U32 c)
  {
    m_pFace->Set((INDEX)a, (INDEX)b, (INDEX)c);
  }
  void IFXAPI SetA(U32 a)
  {
    m_pFace->SetA((INDEX)a);
  }
  void IFXAPI SetB(U32 b)
  {
    m_pFace->SetB((INDEX)b);
  }
  void IFXAPI SetC(U32 c)
  {
    m_pFace->SetC((INDEX)c);
  }

  void IFXAPI Set(U32 uIndex, U32 uVal)
  {
    m_pFace->Set(uIndex, (INDEX)uVal);
  }

  U32 IFXAPI VertexA() const
  {
    return (U32)m_pFace->VertexA();
  }
  U32 IFXAPI VertexB() const
  {
    return (U32)m_pFace->VertexB();
  }
  U32 IFXAPI VertexC() const
  {
    return (U32)m_pFace->VertexC();
  }

  U32 IFXAPI Vertex(U32 uIndex) const
  {
    return (U32)m_pFace->Vertex(uIndex);
  }

private:
  IFXFaceT<INDEX>* m_pFace;
};
typedef IFXGenFaceT<U16> IFXGenFaceU16;
typedef IFXGenFaceT<U32> IFXGenFaceU32;

IFXINLINE const IFXFace& IFXFace::operator=(const IFXFace& operand)
{
  Set(operand.VertexA(), operand.VertexB(), operand.VertexC());

  return *this;
}

IFXINLINE void IFXFace::Clone(const IFXFace& operand)
{
  SetDataPtr(operand.GetDataPtr());
}

#endif // IFX_FACE_H
