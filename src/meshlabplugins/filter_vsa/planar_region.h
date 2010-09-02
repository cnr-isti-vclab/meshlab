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


template <class FaceType>
struct FaceError{
	FaceType * f;
	float val;
	FaceError(){};
	FaceError(FaceType * _f,double _val):f(_f),val(_val){}
	const bool operator <(const FaceError & o) const {return val < o.val;}
};

template<class MeshType>
class PlanarRegion{
public:
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::FaceType::CoordType CoordType;
        typedef typename MeshType::FaceType::ScalarType ScalarType;
	typedef typename std::vector<typename MeshType::FaceType*>::iterator FaceIterator;
	typedef typename std::list<PlanarRegion*>::iterator AdjIterator;
	typedef FaceError<FaceType> FaceErr;

	FaceErr  worst;

        PlanarRegion(){isd=false;size = 0;};


	int id,size;
	int cleansize;
        ScalarType epsilon;
        ScalarType approx_err;
        ScalarType approx_var;
        int changed;
	// faces belonging to the region
	std::vector<FaceType*> face;


	// planes characterizing the region
        vcg::Plane3<ScalarType,true> p;
	
	vcg::Color4b color;

	CoordType center;

	// adjacent regions
	std::list<PlanarRegion*> adj,nx_adj;

	// some flag
	bool isd;

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
	void Refit();

	// keep the best % percentile
	void CutOff( FaceType*f);
	
	// restart from the best fitting face
        void Restart();

        typedef std::pair< vcg::Shotf, vcg::Box2i > Pov;
        std::vector<Pov>   povs;

        void ComputeShot(int *vpsize, int pps, std::vector<Pov> &  povs);
        void ComputeShot(int *vpsize, int pps);


        ScalarType	PlaneFittingError(std::vector<CoordType> &  samples,vcg::Plane3<float> p);
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
	
	vcg::Point4<typename PlaneType::ScalarType> pl1,pl2;
	pl1[0] = p1.Direction()[0];pl1[1] = p1.Direction()[1];pl1[2] = p1.Direction()[2];	pl1[3] = p1.Offset();
	pl2[0] = p2.Direction()[0];pl2[1] = p2.Direction()[1];pl2[2] = p2.Direction()[2];	pl2[3] = p2.Offset();

	pl1.Normalize();
	pl2.Normalize();

        ang = pl1*pl2;
	//err = fabs(p1.Direction()[0] * p2.Direction()[0] +
	//			p1.Direction()[1] * p2.Direction()[1] +
	//			p1.Direction()[2] * p2.Direction()[2] +
	//			(-p1.Offset()* (- p2.Offset())));

	return (fabs(ang) > 1- epsilon);
}

	// check if two regions are mergeable in one
	// i.e. if they have the same planes and coincide i non e extreme
