/*#***************************************************************************
 * Volume.h                                                         o o      *
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

#ifndef __VOLUME_H__
#define __VOLUME_H__

#ifdef __MINGW32__
#define _int64 __int64
#endif

#include "voxel.h"
#include "svoxel.h"
#include <vector>
#include <vcg/space/index/grid_static_ptr.h>

//#define BLOCKSIDE() 8

// Stato di un voxel

// B() dice se ci sono dati in uno stadio usabile.
// Cnt() dice quanti ce ne sono stati sommati (per la normalizzazione)

// b==false cnt==0 totalmente non inzializzato (Zero)
// b==false cnt >0 da normalizzare
// b==true  cnt==0 gia' normalizzato
// b==true  cnt >0 Errore!!!

// forward definition
template < class VOL >
class VolumeIterator;
 
//******************************************
//******************************************
//typedef Voxel<float> Voxelf;

const char *SFormat( const char * f, ... )
    {
        static char buf[4096];
        va_list marker;
        va_start( marker, f );
        vsprintf(buf,f,marker);
        va_end( marker );
        return buf;
    }


template<class VOX_TYPE, class SCALAR_TYPE=float>
class Volume {
public:
	typedef SCALAR_TYPE scalar;
  typedef Point3<scalar> Point3x;
  typedef Box3<scalar> Box3x;
  
  typedef VOX_TYPE voxel_type;

	static const int BLOCKSIDE() { return 8;}
	Volume(){
		SetDefaultParam();
	}
	// I dati veri e propri
	// Sono contenuti in un vettore di blocchi. 
    std::vector<  std::vector<VOX_TYPE>  > rv;
    Box3x   bbox;

        _int64 AskedCells;
	Point3x dim;  /// Dimensione spaziale (lunghezza lati) del bbox
  
	Point3i sz;   /// Dimensioni griglia come numero di celle per lato
 	
	Point3i ssz;  /// Dimensioni sottoblocco in esame come numero di celle per lato
  
	Point3i rsz;  /// Dimensioni macro griglia dei blocchi in cui e' suddiviso il volume (ogni blocco e' BLOCKSIDE()^3 celle)
	
	Point3i asz;  /// Dimensioni macro griglia dei blocchi relativa al sottoblocco in questione (quello allocato davvero!)

  
	Point3x voxel; /// Dimensioni di una cella
  
	
	int WN,WP; // di quanti vox mi devo allargare per settare la manhattan distance in neg e pos
  int DeltaVoxelSafe; // di quanti vox mi devo allargare per stare sicuro nel fare solo una sottoparte.

  const Point3i ISize() { return sz; }
private :
	// offset e distanze varie precalcolate una volta per tutte
	Point3f nnf[26];   
	Point3i nni[26];
	float len[26];
	float slen[26];

		/// Gestione sottoparte
	Point3i div;
	Point3i pos;
public:
	Box3i	  SubPart;                 // Sottoparte del volume da considerare ufficialmente
	Box3x     SubBox;                  // BBox della sottoparte del volume da considerare in coord assolute
	Box3i	  SubPartSafe;             // come sopra ma aumentati per sicurezza.
	Box3x     SubBoxSafe;

 FILE *LogFP;
bool Verbose; // se true stampa un sacco di info in piu su logfp;

	void SetDefaultParam(){
		 WN=0; 
		 WP=1;
		 //WN=-2;//
		 //WP=3;
		 DeltaVoxelSafe=BLOCKSIDE();
		 Verbose=true;
		 LogFP=stderr;
		}

	void Init(const Volume &VV)
	{
		SetDefaultParam();
		WN=VV.WN;
		WP=VV.WP;
		DeltaVoxelSafe=VV.DeltaVoxelSafe;
    Init(VV.AskedCells,VV.bbox,VV.div,VV.pos);
	}

        void Init(_int64 cells, Box3x bb, Point3i _div=Point3i(1,1,1), Point3i _pos=Point3i(0,0,0))
	{
		Point3d voxdim;voxdim.Import(bb.max-bb.min);
		AskedCells=cells;
        vcg::BestDim<double>( cells, voxdim, sz );
		bbox=bb;
/*
		printf("grid of ~%i kcells: %d x %d x %d \n",int(cells/1000),sz[0],sz[1],sz[2]);
		printf("grid voxel size of %f %f %f\n",voxdim[0]/sz[0],voxdim[1]/sz[1],voxdim[2]/sz[2]);
*/
		// il box deve essere multipli di BLOCKSIDE() 
	  sz=((sz/BLOCKSIDE())+Point3i(1,1,1))*BLOCKSIDE();
	

		rsz=sz/BLOCKSIDE();
		if(sz!=rsz*BLOCKSIDE()) {
			assert(0); // il box deve essere multipli di BLOCKSIDE()
      exit(-1);
		}
		
		dim=bbox.max-bbox.min;
		voxel[0]=dim[0]/sz[0];
		voxel[1]=dim[1]/sz[1];
		voxel[2]=dim[2]/sz[2];

		SetSubPart(_div,_pos);
		ssz=SubPartSafe.max-SubPartSafe.min;
		asz=ssz/BLOCKSIDE() + Point3i(1,1,1);
        rv.clear();
        rv.resize(asz[0]*asz[1]*asz[2]);
        for(size_t i=0;i<rv.size();++i)
            rv[i].resize(0,VOX_TYPE::Zero());
        SetDim(bb);
	};

private:

	// Da chiamare sempre DOPO la resize...
	void SetDim(const Box3x & /*bb*/)
	{
	
	// Setup the precomputed offsets and offset normals
		int cnt=0,x,y,z;
		for(z=-1;z<=1;++z){
		 for(y=-1;y<=1;++y){
			for(x=-1;x<=1;++x)
					if(x!=0 || y!=0 || z!=0)
					{
						nnf[cnt]=Point3f(x,y,z);
						len[cnt]=nnf[cnt].Norm();
						slen[cnt]=nnf[cnt].SquaredNorm();
						nnf[cnt].Normalize();
						nni[cnt]=Point3i(x,y,z);
						cnt++;
					}
				}
		}
	}

/*
Parametri 
div indica il numero di blocchi da fare lungo i vari assi (sempre >=1)
pos indicano la coord del subbox da prendere in considerazione (sempre >=0 &&  < xdiv,ydiv,zdiv)
*/

