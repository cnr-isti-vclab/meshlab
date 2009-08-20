#ifndef _ISO_TANGENTSPACE
#define _ISO_TANGENTSPACE
#include <iso_parametrization.h>

class TangentSpace{
	typedef IsoParametrization::CoordType CoordType;
	typedef IsoParametrization::ScalarType ScalarType;
	IsoParametrization *isoParam;
public:

	

	///initialize the sampler 
	void Init(IsoParametrization *_isoParam)
	{
		isoParam=_isoParam;
	}
	
	///given an initial position in parametric space (I0,bary0)
	///and a 2D vector (vect) expressed in parametric space modify the final 
	///position (I1,bary1) abd return true if everithing was ok, false otherwise
	bool Sum(const int &I0,const vcg::Point2<ScalarType> &bary0,
			 const vcg::Point2<ScalarType> &vect,
			 int &I1,vcg::Point2<ScalarType> &bary1,int &domain)
	{
		vcg::Point2<ScalarType> dest=bary0+vect;
		ScalarType alpha=dest.X();
		ScalarType beta=dest.Y();
		///point inside the face
		if ((alpha>=0)&&(alpha<=1)&&(beta>=0)&&(beta<=1)&&((alpha+beta)<=1))
		{
			bary1=dest;
			I1=I0;
			domain=0;
			return true;
		}
		
		
		///control edges
		int edge=-1;
		if ((alpha<=1)&&(beta<=1)&&((alpha+beta)>=1))
			edge=0;
		else
		if ((alpha<=0)&&(beta<=1)&&((alpha+beta)>=0))
			edge=1;
		else
		if ((alpha<=1)&&(beta<=0)&&((alpha+beta)>=0))
			edge=2;
		if (edge!=-1)
		{
			int DiamIndex=isoParam->GetDiamond(I0,edge);
			vcg::Point2<ScalarType> UVDiam;
			///transform to diamond coordinates
			isoParam->GE1(I0,dest,DiamIndex,UVDiam);
			///trasform back to I,alpha,beta
			isoParam->inv_GE1(DiamIndex,UVDiam,I1,bary1);
			domain=1;
			return true;
		}
		int star=-1;
		ScalarType gamma=(1-alpha-beta);
		if ((alpha>beta)&&(alpha>gamma))
			star=0;
		else
		if ((beta>alpha)&&(beta>gamma))
			star=1;
		else
			star=2;

		///get the index of star
		int StarIndex=isoParam->GetStarIndex(I0,star);
		vcg::Point2<ScalarType> UVHstar;
		///transform to UV
		bool found=isoParam->GE0(I0,dest,StarIndex,UVHstar);
		///trasform back to I,alpha,beta
		if (!found)
			return false;
		found=isoParam->inv_GE0(StarIndex,UVHstar,I1,bary1);
		/*AbstractFace* f0=&isoParam->AbsMesh()->face[I0];
			AbstractFace* f1=&isoParam->AbsMesh()->face[I1];
			AbstractVertex *v0=f0->V(0);
			AbstractVertex *v1=f0->V(1);
			AbstractVertex *v2=f0->V(2);
			AbstractVertex *v3=f1->V(0);
			AbstractVertex *v4=f1->V(1);
			AbstractVertex *v5=f1->V(2);*/
		domain=2;
		return found;
	}
	
	///given two positions in parametric space (I0,bary0) and (I1,bary1)
	///modify the 2D vector (vect) and return true if everithing was ok, false otherwise
	bool Sub(const int &I0,const vcg::Point2<ScalarType> &bary0,
			 const int &I1,const vcg::Point2<ScalarType> &bary1,
			 vcg::Point2<ScalarType> &vect,int &num)
	{
		int IndexDomain;
		num=isoParam->InterpolationSpace(I0,I1,IndexDomain);
		///is a face
		if (num==0)
		{
			//printf("F");
			assert(I0==I1);
			vect=bary0-bary1;
			return true;
		}
		else
		///a diamond
		if (num==1)
		{
			//printf("D");
			///tranform in UV space
			vcg::Point2<ScalarType> UVDiam;
			isoParam->GE1(I1,bary1,IndexDomain,UVDiam);
			///then find bary coords wich respect to the first face
			vcg::Point2<ScalarType> bary2;
			isoParam->inv_GE1_fixedI(IndexDomain,UVDiam,I0,bary2);
			vect=bary0-bary2;
			return true;
		}
		else
		///a star
		if (num==2)
		{
			//printf("S");
			///tranform in UV space
			vcg::Point2<ScalarType> UVStar;
			isoParam->GE0(I1,bary1,IndexDomain,UVStar);
			///then find bary coords wich respect to the first face
			vcg::Point2<ScalarType> bary2;
			isoParam->inv_GE0_fixedI(IndexDomain,UVStar,I0,bary2);
			vect=bary0-bary2;
			
			return true;
		}
		else 
			return false;
	}
	
