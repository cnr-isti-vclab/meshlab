/*#***************************************************************************
 * SVoxel.h                                                         o o      *
 *                                                                o     o    *
 * Visual Computing Group                                         _  O  _    *
 * IEI Institute, CNUCE Institute, CNR Pisa                        \/)\/     *
 *                                                                /\/|       *
 * Copyright(C) 2002 by Paolo Cignoni                                |       *
 * All rights reserved.                                              \       *
 *                                                                           *
 * Permission  to use, copy, modify, distribute  and sell this  software and *
 * its documentation for any purpose is hereby granted without fee, provided *
 * that  the above copyright notice appear  in all copies and that both that *
 * copyright   notice  and  this  permission  notice  appear  in  supporting *
 * documentation. the author makes  no representations about the suitability *
 * of this software for any purpose. It is provided  "as is" without express *
 * or implied warranty.                                                      *
 *					                                                                 *
 *****************************************************************************/

#ifndef __SVOXEL_H__
#define __SVOXEL_H__
template<class SCALAR_TYPE=float>
class SVoxel
{
	
private:
	short v;          // distanza dalla superficie espressa in centesimi di voxel;
	short q;          // come distanza dal bordo espressa   in centesimi di voxel; // nota che questo implica che non si potrebbe rasterizzare una singola mesh in un volume di lato > 3000^3
	char n[3];
	unsigned char cnt;        
	// se != 0 b deve essere true;
	// b e'tenuto implicitamente usando cnt; 
	// se cnt == 0 || cnt>0 -> b=false
	// se cnt == 255  -> b=true

// b==false cnt==0 totalmente non inzializzato (Zero)
// b==false cnt >0 da normalizzare
// b==true  cnt==0 gia' normalizzato
// b==true  cnt >0 Errore!!!


	
public:
	typedef SCALAR_TYPE scalar;
	SVoxel(SCALAR_TYPE vv, bool bb, Point3<scalar> &nn, scalar qq) {SetV(vv);SetN(nn);SetQ(qq);}
	SVoxel(SCALAR_TYPE vv, const Point3<scalar> &nn, scalar qq) {SetV(vv);SetN(nn);SetQ(qq);cnt=255;}
	const bool B() const {return cnt==255;};  // puo' essere a true solo se cnt == 0; (il che significa che e' stato gia' normalizzato
	
	void SetB(bool val) { 
		assert( val == (cnt==255 || cnt==0) ); 
	  if(val) cnt=255;
		else if(cnt==255) cnt=0; 
	} 
	
	const int Cnt()      { 
		if(cnt==255) return 0; 
		else return int(cnt);
	}
	void SetCnt(int val) { cnt=(unsigned char)val;}
	
	
	Point3<scalar> const N() const  	{ 
		return Point3<scalar>(scalar(n[0])/127.0f,scalar(n[1])/127.0f,scalar(n[2])/127.0f); 		
	}
	const scalar N(const int i) const
		{
		 return scalar(n[i])/127.0f;
		}

	void SetN(const Point3<scalar> &nn) 
		{
			n[0]=char( nn[0]*127.0f );
			n[1]=char( nn[1]*127.0f );
			n[2]=char( nn[2]*127.0f );
		}
	const scalar V() const
		{
		 return scalar(v)/100.0f;
		}

	inline void Blend( SVoxel const & vx, scalar w)
	{
		float w1=1.0-w;
		SetV(V()*w1+vx.V()*w);
		SetQ(Q()*w1+vx.Q()*w);
		SetN(N()*w1+vx.N()*w);
		//return *this;
	}

	void SetV(const float &vv) 
		{
		 v= short(vv*100.0f);
		 if(v==0) v=1;
		}

	const scalar Q() const
		{
		 return scalar(q)/20.0f;
		}

	void SetQ(const float &qq) 
		{
		 int qi = qq * 20.0f;
		 if (qi>32767) qi =32767;
		 q = qi;
		}

	void Merge(const SVoxel &VOX)
		{
			v=(v*Q()+VOX.Q()*VOX.v)/(Q()+VOX.Q());
			SetQ(Q()+VOX.Q());
		}
	void Set(const SVoxel &VOX)
		{
			v=VOX.v;
			n[0]=VOX.n[0];
			n[1]=VOX.n[1];
			n[2]=VOX.n[2];
			q=VOX.q;

		}

		inline SVoxel & operator += ( SVoxel const & vx)
		{
	 if(cnt==0)
		 {
		  v=vx.v;
			q=vx.q;
			n[0]=vx.n[0];
			n[1]=vx.n[1];
			n[2]=vx.n[2];
			cnt=1;
	 	 }
	 else
		{
		 const int cnt1=cnt+1;
		 v+=vx.v;
		 q = (q*cnt+vx.q)/(cnt1);
		 n[0]=(vx.n[0]*int(cnt) +vx.n[0])/(cnt1) ;
		 n[1]=(vx.n[1]*int(cnt) +vx.n[1])/(cnt1) ;
		 n[2]=(vx.n[2]*int(cnt) +vx.n[2])/(cnt1) ;
		 if(cnt==255) cnt=1;
		 else ++cnt;
		 }
		 return *this;
		}

	inline bool Normalize(int thr)
	{
	 assert(cnt>0);
	 if(cnt<thr)
	 {
		 (*this) = Zero();
		 return false;
	 }
   v= v/cnt; // gli altri membri sono gia' normalizzati
	 cnt=255;
	 // b=true; inutile!
	return true;
	}

 bool IsZero() const { return v==0; }

	static const SVoxel &Zero() {
		static SVoxel tt(0,false,Point3f(0,0,0),0);
		return tt;
	}
};

#endif