void SetSubPart(Point3i _div, Point3i _pos)
{
	int k;
	// Controllo correttezza parametri.
	for(k=0;k<3;++k)
		{
			assert(_div[k]>0);
			if(_div[k]==0){
				printf("Error in subbox definition:\n the subdiv settings must be greater than 0; it was %i %i %i\n",_div[0],_div[1],_div[2]);
				exit(-1);
			}
			if(_pos[k]<0 || _pos[k]>=_div[k]){
				printf("Error in subbox definition:\n the Position of the subbox must be between (0,0,0) and (%i,%i,%i); it was %i %i %i\n",_div[0],_div[1],_div[2],_pos[0],_pos[1],_pos[2]);
				exit(-1);
			}
		}

	div=_div;
	pos=_pos;

	// Setting the subpart under analisys
	for(k=0;k<3;++k)
		{
			SubPart.min[k]= pos[k]*sz[k]/div[k];
			SubPart.max[k]=(pos[k]+1)*sz[k]/div[k];
			SubBox.min[k]= bbox.min[k]+SubPart.min[k]*voxel[k];
			SubBox.max[k]= bbox.min[k]+SubPart.max[k]*voxel[k];
		}

	// Setting the Safe Subpart under analisys
	SubPartSafe=SubPart;
	for(k=0;k<3;++k)
		{
			SubPartSafe.min[k] -= DeltaVoxelSafe;
			SubPartSafe.max[k] += DeltaVoxelSafe;

			if( SubPartSafe.min[k]< 0     ) SubPartSafe.min[k] = 0;
			if( SubPartSafe.max[k]> sz[k] ) SubPartSafe.max[k] = sz[k];
			SubBoxSafe.min[k]= bbox.min[k]+SubPartSafe.min[k]*voxel[k];
			SubBoxSafe.max[k]= bbox.min[k]+SubPartSafe.max[k]*voxel[k];
		}		
/*
		printf("  Computing only subvolume: (%d x %d x %d)= %dk cells  \n"
			     "                             %d,%d,%d -> %d,%d,%d\n"
                        ,SubPart.DimX(),SubPart.DimY(),SubPart.DimZ(),(int)(((__int64)SubPart.DimX()*(__int64)SubPart.DimY()*(__int64)SubPart.DimZ())/1000)
			,SubPart.min[0]			,SubPart.min[1]			,SubPart.min[2]
			,SubPart.max[0]			,SubPart.max[1]			,SubPart.max[2]		);
*/
}

public:
	
	// Sa
	/*bool Write(string filename, const float &minv, const float &maxv )
		{
		  FILE *fp;
			if(div!=Point3i(1,1,1)) {
					string subvoltag;
					GetSubVolumeTag(subvoltag);	
					filename+=subvoltag;
			}
			string datname=filename;
			string rawname=filename;
			datname+=".dat";
			rawname+=".raw";
     
		  fp=fopen(datname,"w");

			fprintf(fp,"ObjectFileName: %s\n",rawname.c_str());
			fprintf(fp,"TaggedFileName: ---\n");
			fprintf(fp,"Resolution:     %i %i %i\n",SubPart.max[0]-SubPart.min[0],SubPart.max[1]-SubPart.min[1],SubPart.max[2]-SubPart.min[2]);
			fprintf(fp,"SliceThickness: %f %f %f\n",voxel[2]/voxel[0],voxel[2]/voxel[1],voxel[2]/voxel[2]);
			fprintf(fp,"Format:         UCHAR\n");
			fprintf(fp,"NbrTags:        0\n");
			fprintf(fp,"ObjectType:     TEXTURE_VOLUME_OBJECT\n");
			fprintf(fp,"ObjectModel:    RGBA\n");
			fprintf(fp,"GridType:       EQUIDISTANT\n");

			fclose(fp);
      fp=fopen(rawname,"wb");
		 if(!fp) 
		 {
			 printf("Error: unable ro open output volume file '%s'\n",filename);
			 return false;
		 }

		 int i,j,k;
		 for(k=SubPart.min[2];k<SubPart.max[2];++k)
			 for(j=SubPart.min[1];j<SubPart.max[1];++j)
				 for(i=SubPart.min[0];i<SubPart.max[0];++i)
					{ 
						float fv=V(i,j,k).V();
					  fv=(fv-minv)/(maxv-minv);
						if(fv<0) fv=0;
						else if(fv>1) fv=1;
						fv=((fv*2.0f)-1.0f)*127;
						char fs= (char) fv;
						fwrite(&fs,sizeof(char),1,fp);
					}
		fclose(fp);
		return true;
		}*/
	void AbsPos(Point3i pi, Point3x &p)
		{
			p[0]=bbox.min[0]+pi[0]*voxel[0];
			p[1]=bbox.min[1]+pi[1]*voxel[1];
			p[2]=bbox.min[2]+pi[2]*voxel[2];
		}


    void GetSubVolumeTag(std::string &subtag)
	{
    char buf[32];
		if     (div[0]<=  10 && div[1]<=  10 && div[2]<=  10 ) sprintf(buf,"_%01d%01d%01d",pos[0],pos[1],pos[2]);
		else if(div[0]<= 100 && div[1]<= 100 && div[2]<= 100 ) sprintf(buf,"_%02d%02d%02d",pos[0],pos[1],pos[2]);
		                                                 else  sprintf(buf,"_%03d%03d%03d",pos[0],pos[1],pos[2]);
		subtag=buf;
	}

	/*
	Data una posizione x,y,z restituisce true se tale posizione appartiene a  un blocco gia' allocato 
	In ogni caso mette in rpos la posizione del subbloc e in lpos la posizione all'interno del sottoblocco
	*/
	bool Pos(const int &_x,const int &_y,const int &_z, int & rpos,int &lpos) const
	{
		int x=_x-SubPartSafe.min[0];		int y=_y-SubPartSafe.min[1];		int z=_z-SubPartSafe.min[2];
    		
		assert(_x>=SubPartSafe.min[0] && _x<SubPartSafe.max[0] &&
			     _y>=SubPartSafe.min[1] && _y<SubPartSafe.max[1] && 
					 _z>=SubPartSafe.min[2] && _z<SubPartSafe.max[2]);

	//	assert(x>=0 && x<sz[0] && y>=0 && y<sz[1] && z>=0 && z<sz[2]);

		int rx=x/BLOCKSIDE();		int ry=y/BLOCKSIDE();		int rz=z/BLOCKSIDE();
		assert(rx>=0 && rx<asz[0] && ry>=0 && ry<asz[1] && rz>=0 && rz<asz[2]);
		rpos = rz*asz[0]*asz[1]+ry*asz[0]+rx;
        assert(rpos < int(rv.size()));
		int lx = x%BLOCKSIDE();		int ly = y%BLOCKSIDE();		int lz = z % BLOCKSIDE();
		lpos = lz*BLOCKSIDE()*BLOCKSIDE()+ly*BLOCKSIDE()+lx;		
		if(rv[rpos].empty()) return false;
		return true;
	 }	

	/*
	Funzione inversa della precedente
	Date due posizioni rpos e lpos  restituisce x,y,z assoluti
	*/
	bool IPos(int &x,int &y,int &z, const int & rpos, const int &lpos) const
	{
		assert (rpos>=0 && lpos  >=0);

		int rz =   rpos / (asz[0]*asz[1]);	int remainder =  rpos % (asz[0]*asz[1]);
		int ry = ( remainder ) / asz[0] ; 
		int rx =   remainder % asz[0];
		
		assert(rx>=0 && rx<asz[0] && ry>=0 && ry<asz[1] && rz>=0 && rz<asz[2]);
		
		int lz =   lpos / (BLOCKSIDE()*BLOCKSIDE());	int lemaindel =  lpos % (BLOCKSIDE()*BLOCKSIDE());
		int ly = ( lemaindel ) / BLOCKSIDE(); 
		int lx =   lemaindel % BLOCKSIDE();
		
		x = rx*BLOCKSIDE()+lx;
		y = ry*BLOCKSIDE()+ly;
		z = rz*BLOCKSIDE()+lz;
	
		x+=SubPartSafe.min[0];
		y+=SubPartSafe.min[1];
		z+=SubPartSafe.min[2];

		assert(x>=0 && x<sz[0] && y>=0 && y<sz[1] && z>=0 && z<sz[2]);
		//int trpos,tlpos;
		//assert(rv[rpos].size()>0);
		//Pos(x,y,z,trpos,tlpos);
		//assert(trpos==rpos && tlpos == lpos);
		return true;
	 }	

	void Alloc(int rpos, const VOX_TYPE &zeroval)
	{
		rv[rpos].resize(BLOCKSIDE()*BLOCKSIDE()*BLOCKSIDE(),zeroval);
	}
	/************************************/
	// Funzioni di accesso ai dati

  float Val(const int &x,const int &y,const int &z) const {
    if(!cV(x,y,z).B())  return 1000;
      return cV(x,y,z).V(); 
    //else return numeric_limits<float>::quiet_NaN( ); 
  }

	VOX_TYPE &V(const int &x,const int &y,const int &z) {
		int rpos,lpos;
		if(!Pos(x,y,z,rpos,lpos)) Alloc(rpos,VOX_TYPE::Zero());
		return rv[rpos][lpos]; 
	}

	const VOX_TYPE &cV(const int &x,const int &y,const int &z) const 
	{
		int rpos,lpos;
		if(!Pos(x,y,z,rpos,lpos)) return VOX_TYPE::Zero();
		else return rv[rpos][lpos]; 
	}
	const VOX_TYPE &V(const int &x,const int &y,const int &z) const 
	{
		int rpos,lpos;
		if(!Pos(x,y,z,rpos,lpos)) return VOX_TYPE::Zero();
		else return rv[rpos][lpos]; 
	}
	/************************************/
	void Fill(VOX_TYPE (__cdecl *func)(const Point3i &p) )
{
	int x,y,z;
	for(z=0;z<sz[2];++z)
		for(y=0;y<sz[1];++y)
			for(x=0;x<sz[0];++x)
						{
								Point3i p(x,y,z);
								V(x,y,z)=func(p);
						}
}

