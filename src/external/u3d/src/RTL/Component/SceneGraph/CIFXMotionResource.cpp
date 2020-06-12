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
	@file CIFXMotionResource.cpp
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXMotionResource.h"
#include "IFXMotion.h"
#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"


#include "IFXExportingCIDs.h"

//---------------------------------------------------------------------------
//	CIFXMotionResource::CIFXMotionResource
//
//	This is the constructor.  It sets up the object's initial state.
//---------------------------------------------------------------------------

CIFXMotionResource::CIFXMotionResource() :
	m_refCount( 0 )
{
	// IFXMotionResource attributtes...
	m_pMotion = NULL;
}


//---------------------------------------------------------------------------
//	CIFXMotionResource::~CIFXMotionResource
//
//	This is the destructor.  It performs any necessary cleanup activities.
//---------------------------------------------------------------------------

CIFXMotionResource::~CIFXMotionResource()
{
	if (m_pMotion)
		delete m_pMotion;
}


//---------------------------------------------------------------------------
//	CIFXMotionResource_Factory
//
//	This is the CIFXMotionResource component factory function.  The
//	CIFXMotionResource component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXMotionResource_Factory( IFXREFIID	interfaceId, void**	ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// Create the CIFXMotionResource component.
		CIFXMotionResource	*pComponent	= new CIFXMotionResource;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN(result);
}


//---------------------------------------------------------------------------
//	CIFXMotionResource::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXMotionResource::AddRef()
{
	return ++m_refCount;
}


//---------------------------------------------------------------------------
//	CIFXMotionResource::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXMotionResource::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}


//---------------------------------------------------------------------------
//	CIFXMotionResource::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT CIFXMotionResource::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXMotionResource )
			*ppInterface = ( IFXMotionResource* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = (IFXMarker*)this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = (IFXMarkerX*)this;
		else if ( interfaceId == IID_IFXMetaDataX ) 
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN(result);
}


// IFXMarker
IFXRESULT CIFXMotionResource::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT rc = IFX_OK ;

	rc = CIFXMarker::SetSceneGraph( pInSceneGraph );

	if (IFXSUCCESS(rc))
	{
		IFXDELETE( m_pMotion );
			
		m_pMotion = new IFXMotion ;

		if (m_pMotion == NULL)
			rc = IFX_E_OUT_OF_MEMORY;
	}

	IFXRETURN(rc);
}



// IFXMarkerX interface...
void CIFXMotionResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXMotionResourceEncoder, rpEncoderX);
}



// IFXMotionResource interface...
IFXRESULT CIFXMotionResource::GetTrackCount(U32 *puTrackCount)
{
	*puTrackCount=m_pMotion->GetTracks().GetNumberElements();
	IFXRETURN(IFX_OK);
}

IFXRESULT CIFXMotionResource::FindTrack(IFXString *pTrackName,U32 *puTrackID)
{
	long trackid=m_pMotion->GetTrackIndexByName(pTrackName->Raw());

	if(trackid>=0)
	{
		*puTrackID=trackid;
		return IFX_OK;
	}

	*puTrackID=0;
	IFXRETURN(IFX_E_INVALID_HANDLE);
}



IFXRESULT CIFXMotionResource::AddTrack(IFXString *pTrackName, U32 *puTrackID)
{
	IFXKeyTrack& track = m_pMotion->GetTracks().CreateNewElement();

	track.SetName(pTrackName->Raw());

	*puTrackID=m_pMotion->GetTracks().GetNumberElements()-1;
	return IFX_OK;
}


IFXRESULT CIFXMotionResource::GetTrackName(U32 uTrackID, IFXString *pTrackName)
{
	IFXKeyTrack& keyTrack = m_pMotion->GetTrack(uTrackID);
	// changed to U8 from u16
	pTrackName->Assign( &( keyTrack.GetNameConst() ) );
	return IFX_OK;
}


