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
//  CIFXPalette.h
//
//  DESCRIPTION
//      Header file for the palette classes.
//
//  NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXPALETTE_H__
#define __CIFXPALETTE_H__

#include "CIFXSubject.h"
#include "IFXPalette.h"
#include "IFXHashMap.h"
#include "IFXModifierChain.h"
#include "IFXModifier.h"
#include "CIFXSimpleObject.h"

class CIFXPalette :
		virtual public CIFXSubject,
				public IFXPalette
{
public:
    // IFXUnknown methods
    U32 IFXAPI  AddRef (void);
    U32 IFXAPI  Release (void);
    IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

    // IFXPalette methods
    IFXRESULT  IFXAPI 	 Initialize(U32 uInitialSize,U32 uGrowthSize=0);
    IFXRESULT  IFXAPI 	 Add(const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId);
    IFXRESULT  IFXAPI 	 Add(IFXString* pPaletteEntryName, U32* pPaletteEntryId);

    IFXRESULT  IFXAPI 	 AddHidden(IFXString* pPaletteEntryName, U32* pPaletteEntryId);
    IFXRESULT  IFXAPI 	 IsHidden(U32 uInIndex, BOOL* pbOutHidden);

    IFXRESULT  IFXAPI 	 Find(const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId);
    IFXRESULT  IFXAPI 	 Find(const IFXString* pPaletteEntryName, U32* pPaletteEntryId);

    IFXRESULT  IFXAPI 	 FindByResourcePtr(IFXUnknown* pPointer, U32* pIndex);

    IFXRESULT  IFXAPI 	 GetName(U32 uPaletteEntryId, IFXCHAR* pPaletteEntryName, U32* pNameLength);
    IFXRESULT  IFXAPI 	 GetName(U32 uPaletteEntryId, IFXString* pPaletteEntryName);

    IFXRESULT  IFXAPI 	 GetPaletteSize(U32* pPaletteSize);
    IFXRESULT  IFXAPI 	 DeleteByName(const IFXCHAR* pPaletteEntryName);
    IFXRESULT  IFXAPI 	 DeleteByName(IFXString* pPaletteEntryName);

    IFXRESULT  IFXAPI 	 DeleteById(U32 uPaletteEntryId);
    IFXRESULT  IFXAPI 	 GetResourcePtr(U32 uIndex, IFXUnknown** ppObject);
    IFXRESULT  IFXAPI 	 GetResourcePtr(U32 uIndex, IFXREFIID interfaceId, void** ppObject);
    IFXRESULT  IFXAPI 	 SetResourcePtr(U32 uIndex, IFXUnknown* pObject);

    // Default entry methods
    IFXRESULT  IFXAPI 	 SetDefault(IFXString* pPaletteEntryName);
    IFXRESULT  IFXAPI 	 SetDefaultResourcePtr(IFXUnknown* pObject);
    IFXRESULT  IFXAPI 	 DeleteDefault();

    IFXRESULT  IFXAPI 	 SetResourceObserver(U32 uCurrentIndex, U32 uNewIndex, IFXModel* pModel);

    // Iterator methods
    IFXRESULT  IFXAPI 	 First(U32* pID);
    IFXRESULT  IFXAPI 	 Last(U32* pID);
    IFXRESULT  IFXAPI 	 Next(U32* pID);
    IFXRESULT  IFXAPI 	 Previous(U32* pID);

    // Factory function.
    friend IFXRESULT IFXAPI_CALLTYPE CIFXPalette_Factory( IFXREFIID interfaceId, void** ppInterface );

private:
    CIFXPalette();
    ~CIFXPalette();

    IFXHashMap* m_pHashMap;

    typedef struct SPaletteObject
    {
        IFXString* m_pName;
        IFXUnknown* m_pObject;
        IFXModifierChain* m_pModChain;
        U32         m_uNextFreeId;
        BOOL        m_bHidden; // Default = FALSE; hidden entries are not written to the file or exposed to the author.

        IFXSubject* m_pSimpleObject; // Provides support for NULL resource
    } SPaletteObject;

    U32     m_uRefCount;
    U32     m_uLastFree;
    U32     m_uGrowthSize;
    U32     m_uNumberEntries;
    U32     m_uPaletteSize;
    BOOL    m_bUniqueNameMode;
    BOOL    m_bDefaultMode;
    U32     m_uLatestTagID;

    SPaletteObject* m_pPalette;
};


#endif
