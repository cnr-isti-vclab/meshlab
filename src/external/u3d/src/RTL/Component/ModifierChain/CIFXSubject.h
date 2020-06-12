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
#ifndef _CIFXSUBJECT_H_
#define _CIFXSUBJECT_H_


#include "CArrayList.h"
#include "CIFXObserverStateTree.h"

	
typedef CArrayList<SIFXObserverRequest*> OBSERVERSTATELIST; 
typedef CArrayList<IFXObserver*> OBSERVERLIST; 

struct ObserverInterest
{
	U32 interest;
	OBSERVERSTATELIST* pObserverStates;
};

typedef CArrayList<ObserverInterest> INTERESTLIST; 


class CIFXSubject : virtual public IFXSubject 
{
protected :
	OBSERVERSTATETREE	m_observerTree;
	OBSERVERSTATELIST	m_observerList;
	OBSERVERSTATELIST	m_tmpAttachList;
	OBSERVERLIST		m_tmpDettachList;
	INTERESTLIST		m_interestList;
	BOOL				m_bSafeToModifyList;
	BOOL				m_bNeedResolution;
	U32					m_changedBits;
	U32					m_interestBits;
	U32					m_currentTimeStamp;

private :
	// Updates the List with the Queued up additions and deletion requests that occured during iteration
	void ResolvePendingAttachments()
	{		
		U32 iterObsList; 	
		if(!m_tmpAttachList.empty())
		{
			for(iterObsList = m_tmpAttachList.begin(); iterObsList != m_tmpAttachList.end();iterObsList++)
			{
				SIFXObserverRequest* oreq = m_tmpAttachList[iterObsList];
				Attach(oreq->pObserver,oreq->uObserverInterests,oreq->rInterfaceType);
				oreq->pObserver->Release();
				delete oreq;
			}
		
			m_tmpAttachList.clear();
		}
		if(!m_tmpDettachList.empty())
		{
			U32 iter;
			for(iter = m_tmpDettachList.begin(); iter != m_tmpDettachList.end();iter++)
				Detach(m_tmpDettachList[iter]);
			// Clean up tmpDetachList
			m_tmpDettachList.clear();

		}
		m_bNeedResolution = FALSE;
	}

	
	BOOL FindObserver(IFXObserver* pObs, U32 & iterObsList)
	{
		BOOL found = FALSE;
		for( iterObsList = m_observerList.begin(); iterObsList != m_observerList.end(); ++iterObsList )
		{
			if( m_observerList[iterObsList]->pObserver == pObs )
			{
				found = TRUE;
				break;
			}
		}
		
		return found;
	}

public :
	CIFXSubject();
	virtual ~CIFXSubject();

	// attaches an observer to this subject
	virtual IFXRESULT IFXAPI  Attach(IFXObserver* pObs, U32 interest,
						IFXREFIID rIType=IID_IFXUnknown, U32 shiftBits=0);
	// removes an observer 
	virtual IFXRESULT IFXAPI  Detach(IFXObserver* pObs);

	// the observers will get update calls when the 
	// subject state the observer is interested in changes
	virtual void IFXAPI  PostChanges(U32 changedBits);

	void  IFXAPI 	PreDestruct();
};

#endif
