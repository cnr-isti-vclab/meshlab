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

/**
	@file  IFXFrustum.h

	The header file that defines the IFXFrustum class.
*/

#ifndef __IFXFRUSTUM_H__
#define __IFXFRUSTUM_H__

#include "IFXResult.h"
#include "IFXDataTypes.h"
#include "IFXMatrix4x4.h"
#include "IFXRay.h"

#define IFX_EPSILON 1e-6f
#define DEFAULT_CLIP_PLANE_COUNT 6

enum EIFXFrustumMode
{
  IFX_FRUSTUM_PERSPECTIVE_PROJECTION,
  IFX_FRUSTUM_ORTHOGRAPHIC_PROJECTION,
  IFX_FRUSTUM_PROJECTION_FORCE_32BIT = 0xffffffff
};

#define IFX_FRUSTUM_BOTTOM  0x0000
#define IFX_FRUSTUM_TOP   0x0001
#define IFX_FRUSTUM_RIGHT 0x0002
#define IFX_FRUSTUM_LEFT  0x0003
#define IFX_FRUSTUM_FRONT 0x0004
#define IFX_FRUSTUM_BACK  0x0005

/**
 *  The IFXFrustum defines a view frustum in world space.  This is a simply a
 *  storage element. For example, changing the field of view or world transform
 *  will not change the associated view frustum plane positions or normals.
 */
class IFXFrustum
{
public:

  /**
   *  Constructor - Initializes to an infinitely small orthographic view.
   */
  IFXFrustum()
  {
    m_uPlaneCount = DEFAULT_CLIP_PLANE_COUNT;
    m_mWorld.Reset();
    m_fFOV = IFX_EPSILON;
    SetPlane(IFX_FRUSTUM_BOTTOM,IFXRay(IFXVector3(0,0,0),  IFXVector3(0,-1,0)));
    SetPlane(IFX_FRUSTUM_TOP,   IFXRay(IFXVector3(0,0,0),  IFXVector3(0,1,0) ));
    SetPlane(IFX_FRUSTUM_RIGHT, IFXRay(IFXVector3(0,0,0),  IFXVector3(1,0,0) ));
    SetPlane(IFX_FRUSTUM_LEFT,  IFXRay(IFXVector3(0,0,0),  IFXVector3(-1,0,0)));
    SetPlane(IFX_FRUSTUM_FRONT, IFXRay(IFXVector3(0,0,-1), IFXVector3(0,0,1) ));
    SetPlane(IFX_FRUSTUM_BACK,  IFXRay(IFXVector3(0,0,-2), IFXVector3(0,0,-1)));
    m_fAspect = 1;
  }

  /**
   *  Gets the IFXRay of the plane specified by @a uInPlaneIndex.
   *
   *  @param  uInPlaneIndex Input U32 specifying which plane to
   *              to return.  Possible values are:
   *              - @b IFX_FRUSTUM_BOTTOM
   *              - @b IFX_FRUSTUM_TOP
   *              - @b IFX_FRUSTUM_LEFT
   *              - @b IFX_FRUSTUM_FRONT
   *              - @b IFX_FRUSTUM_RIGHT
   *              - @b IFX_FRUSTUM_BACK
   *
   *  @return An IFXRay denoting position and normal direction of
   *      the plane specified by @a uInPlaneIndex.
   */
  const IFXRay&       IFXAPI GetPlane( U32 uInPlaneIndex ) const;

  /**
   *  Sets the plane position and normal direction for the plane specified
   *  with @a uInPlaneIndex.
   *
   *  @param  uInPlaneIndex Input U32 specifying which plane to
   *              to set.  Possible values are:
   *              - @b IFX_FRUSTUM_BOTTOM
   *              - @b IFX_FRUSTUM_TOP
   *              - @b IFX_FRUSTUM_LEFT
   *              - @b IFX_FRUSTUM_FRONT
   *              - @b IFX_FRUSTUM_RIGHT
   *              - @b IFX_FRUSTUM_BACK
   *  @param  InPlane Input IFXRay specifying the position and normal
   *      of the plane referenced by @a uInPlaneIndex.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The value of @a uInPlaneIndex is invalid.
   */
  IFXRESULT           IFXAPI SetPlane( U32             uInPlaneIndex,
                                         const IFXRay&  InPlane );

  /**
   *  Retrieves the world transform of the view frustum.  This can be inverted and
   *  used to transform objects into view space if needed.
   *
   *  @return The world transform of the IFXFrustum.
   */
  const IFXMatrix4x4& IFXAPI GetWorldTransform( void ) const;

  /**
   *  Sets the world transform of the view frustum.
   *
   *  @param  mTrans  Input IFXMatrix4x4 to be stored as the world transform
   *          of the view frustum.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  IFXRESULT           IFXAPI SetWorldTransform( const IFXMatrix4x4& mTrans );

  /**
   *  Gets the current field of view (y dimension) of the view frustum.
   *
   *  @return An F32 value containing the current y field of view.
   */
  F32           IFXAPI GetFOV( void ) const;

