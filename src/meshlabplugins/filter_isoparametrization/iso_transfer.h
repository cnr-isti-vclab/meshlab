#include <iso_parametrization.h>

#include<vcg/simplex/edge/base.h>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/edges.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/algorithms/closest.h>

#ifndef _ISO_TRANSFER
#define _ISO_TRANSFER

class IsoTransfer
{
	typedef vcg::GridStaticPtr<ParamMesh::FaceType, ParamMesh::ScalarType> TriMeshGrid;
	typedef ParamMesh::CoordType CoordType;
	typedef ParamMesh::ScalarType ScalarType;
	TriMeshGrid TRGrid;

	void Clamp(CoordType &bary)
	{
	/*	float eps=0.01;*/
		float sum=0;
		int bigger=0;
		int lower=0;
	/*	for (int i=0;i<3;i++)
		{
			if ((bary.V(i)<eps)&&(bary.V(i)>-eps))
				bary.V(i)=0;
			if ((bary.V(i)<1+eps)&&(bary.V(i)>1-eps))
				bary.V(i)=1;

			sum+=bary.V(i);

			if (bary.V(i)>bary.V(bigger))
				bigger=i;
			if (bary.V(i)<bary.V(lower))
				lower=i;
		}
		assert(bigger!=lower);
		if (sum>(1.0+eps))
		{
			float diff=sum-1.0;
			bary.V(bigger)-=diff;
		}
		else
		if (sum<(1.0-eps))
		{
			float diff=1.0-sum;
			bary.V(lower)+=diff;
		}*/
			for (int i=0;i<3;i++)
		{
			if (bary.V(i)<0)
				bary.V(i)=0;
			if (bary.V(i)>1)
				bary.V(i)=1;

			sum+=bary.V(i);

			if (bary.V(i)>bary.V(bigger))
				bigger=i;
			if (bary.V(i)<bary.V(lower))
				lower=i;
		}
		//assert(bigger!=lower);
		if (sum>(1.0))
		{
			float diff=sum-1.0;
			bary.V(bigger)-=diff;
		}
		else
		if (sum<(1.0))
		{
			float diff=1.0-sum;
			bary.V(lower)+=diff;
		}
	}

	public:
	template <class MeshType>
	void Transfer(IsoParametrization &IsoParam,
								MeshType &to_assing)
	{
		///put the mesh in the grid
		typedef typename MeshType::ScalarType ScalarType;
		vcg::tri::UpdateBounding<ParamMesh>::Box(*IsoParam.ParaMesh());
		vcg::tri::UpdateNormals<ParamMesh>::PerFaceNormalized(*IsoParam.ParaMesh());
		vcg::tri::UpdateNormals<ParamMesh>::PerVertexAngleWeighted(*IsoParam.ParaMesh());
		vcg::tri::UpdateNormals<ParamMesh>::NormalizeVertex(*IsoParam.ParaMesh());
		
		TRGrid.Set(IsoParam.ParaMesh()->face.begin(),IsoParam.ParaMesh()->face.end());
		ScalarType maxDist=IsoParam.ParaMesh()->bbox.Diag();
		///then for each vertex find the closest
    for (size_t i=0;i<to_assing.vert.size();i++)
		{
      typename MeshType::VertexType *vert=&to_assing.vert[i];
			if (!vert->IsD())
			{
				ScalarType dist;
				CoordType closest,norm,bary;
				ParamMesh::FaceType * f=NULL;
				f=GetClosestFaceBase(*IsoParam.ParaMesh(),TRGrid,vert->P(), maxDist,dist,closest);
				vcg::InterpolationParameters<typename ParamMesh::FaceType,typename ParamMesh::ScalarType>(*f,f->N(),closest, bary);
				assert(f!=NULL);
				
				///then find back the coordinates
				if (!((bary.X()>=0)&&(bary.X()<=1)&&
					  (bary.Y()>=0)&&(bary.Y()<=1)&&
						(bary.Z()>=0)&&(bary.Z()<=1)))
				{
            printf("%i,%3.3f,%3.3f,%3.3f",int(i),bary.X(),bary.Y(),bary.Z());
						system("pause");
				}
				Clamp(bary);
				int I;
				vcg::Point2<ScalarType> UV;
				IsoParam.Phi(f,bary,I,UV);
				///and finally set to the vertex
				assert(I>=0);
				vert->T().P()=UV;
				vert->T().N()=I;
				vert->Q()=(typename MeshType::ScalarType)I;
			}
		}
	}

};

#endif
