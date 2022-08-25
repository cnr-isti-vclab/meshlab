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

#include "CIFXSubject.h"

// This option enables the USING the OBSERVERLIST TO POST MESSAGES
//#define _USELIST

void NotifyObserverOfShutDown(OBSERVERSTATETREE::iterator iter, void * data);

CIFXSubject::CIFXSubject()
{
	m_bSafeToModifyList = TRUE;
	m_bNeedResolution = FALSE;
	m_changedBits =0;
	m_interestBits =0;
	m_currentTimeStamp = 0;
}


CIFXSubject::~CIFXSubject()
{
	m_bSafeToModifyList = FALSE;
	U32 iter;
	for( iter = m_interestList.begin(); iter < m_interestList.end(); ++iter )
	{
		m_interestList[iter].pObserverStates->clear();
		IFXDELETE(m_interestList[iter].pObserverStates);
	}
	m_interestList.clear();

#ifdef _USELIST	// In case a subject decides to go away

	U32 iterCMList = m_observerList.begin();
	while( iterCMList != m_observerList.end() )
	{
		SIFXObserverRequest* oreq = m_observerList.m_data[ iterCMList ];
		IFXObserver *pObserver = m_observerList.m_data[iterCMList]->pObserver;
		IFXREFIID iid = m_observerList.m_data[iterCMList]->rInterfaceType;
		pObserver->Update( this, 0, iid );
		delete oreq;
		iterCMList++;
	}

	m_observerList.clear();

#else
	
	/// @todo Replace with iterative version after testing.
	m_observerTree.traverseInOrder(NotifyObserverOfShutDown,(void *)(IFXSubject *)this);	
//	m_observerTree.inorderIterative(NotifyObserverOfShutDown,(void *)(IFXSubject *)this);	
#endif
	m_observerTree.clear();
	m_tmpAttachList.clear();
	m_tmpDettachList.clear();
	m_interestBits = 0;
	m_bSafeToModifyList = TRUE;
}

void CIFXSubject::PreDestruct()
{
	m_bSafeToModifyList = FALSE;
	//	m_observerList.dump();
	//	m_observerTree.dump();
	U32 iter;
	for( iter = m_interestList.begin(); iter < m_interestList.end(); ++iter )
	{
		OBSERVERSTATELIST* pObsStateList = m_interestList[iter].pObserverStates;
		pObsStateList->clear();
		IFXDELETE(pObsStateList);
	}
	m_interestList.clear();

#ifdef _USELIST	// In case a subject decides to go away
	FILE *fp = 0;
	U32 iterCMList = m_observerList.begin();
	IFXSubject * pSubject = (IFXSubject * )this;
	if(iterCMList != m_observerList.end())
	{
		fp = fopen("shutdown.txt","at");
		fprintf(fp,"\n\n Subject Predestruct... \n Address %x \n", this);
	}
	while( iterCMList != m_observerList.end() )
	{
		SIFXObserverRequest* oreq = m_observerList.m_data[ iterCMList ];
		IFXObserver *pObserver = m_observerList.m_data[iterCMList]->pObserver;
		IFXREFIID iid = m_observerList.m_data[iterCMList]->rInterfaceType;
			fprintf(fp," Subject  %x,  Observer  %x \n",pSubject,pObserver);
		pObserver->Update( pSubject, 0, iid );
		delete oreq;
		iterCMList++;
	}
	if(fp)
		fclose(fp);

	m_observerList.clear();

#else

	/// @todo Replace with iterative version after testing.
	m_observerTree.traverseInOrder(NotifyObserverOfShutDown, (void *)(IFXSubject *)this);	
//	m_observerTree.inorderIterative(NotifyObserverOfShutDown,(void *)(IFXSubject *)this);

#endif

	m_observerTree.clear();
	m_interestBits = 0;
	m_bSafeToModifyList = TRUE;
}

		
IFXRESULT CIFXSubject::Attach(IFXObserver* pObs, U32 inInterest, IFXREFIID rIType, U32 shift)
{
	BOOL found = FALSE;
	IFXRESULT iResult = IFX_E_UNDEFINED;
	OBSERVERSTATETREE::iterator pObsStateIter;
	U32 interest = inInterest << shift;
	SIFXObserverRequest * pObsRequest =0;

	if(pObs)
	{
		if (m_bSafeToModifyList)
		{
			// Add to Observer Tree
			//Find In Observer Tree
			found = m_observerTree.find(pObs, pObsStateIter);

#ifdef _USELIST
			U32 iterList;
			BOOL listFound = FALSE;
			listFound = FindObserver(pObs, iterList);
			if(found != listFound)
				iResult = IFX_E_UNDEFINED;
#endif
			if (found)
			{	// Update Observer Interests
				pObsRequest = *pObsStateIter;
				pObsRequest->uObserverInterests |= inInterest;
			}
			else 
			{
				//	insert into observer Tree
				pObsRequest = new SIFXObserverRequest(pObs, interest, rIType, shift, (U32)-1);
				m_observerTree.insert(pObsRequest);
#ifdef _USELIST
				m_observerList.push_back(pObsRequest);		
//				found = m_observerTree.find(pObs,pObsStateIter);
//				IFXASSERT(found); // Make sure it was inserted
#endif
			}

			// Add to Interest Lists		
			// the Interest bits already being serviced by the subject
			U32 supportedInterests = m_interestBits & interest; 

			// these Interests need to be added 
			U32 newInterests = interest & ~m_interestBits;

			if(supportedInterests)
			{
				U32 iter;
				for(iter = m_interestList.begin(); iter < m_interestList.end(); ++iter)
				{
					if(m_interestList[iter].interest & interest)
					{
/*#ifdef _DEBUG
						U32 tmp =0;
						found = m_interestList.m_data[iter].pObserverStates->find(pObsRequest,tmp);
						if(found)
						{
							IFXTRACE_GENERIC("****Double Attach for the same interest \n???");
							IFXASSERT(0);
						}
#endif*/
						m_interestList[iter].pObserverStates->push_back(pObsRequest);
					}
				}
			}
			// New Interests 			 
			if(newInterests)
			{
				U32 currInterest = 1;
				while (newInterests)
				{
					if(currInterest & newInterests)
					{
						OBSERVERSTATELIST* pStateList = new OBSERVERSTATELIST();
						
						pStateList->push_back(pObsRequest);
						ObserverInterest obsInterests = {currInterest, pStateList};
						pStateList = NULL;
						m_interestList.push_back(obsInterests);

						// Update SupportedInterests with New Interests
						m_interestBits |= currInterest;
						newInterests &= ~currInterest;
					}
					currInterest <<= 1;
				}
			}
		}
		else 
		{
			SIFXObserverRequest* pQueuedObserverRequest = new SIFXObserverRequest(pObs,interest,rIType,shift);
			m_tmpAttachList.push_back(pQueuedObserverRequest);
			m_bNeedResolution = TRUE;
		}
		iResult = IFX_OK;
	}

	return iResult;
}