  /**
   *  Sets the current y field of view.
   *
   *  @param  fFOV  Input F32 specifying the new y field of view.
   *
   *  @return void
   */
  void        IFXAPI SetFOV( const F32 fFOV);

  /**
   *  Retrieves the current aspect (width / height) ratio of the view
   *  frustum.
   *
   *  @return An F32 value containing the current aspect ratio.
   */
  F32           IFXAPI GetAspect( void ) const;

  /**
   *  Sets the current aspect (width / height) ratio of the view frustum.
   *
   *  @param  fAspect Input F32 value containing the new aspect ratio.
   *
   *  @return void
   */
  void        IFXAPI SetAspect( const F32 fAspect);

  /**
   *  Gets an array containing all frustum plane positions and normals.
   *
   *  @return A pointer to an array of IFXRay objects, each containing
   *      a position and normal of one plane of the frustum.
   */
  const IFXRay*   IFXAPI GetPlanes() const { return m_FrustumPlanes; };

  /**
   *  Gets the number of planes in the array returned by GetPlanes().
   *
   *  @return A U32 value containing the number of IFXRay objects returned
   *      by GetPlanes()
   */
  U32     IFXAPI GetPlaneCount() const { return m_uPlaneCount; };

  /**
   *  Gets the projection mode of the view frustum, either perspective or
   *  orthographic.
   *
   *  @return An EIFXFrustumMode value.
   *  @retval IFX_FRUSTUM_PERSPECTIVE_PROJECTION  The frustum is using a
   *      symmetrical perspective projection.
   *  @retval IFX_FRUSTUM_ORTHOGRAPHIC_PROJECTION The frustum is using a
   *      symmetrical orthographic projection.
   */
  EIFXFrustumMode IFXAPI GetProjectionMode( void ) const;

  /**
   *  Implementation of the '=' operator to account for the use of heap memory
   *  allocations.
   */
  IFXFrustum& operator=( const IFXFrustum& vdIn )
  {
    m_mWorld  = vdIn.GetWorldTransform();
    m_fFOV    = vdIn.GetFOV();
    m_fAspect = vdIn.GetAspect();
	U32 i;
    for (i = 0; i< m_uPlaneCount; i++) m_FrustumPlanes[i] = vdIn.GetPlane(i);
    return (*this);
  }

private:
  /// Array of clip planes that define the frustum
  IFXRay        m_FrustumPlanes[DEFAULT_CLIP_PLANE_COUNT];
  /// World transform of the view frustum.
  IFXMatrix4x4        m_mWorld;
  /// Width / Height aspect ratio of the frustum.
  F32                 m_fAspect;
  /// Y field of view of the frustum
  F32                 m_fFOV;
  /// Number of planes in @a m_FrustumPlanes
  U32         m_uPlaneCount;
};


IFXINLINE const IFXRay& IFXAPI
IFXFrustum::GetPlane( U32 uInPlaneIndex ) const
{
  return m_FrustumPlanes[uInPlaneIndex];
}


IFXINLINE IFXRESULT IFXAPI
IFXFrustum::SetPlane( U32            uInPlaneIndex,
                      const IFXRay&  InPlane )
{
  if ( uInPlaneIndex < m_uPlaneCount )
  {
    m_FrustumPlanes[uInPlaneIndex] = InPlane;
    return IFX_OK;
  }
  else
    return IFX_E_INVALID_RANGE;
}


IFXINLINE const IFXMatrix4x4& IFXAPI
IFXFrustum::GetWorldTransform( void ) const
{
  return m_mWorld;
}


IFXINLINE IFXRESULT IFXAPI
IFXFrustum::SetWorldTransform( const IFXMatrix4x4& rInWorldTransform )
{
  m_mWorld = rInWorldTransform;
  return IFX_OK;
}


IFXINLINE F32 IFXAPI
IFXFrustum::GetFOV( ) const
{
  return m_fFOV;
}


IFXINLINE void IFXAPI
IFXFrustum::SetFOV( const F32 fFOV )
{
  m_fFOV = fFOV;
}


IFXINLINE F32 IFXAPI
IFXFrustum::GetAspect( ) const
{
  return m_fAspect;
}


IFXINLINE void IFXAPI
IFXFrustum::SetAspect( const F32 fAspect )
{
  m_fAspect = fAspect;
}


IFXINLINE EIFXFrustumMode IFXAPI
IFXFrustum::GetProjectionMode( void ) const
{
  F32 fDP = m_FrustumPlanes[IFX_FRUSTUM_RIGHT].direction.
        DotProduct(m_FrustumPlanes[IFX_FRUSTUM_LEFT].direction);

  if( fabsf(fDP + 1) < IFX_EPSILON )
    return IFX_FRUSTUM_ORTHOGRAPHIC_PROJECTION;
  else
    return IFX_FRUSTUM_PERSPECTIVE_PROJECTION;
}


#endif
