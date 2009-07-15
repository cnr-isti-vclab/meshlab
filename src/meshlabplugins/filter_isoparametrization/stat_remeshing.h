#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/space/triangle3.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/math/histogram.h>

#ifndef STAT_REMESHING
#define STAT_REMESHING

#define PI 3.14159265



//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class MeshType>
typename MeshType::ScalarType MinimumAspectRatio(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	ScalarType res=1.f;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{
			ScalarType test=vcg::QualityRadii((*Fi).P(0),(*Fi).P(1),(*Fi).P(2));
			if (test<res)
				res=test;
		}
	return (res);
}

//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class MeshType>
typename MeshType::ScalarType MinimumArea(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	ScalarType res=10000.f;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{
			ScalarType test=vcg::DoubleArea(*Fi)/2.0;
			if (test<res)
				res=test;
		}
	return (res);
}

//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class MeshType>
typename MeshType::ScalarType MaximumArea(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	ScalarType res=0.f;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{
			ScalarType test=vcg::DoubleArea(*Fi)/2.0;
			if (test>res)
				res=test;
		}
	return (res);
}

//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class FaceType>
typename FaceType::ScalarType MinAngleFace(const FaceType &f)
{
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::CoordType CoordType;
	ScalarType res=360.0;
	for (int i=0;i<3;i++)
	{
                CoordType v0=f.P((i+1)%3)-f.P(i);
                CoordType v1=f.P((i+2)%3)-f.P(i);
		v0.Normalize();
		v1.Normalize();
		ScalarType angle=acos(v0*v1)* 180.0 / PI;
		if (angle<res)
				res=angle;
	}
	assert(res<=60.0);
	return (res);
}

//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class FaceType>
typename FaceType::ScalarType MaxAngleFace(const FaceType &f)
{
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::CoordType CoordType;
	ScalarType res=0;
	for (int i=0;i<3;i++)
	{
                CoordType v0=f.P((i+1)%3)-f.P(i);
                CoordType v1=f.P((i+2)%3)-f.P(i);
		v0.Normalize();
		v1.Normalize();
		ScalarType angle=acos(v0*v1)* 180.0 / PI;
		if (angle>res)
				res=angle;
	}
	return (res);
}

template <class MeshType>
typename MeshType::ScalarType MinAngle(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	ScalarType res=360.0;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{
			ScalarType testAngle=MinAngleFace(*Fi);
			if (testAngle<res)
				res=testAngle;
		}
	return (res);
}

template <class MeshType>
typename MeshType::ScalarType MaxAngle(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	ScalarType res=0;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{
			ScalarType testAngle=MaxAngleFace(*Fi);
			if (testAngle>res)
				res=testAngle;
		}
	return (res);
}

template <class MeshType>
void MaxMinEdge(const MeshType &mesh,typename MeshType::ScalarType &min,
				typename MeshType::ScalarType &max)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;
	min=10000.0;
	max=0.0;
        typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
		{

			for (int i=0;i<3;i++)
			{
                                typename MeshType::VertexType* v0=(*Fi).V(i);
                                typename MeshType::VertexType* v1=(*Fi).V((i+1)%3);
				if (v0>v1)
				{
					ScalarType dist=(v0->P()-v1->P()).Norm();
					if (dist<min)
						min=dist;
					if (dist>max)
						max=dist;
				}
			}
		}
}
////return moltiplication of aspect ratio of faces of the mesh
/////to see if add + normalize is better
//template <class MeshType>
//ScalarType AverageMinAngle(const MeshType &mesh)
//{
//	ScalarType res=0;
//	MeshType::ConstFaceIterator Fi;
//	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
//		if ((!(*Fi).IsD()))
//		{
//			ScalarType testAngle=MinAngleFace(*Fi);
//			res+=testAngle;
//		}
//	return (res/((ScalarType)mesh.fn));
//}

