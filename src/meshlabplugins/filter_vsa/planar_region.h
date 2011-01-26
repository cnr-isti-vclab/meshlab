#ifndef VCGLIB_TRIANGLESREGION
#define VCGLIB_TRIANGLESREGION

#include <list>
#include <vcg/space/point2.h>
#include <vcg/space/box2.h>
#include <vcg/space/plane3.h>
#include <vcg/space/intersection3.h>
#include <vcg/space/line3.h>
#include <vcg/space/fitting3.h>
#include <vcg/math/shot.h>

#include "region_growing.h"

template<class MeshType>
class PlanarRegion{
public:
	typedef PlanarRegion RegionType;
	typedef typename MeshType  MeshType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::FaceType::CoordType CoordType;
    typedef typename MeshType::FaceType::ScalarType ScalarType;
	typedef typename std::vector<typename MeshType::FaceType*>::iterator FaceIterator;
	typedef typename std::list<PlanarRegion*>::iterator AdjIterator;
	typedef FaceError<FaceType> FaceError;
	FaceError  worst;

	int id,size;
	int cleansize;
        ScalarType epsilon;
        ScalarType approx_err;
        ScalarType approx_var;
        int changed;
	// faces belonging to the region
	std::vector<FaceType*> face;

	// adjacent regions
	std::list<RegionType*> adj,nx_adj;

    PlanarRegion():isd(false),size(0){};

	bool isd;
	// planes characterizing the region
    vcg::Plane3<ScalarType,true> p;

	CoordType center;
	vcg::Color4b color;

	ScalarType	PlaneFittingError(std::vector<CoordType> &  samples,vcg::Plane3<float> p);


		// evaluate the gain if the triangle is added to the region
    ScalarType Evaluate( FaceType & f);

	// update the approximation error of the region
	void  UpdateError( );

	// check if two regions are mergeable in one
	// i.e. if they have the same planes and coincide i non extreme
	ScalarType Mergeable(  PlanarRegion & tr);

	// set the region as adjacent to this one
	void Connect( PlanarRegion * tr);

	// compute the faces which are on the border
	void ComputeOnBorder( std::vector<FaceType*> & onBorder);

	// clean the list of adjacencies
	void Clean();

	// refit the plane
	void Fit(std::vector<FaceType*>& faces, vcg::Plane3f & plane, float & err);
	void Refit();

	// keep the best % percentile
	void CutOff( FaceType*f);
	
	// restart from the best fitting face
    void Restart();


	void Decorate();

};


        template<class MeshType>
typename PlanarRegion<MeshType>::ScalarType
        PlanarRegion< MeshType>:: PlaneFittingError( std::vector<CoordType> &  samples,vcg::Plane3<float> p){

                typename PlanarRegion<MeshType>::ScalarType err =0.0;
                typename std::vector< CoordType>::iterator  si;
                for(si = samples.begin(); si != samples.end(); ++si)
                        err += fabs((*si)*p.Direction()-p.Offset());
                return err/samples.size();
}

// evaluate the gain if the triangle is added to the region
template<class MeshType>
typename PlanarRegion<MeshType>::ScalarType PlanarRegion< MeshType>::Evaluate( FaceType & f){

        vcg::Plane3<typename PlanarRegion<MeshType>::ScalarType >pl;
	std::vector< typename PlanarRegion<MeshType>::CoordType > samples;
	FaceIterator fi;

	return (vcg::NormalizedNormal(f)-p.Direction()).SquaredNorm()*vcg::DoubleArea(f)*0.5;
}

	// add the face to the region
//template<class MeshType>
//void PlanarRegion< MeshType>::Add( FaceType & f){
//	face.push_back(&f);
//	f.r = (int*) this;
//	if(f.r!=f.r_old) ++changed;
//	f.r_old = f.r;
//	++size;
//}

template <class PlaneType>
bool Similar(PlaneType p1,PlaneType p2, double epsilon=0.1){
	typename PlaneType::ScalarType ang;
	ang = p1.Direction()*p2.Direction();
	return ( ang  > 1- epsilon);
}

	// check if two regions are mergeable in one
	// i.e. if they have the same planes and coincide i non e extreme
