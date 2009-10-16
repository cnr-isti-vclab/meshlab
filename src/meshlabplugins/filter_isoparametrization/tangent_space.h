#ifndef _ISO_TANGENTSPACE
#define _ISO_TANGENTSPACE
#include "iso_parametrization.h"
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/update/normal.h>

class TangentSpace{
	typedef IsoParametrization::CoordType CoordType;
	typedef IsoParametrization::ScalarType ScalarType;
	

	vcg::SimpleTempData<typename ParamMesh::VertContainer,vcg::Matrix33<ScalarType> > *ProjMatrix;
	
public:
	IsoParametrization *isoParam;
	

  bool isoParamTheta(int i, vcg::Point2<ScalarType> p, vcg::Point3<ScalarType> &res) const{
    return isoParam->Theta(i,p,res);
  }

	//
  void Theta(int i,
		const vcg::Point2<ScalarType> &UV,
		CoordType &pos3D){
      isoParamTheta(i,UV,pos3D);
  }
	
	bool Theta(const int &I,
		const vcg::Point2<ScalarType> &alpha_beta, // alphaBeta
		std::vector<ParamFace*> &face,
		std::vector<CoordType> &baryVal)
	{
      int ret=isoParam->Theta(I,alpha_beta,face,baryVal);
			return (ret!=-1);
  }

	///initialize the sampler 
	void Init(IsoParametrization *_isoParam,ScalarType radius=(ScalarType)0.1)
	{
		isoParam=_isoParam;
		ProjMatrix   = new vcg::SimpleTempData<typename ParamMesh::VertContainer,vcg::Matrix33<ScalarType> > (isoParam->ParaMesh()->vert);
		
		InitProjectionMatrix(radius);
		vcg::tri::UpdateNormals<ParamMesh>::PerFaceNormalized(*isoParam->ParaMesh());
		vcg::tri::UpdateCurvature<ParamMesh>::PrincipalDirectionsNormalCycles(*isoParam->ParaMesh());
	}
	
