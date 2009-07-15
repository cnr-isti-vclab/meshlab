#include <iso_parametrization.h>

#ifndef _ISO_STATISTICS
#define _ISO_STATISTICS

template <class ScalarType>
ScalarType geomAverage(const ScalarType &val0,
						   const ScalarType &val1,
						   const ScalarType &exp0,
						   const ScalarType &exp1)
{
	ScalarType v0=pow(val0,exp0);
	ScalarType v1=pow(val1,exp1);
	ScalarType ret=pow((ScalarType)(v0*v1),(ScalarType)(1.0/(exp0+exp1)));
	return (ret);
}

//template <class MeshType>
class Statistic
{
	IsoParametrization *Isoparam;
	typedef IsoParametrization::ScalarType ScalarType;
	typedef IsoParametrization::CoordType CoordType;
	ScalarType Area3d;
	ScalarType AbstractArea;

public:

	void AreaDistorsion(ParamFace *f,ScalarType &distorsion,ScalarType &area_3d)
	{
		const float epsilon=0.000001f;
		const float maxRatio=10.f;
		int indexDomain;
		vcg::Point2f UV0,UV1,UV2;
		/*int domainType=*/Isoparam->InterpolationSpace(f,UV0,UV1,UV2,indexDomain);

		
		area_3d=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm()/(ScalarType)2.0;
		ScalarType area_2d=((UV1-UV0)^(UV2-UV0))/(ScalarType)2.0;
		area_3d/=Area3d;
		area_2d/=AbstractArea;

		if (fabs(area_2d)<epsilon)area_2d=epsilon;
		if (fabs(area_3d)<epsilon)area_3d=epsilon;

		ScalarType r0=area_3d/area_2d;
		ScalarType r1=area_2d/area_3d;
		if (r0>maxRatio)r0=maxRatio;
		if (r1>maxRatio)r1=maxRatio;

		distorsion=((r0+r1)/(ScalarType)2.0)-(ScalarType)1.0;
	}
	
	void AngleDistorsion(ParamFace *f,ScalarType &distortion)
	{
		const float epsilon=0.000001f;
		ScalarType area_3d=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm();
		int indexDomain;
		vcg::Point2f UV0,UV1,UV2;
		/*int domainType=*/Isoparam->InterpolationSpace(f,UV0,UV1,UV2,indexDomain);
		ScalarType area_2d=fabs((UV1-UV0)^(UV2-UV0));
		ScalarType a2=(f->P(1)-f->P(0)).SquaredNorm();
		ScalarType b2=(f->P(2)-f->P(1)).SquaredNorm();
		ScalarType c2=(f->P(0)-f->P(2)).SquaredNorm();
		ScalarType cot_a=((UV2-UV1)*(UV0-UV2));
		ScalarType cot_b=((UV0-UV2)*(UV1-UV0));
		ScalarType cot_c=((UV1-UV0)*(UV2-UV1));

		ScalarType num;
		if ((fabs(area_2d)<epsilon)||(fabs(area_3d)<epsilon))
		{
			distortion=0;
			num=0;
		}
		else
		{
			num=(cot_a*a2+cot_b*b2+cot_c*c2)/area_2d;
			distortion=fabs(num/(Area3d*(ScalarType)4.0));
			assert(distortion<1);
		}
	}

	ScalarType AreaDistorsion()
	{
		ScalarType sum=0;
		ParamMesh *param_mesh=Isoparam->ParaMesh();
		for (unsigned int i=0;i<param_mesh->face.size();i++)
		{
			float area3d=0;
			ScalarType distorsion=0;
			AreaDistorsion(&param_mesh->face[i],distorsion,area3d);
			sum+=distorsion*area3d;
		}
		return(fabs(sum));
	}
	
	ScalarType AngleDistorsion()
	{
		ScalarType sum=0;
		ParamMesh *param_mesh=Isoparam->ParaMesh();
		for (unsigned int i=0;i<param_mesh->face.size();i++)
		{
			ScalarType distorsion=0;
			AngleDistorsion(&param_mesh->face[i],distorsion);
			sum+=distorsion;
		}
		return ((sum)-(ScalarType)1.0);
	}

	ScalarType AggregateDistorsion()
	{
		ScalarType d0=AreaDistorsion();
		ScalarType d1=AngleDistorsion();
		ScalarType ret=geomAverage<ScalarType>(d0+(ScalarType)1.0,d1+(ScalarType)1.0,3,1)-(ScalarType)1;
		return ret;
	}

	void PrintAttributes()
	{
		printf("\n STATISTICS \n"),
		printf("AREA	   distorsion:%lf ;\n",AreaDistorsion());
		printf("ANGLE	   distorsion:%lf ;\n",AngleDistorsion());
		printf("AGGREGATE  distorsion:%lf ;\n",AggregateDistorsion());
		printf("L2 STRETCH efficiency:%lf ;\n",L2Error());
	}