void Fill(VOX_TYPE const p)
{
	int x,y,z;
	for(z=0;z<sz[2];++z)
		for(y=0;y<sz[1];++y)
			for(x=0;x<sz[0];++x)
				{
								V(x,y,z)=p;
				}
}


// Copia sul volume corrente una versione smoothed del volume in ingresso S.
// il parametro serve a specificare il range di valori di campo vicini allo zero che non vanno mediati!
// questo perche se si smootha anche sullo zero si smoota anche dove e' bene allineato

void CopySmooth( Volume<VOX_TYPE> &S, scalar SafeZone=1, scalar SafeQuality=0)
{
	if(sz!=S.sz)
		{
			printf("Error");
			exit(-1);
		}
 int lcnt=0;
 VolumeIterator< Volume > vi(S);
 vi.Restart();
 vi.FirstNotEmpty();
 // const Voxelf *VC;
 while(vi.IsValid()) 
	 // scandisci il volume in ingresso, per ogni voxel non vuoto del volume 
	 // in ingresso calcola la media con gli adiacenti
	{
		if((*vi).B())
		{
			int x,y,z;
			IPos(x,y,z,vi.rpos,vi.lpos);
			if(Bound1(x,y,z))
				{
				  VOX_TYPE &VC =  V(x,y,z);
					for(int i=0;i<26;++i)
					{ 
						VOX_TYPE &VV= S.V(x+nni[i][0],y+nni[i][1],z+nni[i][2]);
						if(VV.B()) VC+=VV;			
					}
					lcnt++;
					
			    /*
					Voxelf &VV=V(x,y,z);
					//Voxelf &VV=rv[vi.rpos][vi.lpos];
					for(int i=0;i<26;++i)
					{ 
						VC = &(S.V(x+nni[i][0],y+nni[i][1],z+nni[i][2]));
						if(VC->b)
						{
							VV+=*VC;														
							lcnt++;
						}
					}*/
				}
		}
		vi.Next();
		if(vi.IsValid()) vi.FirstNotEmpty();
		//if((lcnt%100)==0) vi.Dump();

	}
 // Step 2, 
 // dopo aver calcolato la media, 
 
 VolumeIterator< Volume > svi(*this);
 svi.Restart();
 svi.FirstNotEmpty();
 int smoothcnt=0;
 int preservedcnt=0;
 int blendedcnt=0;
 const float FieldBorder = 1; // dove finisce la transizione tra la zona safe e quella smoothed
 const float EndFBorderZone = SafeZone+FieldBorder;
 const float EndQBorderZone = SafeQuality*1.5;
 const float QBorder = EndQBorderZone-SafeQuality; // dove finisce la transizione tra la zona safe e quella smoothed
 while(svi.IsValid())
	{
		if((*svi).Cnt()>0)
		{ 
			VOX_TYPE &sv=S.rv[svi.rpos][svi.lpos];
			(*svi).Normalize(1); // contiene il valore mediato
			float SafeThr = fabs(sv.V());

			// Se la qualita' e' bassa o se siamo distanti si smootha sempre
			// se siamo vicini allo zero e con buona qualita' si deve fare attenzione
			if(SafeThr<EndFBorderZone && sv.Q() > EndQBorderZone) 
			{		// se il voxel corrente aveva un valore < safezone E qualita' > SafeQuality 
					// allora si copia il valore originale di S
					if((SafeThr <= SafeZone) && sv.Q() > SafeQuality )
						{  
							(*svi)=sv;
							(*svi).SetB(true);
							++preservedcnt;
						}
						else
						{	// Siamo nella zona di transizione o per field o per quality
                            float blendq= std::max(0.0f,std::min(1.0f,(EndQBorderZone-sv.Q())/QBorder));
                            float blendf= std::max(0.0f,std::min(1.0f,(EndFBorderZone-SafeThr)/FieldBorder));
                            float BlendFactor = 1.0-std::max(blendf,blendq); // quanto del voxel originale <sv> si prende;
							(*svi).Blend(sv,BlendFactor);
							++blendedcnt;
						}
			}
			++smoothcnt;
		}
		svi.Next();
		if(svi.IsValid()) svi.FirstNotEmpty();
	}

 if(Verbose) fprintf(LogFP,"CopySmooth %i voxels, %i preserved, %i blended\n",smoothcnt,preservedcnt,blendedcnt);
}

