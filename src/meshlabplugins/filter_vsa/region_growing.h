#ifndef VCGLIB_REGION_GROWING
#define VCGLIB_REGION_GROWING

#include <list>
#include <vector>

#include <vcg/complex/allocate.h>

template <class FaceType>
struct FaceError{
	FaceType * f;
	float val;
	FaceError(){};
	FaceError(FaceType * _f,double _val):f(_f),val(_val){}
	const bool operator <(const FaceError & o) const {return val < o.val;}
};


template<class MeshType>
struct Region{
	Region():isd(false){}
	typedef Region RegionType;
	typedef typename MeshType  MeshType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::FaceType::CoordType CoordType;
    typedef typename MeshType::FaceType::ScalarType ScalarType;
	typedef typename std::vector<typename MeshType::FaceType*>::iterator FaceIterator;
	typedef typename std::list<Region*>::iterator AdjIterator;
	typedef FaceError<FaceType> FaceError;

	// adjacent regions
	std::list<RegionType*> adj,nx_adj;


	// some flag
	bool isd;

	// evaluate the gain if the triangle is added to the region
    ScalarType Evaluate( FaceType & f);

	// update the approximation error of the region
	void UpdateError( );

	// check if two regions are mergeable in one
	// i.e. if they have the same planes and coincide i non extreme
    ScalarType Mergeable(  RegionType & tr);

	// set the region as adjacent to this one
	void Connect( RegionType * tr);

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

	void Decorate();

};


template < class RegionType>
class RegionGrower{
public:
	RegionGrower():lastAdded(NULL),ave_error(-1),ave_var(-1),n_steps(0){}
  	typedef typename RegionType RegionType;
  	typedef typename RegionType::MeshType MeshType;
  	typedef typename RegionType::FaceType FaceType;
    typedef typename FaceType::ScalarType ScalarType;
	typedef typename std::list<RegionType> ::iterator TriRegIterator;
    typedef typename std::list<RegionType*>::iterator AdjIterator;
    typedef typename RegionType::FaceError FaceError;

	std::list<RegionType> regions;
	std::vector<RegionType*> workingset;
        int n_faces,target_max_regions;
	FaceType * lastAdded;
    ScalarType	 ave_error // average error (over single regions' error)
                ,ave_var// average variance (over single regions' error)
                ,changed	// faces that have changed from previous step ([0..1))
                ,err
                ,target_error; // taget error

	FaceType * worst;
    ScalarType worst_err ;
    MeshType * m;

	typename MeshType:: template  PerFaceAttributeHandle<int*> attr_r;
	typename MeshType:: template  PerFaceAttributeHandle<int*> attr_r_old;

	unsigned int n_steps;//  number of steps done

	struct CandiFace{
		FaceType * f;
		float val;
		RegionType * r;
		char inPlane; // a quale piano di r 
		CandiFace(){};
		CandiFace(FaceType * _f,double _val,RegionType * _r):f(_f),val(_val),r(_r){}
		const bool operator <(const CandiFace & o) const {return val < o.val;}
	};

	std::vector<CandiFace> facesheap;
	std::vector<FaceError  > faceserr;

        struct ErrorEval {

            void Add(const float & v){
                if(!ns){
                if(v < samples[i_min]) i_min = 0; else {++i_min;
                    if(v > samples[i_max])  i_max = 0;else ++i_max;
                }
                if(i_min == samples.size()) {i_min = 0; for(int i= 0; i < samples.size(); ++i) if(samples[i]<samples[i_min]) i_min = i; }
                if(i_max == samples.size()) {i_max = 0; for(int i= 0; i < samples.size(); ++i) if(samples[i]>samples[i_max]) i_max = i; }
            }
                samples.pop_back();samples.push_front(v);
                boxes.pop_back(); boxes.push_front(vcg::Point2f(samples[i_min],samples[i_max]));
                ++ns ;
            }

            float BoxOverlap(){
                float maxsize =  std::max( boxes.back()[1]-boxes.back()[0], (*boxes.begin())[1]-(*boxes.begin())[0]);
                float overlap =  std::max(0.f, std::min(boxes.back()[1],(*boxes.begin())[1])-std::max(boxes.back()[0],(*boxes.begin())[0]));
                assert(overlap <= maxsize);
                return  (maxsize  > 0.f)?overlap / maxsize:0.0;
            }
            float RelativeDecrease(){
                if (ns<2) return std::numeric_limits<float>::max();
                return (samples[0]<10e-22)?0.0:(samples[1]-samples[0])/samples[0];
            }

