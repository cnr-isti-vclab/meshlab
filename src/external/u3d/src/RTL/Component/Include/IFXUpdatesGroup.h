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

#ifndef IFXUpdatesGroup_h
#define IFXUpdatesGroup_h


#include "IFXUpdates.h"
#include "IFXDebug.h"
#include <memory.h>


/**
The IFXUpdatesGroup interface groups clodolution updates. The update group is 
typically associated with a mesh group in parallel array fashion.

The IFXUpdatesGroup and IFXUpdates structures are integral elements of the material 
partitioned and vertex array organization of the IFXMeshGroup and IFXMesh structures. 
Vertices that lie on a material boundary are shared by at least two faces with different 
material associations. These vertices are replicated and exist in the IFXMesh objects 
for each of the two materials. Changes in vertex resolution are done in terms of "units" 
rather than vertices. For vertices on the interior of an IFXMesh, a unit constitutes one 
vertex. However, for those vertices shared by more than one IFXMesh, the unit represents 
all of those shared vertices. The bookkeeping that maps units to vertices is kept in the 
"sync" tables structure owned by the IFXUpdatesGroup object.

All of the internal bookkeeping is managed transparently to the user if the IFXCLODManager 
class is used for changing the resolution of the mesh.
*/
class IFXUpdatesGroup
{
public:
	U32 AddRef();
	U32 Release();
	/**This method gets the number of update records. 
	
		@return	U32
	*/
	U32 GetNumUpdates() 
	{ 
		return m_size;
	};
	
	/**Gets the update records.
	
		@param	index	Index number for update record. May return NULL.
		
		@return	IFXUpdates*
	*/
	IFXUpdates* GetUpdates(U32 index) { return m_updates[index]; };
	/**Returns the update records passed in.
	
		@param	index	Index number for update record. 
		@param	pUpdates	Pointer to IFXUpdates
		@return	IFXUpdates*
	*/
	IFXUpdates* SetUpdates(U32 index, IFXUpdates* pUpdates);
	
	/**This method sets data that allows resolution of each material to synchronize.
		
		 @param	index	Index number for update record
		
		@return	U32	
	*/
	U32* GetSyncTable(U32 index) { return m_synchTables[index]; };
	/**This method gets data that allows resolution of each material to synchronize.
		
		@param	index	Index number for update record
		@param	pTable	Pointer to the Sync table

		@return	U32	
	*/
	U32* SetSyncTable(U32 index, U32* pTable);
	/**Allocates the number of records.
	
		@param	numUpdates	Number of update records

		@return	U32
	
	*/
	IFXRESULT Allocate(U32 numUpdates)
	{	
		IFXRESULT returnCode = IFX_OK;
		IFXASSERT(m_updates == NULL  &&  m_synchTables == NULL);

		if (m_updates)
		{
			delete [] m_updates;
			m_updates = NULL;
		}

		if (m_synchTables)
		{
			delete [] m_synchTables;
			m_synchTables = NULL;
		}

		m_updates	  = new IFXUpdates*[numUpdates];
		m_synchTables = new U32*[numUpdates];

		if(m_updates && m_synchTables)
		{	
			memset(m_updates,0,numUpdates*sizeof(IFXUpdates*));
			memset(m_synchTables,0,numUpdates*sizeof(U32*));
			m_size = numUpdates;
		} 
		else 
		{
			if (m_updates)
			{
				delete [] m_updates;
				m_updates = NULL;
			}
			
			if (m_synchTables)
			{
				delete [] m_synchTables;
				m_synchTables = NULL;
			}
			returnCode = IFX_E_OUT_OF_MEMORY; 
		}
		return returnCode;
	};
	
	// perhaps neighbor mesh will need to add to this.
	//IFXRESULT GetInterface(U32 interfaceIdentifier, void** pInterface) { *pInterface = NULL; return IFX_FAIL; };