void Merge(Volume<VOX_TYPE> &S)
{
 VolumeIterator< Volume > svi(S);
 svi.Restart();
 svi.FirstNotEmpty();
 int loccnt=0;
 
 while(svi.IsValid())
	{
	 if((*svi).B()) 
		 {
		  int x,y,z;
			IPos(x,y,z,svi.rpos,svi.lpos);
			if(cV(x,y,z).B())	V(x,y,z).Merge( (*svi));
					else {
						V(x,y,z).Set((*svi));
						V(x,y,z).SetB(true);
					}
			++loccnt;
		 }
	svi.Next();
  if(svi.IsValid()) svi.FirstNotEmpty();
	}
 
 printf("Merge2 %i voxels\n",loccnt);

}


void Interize( Point3x & vert ) const // OK
{
	for(int j=0;j<3;++j)
	{
		assert(vert[j]>=bbox.min[j]);
		assert(vert[j]<=bbox.max[j]);
		vert[j] = (vert[j] - bbox.min[j]) * sz[j] / (bbox.max[j] - bbox.min[j]);
	}
}


void DeInterize( Point3x & vert ) const	// OK
{
	for(int j=0;j<3;++j)
		vert[j] = vert[j] * (bbox.max[j] - bbox.min[j]) / sz[j] + bbox.min[j];
}

bool SplatVert( const Point3x & v0, double quality, const Point3x & nn, Color4b c)
{
	Box3i ibox;

  assert(math::Abs(SquaredNorm(nn) - 1.0) < 0.0001); // Just a safety check that the vertex normals are NORMALIZED!
	ibox.min=Point3i(floor(v0[0]),floor(v0[1]),floor(v0[2]));
	ibox.max=Point3i( ceil(v0[0]), ceil(v0[1]), ceil(v0[2]));
	ibox.Intersect(SubPartSafe);

    ibox.max[0] = std::min(SubPartSafe.max[0]-1,ibox.max[0]);
    ibox.max[1] = std::min(SubPartSafe.max[1]-1,ibox.max[1]);
    ibox.max[2] = std::min(SubPartSafe.max[2]-1,ibox.max[2]);


	 // Skip faces not colliding current subvolume.
	if(ibox.IsEmpty()) 
		{
			// point outside the box do nothing
			return false;
		}
	
	Point3x iV, deltaIV;
	
	// Now scan the eight voxel surrounding the splat 
	// and fill them with the distance from the plane
	for(iV[0]=ibox.min[0]; iV[0]<=ibox.max[0]; ++iV[0])
		for(iV[1]=ibox.min[1]; iV[1]<=ibox.max[1]; ++iV[1])
			for(iV[2]=ibox.min[2]; iV[2]<=ibox.max[2]; ++iV[2])
				{
					deltaIV = v0-iV;
					scalar offset = deltaIV * nn;
					VOX_TYPE &VV=V(iV[0],iV[1],iV[2]);
					VV=VOX_TYPE(offset,nn,quality,c);
				}
		return true;	
}

template <const int CoordZ>
        void RasterFace(const int sx, const int ex, const int sy, const int ey,
                        scalar dist, const Point3x &norm, scalar quality,
                        const Point3x &v0,  const Point3x &v1,  const Point3x &v2,
                        const Point3x &d10, const Point3x &d21, const Point3x &d02)
{
  const scalar EPS     = scalar(1e-12);
  const int crd0 = CoordZ;
  const int crd1 = (CoordZ+1)%3;
  const int crd2 = (CoordZ+2)%3;
  assert(fabs(norm[crd0])+0.001 > fabs(norm[crd1]));
  assert(fabs(norm[crd0])+0.001 > fabs(norm[crd2]));
  scalar x,y;
	for(x=sx;x<=ex;++x)
		for(y=sy;y<=ey;++y)
		{
            scalar n0 = (x-v0[crd1])*d10[crd2] - (y-v0[crd2])*d10[crd1];
            scalar n1 = (x-v1[crd1])*d21[crd2] - (y-v1[crd2])*d21[crd1];
            scalar n2 = (x-v2[crd1])*d02[crd2] - (y-v2[crd2])*d02[crd1];

            if( (n0>-EPS && n1>-EPS && n2>-EPS) ||
                   n0< EPS && n1< EPS && n2< EPS )
			{
                scalar iz = ( dist - x*norm[crd1] - y*norm[crd2] ) / norm[crd0];
				//assert(iz>=fbox.min[2] && iz<=fbox.max[2]);
                AddIntercept<CoordZ>(x,y,iz, quality, norm );
			}
		}
}

// Si sa che la faccia ha una intercetta sull'asse z-dir di coord xy alla posizione z;
// quindi si setta nei 2 vertici prima e 2 dopo la distanza corrispondente.
template<int CoordZ>
        void AddIntercept( const int x, const int y, const scalar z, const scalar q, const Point3f &n )
{
    scalar esgn = (n[CoordZ] > 0 ? -1 : 1);
    int  zint = floor(z);
    scalar dist=z-zint;  // sempre positivo e compreso tra zero e uno

    for(int k=WN;k<=WP;k++)
    {
        if(zint+k >= SubPartSafe.min[CoordZ] && zint+k < SubPartSafe.max[CoordZ])
        {
            VOX_TYPE *VV;
            if(CoordZ==2) VV=&V(x,y,zint+k);
            if(CoordZ==1) VV=&V(y,zint+k,x);
            if(CoordZ==0) VV=&V(zint+k,x,y);
            scalar nvv= esgn*( k-dist);
            if(!VV->B() || fabs(VV->V()) > fabs(nvv))		{
                *VV=VOX_TYPE(nvv,n,q);
            }
        }
    }
}

