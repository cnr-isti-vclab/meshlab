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
@file ViewNodeData.h

This header defines the ... functionality.

@note
*/


#ifndef ViewNodeData_H
#define ViewNodeData_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXString.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

struct ViewTexture
{
	IFXString m_name;
	F32 m_blend;
	F32 m_rotation;
	F32 m_locationX;
	F32 m_locationY;
	I32 m_regPointX;
	I32 m_regPointY;
	F32 m_scaleX;
	F32 m_scaleY;
};

/**
This is the implementation of a class that is used to @todo: usage.
*/
class ViewNodeData
{
public:
	ViewNodeData() {};
	virtual ~ViewNodeData() {};

	/**
	*/
	void SetUnitType( const IFXString& rUnitType );
	const IFXString& GetUnitType() const;

	void SetType( const IFXString& rType );
	const IFXString& GetType() const;

	void SetClipping( const F32& rNearClip, const F32& rFarClip );
	IFXRESULT GetClipping( F32* pNearClip, F32* pFarClip ) const;

	void SetProjection( const F32& rProjection );
	const F32& GetProjection() const;

	void SetViewPort( const F32& rWidth, const F32& rHeight,
		const F32& rHorPos, const F32& rVertPos );

	IFXRESULT GetViewPort( F32* pWidth, F32* pHeight,
		F32* pHorPos, F32* pVertPos ) const;

	void AddBackdrop( const ViewTexture& rOverlay );
	const ViewTexture& GetBackdrop( const U32 index ) const;
	const U32& GetBackdropCount() const;

	void AddOverlay( const ViewTexture& rOverlay );
	const ViewTexture& GetOverlay( const U32 index ) const;
	const U32& GetOverlayCount() const;

private:
	IFXString m_type;
	IFXString m_unitType;
	F32 m_nearClip;
	F32 m_farClip;
	F32 m_projection;
	F32 m_width;
	F32 m_height;
	F32 m_horizontalPosition;
	F32 m_verticalPosition;
	IFXArray< ViewTexture > m_backdropList;
	IFXArray< ViewTexture > m_overlayList;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void ViewNodeData::SetUnitType( const IFXString& rUnitType )
{
	m_unitType = rUnitType;
}

IFXFORCEINLINE const IFXString& ViewNodeData::GetUnitType() const
{
	return m_unitType;
}

IFXFORCEINLINE void ViewNodeData::SetType( const IFXString& rType )
{
	m_type = rType;
}

IFXFORCEINLINE const IFXString& ViewNodeData::GetType() const
{
	return m_type;
}

IFXFORCEINLINE void ViewNodeData::SetProjection( const F32& rProjection )
{
	m_projection = rProjection;
}

IFXFORCEINLINE const F32& ViewNodeData::GetProjection() const
{
	return m_projection;
}

IFXFORCEINLINE void ViewNodeData::SetClipping( const F32& rNearClip, const F32& rFarClip )
{
	m_nearClip = rNearClip;
	m_farClip = rFarClip;
}

IFXFORCEINLINE IFXRESULT ViewNodeData::GetClipping( F32* pNearClip, F32* pFarClip ) const
{
	IFXRESULT result = IFX_OK;

	if( NULL == pNearClip || NULL == pFarClip )
		result = IFX_E_INVALID_POINTER;
	else
	{
		*pNearClip = m_nearClip;
		*pFarClip = m_farClip;
	}

	return result;
}

IFXFORCEINLINE void ViewNodeData::SetViewPort( 
									const F32& rWidth,
									const F32& rHeight,
									const F32& rHorPos,
									const F32& rVertPos )
{
	m_width = rWidth;
	m_height = rHeight;
	m_horizontalPosition = rHorPos;
	m_verticalPosition = rVertPos;
}

IFXFORCEINLINE IFXRESULT ViewNodeData::GetViewPort( 
									F32* pWidth, F32* pHeight,
									F32* pHorPos, F32* pVertPos ) const
{
	IFXRESULT result = IFX_OK;

	if( NULL == pWidth || NULL == pHeight || NULL == pHorPos || NULL == pVertPos )
		result = IFX_E_INVALID_POINTER;
	else
	{
		*pWidth = m_width;
		*pHeight = m_height;
		*pHorPos = m_horizontalPosition;
		*pVertPos = m_verticalPosition;
	}

	return result;
}

IFXFORCEINLINE void ViewNodeData::AddBackdrop( const ViewTexture& rBackdrop )
{
	ViewTexture& backdrop = m_backdropList.CreateNewElement();
	backdrop = rBackdrop;
}

IFXFORCEINLINE const ViewTexture& ViewNodeData::GetBackdrop( const U32 index ) const
{
	return m_backdropList.GetElementConst( index );
}

IFXFORCEINLINE const U32& ViewNodeData::GetBackdropCount() const
{
	return m_backdropList.GetNumberElements();
}

IFXFORCEINLINE void ViewNodeData::AddOverlay( const ViewTexture& rOverlay )
{
	ViewTexture& overlay = m_overlayList.CreateNewElement();
	overlay = rOverlay;
}

IFXFORCEINLINE const ViewTexture& ViewNodeData::GetOverlay( const U32 index ) const
{
	return m_overlayList.GetElementConst( index );
}

IFXFORCEINLINE const U32& ViewNodeData::GetOverlayCount() const
{
	return m_overlayList.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Failure return codes
//***************************************************************************

}

#endif
