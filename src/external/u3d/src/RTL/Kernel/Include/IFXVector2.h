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

#ifndef IFXVector2_h
#define IFXVector2_h


#include "IFXDebug.h"
#include "IFXDataTypes.h"
#include "math.h"


/**************************************************************************/
/**
  \brief Vector of 2 Floats

  There is no default initialization.  The vector is undefined unless set.
*/
/***************************************************************************/
/**This class handles vectors of two floats. */
class IFXVector2
{
  public:
          /// Construct without initializing.
          IFXVector2(void)        { }

          /// Construct using 2 explicit values.
          IFXVector2(F32 x,F32 y)     { Set(x,y); }

          /** Construct using an array of at least 2 floats
            representing the 2 elements in the vector. */
          IFXVector2(const float *floats) { operator=(floats); }

          /// Copies constructor.
          IFXVector2(const IFXVector2 &operand)
                          { operator=(operand); }

          /// Sets all three components to zero.
    void    Reset(void);

          /// Explicitly sets the two components in the vector.
    void    Set(F32 u, F32 v);

          /** Populates the vector using an array of at least 2
            floats representing the 2 components in the vector. */
    IFXVector2  &operator=(const float *floats);

          /// Deep copy.
    IFXVector2  &operator=(const IFXVector2 &operand);

          /// Exact compare operation (zero tolerance).
    bool    operator==(const IFXVector2 &operand) const;

          /** Returns TRUE if each component in the operand is
            with the given margin from the value of the
            corresponding component in this vector. */
    bool    IsApproximately(const IFXVector2 &operand,F32 margin) const;

          /** Returns a pointer to the internal float array
            containing the 2 floats.  This is not a copy. */
    F32     *Raw(void)            { return m_value; };

          /// Returns a const pointer to the internal float array.
const F32     *RawConst(void) const     { return m_value; };

          /// Accesses a component of the internal float array.
    F32     &operator[](const int n)
          { IFXASSERT(n < 2);
            return m_value[n]; };

          /// Accesses a component as const.
    F32     operator[](const int n) const
          { IFXASSERT(n < 2);
            return m_value[n]; };

          /// Accesses a component of the internal float array.
    F32&    Value(U32 index);

          /// Accesses a component as const.
    F32     Value(U32 index) const;

          /// Accesses the first component of the vector.
    F32&    X(void);

          /// Accesses the second component of the vector.
    F32&    Y(void);

          /// Accesses the first component of the vector as const.
    F32     X(void) const;

          /// Accesses the second component of the vector as const.
    F32     Y(void) const;

          /// Accesses the first component of the vector.
    F32&    U(void);

          /// Accesses the second component of the vector.
    F32&    V(void);

          /// Accesses the first component of the vector as const.
    F32     U(void) const;

          /// Accesses the second component of the vector as const.
    F32     V(void) const;

          /// Adds the operand into this vector.
    IFXVector2  &Add(const IFXVector2 &operand);

          /// Populates this vector with the sum of the operands.
    IFXVector2  &Add(const IFXVector2 &left, const IFXVector2 &right);

          /// Subtracts the operand out of this vector.
    IFXVector2  &Subtract(const IFXVector2 &operand);

          /** Populates this vector with the difference of the
            operands. */
    IFXVector2  &Subtract(const IFXVector2 &left, const IFXVector2 &right);

          /** Multiplies each component in the operand into
            the corresponding component of this vector. */
    IFXVector2  &Multiply(const IFXVector2 &operand);

          /** Populates components of this vector with the product
            of the corresponding components of the operands. */
    IFXVector2  &Multiply(const IFXVector2 &left, const IFXVector2 &right);

          /** Divides each component in the vector with the
            corresponding component in the operand. */

    IFXVector2  &Divide(const IFXVector2 &operand);
          /** Divides each component in the left vector with the
            corresponding component in the right vector and
            store the result in this vector. */
    IFXVector2  &Divide(const IFXVector2 &left, const IFXVector2 &right);

          /** Multiplies each component of this vector by the
            given scalar. */
    IFXVector2  &Scale(F32 scalar);

          /// Sets the length of the vector (normalize, then scale).
    IFXVector2  &Rescale(F32 magnitude);

          /// Multiplies all components by -1 (additive inverse).
    IFXVector2  &Invert(void);

          /** Returns the dot product of this vector with the
                            operand.  If you expect to take the acos() of
                            result to find an angle, both vectors must
                            be  normalized before calling the method. */
    F32     DotProduct(const IFXVector2 &operand);

          /** Returns the dot product of the two operands.
                            If you expect to take the acos() of
                            result to find an angle, both vectors must
                            be  normalized before calling the method. */
static  F32     DotProduct(const IFXVector2 &left, const IFXVector2 &right);

          /// Scales the vector to a length of 1.
    IFXVector2  &Normalize(void);

          /// Returns the length of the vector.
    F32     CalcMagnitude(void);

          /** Returns the square of the length of the vector.
            This is faster than computing the actual length. */
    F32     CalcMagnitudeSquared(void);

private:
    F32     m_value[2];
};


IFXINLINE void IFXVector2::Set(F32 u, F32 v)
{
  m_value[0] = u;
  m_value[1] = v;
}