	ScalarType  L2Error()
	{
		///equilateral triagle
		vcg::Point2f p0(-0.5,0.0);
		vcg::Point2f p1(0.5,0.0);
		vcg::Point2f p2(0,0.866025f);
		ParamMesh::FaceIterator Fi;

		float sum=0;
		float A3dtot=0;
		float A2dtot=0;
		ParamMesh *param_mesh=Isoparam->ParaMesh();
		for (Fi=param_mesh->face.begin();Fi!=param_mesh->face.end();Fi++)
		{
			if (!(*Fi).IsD())
			{
				ParamFace *f=&(*Fi);
				CoordType q1=(*Fi).V(0)->P();
				CoordType q2=(*Fi).V(1)->P();
				CoordType q3=(*Fi).V(2)->P();
				///map to equilateral triangle
				/*vcg::Point2f UV1=p0*(*Fi).V(0)->Bary.X()+p1*(*Fi).V(0)->Bary.Y()+p2*(*Fi).V(0)->Bary.Z();
				vcg::Point2f UV2=p0*(*Fi).V(1)->Bary.X()+p1*(*Fi).V(1)->Bary.Y()+p2*(*Fi).V(1)->Bary.Z();
				vcg::Point2f UV3=p0*(*Fi).V(2)->Bary.X()+p1*(*Fi).V(2)->Bary.Y()+p2*(*Fi).V(2)->Bary.Z();*/
				vcg::Point2f UV1,UV2,UV3;
				int indexDomain;
				/*int domainType=*/Isoparam->InterpolationSpace(f,UV1,UV2,UV3,indexDomain);
				ScalarType s1=UV1.X();
				ScalarType t1=UV1.Y();
				ScalarType s2=UV2.X();
				ScalarType t2=UV2.Y();
				ScalarType s3=UV3.X();
				ScalarType t3=UV3.Y();
				ScalarType A=fabs(((s2-s1)*(t3-t1)-(s3-s1)*(t2-t1))/(ScalarType)2.0);
				if (A<(ScalarType)0.00001)
					A=(ScalarType)0.00001;
				ScalarType A3d=((q2 - q1) ^ (q3 - q1)).Norm()/(ScalarType)2.0;
				A3dtot+=A3d;
				A2dtot+=A;
				CoordType Ss=(q1*(t2-t3)+q2*(t3-t1)+q3*(t1-t2))/((ScalarType)2.0*A);
				CoordType St=(q1*(s3-s2)+q2*(s1-s3)+q3*(s2-s1))/((ScalarType)2.0*A);
				ScalarType a=Ss*Ss;
				/*ScalarType b=Ss*St;*/
				ScalarType c=St*St;
				ScalarType L2=sqrt((a+c)/(ScalarType)2.0);
				sum+=L2*L2*A3d;
			}
	}
	sum=sqrt(sum/A3dtot)*sqrt(A2dtot/A3dtot);
	return sum;
	}

	void Init(IsoParametrization *_Isoparam)
	{
		float fix_num=sqrt((ScalarType)3.0)/(ScalarType)4.0;

		Isoparam=_Isoparam;
		ParamMesh *param_mesh=Isoparam->ParaMesh();
		AbstractMesh *abstract_mesh=Isoparam->AbsMesh();

		Area3d=vcg::tri::Stat<ParamMesh>::ComputeMeshArea(*param_mesh);
		AbstractArea=(ScalarType)abstract_mesh->fn*fix_num;
	}
};

template <class MeshType>
typename MeshType::ScalarType ApproxAreaDistortion(MeshType &mesh,const int &num_face)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::ScalarType ScalarType;

	const ScalarType maxRatio=10;
	const ScalarType epsilon=(ScalarType)0.000001;

	ScalarType sum=0,div=0;
	ScalarType area_tot=Area<MeshType>(mesh);

	for (unsigned int i=0;i<mesh.face.size();i++)
	{
		FaceType* f=&mesh.face[i];
		if ((f->V(0)->father==f->V(1)->father)&&(f->V(1)->father==f->V(2)->father))
		{
			ScalarType area_3d=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm()/area_tot;
			ScalarType area_2d=fabs(AreaUV<FaceType>(*f)/((ScalarType)num_face));
			if (fabs(area_2d)<epsilon)area_2d=epsilon;
			if (fabs(area_3d)<epsilon)area_3d=epsilon;
			ScalarType r0=area_3d/area_2d;
			ScalarType r1=area_2d/area_3d;
			if (r0>maxRatio)r0=maxRatio;
			if (r1>maxRatio)r1=maxRatio;
			sum+=area_3d*(r0+r1);
			div+=area_3d;
		}
	}
	return (sum/(div*2))-1.0;
}