            void Init(int n ){

            samples.clear();
            boxes.clear();
            for(int i = 0 ; i < n; ++i) samples.push_front(std::numeric_limits<float>::max());
            for(int i = 0 ; i < n; ++i) boxes.push_front(vcg::Point2f(n,n-i));
            i_max = i_min = 0;
            ns = 0;
        }

        private:
            int i_min,i_max;            // index of min and max element in the queue
            std::deque<float> samples;
            std::deque<vcg::Point2f> boxes;
            int ns;

        };

        ErrorEval erroreval;
        // init
        void Init(MeshType & mesh, int n_seeds,int max_regions, float max_err){
            erroreval.Init(10);
            m = &mesh;
            vcg::tri::Allocator<MeshType>::CompactFaceVector(*m);
            vcg::tri::UpdateTopology<MeshType>::FaceFace(*m);

            float area = 0.f;
            for(typename MeshType::FaceIterator fi =  m->face.begin(); fi != m->face.end(); ++fi)
                area += vcg::DoubleArea(*fi);
            area/=2.f;
            target_error = area*max_err*max_err;
            target_max_regions = max_regions;

            // tte an attibute that will store the address in ocme for the vertex
            attr_r = vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<int*> (*m,"r");
            if(!vcg::tri::Allocator<MeshType>::IsValidHandle(*m,attr_r))
                    attr_r = vcg::tri::Allocator<MeshType>::template AddPerFaceAttribute<int*> (*m,"r");

            attr_r_old = vcg::tri::Allocator<MeshType>::template GetPerFaceAttribute<int*> (*m,"r_old");
            if(!vcg::tri::Allocator<MeshType>::IsValidHandle(*m,attr_r_old))
                    attr_r_old = vcg::tri::Allocator<MeshType>::template AddPerFaceAttribute<int*> (*m,"r_old");

            regions.clear();

             for(int i = 0; i < m->face.size(); ++i){
                     attr_r[i] = NULL;
                    attr_r_old[i] = NULL;
                    if( (i%(m->fn/n_seeds))==0)
                                CreateRegion(&m->face[i]);

                }
        }

	// add a region
	void AddRegion(const RegionType  & r){regions.push_back(r);}

	// remove a region
	void DeleteRegion(const typename std::list<RegionType>::iterator & ri){std::remove(ri);}

	// initialize a region
	void CreateRegion(FaceType * fi){
		AddRegion(RegionType());
		RegionType & tr =regions.back();
        AddFaceToRegion(tr,fi);
		tr.Refit();
		tr.color = vcg::Color4b::Scatter(2000,(int)regions.size());	
	}
        void AddFaceToRegion( RegionType & r, FaceType * f){
            r.face.push_back(f);
            attr_r[*f] = (int*) &r;
            if(attr_r[*f]!=attr_r_old[*f]) ++r.changed;
            attr_r_old[*f]  = attr_r[*f];
            ++r.size;
        }

        void Merge(RegionType & r0,RegionType & r1){
            assert(!r1.isd);
            typename RegionType::FaceIterator fi;
            AdjIterator ai;
            for(fi = r1.face.begin();fi != r1.face.end(); ++fi)
                AddFaceToRegion(r0,(*fi));
            for(ai= r1.adj.begin(); ai != r1.adj.end();++ai) 
				if( !(*ai)->isd && (*ai)!=&r0)
				 r0.nx_adj.push_back(*ai);

            r1.face.clear();
            r1.isd = true;
			r0.Refit();
        }


void PushHeap(std::vector<FaceType*> & candi, RegionType & r){
                typename std::vector<FaceType*>::iterator ci;
			for(ci = candi.begin(); ci != candi.end(); ++ci)
 					{
						facesheap.push_back(CandiFace( *ci,-r.Evaluate(*(*ci)), &r));
 						push_heap(facesheap.begin(),facesheap.end());
 					}
}

// the two regions are adjacent
		void Connect(RegionType  * r1,RegionType  * r2){			 
			assert(r1!=r2);
			r1->Connect(r2);
			r2->Connect(r1);
		}

// for each region take the candidates and fill in facesheap
		void Refill( ){
			facesheap.clear();

			 typename std::list<RegionType>::iterator ri;

			std::vector<FaceType*>  candi;
			for(ri = regions.begin(); ri != regions.end(); ++ri) if(!(*ri).isd)
				{
					candi.clear();
					Candidates((*ri),candi);
					PushHeap(candi,*ri);
				}
					std::make_heap(facesheap.begin(),facesheap.end());
		}