IFXRESULT CIFXSubject::Detach( IFXObserver* pObs )
{
	BOOL found = FALSE;
	IFXRESULT iResult = IFX_E_UNDEFINED; 
	OBSERVERSTATETREE::iterator obsStateIter;
	if(pObs)
	{
		if (m_bSafeToModifyList)
		{
			found = m_observerTree.find(pObs,obsStateIter);
#ifdef _USELIST
			U32 iterList;
			BOOL listFound = FindObserver(pObs,iterList);
			if(found != listFound)
				iResult = IFX_E_UNDEFINED;
#endif
			IFXASSERT(found);
			if( found )
			{
				SIFXObserverRequest* pObsRequest  = *obsStateIter; 
				U32 interest = pObsRequest->uObserverInterests;
#ifdef _USELIST
				m_observerList.m_data[iterList];
				if(pObsRequest != *obsStateIter)
					iResult = IFX_E_UNDEFINED;
				m_observerList.remove(iterList);
#endif

				// remove from interest lists
				U32 iter;
				for( iter = m_interestList.begin(); iter < m_interestList.end(); ++iter)
				{
					if(m_interestList[iter].interest & interest)
					{				
						m_interestList[iter].pObserverStates->remove(pObsRequest);
					}
				}
				// Remove from Observer Tree
				m_observerTree.remove(obsStateIter);

				delete pObsRequest;
				iResult = IFX_OK;
			}
		}
		else 
		{
			m_tmpDettachList.push_back( pObs );
			m_bNeedResolution = TRUE;
			iResult = IFX_OK;
		}
	}

	return iResult;
}


void CIFXSubject::PostChanges(U32 changedBits)
{
	m_bSafeToModifyList = FALSE;

	U32 iterObsStateList;
	U32 obsTimeStamp = m_currentTimeStamp;
	IFXASSERT(changedBits);
	OBSERVERSTATELIST* pObsStateList =0;
	SIFXObserverRequest * pObsRequest = 0;

	U32 iter;	
	for( iter = m_interestList.begin(); iter < m_interestList.end(); ++iter )
	{
		if(m_interestList[iter].interest & changedBits)
		{
			pObsStateList = m_interestList[iter].pObserverStates;
			// Update all Observers 
			for( iterObsStateList = pObsStateList->begin(); iterObsStateList != pObsStateList->end(); ++iterObsStateList )
			{
				obsTimeStamp =  (*pObsStateList)[iterObsStateList]->timeStamp;
				// To prevent duplicate posts to  observers.
				if(m_currentTimeStamp != obsTimeStamp)
				{
					pObsRequest = (*pObsStateList)[iterObsStateList];
					pObsRequest->pObserver->Update((IFXSubject*)this, changedBits >> pObsRequest->shift, 
																pObsRequest->rInterfaceType );
//					IFXTRACE_GENERIC("Tree  Subject %x    Observer %x   interest %d \n",
//						this,pObsRequest->pObserver,changedBits >> pObsRequest->shift );	
					pObsRequest->timeStamp = m_currentTimeStamp;
				}
			}
		}
	}
	// all observers notified of change so reset subject state
	changedBits = 0;
	m_currentTimeStamp++;
	m_bSafeToModifyList = TRUE;

	if(m_bNeedResolution)
		ResolvePendingAttachments();
}


void NotifyObserverOfShutDown(OBSERVERSTATETREE::iterator iter, void * data)
{		
	IFXSubject * pSubject = (IFXSubject *)data;	
	SIFXObserverRequest* oreq = *iter;
	IFXASSERT(pSubject && oreq);
	if(pSubject && oreq)
	{
		IFXREFIID iid = oreq->rInterfaceType;
		IFXObserver *pObserver = oreq->pObserver;
		pObserver->Update( pSubject, 0, iid );
//		IFXTRACE_GENERIC("InSubject %x,  Observer %x \n",pSubject,pObserver);

		// This will leave dangling ptrs in the tree but we will not doing any operations
		// as the tree will be deleted after all the observers are updated.
		 delete (oreq);
	}
}