template <class MeshType>
void StatAspectRatio(MeshType &mesh, 
		  typename MeshType::ScalarType &min,
		  typename MeshType::ScalarType &average,
		  typename MeshType::ScalarType &stand_dev)
{
	typedef typename MeshType::ScalarType ScalarType;

	vcg::Histogram<ScalarType> HAspectRatio;
	
	ScalarType minRatio=MinimumAspectRatio(mesh);
	ScalarType maxRatio=1.f;
	HAspectRatio.SetRange(minRatio,maxRatio,500);

	
        typename MeshType::FaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
			HAspectRatio.Add(vcg::QualityRadii((*Fi).P(0),(*Fi).P(1),(*Fi).P(2)));
			
	average=HAspectRatio.Avg();
	stand_dev=HAspectRatio.StandardDeviation();
	min=minRatio;
}

template <class MeshType>
void StatArea(MeshType &mesh, 
		 typename MeshType::ScalarType &min,
		 typename MeshType::ScalarType &max,
		 typename MeshType::ScalarType &average,
		 typename MeshType::ScalarType &stand_dev)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;

	vcg::Histogram<ScalarType> HArea;
	
	ScalarType minArea=MinimumArea(mesh);
	ScalarType maxArea=MaximumArea(mesh);
	HArea.SetRange(minArea,maxArea,500);

	
        typename MeshType::FaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
	{
		CoordType p0=(*Fi).P0(0);
		CoordType p1=(*Fi).P1(0);
		CoordType p2=(*Fi).P2(0);
		ScalarType area=((p1-p0)^(p2-p0)).Norm()/2.0;
		HArea.Add(area);
	}
			
	average=HArea.Avg();
	stand_dev=HArea.StandardDeviation();
	min=minArea;
	max=maxArea;
}

template <class MeshType>
void StatAngle(MeshType &mesh, 
		 typename MeshType::ScalarType &min,
		 typename MeshType::ScalarType &max,
		 typename MeshType::ScalarType &average,
		 typename MeshType::ScalarType &stand_dev)
{

	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;

	vcg::Histogram<ScalarType> HAngle;
	
	ScalarType minAngle=MinAngle(mesh);
	ScalarType maxAngle=MaxAngle(mesh);
	HAngle.SetRange(minAngle,maxAngle,500);

	
        typename MeshType::FaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		HAngle.Add(MinAngleFace((*Fi)));
			
	average=HAngle.Avg();
	stand_dev=HAngle.StandardDeviation();
	min=minAngle;
	max=maxAngle;
}

template <class MeshType>
void StatEdge(MeshType &mesh, 
		 typename MeshType::ScalarType &min,
		 typename MeshType::ScalarType &max,
		 typename MeshType::ScalarType &average,
		 typename MeshType::ScalarType &stand_dev)
{
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;

	vcg::Histogram<ScalarType> HEdge;
	
	ScalarType minEdge;
	ScalarType maxEdge;
	MaxMinEdge(mesh,minEdge,maxEdge);
	HEdge.SetRange(minEdge,maxEdge,500);

	typedef typename MeshType::ScalarType ScalarType;
        typename MeshType::FaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
	{
		for (int i=0;i<3;i++)
		{
                                VertexType* v0=(*Fi).V(i);
                                VertexType* v1=(*Fi).V((i+1)%3);
				if ((v0>v1)||((*Fi).FFp(i)==&(*Fi)))
				{
					ScalarType dist=(v0->P()-v1->P()).Norm();
					HEdge.Add(dist);
				}
		}
	}

	average=HEdge.Avg();
	stand_dev=HEdge.StandardDeviation();
	min=minEdge;
	max=maxEdge;
}

template <class MeshType>
int NumRegular(MeshType &mesh)
{
	typedef typename MeshType::FaceType FaceType;
	///update topology
	vcg::tri::UpdateTopology<MeshType>::VertexFace(mesh);
        typename MeshType::VertexIterator Vi;
	int irregular=0;
	for (Vi=mesh.vert.begin();Vi!=mesh.vert.end();Vi++)
	{
		if ((!(*Vi).IsD())&&(!(*Vi).IsB()))
		{
			vcg::face::VFIterator<FaceType> VFI(&(*Vi));
			int num=0;
			while (!(VFI.End()))
			{
				num++;
				++VFI;
			}
			if (num!=6)
				irregular++;
			}
	}
	return irregular;
}
#endif