        void   Candidates(RegionType & r, std::vector< typename RegionType::FaceType*> & c){
                typename RegionType::FaceIterator fi;

                for(fi = r.face.begin(); fi!= r.face.end(); ++fi)
                        for(int i = 0; i < 3; ++i)
                                if( ((*fi)->FFp(i) != (*fi) ) &&
                                          (attr_r[(*fi)->FFp(i)] != (int*) &r) )
                                                c.push_back((*fi)->FFp(i));
        }

        bool IsRelaxed(){
                ScalarType _ave_error=0;
                ScalarType _ave_var= 0;
                ScalarType _changed = 0.0;
                int nr=0;

                typename std::list<RegionType>::iterator ri;
                worst=NULL;;
                worst_err = -1;
                qDebug("working set size: %d\n",workingset.size());
                for(ri = regions.begin(); ri != regions.end(); ++ri) if(!(*ri).isd){
                        ++nr;
                        (*ri).UpdateError();
                        _ave_error+=(*ri).approx_err;
                        _ave_var+=(*ri).approx_var;
                        _changed+=(*ri).changed;
                        (*ri).changed=0;
                        if((*ri).worst.val*(*ri).size > worst_err){
                                                worst = (*ri).worst.f;
                                                worst_err = (*ri).worst.val*(*ri).size;
                        }
                }


                _ave_error/=nr;
                _ave_var/=nr;
                _changed/=n_faces;

                erroreval.Add(_ave_error);
                qDebug("Err:%f ov: %f-------",_ave_error,erroreval.BoxOverlap());


                return  (erroreval.BoxOverlap() > 0.5) || (erroreval.RelativeDecrease() < 0.1);
            }
	
void GrowStepOnce(){
		CandiFace cf;	
                typename std::list<RegionType>::iterator ri;
		for(ri = regions.begin(); ri != regions.end(); ++ri) if(!(*ri).isd)
				(*ri).Clean();

		if(!facesheap.empty()){
		
			std::vector<FaceType*> toAdd;
			std::pop_heap(facesheap.begin(),facesheap.end());
			cf = facesheap.back();
//			qDebug("err:%f\n",cf.val);
			facesheap.pop_back();
                        if (attr_r[*cf.f]==NULL){
				  lastAdded = cf.f;
                                        AddFaceToRegion(*cf.r,cf.f); // adds to region
					for(int i=0; i < 3;++i) // put the adjacent in the set of faces to possibly add
                                                if( (attr_r[cf.f->FFp(i)] == NULL) )
							toAdd.push_back(cf.f->FFp(i));
					PushHeap(toAdd,*cf.r);
			}
			else
			{
                            int * aa = attr_r[*cf.f] ;
                                if ( attr_r[*cf.f] != (int*)(cf.r) )
                                Connect((RegionType*)attr_r[*cf.f],(RegionType*)cf.r);
			}
		}
		int h = (int)facesheap.size();
 //               qDebug("----> %d\n",h);
	}