IFXRESULT CIFXMotionResource::ClearTrack(U32 uTrackID)
{
	m_pMotion->GetTrack(uTrackID).DeleteAll();
	return IFX_OK;
}


IFXRESULT CIFXMotionResource::FilterAndCompress(F32 deltatime,
						F32 deltalocation,F32 deltarotation,F32 deltascale)
{
	m_pMotion->FilterAllTracks(deltatime);
	m_pMotion->CompressAllTracks(deltalocation,deltarotation,deltascale);
	return IFX_OK;
}


IFXRESULT CIFXMotionResource::InsertKeyFrames(U32 uTrackID,U32 uKeyFrameCount,
												IFXKeyFrame *pKeyFrameArray)
{
	IFXKeyTrack &track=m_pMotion->GetTrack(uTrackID);

	U32 m;
	for(m=0;m<uKeyFrameCount;m++)
		{
		//* using one context for multiple lists will thrash
//		track.InsertNewKeyFrame(pKeyFrameArray[m].Time(),
//										pKeyFrameArray[m],&m_insertContext);
		track.InsertNewKeyFrame(pKeyFrameArray[m].Time(),pKeyFrameArray[m]);
		}

	return IFX_OK;
}


IFXRESULT CIFXMotionResource::GetKeyFrameCount(U32 uTrackID,
												U32 *puKeyFrameCount)
{
	*puKeyFrameCount=m_pMotion->GetTrack(uTrackID).GetNumberElements();
	return IFX_OK;
}

IFXRESULT CIFXMotionResource::GetKeyFrames(U32 uTrackID,U32 uKeyFrameStart,
							U32 uKeyFrameCount,IFXKeyFrame *pKeyFrameArray)
{
	IFXKeyTrack &track=m_pMotion->GetTrack(uTrackID);
	IFXListContext context;

	track.ToHead(context);

	U32 m;
	for(m=0;m<uKeyFrameStart;m++)
		track.PostIncrement(context);

	for(m=0;m<uKeyFrameCount;m++)
		pKeyFrameArray[m]=*track.PostIncrement(context);

	return IFX_OK;
}


IFXRESULT CIFXMotionResource::GetInterpolatedFrame(U32 uTrackID,F32 fTime,
												IFXKeyFrame *pKeyFrame)
{
	//* using one context for multiple lists will thrash
//	m_pMotion->CalcInstantConst(fTime,uTrackID,pKeyFrame,&m_interpolateContext);
	m_pMotion->CalcInstantConst(fTime,uTrackID,pKeyFrame);

	pKeyFrame->SetTime(fTime);

	return IFX_OK;
}


IFXRESULT CIFXMotionResource::GetDuration(U32 uTrackID, F32* pfOutDuration)
{
	IFXRESULT result = IFX_OK;

	if ( pfOutDuration )
		if (m_pMotion)
			{
			F32 min,max;
			m_pMotion->GetTimeLimits(&min,&max);
			*pfOutDuration=max-min;
/*
			*pfOutDuration =
				m_pMotion->GetTrackConst(uTrackID).GetTail()->Time() -
				m_pMotion->GetTrackConst(uTrackID).GetHead()->Time();
*/
			}
		else
			result = IFX_E_NOT_INITIALIZED;
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN(result); // was: return IFX_OK;
}


IFXRESULT CIFXMotionResource::GetDuration(F32* pfOutDuration)
{
	IFXRESULT result = IFX_OK;

	if ( pfOutDuration )
		if (m_pMotion)
			if (m_pMotion->GetTracks().GetNumberElements())
				*pfOutDuration =
					m_pMotion->GetTrackConst(0).GetTail()->Time() -
					m_pMotion->GetTrackConst(0).GetHead()->Time();
			else
				*pfOutDuration = 0;
		else
			result = IFX_E_NOT_INITIALIZED;
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN(result);
}


IFXMotion* CIFXMotionResource::GetMotionRef( void )
{
	return m_pMotion;
}