// assume che i punti della faccia in ingresso siano stati interized 
bool ScanFace2( const Point3x & v0, const Point3x & v1, const Point3x & v2,
                       scalar quality, const Point3x & norm)//, const int name )	// OK
{

	Box3x fbox;		// Bounding Box della faccia (double)
	Box3i ibox;		// Bounding Box della faccia (int)
	int sx,sy,sz;	// Bounding Box intero
	int ex,ey,ez;

		// Calcolo bbox della faccia
	fbox.Set(v0);
	fbox.Add(v1);
	fbox.Add(v2);

	// BBox intero (nota che il cast a int fa truncation (funge solo perche' v0,v1,v2 sono positivi)

	ibox.min[0] =sx = floor(fbox.min[0]); if( ((scalar)sx)!=fbox.min[0] ) ++sx; // necessario se il punto e'approx a .9999 
	ibox.min[1] =sy = floor(fbox.min[1]); if( ((scalar)sy)!=fbox.min[1] ) ++sy;
	ibox.min[2] =sz = floor(fbox.min[2]); if( ((scalar)sz)!=fbox.min[2] ) ++sz;
	ibox.max[0] =ex = floor(fbox.max[0]);
	ibox.max[1] =ey = floor(fbox.max[1]);
	ibox.max[2] =ez = floor(fbox.max[2]);
	 // Skip faces not colliding current subvolume.
	if(!ibox.Collide(SubPartSafe)) return false;
	
	Point3x d10 = v1 - v0;
	Point3x d21 = v2 - v1;
	Point3x d02 = v0 - v2;

    assert(norm.Norm() >0.999f && norm.Norm()<1.001f);
//    assert(0);
    scalar  dist = norm * v0;


		/**** Rasterizzazione bbox ****/

	// Clamping dei valori di rasterizzazione al subbox corrente
	sx = std::max(SubPartSafe.min[0],sx); ex = std::min(SubPartSafe.max[0]-1,ex);
	sy = std::max(SubPartSafe.min[1],sy); ey = std::min(SubPartSafe.max[1]-1,ey);
	sz = std::max(SubPartSafe.min[2],sz); ez = std::min(SubPartSafe.max[2]-1,ez);

    if(fabs(norm[0]) > fabs(norm[1]) && fabs(norm[0])>fabs(norm[2])) RasterFace<0>(sy,ey,sz,ez,dist,norm,quality,v0,v1,v2,d10,d21,d02);
    else if(fabs(norm[1]) > fabs(norm[0]) && fabs(norm[1])>fabs(norm[2])) RasterFace<1>(sz,ez,sx,ex,dist,norm,quality,v0,v1,v2,d10,d21,d02);
    else RasterFace<2>(sx,ex,sy,ey,dist,norm,quality,v0,v1,v2,d10,d21,d02);


return true;
}

// assume che i punti della faccia in ingresso siano stati interized 
bool ScanFace( const Point3x & v0, const Point3x & v1, const Point3x & v2,
					   double quality, const Point3x & nn)//, const int name )	// OK
{
	const scalar EPS     = scalar(1e-12);
//	const scalar EPS_INT = scalar(1e-20);
	const scalar EPS_INT = .3f;//scalar(1e-20);

#ifndef _NDEBUG
	if(quality==0.0)
	{
		printf("Zero quality face are not allowed!\n");
		exit(-1);
	}
#endif

//	++nfaces;

	Box3x fbox;		// Bounding Box della faccia (double)
	Box3i ibox;		// Bounding Box della faccia (int)
	int sx,sy,sz;	// Bounding Box intero
	int ex,ey,ez;

		// Calcolo bbox della faccia
	fbox.Set(v0);
	fbox.Add(v1);
	fbox.Add(v2);

	// BBox intero (nota che il cast a int fa truncation (funge solo perche' v0,v1,v2 sono positivi)

	ibox.min[0] =sx = floor(fbox.min[0]); if( ((scalar)sx)!=fbox.min[0] ) ++sx; // necessario se il punto e'approx a .9999 
	ibox.min[1] =sy = floor(fbox.min[1]); if( ((scalar)sy)!=fbox.min[1] ) ++sy;
	ibox.min[2] =sz = floor(fbox.min[2]); if( ((scalar)sz)!=fbox.min[2] ) ++sz;
	ibox.max[0] =ex = floor(fbox.max[0]);
	ibox.max[1] =ey = floor(fbox.max[1]);
	ibox.max[2] =ez = floor(fbox.max[2]);
	 // Skip faces not colliding current subvolume.
	if(!ibox.Collide(SubPartSafe)) return false;

		/**** Dati per controllo intersezione ****/

		// Versori degli spigoli della faccia

	Point3x d10 = v1 - v0;
	Point3x d21 = v2 - v1;
	Point3x d02 = v0 - v2;

		// Normale al piano della faccia e distanza origine

	Point3x norm = d10 ^ d21;
	norm.Normalize();
	double  dist = norm * v0;

		/**** Rasterizzazione bbox ****/

	int x,y,z;
	

	// Clamping dei valori di rasterizzazione al subbox corrente
    sx = std::max(SubPartSafe.min[0],sx); ex = std::min(SubPartSafe.max[0]-1,ex);
    sy = std::max(SubPartSafe.min[1],sy); ey = std::min(SubPartSafe.max[1]-1,ey);
    sz = std::max(SubPartSafe.min[2],sz); ez = std::min(SubPartSafe.max[2]-1,ez);

		// Rasterizzazione xy

	if(fabs(norm[2])>EPS_INT)
	for(x=sx;x<=ex;++x)
		for(y=sy;y<=ey;++y)
		{
			double n0 = ((double)x-v0[0])*d10[1] - ((double)y-v0[1])*d10[0];
			double n1 = ((double)x-v1[0])*d21[1] - ((double)y-v1[1])*d21[0];
			double n2 = ((double)x-v2[0])*d02[1] - ((double)y-v2[1])*d02[0];

			if( (n0>-EPS && n1>-EPS && n2>-EPS) ||
				   n0< EPS && n1< EPS && n2< EPS )
			{
				double iz = ( dist - double(x)*norm[0] - double(y)*norm[1] ) / norm[2];
				//assert(iz>=fbox.min[2] && iz<=fbox.max[2]);
				AddXYInt(x,y,iz,-norm[2], quality, nn );
			}
		}
		
		// Rasterizzazione xz

	if(fabs(norm[1])>EPS_INT)
	for(x=sx;x<=ex;++x)
		for(z=sz;z<=ez;++z)
		{
			double n0 = ((double)x-v0[0])*d10[2] - ((double)z-v0[2])*d10[0];
			double n1 = ((double)x-v1[0])*d21[2] - ((double)z-v1[2])*d21[0];
			double n2 = ((double)x-v2[0])*d02[2] - ((double)z-v2[2])*d02[0];

			if( (n0>-EPS && n1>-EPS && n2>-EPS) ||
				   n0< EPS && n1< EPS && n2< EPS )
			{
				double iy = ( dist - double(x)*norm[0] - double(z)*norm[2] ) / norm[1]; 
				//assert(iy>=fbox.min[1] && iy<=fbox.max[1]);
				AddXZInt(x,z,iy,-norm[1], quality,nn  );
			}
		}

			// Rasterizzazione yz

	if(fabs(norm[0])>EPS_INT)
	for(y=sy;y<=ey;++y)
		for(z=sz;z<=ez;++z)
		{
			double n0 = ((double)y-v0[1])*d10[2] - ((double)z-v0[2])*d10[1];
			double n1 = ((double)y-v1[1])*d21[2] - ((double)z-v1[2])*d21[1];
			double n2 = ((double)y-v2[1])*d02[2] - ((double)z-v2[2])*d02[1];

			if( (n0>-EPS && n1>-EPS && n2>-EPS) || 
				   n0< EPS && n1< EPS && n2< EPS )
			{
				double ix = ( dist - double(y)*norm[1] - double(z)*norm[2] ) / norm[0];
				//assert(ix>=fbox.min[0] && ix<=fbox.max[0]);
				AddYZInt(y,z,ix,-norm[0], quality, nn );
			}
		}
		return true;
}
// Si sa che la faccia ha una intercetta sull'asse z-dir di coord xy alla posizione z;
// quindi si setta nei 2 vertici prima e 2 dopo la distanza corrispondente.