template<class MeshType>
typename PlanarRegion< MeshType>::ScalarType PlanarRegion< MeshType>::Mergeable(  PlanarRegion & tr){
/*
two regions are mergeable if:
- they have the same planes
- they have an extreme in common
*/
	return (Similar(this->p,tr.p) && (approx_var < 0.1) && (tr.approx_var<0.1));
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
        typename std::list<FaceType*>::iterator fi;
	for(fi = face.begin();fi != face.end(); ++fi)
		if((*fi)->FFp(0)!=(*fi))
			if( ( (*fi)->FFp(0)->r == NULL) ||( (*fi)->FFp(1)->r == NULL) || ( (*fi)->FFp(1)->r == NULL))
				onBorder.push_back(*fi);

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
void PlanarRegion< MeshType>::Refit(){
	
	std::vector< typename PlanarRegion<MeshType>::CoordType > samples;
	FaceIterator fi;
	center = CoordType(0.0,0.0,0.0);

	if(face.size()<3){
			samples.push_back((*face.begin())->V(0)->P());
			samples.push_back((*face.begin())->V(1)->P());
			samples.push_back((*face.begin())->V(2)->P());
			center+=vcg::Barycenter(*(*face.begin()))*3;
	}else
	for( fi = face.begin(); fi != face.end(); ++fi)
	{
		
		//AddSamples(samples,*(*fi));
		samples.push_back(vcg::Barycenter(*(*fi)));
		center+=vcg::Barycenter(*(*fi));
	}
	center*=1/(typename MeshType::ScalarType)(samples.size());


	if(samples.size()==3){
                p.SetDirection(vcg::Normal(vcg::Triangle3<typename PlanarRegion<MeshType>::ScalarType >(samples[0],samples[1],samples[2])));
                typename MeshType::ScalarType off=samples[0]*p.Direction();
		p.SetOffset(off);
	}else
	{ 	vcg::PlaneFittingPoints<typename MeshType::ScalarType >(samples,p);
	}


	approx_err = PlaneFittingError(samples,p);


}

template<class MeshType>
void PlanarRegion< MeshType>::UpdateError( ){
//	std::vector<FaceErr> toSort;
//	std::vector<FaceErr>::iterator si,sit;
        FaceIterator fi,fi1;

	float err=0,var=0,max_err=-1.0;
	for(fi=face.begin(); fi != face.end(); ++fi)
//		toSort.push_back(FaceErr((*fi),Evaluate(**fi)));
	{
	//for(si=toSort.begin(); si != toSort.end(); ++si)
	//{
	float e =Evaluate(**fi);
		err+=e;
		if(e > max_err) {
			worst.val = e;
			worst.f = *fi;
		 }
}
	//}
	approx_err =err;

//	for(si=toSort.begin(); si != toSort.end(); ++si)
//		var+=sqrt(((*si).val-err)*((*si).val-err));
//	var/=toSort.size();
	var = 0.0;
	approx_var = var;
}

template<class MeshType>
void PlanarRegion< MeshType>::CutOff(FaceType*f){
	std::vector<FaceErr> toSort;
	std::vector<FaceType*> intervals_f;
        typename std::vector<FaceErr>::iterator si,sit,worst;
	FaceIterator fi,fi1;
	for(fi=face.begin(); fi != face.end(); ++fi)
		toSort.push_back(FaceErr((*fi),Evaluate(**fi)));

	float err=0,var=0,max_err=0.0;
	for(si=toSort.begin(); si != toSort.end(); ++si)
	{
		err+=(*si).val;
		if((*si).val > max_err) {
			max_err = (*si).val;
			worst = si;
		}
	}
	err/=toSort.size();
	for(si=toSort.begin(); si != toSort.end(); ++si)
		var+=sqrt(((*si).val-err)*((*si).val-err));
	var/=toSort.size();
	approx_var = var;

	std::vector<int> intervals;
	int k=20,cutoff=0,cut_value=0;

	intervals.resize(k);
	intervals_f.resize(k);
	for(std::vector<int>::iterator ii = intervals.begin(); ii !=  intervals.end(); ++ii)
		(*ii)=0;
//	qDebug("var:   %f\n",var/err);

		for(si = toSort.begin();si!=toSort.end();++si){
			double vvv= (*si).val;
			double ee= (*si).val/(max_err/k);
			int g= (int)floor((*si).val/(max_err/k));
			g=(g==k)?k-1:g;
			assert(g<k);
			intervals[g]++;
			intervals_f[g] = (*si).f;
		}
		int i;
//		for(i = 0; i < k ;++i)
//			qDebug("%d ;",intervals[i]);
//		qDebug("\n");
	
		for(i=1;i< k; ++i)
			if(intervals[i]-intervals[i-1] > cut_value)
			{
				cut_value = intervals[i]-intervals[i-1];
				cutoff=i;
			}
		f= intervals_f[i];
	
}


template<class MeshType>
void PlanarRegion< MeshType>:: ComputeShot( int *vpsize, int pps , std::vector<Pov > &  povs ){


    assert(!face.empty());

    CoordType pn = face[0]->V(0)->P();
    CoordType ori;
    ori = p.Projection(pn);// origin of the coord system
    CoordType Y =p.Direction() ,u,v;
    vcg::GetUV(Y,u,v);
    vcg::Point2<ScalarType> p2;
    vcg::Box2<ScalarType> box;

    // make sure the plane does not intersect the triangles
   float delta = 0.0, projz;
    for(FaceIterator fi = face.begin(); fi != face.end(); ++fi)
        for(int i = 0; i < 3; ++i){
        projz = ((**fi).V(i)->P() - ori) * p.Direction();
        if(  projz > delta)
            delta = projz;
    }

    // project all the vertices on the approximating plane and create the bbox 2d
    ori+=p.Direction()*delta;
    p.Init(ori,p.Direction());
    for(FaceIterator fi = face.begin(); fi != face.end(); ++fi)
        for(int i = 0; i < 3; ++i){
            pn = this->p.Projection((**fi).V(i)->P());
            p2[0] = (pn-ori)*u;
            p2[1] = (pn-ori)*v;
            box.Add(p2);
            }


    int wanted_vpsize[2];
    wanted_vpsize[0] = pps*box.DimX();
    wanted_vpsize[1] = pps*box.DimY();

    int xcount =  wanted_vpsize[0] / vpsize[0]; // how many shots in x
    int ycount =  wanted_vpsize[1] / vpsize[1]; // how any shots in y

    int dx =  ( (wanted_vpsize[0] % vpsize[0]) > vpsize[0]*0.3);
    int dy =  ( (wanted_vpsize[1] % vpsize[1]) > vpsize[1]*0.3);


    float dimx = vpsize[0] * box.DimX() / wanted_vpsize[0];
    float dimy = vpsize[1] * box.DimY() / wanted_vpsize[1];

    for(unsigned int i = 0; i < xcount+dx; ++i)
        for(unsigned int j = 0; j < ycount+dy; ++j){

            vcg::Shotf s;
            vcg::Point2<ScalarType> vp2 = box.min + vcg::Point2<ScalarType>(i*dimx+dimx/2,j*dimy+dimy/2);
            CoordType vp3 = ori + u*vp2[0]+v*vp2[1];

            s.Intrinsics.CenterPx[0] = vpsize[0] / 2;
            s.Intrinsics.CenterPx[1] = vpsize[1] / 2;
            s.Intrinsics.SetFrustum(-dimx/2, dimx/2,-dimx/2,dimy/2,1,vcg::Point2i(vpsize[0],vpsize[1]));
            s.SetViewPoint(vp3);
            s.Intrinsics.cameraType = 1;
            s.LookTowards(-Y,v);

            vcg::Box2i b;b.Add(vcg::Point2i(0,0));
            int xclip = (i==xcount)?wanted_vpsize[0]-xcount*vpsize[0]:vpsize[0];
            int yclip = (j==ycount)?wanted_vpsize[1]-ycount*vpsize[1]:vpsize[1];
            b.Add(vcg::Point2i(xclip,yclip));
            povs.push_back(std::make_pair(s,b));
    }
}



    template<class MeshType>
    void PlanarRegion< MeshType>:: ComputeShot( int *vpsize, int pps ){
        ComputeShot(vpsize,pps,this->povs);
    }


#endif









