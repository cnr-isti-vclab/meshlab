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
//
//	CIFXUVMapperReflection.h
//
//		Class implementation file for the Reflection texture coordinate 
//	generator class.
//
//***************************************************************************

#include "CIFXUVMapperReflection.h"

CIFXUVMapperReflection::CIFXUVMapperReflection() 
{
}

CIFXUVMapperReflection::~CIFXUVMapperReflection() 
{
}

// Factory method
IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperReflection_Factory( IFXREFIID interfaceId, void** ppInterface ) {
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.  Note:  The component
		// class sets up gs_pSingleton upon construction and NULLs it
		// upon destruction.
		CIFXUVMapperReflection	*pComponent	= new CIFXUVMapperReflection;

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

	return result;
}

/*
BOOL CIFXUVMapperReflection::NeedToMap(	IFXMesh& rMesh, 
										IFXUVMapParameters* pParams)
{
	return TRUE;
}
*/

/*
IFXRESULT CIFXUVMapperReflection::Map(IFXMesh& rMesh, 
									  IFXUVMapParameters* pParams,
									  IFXMatrix4x4* pModelMatrix, 
									  IFXMatrix4x4* pViewMatrix, 
									  IFXLightSet* pLightSet) 
{
	IFXRESULT iResult=IFX_OK;

	if(pMesh==NULL || pParams==NULL) 
		iResult=IFX_E_INVALID_POINTER;
	
	if(IFXSUCCESS(iResult)) 
	{
		IFXVector3Iter vi;
		pMesh->GetPositionIter( vi );

		IFXVector3Iter ni;
		pMesh->GetNormalIter( ni );
	
		IFXVector2Iter ti;
		pMesh->GetTexCoordIter( ti, pParams->uTextureLayer ); 

		U32 nVerts = pMesh->GetNumVertices();
		
		// The reflection mapping function is as follows:
		//   Vv = unit vector from view to vertex in camera space.
		//   Nv = vertex normal in the camera space.
		//   Fv = Vv - 2 * (Nvx^2+Nvy^2+Nvz^2) * Vv
		//   Ms = 2 * sqrt(Fvx^2 + Fvy^2 + (1+Fvz^2))
		//   u = 0.5 + (Fvx / Ms)
		//   v = 0.5 + (Fvy / Ms)
		F32 Ms;
		IFXVector3 Vv, Nv, Fv, tempVector;
		IFXMatrix4x4 view;
		IFXMatrix4x4 viewInverse;
		pParams->pView->GetMatrix( &view );
		viewInverse = view.inverse();
		IFXMatrix4x4 modelViewMatrix = modelViewMatrix.Multiply(
			*((IFXMatrix4x4*)(&viewInverse)),
			*pParams->pModelMatrix
			);
		F32 NNt[16];
		NNt[3]  = 0;
		NNt[7]  = 0;
		NNt[11] = 0;
		NNt[12] = 0;
		NNt[13] = 0;
		NNt[14] = 0;
		NNt[15] = 1;

		IFXMatrix4x4 textureMatrix;
		textureMatrix.Invert3x4(pParams->iTransformMatrix);

		U32	uIndex;
		for ( uIndex=0; uIndex < nVerts; uIndex++ )
		{
			//   Vv = unit vector from the camera to the vertex in camera space.
			modelViewMatrix.TransformVector( *(vi.Next()), Vv );
			Vv.Normalize();

			//   Nv = a vertex normal in the camera space.
			modelViewMatrix.RotateVector( *(ni.Next()), Nv );

			//   Fv = Vv - Vv * 2 * N * N(transposed)
			NNt[0]  = Nv.X()*Nv.X();
			NNt[1]  = Nv.Y()*Nv.X();
			NNt[2]  = Nv.Z()*Nv.X();
			NNt[4]  = NNt[1];
			NNt[5]  = Nv.Y()*Nv.Y();
			NNt[6]  = Nv.Z()*Nv.Y();
			NNt[8]  = NNt[2];
			NNt[9]  = NNt[6];
			NNt[10] = Nv.Z()*Nv.Z();
			((IFXMatrix4x4*)NNt)->RotateVector( Vv, tempVector );
			tempVector.Scale( 2 );
			Fv.Subtract( Vv, tempVector );

			//   Ms = 1/(2*sqrt(Fvx^2 + Fvy^2 + (1+Fvz^2)))
			Ms = 0.5f / (F32)sqrt( (Fv.X()*Fv.X()) + (Fv.Y()*Fv.Y()) + (1+Fv.Z()*Fv.Z()) );
			
			//   u = 0.5 + Ms * Fvx
			//   v = 0.5 + Ms * Fvy
			IFXVector2* pvTexCoord = ti.Next();
			pvTexCoord->Set( 0.5f + Ms * Fv.X(), 0.5f + Ms * Fv.Y() );

			iResult = ApplyTextureMatrix(&textureMatrix, pvTexCoord);
		}
	}

	return iResult;
}
*/
