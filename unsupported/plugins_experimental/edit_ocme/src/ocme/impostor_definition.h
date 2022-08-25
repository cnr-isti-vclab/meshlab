#ifndef _OCME_IMPOSTOR_DEF_
#define _OCME_IMPOSTOR_DEF_

 

#include <vcg/space/box3.h>
#include "stdmatrix3.h"
#include "plane_box_quantization.h"
#include "cell.h" 




struct OCME;
struct Impostor{
	Impostor(){ClearAll();non_empty_children = 0;}
	/* box*/
	vcg::Box3f box;

	/* data for cumulating the impostors (sparse for editing) */
	stdMatrix3Sparse<unsigned int,8> n_samples;
	stdMatrix3Sparse<vcg::Point3<char>,8> normals;
	stdMatrix3Sparse<vcg::Point3<char>,8> centroids;
    stdMatrix3Sparse<vcg::Point3<unsigned char>,8> colors;

	/* data for cumulating the impostors (compact for rendering) */
	std::vector<vcg::Point3f> positionsV;
	std::vector<vcg::Point3f> normalsV;
	std::vector<vcg::Point3<unsigned char> > colorsV;

	void SparseToCompact();
	void CompactToSparse();


	vcg::Point3<unsigned char> Acc_0_255(vcg::Point3<unsigned char> p,vcg::Point3<unsigned char> d, unsigned int n);
	vcg::Point3<char> Acc(vcg::Point3<char> p,vcg::Point3<char> d, unsigned int n);

	static unsigned int & Gridsize() {static unsigned int gridsize; return gridsize; }

	struct  PointNormal {
		PointNormal( vcg::Point3<char> _p  , vcg::Point3<char> _n, vcg::Point3<unsigned char> _c){
									p = _p;
									n = _n;
									c = _c;
		}
		PointNormal(){};
		vcg::Point3<char> p,n;
		vcg::Point3<unsigned char>  c;
	};

	struct  PointCell: public std::pair<stdMatrix3Sparse<unsigned short,8>::IndexType, PointNormal > {
			PointCell( stdMatrix3Sparse<unsigned int,8>::IndexType id,PointNormal pn ){
					this->first = id;
					this->second = pn;
			}
	};

	typedef std::vector<PointCell >::iterator  PointCellIterator;

	std::vector<PointCell > proxies;

	// one bit for each of the 8 children that says if its occupancy is != 0
	unsigned char non_empty_children;

	// size of a subcell
	float cellsize;
	
	// how many proxies are created from the data of this cell
	unsigned int data_occupancy;

	// what is the occupancy of the cell
	// The occupancy of a cell is computed as the maximum between the data_occupancy/gridsize^3 and the the average occupancy of the 8 children
	float occupancy;


	// converts from float to char (inside a subcell). a is is intended in the frame with origin at lower
	// corner of the subcell, i.e. 0 -> -127, cellsize -> 127, cellsize/2 ->0
    char F2C(float a) { return  (char) ((a / cellsize -0.5) * 255); };
    char F2C_01(float a) { return (char) (a *0.5 * 255); };

    vcg::Point3<unsigned char> F2C(vcg::Color4b  a) {return vcg::Point3<unsigned char>(a[0],a[1],a[2]);}
    vcg::Point3<char> F2C(vcg::Point3f  a, int ii, int jj, int kk) {
                    a = a- box.min;
                    a[0] -= ii *cellsize;
                    a[1] -= jj *cellsize;
                    a[2] -= kk *cellsize;
                    return vcg::Point3<char>(F2C(a[0]),F2C(a[1]),F2C(a[2]));
    };

	vcg::Point3<char> F2C_01(vcg::Point3f  a) { return vcg::Point3<char>(F2C_01(a[0]),F2C_01(a[1]),F2C_01(a[2])); };

	float  C2F(char a, unsigned int i, float ax){
			return box.min[ax] + i*cellsize + ((a/255.f)+0.5)* cellsize;
	}
	vcg::Point3f  C2F(stdMatrix3Sparse<vcg::Point3<char> ,8>::It si ){
			unsigned char i,j,k;
			 vcg::Point3<char> a  = (*si).second;
			stdMatrix3Sparse<vcg::Point3<char>,8>::Index((*si).first,i,j,k);
			return vcg::Point3f(C2F(a[0],i,0),C2F(a[1],j,1),C2F(a[2],k,2));
	}

	float  C2F_01(char a){
			return a/(255.f*0.5f);
	}

    void GetPointNormalColor( PointCell pn, vcg::Point3f & p, vcg::Point3f & n,vcg::Point3<unsigned char> & c){
		unsigned char i,j,k;
		stdMatrix3Sparse<vcg::Point3<char>,8>::Index(pn.first,i,j,k);
        vcg::Point3<char> & a = pn.second.p;
		p = vcg::Point3f(C2F (a[0],i,0),C2F (a[1],j,1),C2F (a[2],k,2));
        vcg::Point3<char> & b = pn.second.n;
		n = vcg::Point3f(C2F_01(b[0]),C2F_01(b[1]),C2F_01(b[2])).Normalize();
        c = pn.second.c;
    }

	void Render();

    void AddSample( vcg::Point3f   p,  vcg::Point3f   n, vcg::Color4b )	;
		
	// add samples from the faces of a mesh
	template <class MeshType>
	void	AddSamplesFromFace(MeshType & m);

	// add samples from the vertices  of a mesh
	template <class MeshType>
	void	AddSamplesFromVertex(MeshType & m);
	
	// add samples passed
	void AddSamples(std::vector<vcg::Point3f>   & samples);

	void AddSamplesFromImpostor(Impostor * ch);

	// get samples
	void GetSamples(std::vector<vcg::Point3f>   & samples);

	/* 
	Fit the planes with the current state of the samples
	and  if 	and_data_occupancy also compute data_occupancy
	*/
//	void  FitPlanes(bool and_data_occupancy = true );
	void SetCentroids(bool and_data_occupancy = true );

	// compute how many cells are occupied
	void ComputeDataOccupancy();

	// Init/clear those data structures needed for cumulating the data
	void InitDataCumulate( vcg::Box3f );
	void ClearDataCumulate();

	void ClearAll();

	void CreatePolygons(OCME*o,CellKey ck);

	void Create(OCME*o,CellKey ck);

	void GetSamplesFromPolygons(std::vector<vcg::Point3f> & smp);
 

	void Render(bool drawsubcells=false);

	/* serialization */
	int SizeOf();
	char * Serialize (char * ptr );
	char * DeSerialize (char * ptr);

};



#endif