	void GrowStep(){
		CandiFace cf;	
        typename std::list<RegionType>::iterator ri;

		n_steps++;
		for(ri = regions.begin(); ri != regions.end(); ++ri) if(!(*ri).isd)
				(*ri).Clean();

		while(!facesheap.empty() ){
				std::vector<FaceType*> toAdd;
				std::pop_heap(facesheap.begin(),facesheap.end());
				cf = facesheap.back();
				//printf("err:%f\n",cf.val);		
				facesheap.pop_back();
				if (attr_r[*cf.f]==NULL){
					AddFaceToRegion( *cf.r,cf.f); // ads to region
					lastAdded = &*cf.f;
					for(int i=0; i < 3;++i) // put the adjacent in the set of faces to possibly add
							if( attr_r[ *cf.f->FFp(i)] == NULL )
								toAdd.push_back(cf.f->FFp(i));
							else
							if(attr_r[*cf.f->FFp(i)] != attr_r[*cf.f])
								Connect((RegionType*)attr_r[*cf.f->FFp(i)],(RegionType*)cf.r);
			
					PushHeap(toAdd,*cf.r);
				}
				else
				{
					if ( attr_r[*cf.f] != (int*)(cf.r) )
					Connect((RegionType*)attr_r[*cf.f],(RegionType*)cf.r);
				}
		}
		int h = (int) facesheap.size();
		printf("----> %d\n",h);
	}

	unsigned int MergeStep(){
		 TriRegIterator tri;
		 unsigned int merged = 0;
         typename RegionType::AdjIterator ai;

		 for(tri = regions.begin(); tri != regions.end(); ++tri)if(!(*tri).isd)
				for(ai = (*tri).adj.begin(); ai != (*tri).adj.end(); ++ai) if(!(*ai)->isd)
					if((*tri).Mergeable(*(*ai))){
						Merge((*tri),*(*ai));
						merged++;
					}


         for(tri = regions.begin(); tri != regions.end(); ++tri){
			 for(ai = (*tri).nx_adj.begin(); ai != (*tri).nx_adj.end();++ai)
				 if(!(*ai)->isd) 
					 (*tri).adj.push_back(*ai);
			 (*tri).adj.sort();
			 (*tri).adj.unique();
             }
		return merged;
	}

	//void TeleportStep(){
	//	 TriRegIterator tri;
 //        typename RegionType::AdjIterator ai;

	//	 for(tri = regions.begin(); tri != regions.end(); ++tri)if(!(*tri).isd){
	//			std::vector<FaceType*> & onBorder
	//			ComputeOnBorder(onBorder);
	//			if(onBorder.empty
	//	 }
	//	
	//}

    bool Restart(){
            std::vector<FaceType*>  candi;
            TriRegIterator ri;
            facesheap.clear();

            if(IsRelaxed()){
                if( (worst_err <= target_error) || (regions.size() >= target_max_regions))
                    return false;
                else
                {
                    erroreval.Init(10);
                    FaceError   wrs;
                    wrs.f = worst;
                    wrs.val = worst_err;
                    printf("worst triangle error %f\n",worst_err);

                    CreateRegion(wrs.f);// CreateRegion changes wr->r

                    // reset state variables
                     ave_error=-1;
                     ave_var=-1;
                     err=0.0;
                     changed=0;
                }
            }
			
	            for(ri = regions.begin(); ri != regions.end(); )
					if((*ri).isd)
						ri = regions.erase(ri);
					else
						++ri;

                 for(ri = regions.begin(); ri != regions.end(); ++ri)
                 {
                         candi.clear();
                         (*ri).Refit();            // fit a plane to the region
                         Restart(*ri);             // clear stuff in the region, move the seed to the best fitting to the plabne
                         Candidates(*ri,candi);    // take the (three) faces candidatees
                         PushHeap(candi,(*ri));    // put the faces on to the heap
                 }
            return true;
        }


            void Restart(RegionType &r){
                    if(!r.face.empty()){
                            r.size=0;
                            float b_err = r.Evaluate(*(*r.face.begin())),err;
                            FaceType* b_face =(*r.face.begin());
                            typename RegionType::FaceIterator fi;

                            for( fi = r.face.begin(); fi != r.face.end(); ++fi)
                            {
                                    err = r.Evaluate(**fi);
                                    if(err < b_err)
                                    {
                                            b_err = err;
                                            b_face = *fi;
                                    }
                            }
                            for( fi = r.face.begin(); fi != r.face.end(); ++fi) attr_r[(*fi)] = NULL;
                            r.face.clear();
                            r.adj.clear();
                            AddFaceToRegion(r,b_face);
                            r.Refit();
                    }
            }

				void MakeCharts(){
					this->Refill();
					while(this->Restart()){
						//do{ 
							this->GrowStep();
						//} while(Restart());
					}
					//while(this->MergeStep());
				}

};

#endif