	bool TangentDir(const int &I,const vcg::Point2<ScalarType> &bary,
			   CoordType &XAxis,CoordType &YAxis,ScalarType radius=(ScalarType)0.5)
	{
		///3d position of origin
		CoordType origin;
		isoParam->Theta(I,bary,origin);

		const ScalarType h=(ScalarType)2.0/sqrt((ScalarType)3.0);
		vcg::Point2<ScalarType> Xaxis=vcg::Point2<ScalarType>(1,0);
		vcg::Point2<ScalarType> Yaxis=vcg::Point2<ScalarType>(-(ScalarType)0.5,h);
		Yaxis.Normalize();
		Xaxis*=radius;
		Yaxis*=radius;
		vcg::Point2<ScalarType> bary0,bary1,bary2,bary3;
		int I0,I1,I2,I3;
		CoordType X0,X1,Y0,Y1;
		///find paraCoords of four neighbors
		int domain;
		bool done=Sum(I,bary,Xaxis,I0,bary0,domain);
		Xaxis=-Xaxis;
		done&=Sum(I,bary,Xaxis,I1,bary1,domain);
		done&=Sum(I,bary,Yaxis,I2,bary2,domain);
		Yaxis=-Yaxis;
		done&=Sum(I,bary,Yaxis,I3,bary3,domain);
		if (!done)
			return false;
		///get 3d position
		isoParam->Theta(I0,bary0,X0);
		isoParam->Theta(I1,bary1,X1);
		isoParam->Theta(I2,bary2,Y0);
		isoParam->Theta(I3,bary3,Y1);
		///wich respect to origin .. considering one is opposite respect to the other
		X0=X0-origin;
		X1=origin-X1;
		Y0=Y0-origin;
		Y1=origin-Y1;
		///average .. considering one is opposite respect to the other
		XAxis=(X0+X1)/(ScalarType)2.0;
		YAxis=(Y0+Y1)/(ScalarType)2.0;
		///final scaling 
		XAxis/=radius;
		YAxis/=radius;
		return true;
	}

	void Test(int Sample=100,int Ite=100)
	{
		int max=isoParam->AbsMesh()->face.size();
		for (int I=0;I<max;I++)
		{
			printf("\n TESTING %d \n",I);
			for (int i=0;i<Sample;i++)
			{
				for (int j=0;j<Sample;j++)
				{
					if ((i+j)<Sample)
					{
						ScalarType alpha=(ScalarType)i/(ScalarType)Sample;
						ScalarType beta=(ScalarType)j/(ScalarType)Sample;
						assert((alpha+beta)<=1.0);
						vcg::Point2<ScalarType> bary=vcg::Point2<ScalarType>(alpha,beta);
						assert((bary.X()>=0)&&(bary.X()<=1));
						assert((bary.Y()>=0)&&(bary.Y()<=1));
						assert((bary.X()+bary.Y()<=1));
						
						for (int k=0;k<Ite;k++)
						{
							ScalarType d0=((ScalarType)(rand()%1000));
							ScalarType d1=((ScalarType)(rand()%1000));
							vcg::Point2<ScalarType> vect=vcg::Point2<ScalarType>(d0,d1);
							vect.Normalize();
							ScalarType norm=0.05;//((ScalarType)(rand()%1000))/(ScalarType)2000;
							assert(norm<1);
							vect*=norm;
							int I1;
							vcg::Point2<ScalarType> bary1;
							vcg::Point2<ScalarType> vect1;
							int domain0;
							bool b1=Sum(I,bary,vect,I1,bary1,domain0);
							assert(b1);

							assert((bary1.X()>=0)&&(bary1.X()<=1));
							assert((bary1.Y()>=0)&&(bary1.Y()<=1));
							if(!((bary1.X()+bary1.Y())<=1.00001))
							{
								printf("\n SUM %.4f \n",bary1.X()+bary1.Y());
								assert(0);
							}
							assert(I1<max);
							int domain;
							bool b2=Sub(I,bary,I1,bary1,vect1,domain);
							assert(b2);
							ScalarType diff=(vect1+vect).Norm();
							if (domain0==0)
								assert(domain==0);
							if ((domain0==1)&&(domain==2))
								assert(0);
							/*if (domain0!=domain)
								assert(0);*/
							if (diff>0.001)
							{
								printf("\n DIFF %.4f domain SUM %d domain SUB %d \n",diff,domain0,domain);
								//assert(0);
							}
							//assert(fabs(vect1.X()-vect.X())<0.0001);
							//assert(fabs(vect1.Y()-vect.Y())<0.0001);
							
						}
					}
				}
			}
		}
	}

	bool Project(const int &I,const vcg::Point2<ScalarType> &bary,
				 CoordType &vect3d,
				 vcg::Point2<ScalarType> &vect2d,
				 ScalarType radius=(ScalarType)0.5)
	{
		CoordType origin;
		isoParam->Theta(I,bary,origin);
		CoordType XAxis,YAxis;
		///get tangent directions
		bool done=TangentDir(I,bary,XAxis,YAxis,radius);
		if (!done)
			return false;
		///then find dot products
		ScalarType deltaX=vect3d*XAxis;
		ScalarType deltaY=vect3d*YAxis;
		vect2d=vcg::Point2<ScalarType>(deltaX,deltaY);
		return true;
	}

};
#endif