	///given an initial position in parametric space (I0,bary0)
	///and a 2D vector (vect) expressed in parametric space modify the final 
	///position (I1,bary1) abd return true if everithing was ok, false otherwise
	bool Sum(const int &I0,const vcg::Point2<ScalarType> &bary0,
			 const vcg::Point2<ScalarType> &vect,
			 int &I1,vcg::Point2<ScalarType> &bary1,int &domain) const
	{
    
		
		vcg::Point2<ScalarType> dest=bary0+vect;
      //vect[0]*Xaxis + vect[1]*Yaxis;
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
			 vcg::Point2<ScalarType> &vect,int &num) const
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
			   CoordType &XAxis,CoordType &YAxis,ScalarType radius=(ScalarType)0.5) const
	{
		///3d position of origin
		//CoordType origin;
		//isoParam->Theta(I,bary,origin);

    // two axis in alpha-beta space that will be orthogonal in UV-Space
		const ScalarType h=(ScalarType)2.0/sqrt((ScalarType)3.0);
		vcg::Point2<ScalarType> XP=vcg::Point2<ScalarType>(1,0);
		vcg::Point2<ScalarType> YP=vcg::Point2<ScalarType>(-0.5,h);
		
		XP*=radius;
		YP*=radius;
		vcg::Point2<ScalarType> XM=-XP;
		vcg::Point2<ScalarType> YM=-YP;
		//Yaxis.Normalize();
		
		vcg::Point2<ScalarType> bary0,bary1,bary2,bary3;
		int I0,I1,I2,I3;
		CoordType X0,X1,Y0,Y1;
		///find paraCoords of four neighbors
		int domain;
		bool done=true;
    done&=Sum(I,bary,XP,I0,bary0,domain);
		done&=Sum(I,bary,XM,I1,bary1,domain);
		done&=Sum(I,bary,YP,I2,bary2,domain);
		done&=Sum(I,bary,YM,I3,bary3,domain);
		if (!done)
			return false;

		//if (I0!=I1 || I1!=I2 || I2!=I3) return false;

		///get 3d position
		isoParamTheta(I0,bary0,X0);
		isoParamTheta(I1,bary1,X1);
		isoParamTheta(I2,bary2,Y0);
		isoParamTheta(I3,bary3,Y1);
		
		
		///average .. considering one is opposite respect to the other
		XAxis=(X0-X1)/(ScalarType)2.0;
		YAxis=(Y0-Y1)/(ScalarType)2.0;
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
							ScalarType norm=(ScalarType)0.05;//((ScalarType)(rand()%1000))/(ScalarType)2000;
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
	
	void InitProjectionMatrix(ScalarType radius=(ScalarType)0.1)
	{
		for (int i=0;i<isoParam->ParaMesh()->vert.size();i++)
		{
			int I=isoParam->ParaMesh()->vert[i].T().N();
			vcg::Point2<ScalarType> bary=isoParam->ParaMesh()->vert[i].T().P();
			
			CoordType origin;
			isoParamTheta(I,bary,origin);
			CoordType XAxis,YAxis; // tangent axis in 3D Object space
			///get tangent directions
			
			bool done=TangentDir(I,bary,XAxis,YAxis,(ScalarType)0.1);
			if (!done)
			{
				(*ProjMatrix)[i].SetIdentity();
				continue;
			}
		
			// must compute res2d such that:  res2d.X() * XAxis + res2d.Y() * YAxis + dontCare * ZAxis = vect3d
			CoordType ZAxis = -(XAxis^YAxis).Normalize()*XAxis.Norm();
			
			(*ProjMatrix)[i].SetColumn(0,XAxis);
			(*ProjMatrix)[i].SetColumn(1,YAxis);
			(*ProjMatrix)[i].SetColumn(2,ZAxis);
			vcg::Invert((*ProjMatrix)[i]);
		}
	}

	void GetCurvature(const int &I,const vcg::Point2<ScalarType> &alpha_beta,
					  CoordType &d1,CoordType &d2,ScalarType &k1,ScalarType &k2)
	{
		ParamFace* face=NULL;
		CoordType baryVal;
		isoParam->Theta(I,alpha_beta,face,baryVal);
		ParamVertex *v0=face->V(0);
		ParamVertex *v1=face->V(1);
		ParamVertex *v2=face->V(2);
		d1=v0->PD1()*baryVal.X()+v1->PD1()*baryVal.Y()+v2->PD1()*baryVal.Z();
		d2=v0->PD2()*baryVal.X()+v1->PD2()*baryVal.Y()+v2->PD2()*baryVal.Z();
		k1=v0->K1()*baryVal.X()+v1->K1()*baryVal.Y()+v2->K1()*baryVal.Z();
		k2=v0->K2()*baryVal.X()+v1->K2()*baryVal.Y()+v2->K2()*baryVal.Z();
	}

	void GetProjectionMatrix(const int &I,const vcg::Point2<ScalarType> &alpha_beta,
						     vcg::Matrix33<ScalarType> &projMatr) const
	{
		ParamFace* face=NULL;
		CoordType baryVal;
		int dom=isoParam->Theta(I,alpha_beta,face,baryVal);
		if (dom==-1)
		{
			projMatr.SetIdentity();
			return;
		}
		ParamVertex *v0=face->V(0);
		ParamVertex *v1=face->V(1);
		ParamVertex *v2=face->V(2);

		projMatr=(*ProjMatrix)[v0]*baryVal.X();
		projMatr+=(*ProjMatrix)[v1]*baryVal.Y();
		projMatr+=(*ProjMatrix)[v2]*baryVal.Z();
	}

	
	bool Project(const int &I,const vcg::Point2<ScalarType> &bary,
				 const CoordType &vect3d, // in object space
				 vcg::Point2<ScalarType> &res2d) const
	{
		vcg::Matrix33f m;
		GetProjectionMatrix(I,bary,m);

		ScalarType deltaX=vect3d*m.GetRow(0);
		ScalarType deltaY=vect3d*m.GetRow(1);
		
		// two axis in alpha-beta space that will be orthogonal in UV-Space
		const ScalarType h=(ScalarType)2.0/sqrt((ScalarType)3.0);
		vcg::Point2<ScalarType> XP=vcg::Point2<ScalarType>(1,0);
		vcg::Point2<ScalarType> YP=vcg::Point2<ScalarType>(-0.5,h);
		res2d = XP*deltaX + YP*deltaY;
		//res2d=vcg::Point2<ScalarType>(deltaX,deltaY);

		return true;
	}

	 // WEIGHTED INTERPOLATION OF POINTS IN TANGENT SPACE 
	///	weights MUST be normalized
	bool Interpolate(const int &I0,const vcg::Point2<ScalarType> &alpha_beta0,
					 const int &I1,const vcg::Point2<ScalarType> &alpha_beta1,
					 ScalarType weight,
					 int &I_res,
					 vcg::Point2<ScalarType> &alpha_beta_res)
	{
		int IndexDomain;
		int kind=isoParam->InterpolationSpace(I0,I1,IndexDomain);
		if (kind==-1)
			return false;

		vcg::Point2<ScalarType> transformed0,transformed1;
		
		///interpolate in a face
		if (kind==0)
		{
			isoParam->GE2(IndexDomain,alpha_beta0,transformed0);
			isoParam->GE2(IndexDomain,alpha_beta1,transformed1);
		}
		else
		///interpolate in a diamond
		if (kind==1)
		{				
			isoParam->GE1(I0,alpha_beta0,IndexDomain,transformed0);		
			isoParam->GE1(I1,alpha_beta1,IndexDomain,transformed1);		
		}
		else
		{
			isoParam->GE0(I0,alpha_beta0,IndexDomain,transformed0);
			isoParam->GE0(I1,alpha_beta1,IndexDomain,transformed1);
		}

		vcg::Point2<ScalarType> UV_interp=transformed0*weight+transformed1*(1.0-weight);
		///FINALLY......
		///transform back to alpha,beta,I
		if (kind==0)
		{
			isoParam->inv_GE2(IndexDomain,UV_interp,alpha_beta_res);
			I_res=IndexDomain;
		}
		else
		if (kind==1)
		{
			isoParam->inv_GE1(IndexDomain,UV_interp,I_res,alpha_beta_res);
		}
		else
			isoParam->inv_GE0(IndexDomain,UV_interp,I_res,alpha_beta_res);
		return true;
	}

	ScalarType AbstractArea()
	{
		ScalarType Cnum=sqrt(3.0)/4.0;
		return(isoParam->AbsMesh()->fn*Cnum);
	}

    // WEIGHTED INTERPOLATION OF POINTS IN TANGENT SPACE 
	///	weights MUST be normalized
	bool Interpolate(const std::vector<int> &I,
					 const std::vector<vcg::Point2<ScalarType> > &alpha_beta,
					 const std::vector<ScalarType> &weights,
					 int &I_res,
					 vcg::Point2<ScalarType> &alpha_beta_res,
					 int *num=NULL)
	{
		int size;
		if (num==NULL)
			size=alpha_beta.size();
		else
			size=*num;

		int IndexDomain;
		int kind=isoParam->InterpolationSpace(I,IndexDomain,num);
		if (kind==-1)
			return false;

		std::vector<vcg::Point2<ScalarType> > transformed;
		transformed.resize(size);
		
		///interpolate in a face
		if (kind==0)
		{
			for (int i=0;i<size;i++)
				isoParam->GE2(IndexDomain,alpha_beta[i],transformed[i]);
		}
		else
		///interpolate in a diamond
		if (kind==1)
		{				
			for (int i=0;i<size;i++)
				isoParam->GE1(I[i],alpha_beta[i],IndexDomain,transformed[i]);		
		}
		else
		{
			for (int i=0;i<size;i++)
				bool b0=isoParam->GE0(I[i],alpha_beta[i],IndexDomain,transformed[i]);
		}

		/// do the interpolation
		vcg::Point2<ScalarType> UV_interp=vcg::Point2<ScalarType>(0,0);
		for (int i=0;i<size;i++)
			UV_interp+=(transformed[i]*weights[i]);
		
		///FINALLY......
		///transform back to alpha,beta,I
		if (kind==0)
		{
			isoParam->inv_GE2(IndexDomain,UV_interp,alpha_beta_res);
			I_res=IndexDomain;
		}
		else
		if (kind==1)
		{
			isoParam->inv_GE1(IndexDomain,UV_interp,I_res,alpha_beta_res);
		}
		else
			isoParam->inv_GE0(IndexDomain,UV_interp,I_res,alpha_beta_res);
		return true;
	}

};
#endif