void AddXYInt( const int x, const int y, const double z, const double sgn, const double q, const Point3f &n )
{ double esgn = (sgn<0 ? -1 : 1);//*max(fabs(sgn),0.001);
	double dist=z-floor(z);  // sempre positivo e compreso tra zero e uno
	int  zint = floor(z);
	for(int k=WN;k<=WP;k++)
		if(zint+k >= SubPartSafe.min[2] && zint+k < SubPartSafe.max[2])
		{ 
			VOX_TYPE &VV=V(x,y,zint+k);
			double nvv= esgn*( k-dist); 
			if(!VV.B() || fabs(VV.V()) > fabs(nvv))		{
					VV=VOX_TYPE(nvv,n,q);
			}
		}
}
void AddYZInt( const int y, const int z, const double x, const double sgn, const double q, const Point3f &n  )
{ double esgn = (sgn<0 ? -1 : 1);//*max(fabs(sgn),0.001);
	double dist=x-floor(x);  // sempre positivo e compreso tra zero e uno
	int  xint = int(floor(x));
	for(int k=WN;k<=WP;k++)
		if(xint+k >= SubPartSafe.min[0] && xint+k < SubPartSafe.max[0])
		{ 
			VOX_TYPE &VV=V(xint+k,y,z);
			double nvv= esgn*( k-dist); 
			if(!VV.B() || fabs(VV.V()) > fabs(nvv)) {
					VV=VOX_TYPE(nvv,n,q);
			}
		}
}
void AddXZInt( const int x, const int z, const double y, const double sgn, const double q, const Point3f &n  )
{ double esgn = (sgn<0 ? -1 : 1);//*max(fabs(sgn),0.001);
	double dist=y-scalar(floor(y));  // sempre positivo e compreso tra zero e uno
	int  yint = floor(y);
	for(int k=WN;k<=WP;k++)
		if(yint+k >= SubPartSafe.min[1] && yint+k < SubPartSafe.max[1])
		{ 
			VOX_TYPE &VV=V(x,yint+k,z);
			double nvv= esgn*( k-dist); 
			if(!VV.B() || fabs(VV.V()) > fabs(nvv))	{
					VV=VOX_TYPE(nvv,n,q);
			}
		}
}

	

 void Dump(FILE *fp)
 {
	 fprintf(fp,"Volume Info:\n");
	 fprintf(fp,"  BBbox %7.4f %7.4f %7.4f - %7.4f %7.4f %7.4f:\n",bbox.min[0],bbox.min[1],bbox.min[2],bbox.max[0],bbox.max[1],bbox.max[2]);
	 fprintf(fp,"  Size in voxels    %7i %7i %7i (tot: %7.3f M):\n",sz[0],sz[1],sz[2],(double(sz[0]*sz[1])/1000000.0)*sz[2]);
	 fprintf(fp,"  Voxel dimension   %7.4f %7.4f %7.4f \n",voxel[0],voxel[1],voxel[2]);
	 
	 fprintf(fp,"  Size in MacroCell %7i %7i %7i (tot: %7.3f M):\n",rsz[0],rsz[1],rsz[2],double(rsz[0]*rsz[1]*rsz[2])/1000000.0);
     fprintf(fp," Memory Info: \n   Voxel Size %8li b Virtually needed mem %8i Mb\n",
                                        sizeof(VOX_TYPE),int(sizeof(VOX_TYPE)*(_int64)(sz[0])*(_int64)(sz[1])*(_int64)(sz[2])/(1024*1024)));
   if(div!=Point3i(1,1,1))
		 {	 
			fprintf(fp,"  Subdivided in      %6i %6i %6i  (tot: %12i block):\n",div[0],div[1],div[2],div[0]*div[1]*div[2]);
			fprintf(fp,"  Computing subblock %6i %6i %6i :\n",pos[0],pos[1],pos[2]);
			fprintf(fp,"                %6i %6i %6i - %6i %6i %6i :\n",SubPart.min[0],SubPart.min[1],SubPart.min[2],SubPart.max[0],SubPart.max[1],SubPart.max[2]);
			fprintf(fp,"        Safe    %6i %6i %6i - %6i %6i %6i :\n",SubPartSafe.min[0],SubPartSafe.min[1],SubPartSafe.min[2],SubPartSafe.max[0],SubPartSafe.max[1],SubPartSafe.max[2]);
	 
		 }
	 fprintf(fp,"\n");
 }

	int Allocated()
	{int cnt=0;
        for(size_t i=0;i<rv.size();++i)
			if(!rv[i].empty()) cnt++;
			return cnt;
	}

bool Bound1(const int x, const int y, const int z)
{
	return	(x>SubPartSafe.min[0] && x < SubPartSafe.max[0]-1 ) &&
					(y>SubPartSafe.min[1] && y < SubPartSafe.max[1]-1 ) &&
					(z>SubPartSafe.min[2] && z < SubPartSafe.max[2]-1 ) ;
}

/*
Note sull'algoritmo di espansione:

Si riempie i voxel vuoti 

Se il volume e' inizialmente riempito con ii valori delle intercette alla superficie 
nei 2 vertici immediatamente adiacenti all'edge intersecato dalla superficie 
si deve espadnere tale coampo in maniera sensata.

Notare che e' importante che non tutto il campo sia riempito con un approx ella distanza di hausdorf:
il campo deve essere "tagliato" sui bordi della superficie per evitare pasticci. Levoy riempie il campo
solo lungo la direzione dello scanner, io invece riempio lungo la normale alla superficie. In questo modo
si evita il problema che l'espansione e' legata all'acquisizione iniziale

*/


