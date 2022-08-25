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
	@file	IFXSpatial.h

			The header file that defines the IFXSpatial interface.
*/

#ifndef __IFXSPATIAL_H__
#define __IFXSPATIAL_H__


#include "IFXUnknown.h"
#include "IFXSubject.h"
#include "CArrayList.h"

class IFXVector4;

// {7FF89715-4B2F-4269-92A9-3E5BD10B61E4}
IFXDEFINE_GUID(IID_IFXSpatial,
0x7ff89715, 0x4b2f, 0x4269, 0x92, 0xa9, 0x3e, 0x5b, 0xd1, 0xb, 0x61, 0xe4);

/**
	The IFXSpatial interface supplies the abstract services used to manage
	spatials.                                                                
*/
class IFXSpatial : virtual public IFXSubject
{
public:
  enum eType
  {
    OPAQUE_MODEL=0,
    TRANSLUCENT_MODEL,
    INFINITE_LIGHT,
    ATTENUATED_LIGHT,
    GROUP,
    VIEW,
    TYPE_COUNT,

    UNSPECIFIED = 0xffffffff
  }; // Note: eType values are used as array indices. Do not convert it to
     //       a bit field or insert wildcard elements before TYPE_COUNT.

  enum eInterest
  {
    POSITION    = 1<<0,
    BOUND   = 1<<1,
    EOL     = 1<<2, // End of Life in Collection.

    INTEREST_FORCE_32BIT = 0xffffffff
  };

  virtual IFXRESULT IFXAPI GetSpatialBound(IFXVector4& rOutSphere, U32 WorldInstance )=0;
  /**<
          Returns the value of the spatial's world space bounding sphere.

  @param  rOutSphere
           A reference to the vector to contain the value of the spatial's
       world bound sphere.

  @return One of the following IFXRESULT codes:                          \n\n
-          IFX_OK                                                          \n
            No error.                                                      */

  virtual eType IFXAPI GetSpatialType()=0;
  /**<
          Returns the IFXSpatial::eType of the spatial.                    */
};

struct IFXSpatialInstance 
{
	IFXSpatial* m_pSpatial;
	U32 m_Instance;
	BOOL operator==(const IFXSpatialInstance& rOperand) const
	{
		return (m_pSpatial == rOperand.m_pSpatial) && (m_Instance == rOperand.m_Instance);
	}
};

typedef CArrayList<IFXSpatialInstance> SPATIALINSTANCE_LIST;
typedef CArrayList<IFXSpatial*> SPATIAL_LIST;

#endif
