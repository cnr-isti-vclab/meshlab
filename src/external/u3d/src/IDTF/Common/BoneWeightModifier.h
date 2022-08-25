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
@file BoneWeightModifier.h

This header defines the ... functionality.

@note
*/


#ifndef BoneWeightModifier_H
#define BoneWeightModifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "Modifier.h"

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

	class BoneWeightList
	{
	public:
		BoneWeightList() {};
		virtual ~BoneWeightList() {};

		/**
		*/
		void AddBoneWeight( const I32& rBoneWeight );
		const I32& GetBoneWeight( U32 index ) const;
		U32   GetBoneWeightCount() const;

		/**
		*/
		void AddBoneIndex( const I32& rBoneWeightIndex );
		const I32& GetBoneIndex( U32 index ) const;
		U32   GetBoneIndexCount() const;

	private:
		IFXArray< I32 > m_boneWeightList;
		IFXArray< I32 > m_boneIndexList;
	};

	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class BoneWeightModifier : public Modifier
	{
	public:
		BoneWeightModifier() {};
		virtual ~BoneWeightModifier() {};

		/**
		*/
		void SetAttributes( const IFXString& rAttributes );
		const IFXString& GetAttributes() const;

		/**
		*/
		void SetInverseQuant( const F32& rInverseQuant );
		const F32& GetInverseQuant() const;

		void AddBoneWeightList( const BoneWeightList& rBoneWeightList );
		const BoneWeightList& GetBoneWeightList( U32 index ) const;
		U32   GetBoneWeightListCount() const;

	protected:

	private:
		IFXString m_attributes;
		F32 m_inverseQuant;
		IFXArray< BoneWeightList > m_positionBoneWeightList;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void BoneWeightModifier::SetAttributes(
		const IFXString& rAttributes )
	{
		m_attributes = rAttributes;
	}

	IFXFORCEINLINE const IFXString& BoneWeightModifier::GetAttributes() const
	{
		return m_attributes;
	}

	IFXFORCEINLINE void BoneWeightModifier::SetInverseQuant(
		const F32& rInverseQuant )
	{
		m_inverseQuant = rInverseQuant;
	}

	IFXFORCEINLINE const F32& BoneWeightModifier::GetInverseQuant() const
	{
		return m_inverseQuant;
	}

	IFXFORCEINLINE void BoneWeightModifier::AddBoneWeightList(
		const BoneWeightList& rBoneWeightList )
	{
		BoneWeightList& boneWeightList = m_positionBoneWeightList.CreateNewElement();
		boneWeightList = rBoneWeightList;
	}

	IFXFORCEINLINE const BoneWeightList& BoneWeightModifier::GetBoneWeightList( U32 index ) const
	{
		return m_positionBoneWeightList.GetElementConst( index );
	}

	IFXFORCEINLINE U32 BoneWeightModifier::GetBoneWeightListCount() const
	{
		return m_positionBoneWeightList.GetNumberElements();
	}

	IFXFORCEINLINE void BoneWeightList::AddBoneWeight(
		const I32& rBoneWeight )
	{
		I32& boneWeight = m_boneWeightList.CreateNewElement();
		boneWeight = rBoneWeight;
	}

	IFXFORCEINLINE const I32& BoneWeightList::GetBoneWeight( U32 index ) const
	{
		return m_boneWeightList.GetElementConst( index );
	}

	IFXFORCEINLINE U32 BoneWeightList::GetBoneWeightCount() const
	{
		return m_boneWeightList.GetNumberElements();
	}

	IFXFORCEINLINE void BoneWeightList::AddBoneIndex(
		const I32& rBoneWeightIndex )
	{
		I32& boneWeightIndex = m_boneIndexList.CreateNewElement();
		boneWeightIndex = rBoneWeightIndex;
	}

	IFXFORCEINLINE const I32& BoneWeightList::GetBoneIndex( U32 index ) const
	{
		return m_boneIndexList.GetElementConst( index );
	}

	IFXFORCEINLINE U32 BoneWeightList::GetBoneIndexCount() const
	{
		return m_boneIndexList.GetNumberElements();
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

	/**
	@todo:  Insert module/interface specific return code description.
	*/
	//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