template<class MeshType>
typename PlanarRegion< MeshType>::ScalarType PlanarRegion< MeshType>::Mergeable(  PlanarRegion & tr){
/*
two regions are mergeable if:
- they have the same planes
- they have an extreme in common
or
- one is surrounded by the other which is much bigger
*/

	//if(!Similar(this->p,tr.p))
	//	return false;

	vcg::Plane3<ScalarType> plane;
	ScalarType fit_error,new_error=0;

	if( (this->adj.size() == 1) && (tr.face.size()/float(this->face.size()) > 5) ||
		(tr.adj.size() == 1) && (this->face.size()/float(tr.face.size()) > 5)  
		)
		return true;


	std::vector<FaceType*> merged;
	merged.insert(merged.end(),this->face.begin(),this->face.end());
	merged.insert(merged.end(),tr.face.begin(),tr.face.end());
	Fit(merged,plane,fit_error);
	for(std::vector<FaceType*> ::iterator fi= merged.begin(); fi != merged.end(); ++fi)
		new_error+= Evaluate(**fi);
	new_error/=merged.size();

	return new_error < this->approx_err+tr.approx_err;

}

	// merge the region tr into this one
//template<class MeshType>
//void PlanarRegion< MeshType>::Merge( PlanarRegion & tr){
//	assert(!tr.isd);
//	FaceIterator fi;
//	AdjIterator ai;
//	for(fi = tr.face.begin();fi != tr.face.end(); ++fi)
//		Add(*(*fi));
//		for(ai= tr.adj.begin(); ai != tr.adj.end();++ai) if( !(*ai)->isd && (*ai)!=this)
//		 nx_adj.push_back(*ai);
//	tr.face.clear();
//	tr.isd = true;
//}

template<class MeshType>
void PlanarRegion< MeshType>::ComputeOnBorder( std::vector<FaceType*> & onBorder){
 //       typename std::vector<FaceType*>::iterator fi;
	//for(fi = face.begin();fi != face.end(); ++fi)
	//	if((*fi)->FFp(0)!=(*fi))
	//		if( ( (*fi)->FFp(0)->r == NULL) ||( (*fi)->FFp(1)->r == NULL) || ( (*fi)->FFp(1)->r == NULL))
	//			onBorder.push_back(*fi);

}
template<class MeshType>
void PlanarRegion< MeshType>::Connect( PlanarRegion * tr){
	adj.push_back(tr);
}

template<class MeshType>
void PlanarRegion< MeshType>::Clean(){
		adj.sort(); 
		adj.unique();
		nx_adj.clear();
		}
	// return the candidates to be added to the region



	// refit the planes
template<class MeshType>
void PlanarRegion< MeshType>::Fit(std::vector<FaceType*>& faces, vcg::Plane3f & plane, float & err){
	FaceIterator fi;
	center = CoordType(0.0,0.0,0.0);
	std::vector<CoordType>  samples;

	if(faces.size()<3){
			samples.push_back((*faces.begin())->V(0)->P());
			samples.push_back((*faces.begin())->V(1)->P());
			samples.push_back((*faces.begin())->V(2)->P());
			center+=vcg::Barycenter(*(*faces.begin()))*3;
	}else
	for( fi = faces.begin(); fi != faces.end(); ++fi)
	{
		
		//AddSamples(samples,*(*fi));
		samples.push_back(vcg::Barycenter(*(*fi)));
		center+=vcg::Barycenter(*(*fi));
	}
	center*=1/(typename MeshType::ScalarType)(samples.size());

	if(samples.size()==3){
		plane.SetDirection(vcg::Normal(vcg::Triangle3<typename PlanarRegion<MeshType>::ScalarType >(samples[0],samples[1],samples[2])));
		typename MeshType::ScalarType off=samples[0]*plane.Direction();
		plane.SetOffset(off);
	}else
	{ 	vcg::PlaneFittingPoints<typename MeshType::ScalarType >(samples,plane);
	}

	err = PlaneFittingError(samples,plane);
}

template<class MeshType>
void PlanarRegion< MeshType>::Refit(){
	Fit(face,p,this->approx_err);
}

template<class MeshType>
void PlanarRegion< MeshType>::UpdateError( ){
        FaceIterator fi;

        float err=0,var=0,max_err=-1.0;
        for(fi=face.begin(); fi != face.end(); ++fi)
		{
			float e =Evaluate(**fi);
			err+=e;
			if(e > max_err) {
					worst.val = e;
					worst.f = *fi;
					max_err = e;
			 }
		}
        approx_err =err;
        var = 0.0;
        approx_var = var;
}




template<class MeshType>
void PlanarRegion< MeshType>::
Decorate(){
	glBegin(GL_LINES);
	glVertex(this->face[0]->V(0)->P());
	glVertex(this->face[0]->V(0)->P()+ this->p.Direction());
	glEnd();
}


#endif









