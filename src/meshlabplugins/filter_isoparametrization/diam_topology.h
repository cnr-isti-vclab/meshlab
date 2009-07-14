#include <algorithm>
#include <time.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/space/color4.h>
#include <vcg/space/intersection2.h>

#ifndef _DIAMONDTOPOLOGY
#define _DIAMONDTOPOLOGY

class DiamTopology
{
	typedef struct DiamondPatch
	{
		int FFp[4];
		int FFi[4];

		AbstractFace *f;
		int edgeInd;

		DiamondPatch(){
			FFp[0]=-1;
			FFp[1]=-1;
			FFp[2]=-1;
			FFp[3]=-1;
			FFi[0]=-1;
			FFi[1]=-1;
			FFi[2]=-1;
			FFi[3]=-1;
		}
	};

	std::vector<DiamondPatch> patches;
	IsoParametrization *isoParam;
	typedef std::pair<AbstractFace*,int> TriEdge;

	std::vector<DiamondPatch> DDAdiacency;
	std::map<TriEdge,int> edgeMap;
	
	void InsertEdges()
	{
		//insert all the edges
		AbstractMesh *domain=isoParam->AbsMesh();
		int index=0;
		for (int i=0;i<domain->fn;i++)
		{
			AbstractFace *f0=&(domain->face[i]);
			if (!f0->IsD())
			{
				for(int j=0;j<3;j++)
				{
					AbstractFace * f1=f0->FFp(j);
					if (f1>f0)
					{
						DDAdiacency.push_back(DiamondPatch());
						DDAdiacency[index].f=f0;
						DDAdiacency[index].edgeInd=j;
						TriEdge key=TriEdge(f0,j);
						edgeMap.insert(std::pair<TriEdge,int>(key,index));
						index++;
					}
				}
			}

		}
	}



	void SetTopology()
	{

		///data used to calculate topology
		typedef std::pair<AbstractVertex*,AbstractFace*> DiamEdge;
		struct InfoAdiacency
		{
			int index_global;
			int index_local;
		};

		std::map<DiamEdge,InfoAdiacency> EdgeDiamMap;

		for (unsigned int i=0;i<DDAdiacency.size();i++)
		{
			int indexEdge=DDAdiacency[i].edgeInd;
			AbstractFace* face=DDAdiacency[i].f;
			AbstractVertex *v0=face->V0(indexEdge);
			AbstractVertex *v1=face->V1(indexEdge);
			AbstractFace *f0=face;
			AbstractFace *f1=face->FFp(indexEdge);

			///set right order
			if (v1<v0)
				std::swap(v0,v1);
			if (f1<f0)
				std::swap(f0,f1);

			///then put in the map the variuos edge
			DiamEdge de[4];
			de[0]=DiamEdge(v0,f0);
			de[1]=DiamEdge(v1,f0);
			de[2]=DiamEdge(v1,f1);
			de[3]=DiamEdge(v0,f1);

			for (int j=0;j<4;j++)
			{
				///find if already exist
				std::map<DiamEdge,InfoAdiacency>::iterator ItMap=EdgeDiamMap.find(de[j]);

				///first time insert in the table
				if (ItMap==EdgeDiamMap.end())
				{
					InfoAdiacency InfAd;
					InfAd.index_global=i;
					InfAd.index_local=j;
					EdgeDiamMap.insert(std::pair<DiamEdge,InfoAdiacency>(de[j],InfAd));
				}
				else
				{
					int index_global=(*ItMap).second.index_global;
					int index_local =(*ItMap).second.index_local;
					DDAdiacency[index_global].FFp[index_local]=i;
					DDAdiacency[index_global].FFi[index_local]=j;
					DDAdiacency[i].FFp[j]=index_global;
					DDAdiacency[i].FFi[j]=index_local;
				}
			}
		}
	}

	bool TestTopology()
	{
		for (unsigned int i=0;i<DDAdiacency.size();i++)
		{
			for (int j=0;j<4;j++)
			{
				int index_global=DDAdiacency[i].FFp[j];
				int index_local=DDAdiacency[i].FFi[j];
				if ((index_global==-1)||(index_local==-1))
					return false;

				int opposite0=DDAdiacency[index_global].FFp[index_local];
				int index_local1=DDAdiacency[index_global].FFi[index_local];
				///int opposite1=DDAdiacency[i].FFp[index_local];
				if (opposite0!=i)
					return false;
				if (index_local1!=j)
					return false;
			}
		}
		return true;
	}
	

public:
	

	///initialize the parameterization
	void Init(IsoParametrization *_isoParam)
	{
	
		isoParam=_isoParam;

		///initialize edges
		InsertEdges();

		///then set topology
		SetTopology();

		assert(TestTopology());

	}

};
#endif