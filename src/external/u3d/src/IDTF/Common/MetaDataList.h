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
@file MetaDataList.h

This header defines the ... functionality.

@note
*/


#ifndef MetaDataList_H
#define MetaDataList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "IFXArray.h"

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

	class BinaryMetaData
	{
	public:
		BinaryMetaData();
		~BinaryMetaData();
		BinaryMetaData( BinaryMetaData& );

		/**
		Get/Set binary metadata
		*/
		const U8* GetBinaryValue() const;
		IFXRESULT SetBinaryValue( const U8* pValue, const U32 size );
		const U32& GetBinarySize() const;

		BinaryMetaData& operator= ( const BinaryMetaData& rBinary );

	private:
		U8* m_pValue;
		U32 m_size;
	};

	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class StringMetaData
	{
	public:
		StringMetaData();
		virtual ~StringMetaData();

		/**
		Get/Set string metadata
		*/
		const IFXString& GetStringValue() const;
		void SetStringValue( const IFXString& rValue );

	private:
		IFXString m_value;
	};

	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class MetaData : public StringMetaData, public BinaryMetaData
	{
	public:
		MetaData();
		virtual ~MetaData();

		/**
		Get/Set Key
		*/
		void SetKey( const IFXString& rKey );
		const IFXString& GetKey() const;

		/**
		Get/Set metadata attribute
		*/
		void SetAttribute( const IFXString& rAttribute );
		const IFXString& GetAttribute() const;

	private:
		IFXString m_attribute;
		IFXString m_key;
	};

	class MetaDataList
	{
	public:
		MetaDataList();
		virtual ~MetaDataList();

		/**
		Set/Get metadata to/from metadata list
		*/
		IFXRESULT SetMetaData( const MetaData* pMetaData );
		const MetaData& GetMetaData( const U32 index ) const;

		/**
		Get metadata list element count
		*/
		const U32& GetMetaDataCount() const;

	private:
		IFXArray< MetaData > m_metaDataList;
	};


	//***************************************************************************
	//  Inline functions
	//***************************************************************************


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}

#endif