void Expand(scalar AngleThrRad)
{
 int i;
 VolumeIterator< Volume > vi(*this);
 
 float CosThr=math::Cos(AngleThrRad);
// printf("Expand2 angle %f, %f\n",AngleThrRad,CosThr);
 int loccnt=0;

 vi.Restart();
 vi.FirstNotEmpty();
 while(vi.IsValid())
 {
	 if((*vi).B()) // si espande solo i voxel con valori "validi"
		{
			int x,y,z;
			IPos(x,y,z,vi.rpos,vi.lpos);
 			Point3f n=(*vi).N();
      VOX_TYPE vtmp =  (*vi);
			if(Bound1(x,y,z))
			for(i=0;i<26;++i)
						{ 
							float angle = -(nnf[i]*n);    // cos angolo tra la normale alla superficie e la direzione di espansione
							if( fabs(angle)> CosThr ) 
										{
											//bbfloat tt=(*vi).V();
											vtmp.SetV((*vi).V()+len[i]*angle);   // la nuova distanza e' la distanza rispetto al piano passante per il punto a cui si riferisce VV;
											VOX_TYPE &VV= V(x+nni[i][0],y+nni[i][1],z+nni[i][2]);
											if(!VV.B()){
												VV+=vtmp;			
											  loccnt++;
											}
										}
						}
	 }
	vi.Next();
  if(vi.IsValid()) vi.FirstNotEmpty();
 }
 printf("Expand  %8i ",loccnt);
 Normalize(1);
}

// filla i buchi vuoti di un volume; 
// scorre tutti i voxel pieni e aggiunge agli adiacenti vuoti tale valore;
// si riscorre tutti i voxel vuoti e se si sono sommati almeno thr valori sitiene.
// in pratica maggiore il valore di thr meno buchi si riempiono.

void Refill(const int thr,float maxdistance = std::numeric_limits<float>::max() )
{
 int lcnt=0;
 VolumeIterator< Volume > vi(*this);
 vi.Restart();
 vi.FirstNotEmpty();
 
 while(vi.IsValid())
	{
		if((*vi).B())
		{ 
			int x,y,z;
			IPos(x,y,z,vi.rpos,vi.lpos);
			if(Bound1(x,y,z))
				{
					for(int i=0;i<26;++i)
					{ 
						VOX_TYPE &VC= V(x+nni[i][0],y+nni[i][1],z+nni[i][2]);
						if(!VC.B()){
							if(VC.Cnt()==0) lcnt++;
							VC+=(*vi);														
						}
					}
				}
		}
		
		vi.Next();
		
		if(vi.IsValid()) vi.FirstNotEmpty();
		

	}
 printf("ReFill  %8i ",lcnt);
Normalize(thr,maxdistance);
}

/*
Attraversa il volume e modifica il valore di campo in modo da creare una offset surface che si connetta 
bene con la superficie originale
il parametro specifica dove dovrebbe passare l'offset surface

L'idea e' quella di fare un altro zero del distance field al threshold specificato

La cosa deve essere smooth 
quindi scelgo una funzione che abbia 2 zeri (in zero e in thr) e 
*/
void Offset(const float thr) 
{
 int lcnt=0;
 VolumeIterator< Volume > vi(*this);
 vi.Restart();
 vi.FirstNotEmpty();
 float thr2=thr/2.0;
 while(vi.IsValid())
	{
		if((*vi).B())
		{ 
			float vv=(*vi).V();
			if(thr<0) if(vv<thr2) vv=thr-vv;
			if(thr>0) if(vv>thr2) vv=thr-vv;

				(*vi).SetV(vv);
		}
		
		vi.Next();
		
		if(vi.IsValid()) vi.FirstNotEmpty();
		

	}
 printf("ReFill  %8i ",lcnt);
 Normalize(thr);
}

// prende un volume e mette a true il campo b di tutti i voxel che hanno un valore significativo.
// thr indica il numero minimo di valori che si devono essere sommati sul voxel;
// di solito e' uguale a 1; 
int  Normalize(int thr, float maxdistance=std::numeric_limits<float>::max() )
{
 VolumeIterator< Volume > vi(*this);
 vi.Restart();
 vi.FirstNotEmpty();
 int loccnt=0;
 while(vi.IsValid())
	{
	 if(!(*vi).B()) 
		{
		  if((*vi).Normalize(thr))
					++loccnt;
      if(math::Abs((*vi).V())>maxdistance) *vi=VOX_TYPE::Zero();
		 }
	vi.Next();
  if(vi.IsValid()) vi.FirstNotEmpty();
	}
 printf("Normalize(%i) %8i voxels\n",thr,loccnt);
 return loccnt;
}



// Salva 
void SlicedPPMQ( const char * filename,const char *tag,int SliceNum)
	{
        std::string basename=filename;
        std::string name;
		int ix,iy,iz;
		Color4b Tab[100];
		for(int ii=1;ii<100;++ii)
			Tab[ii].ColorRamp(0,100,ii);
		Tab[0]=Color4b::Gray;
		
    int ZStep=sz[2]/(SliceNum+1);
		for(iz=ZStep;iz<sz[2];iz+=ZStep)
		if(iz>=SubPartSafe.min[2] && iz<SubPartSafe.max[2])
		{
			name=SFormat("%s%03i%s_q.ppm",filename,iz,tag);
			FILE * fp = fopen(name.c_str(),"wb");
			fprintf(fp,
				"P6\n"
				"%d %d\n"
				"255\n"
				,sz[1]
				,sz[0]
			);
			unsigned char rgb[3];
			for(ix=0;ix<sz[0];++ix)
			{
				for(iy=0;iy<sz[1];++iy)
				{
					if(	ix>=SubPartSafe.min[0] && ix<SubPartSafe.max[0] &&
							iy>=SubPartSafe.min[1] && iy<SubPartSafe.max[1])
						{
							if(!V(ix,iy,iz).B())	{
								rgb[0]=rgb[1]=rgb[2]=64;
							}
							else
							{ 
								float vv=V(ix,iy,iz).Q();
								int qi=std::min(V(ix,iy,iz).Q()*100.0f,100.0f);

								if( vv>0)		{
									rgb[0]=Tab[qi][0]; 
									rgb[1]=Tab[qi][1]; 
									rgb[2]=Tab[qi][2]; 
								}
								else if(vv<0)
								{
									rgb[0]=128;
									rgb[1]=255+32*vv; 
									rgb[2]=0;//V(ix,iy,iz).Q()*256;
								}
								else  	{
									rgb[0]=255;	rgb[1]=255; rgb[2]=255;
								}
							}
					}
					else{
						rgb[0]=rgb[1]=rgb[2]=64;
					}
					fwrite(rgb,3,1,fp);
				}
			}
			fclose(fp);
		}
	}