template <class MeshType>
typename MeshType::ScalarType ApproxAngleDistortion(MeshType &mesh)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::ScalarType ScalarType;

	//const ScalarType maxRatio=4;
	const ScalarType epsilon=(ScalarType)0.000001;

	ScalarType sum=0,div=0;
	//ScalarType area_tot=Area<MeshType>(mesh);
	vcg::Point2<ScalarType> x_axis(0.5,sqrt(3.0)/2.0);
	vcg::Point2<ScalarType> y_axis(1.0,0);
	for (unsigned int i=0;i<mesh.face.size();i++)
	{
		FaceType* f=&mesh.face[i];
		if ((f->V(0)->father==f->V(1)->father)&&(f->V(1)->father==f->V(2)->father))
		{
			ScalarType area_3d=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm();
			vcg::Point2<ScalarType> t0=f->V(0)->Bary.X()*x_axis+f->V(0)->Bary.Y()*y_axis;
			vcg::Point2<ScalarType> t1=f->V(1)->Bary.X()*x_axis+f->V(1)->Bary.Y()*y_axis;
			vcg::Point2<ScalarType> t2=f->V(2)->Bary.X()*x_axis+f->V(2)->Bary.Y()*y_axis;
			ScalarType area_2d=fabs((t1-t0)^(t2-t0));
			

			ScalarType a2=(f->P(1)-f->P(0)).SquaredNorm();
			ScalarType b2=(f->P(2)-f->P(1)).SquaredNorm();
			ScalarType c2=(f->P(0)-f->P(2)).SquaredNorm();
			ScalarType cot_a=((t2-t1)*(t0-t2));
			ScalarType cot_b=((t0-t2)*(t1-t0));
			ScalarType cot_c=((t1-t0)*(t2-t1));

			ScalarType num;
			if ((fabs(area_2d)<epsilon)||(fabs(area_3d)<epsilon))
				num=0;
			else
				 num=(cot_a*a2+cot_b*b2+cot_c*c2)/area_2d;

			sum+=num;
			//assert(num>=2.0*area_3d);
			div+=area_3d;
		}
	}
	return (fabs(sum)/(div*2)-1.0);
}

template <class MeshType>
typename MeshType::ScalarType ApproxL2Error(MeshType &mesh)
	{
		typedef typename MeshType::ScalarType ScalarType;
		typedef typename MeshType::CoordType CoordType;
		///equilateral triagle
		vcg::Point2f p0(-0.5,0.0);
		vcg::Point2f p1(0.5,0.0);
		vcg::Point2f p2(0,0.866025f);
                typename MeshType::FaceIterator Fi;

		ScalarType sum=0;
		ScalarType A3dtot=0;
		ScalarType A2dtot=0;
		for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		{
			if (!(*Fi).IsD())
			{
				if (((*Fi).V(0)->father==(*Fi).V(1)->father)&&
					((*Fi).V(1)->father==(*Fi).V(2)->father))
				{
					CoordType q1=(*Fi).V(0)->RPos;
					CoordType q2=(*Fi).V(1)->RPos;
					CoordType q3=(*Fi).V(2)->RPos;
					///map to equilateral triangle
					vcg::Point2f UV1=p0*(*Fi).V(0)->Bary.X()+p1*(*Fi).V(0)->Bary.Y()+p2*(*Fi).V(0)->Bary.Z();
					vcg::Point2f UV2=p0*(*Fi).V(1)->Bary.X()+p1*(*Fi).V(1)->Bary.Y()+p2*(*Fi).V(1)->Bary.Z();
					vcg::Point2f UV3=p0*(*Fi).V(2)->Bary.X()+p1*(*Fi).V(2)->Bary.Y()+p2*(*Fi).V(2)->Bary.Z();
					ScalarType s1=UV1.X();
					ScalarType t1=UV1.Y();
					ScalarType s2=UV2.X();
					ScalarType t2=UV2.Y();
					ScalarType s3=UV3.X();
					ScalarType t3=UV3.Y();
					ScalarType A=fabs(((s2-s1)*(t3-t1)-(s3-s1)*(t2-t1))/2.0);
					if (A<0.00001)
						A=(ScalarType)0.00001;
					ScalarType A3d=((q2 - q1) ^ (q3 - q1)).Norm()/2.0;
					A3dtot+=A3d;
					A2dtot+=A;
					CoordType Ss=(q1*(t2-t3)+q2*(t3-t1)+q3*(t1-t2))/(2.0*A);
					CoordType St=(q1*(s3-s2)+q2*(s1-s3)+q3*(s2-s1))/(2.0*A);
					ScalarType a=Ss*Ss;
					/*ScalarType b=Ss*St;*/
					ScalarType c=St*St;
					ScalarType L2=sqrt((a+c)/2.0);
					sum+=L2*L2*A3d;
				}
			}
		}
		sum=sqrt(sum/A3dtot)*sqrt(A2dtot/A3dtot);
		return sum;
	}

#endif