IFXINLINE F32& IFXVector2::X()
{
  return m_value[0];
}

IFXINLINE F32 IFXVector2::X() const
{
  return m_value[0];
}

IFXINLINE F32& IFXVector2::Y()
{
  return m_value[1];
}

IFXINLINE F32 IFXVector2::Y() const
{
  return m_value[1];
}

IFXINLINE F32& IFXVector2::U()
{
  return m_value[0];
}

IFXINLINE F32 IFXVector2::U() const
{
  return m_value[0];
}

IFXINLINE F32& IFXVector2::V()
{
  return m_value[1];
}

IFXINLINE F32 IFXVector2::V() const
{
  return m_value[1];
}

IFXINLINE F32& IFXVector2::Value(U32 index)
{
  IFXASSERT(index < 2);
  return m_value[index];
}

IFXINLINE F32 IFXVector2::Value(U32 index) const
{
  IFXASSERT(index < 2);
  return m_value[index];
}

IFXINLINE void IFXVector2::Reset(void)
{
  m_value[0]=0.0f;
  m_value[1]=0.0f;
}


IFXINLINE IFXVector2 &IFXVector2::operator=(const float *floats)
{
  // 3 floats assumed

  m_value[0]=floats[0];
  m_value[1]=floats[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::operator=(const IFXVector2 &operand)
{
  m_value[0]=operand.m_value[0];
  m_value[1]=operand.m_value[1];

  return *this;
}

IFXINLINE bool IFXVector2::operator==(const IFXVector2 &operand) const
{
  // Unrolled to reduce branch count to 1.
  if( m_value[0] != operand.m_value[0]
    || m_value[1] != operand.m_value[1] )
      return false;

    return true;
}

IFXINLINE bool IFXVector2::IsApproximately(const IFXVector2 &operand,
                          F32 margin) const
{
  if( fabs(m_value[0]-operand.m_value[0])<margin &&
    fabs(m_value[1]-operand.m_value[1])<margin)
    return true;

  return false;
}

IFXINLINE IFXVector2 &IFXVector2::Add(const IFXVector2 &operand)
{
  m_value[0]+=operand.m_value[0];
  m_value[1]+=operand.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Add(const IFXVector2 &left,
                          const IFXVector2 &right)
{
  m_value[0]=left.m_value[0]+right.m_value[0];
  m_value[1]=left.m_value[1]+right.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Subtract(const IFXVector2 &operand)
{
  m_value[0]-=operand.m_value[0];
  m_value[1]-=operand.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Subtract(const IFXVector2 &left,
                          const IFXVector2 &right)
{
  m_value[0]=left.m_value[0]-right.m_value[0];
  m_value[1]=left.m_value[1]-right.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Multiply(const IFXVector2 &operand)
{
  m_value[0]*=operand.m_value[0];
  m_value[1]*=operand.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Multiply(const IFXVector2 &left,
                          const IFXVector2 &right)
{
  m_value[0]=left.m_value[0]*right.m_value[0];
  m_value[1]=left.m_value[1]*right.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Divide(const IFXVector2 &operand)
{
  IFXASSERT(operand.m_value[0] != 0.0f);
  IFXASSERT(operand.m_value[1] != 0.0f);

  m_value[0]/=operand.m_value[0];
  m_value[1]/=operand.m_value[1];

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Divide(const IFXVector2 &left,
                          const IFXVector2 &right)
{
  IFXASSERT(right.m_value[0] != 0.0f);
  IFXASSERT(right.m_value[1] != 0.0f);

  m_value[0]=left.m_value[0]/right.m_value[0];
  m_value[1]=left.m_value[1]/right.m_value[1];

  return *this;
}

// static
IFXINLINE F32 IFXVector2::DotProduct(const IFXVector2 &left,
                        const IFXVector2 &right)
{
  return  left.m_value[0]*right.m_value[0]+
      left.m_value[1]*right.m_value[1];
}

IFXINLINE F32 IFXVector2::DotProduct(const IFXVector2 &operand)
{
  return  m_value[0]*operand.m_value[0]+
      m_value[1]*operand.m_value[1];
}

IFXINLINE IFXVector2 &IFXVector2::Scale(F32 scalar)
{
  m_value[0]*=scalar;
  m_value[1]*=scalar;

  return *this;
}

IFXINLINE IFXVector2 &IFXVector2::Invert(void)
{
  m_value[0]= -m_value[0];
  m_value[1]= -m_value[1];

  return *this;
}

IFXINLINE F32 IFXVector2::CalcMagnitude(void)
{
#if 0
  return ifxFastMagnitude(m_value);
#else
  // Note: sqrtf calls sqrt
  return (float)sqrt(m_value[0]*m_value[0]+m_value[1]*m_value[1]);
#endif
}

IFXINLINE F32 IFXVector2::CalcMagnitudeSquared(void)
{
  return m_value[0]*m_value[0]+m_value[1]*m_value[1];
}

IFXINLINE IFXVector2 &IFXVector2::Normalize(void)
{
  return Scale(1.0f/CalcMagnitude());
}

IFXINLINE IFXVector2 &IFXVector2::Rescale(F32 magnitude)
{
  return Scale(magnitude/CalcMagnitude());
}

#endif
