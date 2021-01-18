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
	@file IFXMeshGroup_Character.h
*/

#ifndef IFXMESHGROUP_CHARACTER_H
#define IFXMESHGROUP_CHARACTER_H

#include "IFXCharacter.h"
#include "IFXMeshGroup_Impl.h"

/**
	presumes a single mesh GEO file, multi-mesh may misbehave

	The mulitple materials of the single mesh geo are converted into
	multiple meshes of the IFXMeshGroup.
	The weights loaded in the bone file are mapped into these new vertices.

*/
class IFXMeshGroup_Character : public IFXCharacter
	{
	class IFXMG_CharShare
		{
		public:
								IFXMG_CharShare(void)   { Reset(); };
			void                Reset(void)
								{
									m_lastinput=NULL;
									m_in_allocated=false;
									m_out_allocated=false;
									m_inmeshgroup=NULL;
									m_outmeshgroup=NULL;
									};

			U32                 m_count;
			IFXMeshGroup        *m_lastinput;

			bool                m_in_allocated;
			IFXMeshGroup        *m_inmeshgroup;
			IFXMeshGroup_Impl   m_impl_in;

			bool                m_out_allocated;
			IFXMeshGroup        *m_outmeshgroup;
			IFXMeshGroup_Impl   m_impl_out;
		};


public:
	IFXMeshGroup_Character(IFXMeshGroup_Character *pCloneOf=NULL)
		:IFXCharacter(pCloneOf)
	{
		Initialize();

		if(pCloneOf)
		{
			m_share=pCloneOf->m_share;
			m_share->m_count++;
		}
		else
		{
			m_share=new IFXMG_CharShare();
			m_share->m_count=1;
		}
	};

	void    Initialize(void)
	{
		m_vertexmapgroup=NULL;
		m_manager=NULL;
	};

virtual ~IFXMeshGroup_Character(void)
	{
		if(!(--m_share->m_count))
		{
			FreeInMesh();
			FreeOutMesh();
			delete m_share;
		}
	};

	IFXRESULT   AdoptMeshGroup(IFXMeshGroup *mg,IFXVertexMapGroup *vmg);

	IFXRESULT   CleanupWeights(bool regenerate,bool removerogueweights,
						   bool smooth,F32 threshhold,F32 weldmax,F32 modelsize);

		void        SetBonesManager(IFXBonesManager* set)
													{ m_manager=set;};
		IFXBonesManager* GetBonesManager(void)
													{ return m_manager; };

		IFXMeshGroup* GetLastInput(void)    { return m_share->m_lastinput; };
		void            SetLastInput(IFXMeshGroup *set)
											{ m_share->m_lastinput=set; };

		IFXMeshGroup* GetInputMeshGroup(void)
											{ return m_share->m_inmeshgroup; };
		IFXMeshGroup* GetOutputMeshGroup(void)
											{ return m_share->m_outmeshgroup; };

		void GetBackmapIndex(U32 mesh, U32 vertex, U32* mesh0, U32* vertex0)
		{
			IFXASSERT(mesh<m_backmap.GetNumberElements());
			IFXASSERT(vertex<m_backmap[mesh].GetNumberElements());
			m_backmap[mesh][vertex].GetMeshVertex(mesh0,vertex0);
		};

		// uses m_backmap and vertexmap to find 0th replication
		virtual IFXRESULT IFXAPI    GetMasterMeshIndex(U32 mesh,U32 vertex,
			U32 *mesh0,U32 *vertex0);

private:
	void        SetAutoInfluence(void);
	void        AttachMeshes(void);
	void        AllocInMesh(void);
	void        FreeInMesh(void);
	void        AllocOutMesh(void);
	void        FreeOutMesh(void);
	IFXRESULT   AdoptVertexMap(void);

	void        FindOverlap(F32 weldmax);
	void        PrintIFXMeshGroup(IFXMeshGroup_Impl* img);
	IFXMG_CharShare                 *m_share;
	IFXVertexMapGroup               *m_vertexmapgroup;
	IFXArray<IFXMeshVertexArray>    m_backmap;

	IFXBonesManager                 *m_manager;
};

#endif