void SlicedPPM( const char * filename,const char *tag,int SliceNum=1)
	{
        std::string basename=filename;
        std::string name;
		int ix,iy,iz;
    int ZStep=sz[2]/(SliceNum+1);
		for(iz=ZStep;iz<sz[2];iz+=ZStep)
		if(iz>=SubPartSafe.min[2] && iz<SubPartSafe.max[2])
		{
			name=SFormat("%s_%03i_%s.ppm",filename,iz,tag);
      printf("Saving slice '%s'",name.c_str());
			FILE * fp = fopen(name.c_str(),"wb");
			if(!fp) return;
			fprintf(fp,
				"P6\n"
				"%d %d\n"
				"255\n"
				,sz[1]
				,sz[0]
			);
			unsigned char rgb[3];
			for(ix=0;ix<sz[0];++ix)
			{
				for(iy=0;iy<sz[1];++iy)
				{
					if(	ix>=SubPartSafe.min[0] && ix<SubPartSafe.max[0] &&
							iy>=SubPartSafe.min[1] && iy<SubPartSafe.max[1])
						{
							if(!V(ix,iy,iz).B())	{
								rgb[0]=rgb[1]=rgb[2]=64;
							}
							else
							{ 
								float vv=V(ix,iy,iz).V();
								if( vv>0)		{
									rgb[0]=255-32*vv; 
									rgb[1]=128;
									rgb[2]=0;//V(ix,iy,iz).Q()*256;
								}
								else if(vv<0)
								{
									rgb[0]=128;
									rgb[1]=255+32*vv; 
									rgb[2]=0;//V(ix,iy,iz).Q()*256;
								}
								else  	{
									rgb[0]=255;	rgb[1]=255; rgb[2]=255;
								}
							}
					}
					else{
						rgb[0]=rgb[1]=rgb[2]=64;
					}
					fwrite(rgb,3,1,fp);
				}
			}
			fclose(fp);
		}
	}
 template < class VertexPointerType >
  void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointerType &v)
	{
			float f1 = Val(p1.X(), p1.Y(), p1.Z());
			float f2 = Val(p2.X(), p2.Y(), p2.Z());
			float u = (float) f1/(f1-f2);
			v->P().X() = (float) p1.X()*(1-u) + u*p2.X();
			v->P().Y() = (float) p1.Y();
			v->P().Z() = (float) p1.Z();
      v->Q()=cV(p1.X(), p1.Y(), p1.Z()).Q();
      v->C()=cV(p1.X(), p1.Y(), p1.Z()).C4b();
	}
	
  template < class VertexPointerType >
  void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointerType &v)
	{
			float f1 = Val(p1.X(), p1.Y(), p1.Z());
			float f2 = Val(p2.X(), p2.Y(), p2.Z());
			float u = (float) f1/(f1-f2);
			v->P().X() = (float) p1.X();
			v->P().Y() = (float) p1.Y()*(1-u) + u*p2.Y();
			v->P().Z() = (float) p1.Z();
      v->Q()=cV(p1.X(), p1.Y(), p1.Z()).Q();
			v->C()=cV(p1.X(), p1.Y(), p1.Z()).C4b();
	}

	template < class VertexPointerType>
  void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointerType &v)
	{
			float f1 = Val(p1.X(), p1.Y(), p1.Z());
			float f2 = Val(p2.X(), p2.Y(), p2.Z());
			float u = (float) f1/(f1-f2);
			v->P().X() = (float) p1.X();
			v->P().Y() = (float) p1.Y();
			v->P().Z() = (float) p1.Z()*(1-u) + u*p2.Z();
      v->Q()=cV(p1.X(), p1.Y(), p1.Z()).Q();
			v->C()=cV(p1.X(), p1.Y(), p1.Z()).C4b();
	}

};



template < class VOL >
class VolumeIterator
{
 public:
	 VOL &V;
   //vector<VOL::voxel_type> vi;
	 VolumeIterator(VOL &_VV):V(_VV) {}

	 //Point3i curPos;
	int rpos;
  int lpos;	

	void Restart(){rpos=0;lpos=0;}
 private :
 public:


	void Set(const Point3i &p)
		{
		 //curPos=p;
		 V.Pos(p[0],p[1],p[2],rpos,lpos);
		}
  bool FirstNotEmpty()
	{
		
		//Dump();
        typename std::vector<std::vector<typename VOL::voxel_type> >::iterator rvi=V.rv.begin()+rpos;
		do
		{
			if((*rvi).empty())
			{
				while(rvi!=V.rv.end() && (*rvi).empty()) ++rvi;
				if(rvi==V.rv.end())
				{
					rpos=-1;
					return false;
				}
				rpos= rvi-V.rv.begin();
				lpos=0;
			} 
            typename std::vector<typename VOL::voxel_type>::iterator lvi= (*rvi).begin()+lpos;
			// un voxel e' non vuoto se ha b!=0;
			while(lvi!=(*rvi).end() && !((*lvi).B() || (*lvi).Cnt()>0)) {
				++lvi;
			}	
			if(lvi!=(*rvi).end()) 
			{
				lpos= lvi-(*rvi).begin();
				//V.IPos(p[0],p[1],p[2],rpos,lpos);
				//Dump();
				return true;
			}
			else lpos=0;
			++rvi;
			rpos= rvi-V.rv.begin();
				
		} while (rvi!=V.rv.end());
		rpos=-1;
		return false;
	}
	
	typename VOL::voxel_type &operator *() 
		{
		  assert(rpos>=0 && lpos >=0);
			return V.rv[rpos][lpos];
		}
	bool Next()
	{
		assert(IsValid());
		if(lpos< VOL::BLOCKSIDE() * VOL::BLOCKSIDE() * VOL::BLOCKSIDE() -1)
		{
			++lpos;
			//V.IPos(p[0],p[1],p[2],rpos,lpos);
			return true;
		}
        if(rpos < int(V.rv.size()-1))
		{
			lpos=0;
			++rpos;
			//V.IPos(p[0],p[1],p[2],rpos,lpos);
			return true;
		}
		rpos=-1;
		lpos=-1;
		return false;	
	}

	bool IsValid() { 
		return rpos>=0;
	}
  void Dump()
		{
		int x,y,z;
		V.IPos(x,y,z,rpos,lpos);
		printf("Iterator r %4i l %4i (%3i %3i %3i)\n",rpos,lpos,x,y,z);
	}

 

};
#endif
