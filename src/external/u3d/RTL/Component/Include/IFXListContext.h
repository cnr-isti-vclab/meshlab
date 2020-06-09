
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

#ifndef __IFXListContext_h__
#define __IFXListContext_h__

#include "IFXDebug.h"
#include "IFXListNode.h"

//* verify pointer usage with lists
#define	IFXLIST_STORE_CHECKPOINTER	TRUE

#define IFXLIST_CHECKPOINTER	(defined(_DEBUG) && IFXLIST_STORE_CHECKPOINTER)


/**************************************************************************/
/**
	Do not access members except through IFXList.

	IFXListContext is similar to an iterator except that it does not contain
	a pointer to an IFXList<>.  All access to an IFXList<> is done directly
	through IFXList<> member functions with an IFXListContext argument.
	This means that
	<UL>
		<LI> IFXListContext is generic and does not need to be templated.
		<LI> The context is very lightweight and has no template
				instantiation.
		<LI> No special permissions are needed for context to list accesses.
		<LI> Your code always explicitly says which list it is accessing.
	</UL>
*/
/***************************************************************************/
/**IFXListContext is similar to an iterator except that it does not contain
	a pointer to an IFXList.  All access to an IFXList is done directly
	through IFXList member functions with an IFXListContext argument.*/
class IFXListContext
{
	public:
						IFXListContext(void)				{ Init(); }

						/** Copies constructor.*/
						IFXListContext(IFXListContext &operand)
						{
							Init();
							SetCurrent(operand.GetCurrent());
							SetAtTail(operand.GetAtTail());
						}

virtual					~IFXListContext(void)
						{
							if(m_current)
								m_current->DecReferences();
						}

						/** Copies state from another context. */
		IFXListContext	&operator=(IFXListContext &operand)
						{
							SetCurrent(operand.GetCurrent());
							SetAtTail(operand.GetAtTail());
							return *this;
						}


						/** (internal) Reinitializes state.*/
		void			Init(void)
						{
							m_current=NULL;
							m_at_tail=false;
#if IFXLIST_CHECKPOINTER
							m_pCoreList=NULL;
#endif
						}

						/** (internal) Gets current position on list.*/
		IFXListNode		*GetCurrent(void)
						{
							CheckValid();
							return m_current;
						}

						/** (internal) Sets current position on list.*/
		void			SetCurrent(IFXListNode *set)
						{
							if(m_current)
								m_current->DecReferences();
							if(set)
							{
								IFXASSERT(set->GetValid());
								set->IncReferences();
							}
							m_current=set;
						}

						/** (internal) Sets whether context is at tail of list.*/
		void			SetAtTail(bool set)					{ m_at_tail=set; };

						/** (internal) Gets whether context is at tail of list.*/
		bool			GetAtTail(void)
						{
							return m_at_tail;
						}


	private:
		void			CheckValid(void)
						{
							// if m_current is fine
							if(!m_current || m_current->GetValid())
								return;

							// find heir
							IFXListNode *heir=m_current;
							while(heir && !heir->GetValid())
								heir=heir->GetHeir();

							// if good heir, copy
							if(heir && heir->GetValid())
							{
//								printf("heir %p\n",heir);
								SetCurrent(heir);
								SetAtTail(false);
							}
							// reset, reset
							else
							{
								SetCurrent(NULL);
								SetAtTail(false);
							}
						}

		IFXListNode		*m_current;
		bool			m_at_tail;

#if	IFXLIST_CHECKPOINTER
	public:
						/** (internal) Stores pointer to current list for debug.*/
		void			SetCorePointer(const IFXCoreList *pSet)
													{ m_pCoreList=pSet; };

						/** (internal) Verifies that we are accessing the
							correct list. */
		bool			CheckCorePointer(const IFXCoreList *pCheck) const
							{
							if(m_pCoreList && m_pCoreList!=pCheck)
								IFXTRACE_GENERIC(L"IFXListContext::CheckCorePointer (%p) does not match %p\n",
								pCheck,m_pCoreList);
							IFXASSERT(!m_pCoreList || m_pCoreList==pCheck);
							return (!m_pCoreList || m_pCoreList==pCheck);
							};
	private:
		const IFXCoreList	*m_pCoreList;
#endif
};

#endif /* __IFXListContext_h__ */