	/**Class constructor. */
	IFXUpdatesGroup()
	{
		m_RefCnt				= 1;
		m_size					= 0;
		m_updates				= NULL;
		m_synchTables 			= NULL;
		m_maxResolution 		= 0;
		m_finalMaxResolution	= 0;
	};
	
	// think about deep unallocate
	//~IFXUpdatesGroup() { delete [] m_updates; delete [] m_synchTables; };
	/**Class destructor. */
	~IFXUpdatesGroup() 
	{ 
		IFXASSERT(m_RefCnt == 0);
		if(m_updates)
		{
			U32 i;
			for(i=0; i < m_size; i++) {
				if(m_updates[i])
				{
					delete m_updates[i];
					m_updates[i] = NULL;
				}
			}
			delete [] m_updates;
			m_updates = NULL;
		}
		
		if(m_synchTables) 
		{
			U32 i;
			for(i=0; i<m_size; i++) {
				if(m_synchTables[i]) 
				{
					delete m_synchTables[i];
					m_synchTables[i] = NULL;
				}
			}
			delete [] m_synchTables;
			m_synchTables = NULL;
		}

		m_size					= 0;
		m_maxResolution 		= 0;
		m_finalMaxResolution	= 0;

	};
	/**This method gets the maximum resolution value.
	
		@return	U32
	*/	
	U32 	GetMaxResolution		();
	/**This method gets gets the final maximum resolution value.
	
		@return	U32
	*/
	U32 	GetFinalMaxResolution	();
	/**This method sets the maximum resolution.
	
		@param	max	Maximum resolution value

		@return	U32
	*/
	void	SetMaxResolution		(U32 max);
	/**This method sets the final maximum resolution.
	
		@param	max	Final maximum resolution value

		@return	U32
	*/
	void	SetFinalMaxResolution	(U32 max);

private:
	U32 m_RefCnt;


	U32				m_size;
	IFXUpdates		**m_updates;  // array of pointers to IFXUpdate 
	U32				**m_synchTables;

	U32 m_maxResolution;	// Maximum resolution allowed given the amount
						// of update records we currently have.  This
						// value increases as update records are streamed
						// progressively down the internet.

	U32 m_finalMaxResolution;	// Maximum resolution when file is 
								// completely downloaded.  Used for
								// allocation purposes.

};

IFXINLINE U32 IFXUpdatesGroup::AddRef()
{
	return ++m_RefCnt;
}

IFXINLINE U32 IFXUpdatesGroup::Release()
{
	if(m_RefCnt == 1)
	{
		m_RefCnt = 0;
		delete this;
		return 0;
	}
	return --m_RefCnt;
}


IFXINLINE U32 IFXUpdatesGroup::GetMaxResolution()
{
	return m_maxResolution;
}


IFXINLINE U32 IFXUpdatesGroup::GetFinalMaxResolution()
{
	return m_finalMaxResolution;
}


IFXINLINE void IFXUpdatesGroup::SetMaxResolution(U32 max)
{
	m_maxResolution = max;
}


IFXINLINE void IFXUpdatesGroup::SetFinalMaxResolution(U32 max)
{
	m_finalMaxResolution = max;
}


IFXINLINE IFXUpdates* IFXUpdatesGroup::SetUpdates(U32 index, IFXUpdates* pUpdates)
{ 
	// Delete existing updates:
	if (m_updates[index] && (pUpdates != m_updates[index]))
	{
		delete m_updates[index];
		m_updates[index] = NULL;
	}

	return m_updates[index] = pUpdates; 
}


IFXINLINE U32* IFXUpdatesGroup::SetSyncTable(U32 index, U32* pTable) 
{
	if( m_synchTables[index] && (m_synchTables[index] != pTable) )
	{
		delete [] ( m_synchTables[index] );
		m_synchTables[index] = NULL;
	}
	return m_synchTables[index] = pTable; 
}

#endif
