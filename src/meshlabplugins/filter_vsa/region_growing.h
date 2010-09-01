#ifndef VCGLIB_REGION_GROWING
#define VCGLIB_REGION_GROWING

#include <list>
#include <vector>


#include "planar_region.h"
#include <vcg/complex/trimesh/allocate.h>

template <class STD_CONTAINER>
void RemoveDuplicates(STD_CONTAINER & cont){
        if(cont.empty()) return;
        std::sort(cont.begin(),cont.end());
        typename STD_CONTAINER::iterator newend = std::unique(cont.begin(),cont.end());
        cont.erase(newend,cont.end());
}
template <class MeshType>
class RegionGrower{
public:
	RegionGrower():lastAdded(NULL),ave_error(-1),ave_var(-1),n_steps(0){}
        typedef PlanarRegion<MeshType> RegionType;
	typedef typename RegionType::FaceType FaceType;
        typedef typename FaceType::ScalarType ScalarType;
	typedef typename std::list<RegionType> ::iterator TriRegIterator;
        typedef typename std::list<RegionType*>::iterator AdjIterator;

	std::list<RegionType> regions;
	std::vector<RegionType*> workingset;
	int n_faces;
	FaceType * lastAdded;
        ScalarType	ave_error // average error (over single regions' error)
				  ,ave_var,// average variance (over single regions' error)
					changed,	// faces that have changed from previous step ([0..1))
					err;
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
	std::vector<FaceError<FaceType> > faceserr;

        // init
        void Init(MeshType & mesh, int n_seeds){
            m = &mesh;
            vcg::tri::Allocator<MeshType>::CompactFaceVector(*m);
            vcg::tri::UpdateTopology<MeshType>::FaceFace(*m);


            // create an attibute that will store the address in ocme for the vertex
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
            for(ai= r1.adj.begin(); ai != r1.adj.end();++ai) if( !(*ai)->isd && (*ai)!=&r0)
                r0.nx_adj.push_back(*ai);
            r1.face.clear();
            r1.isd = true;
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
                        ScalarType _changed=0,nr=0;
			bool itis = true;

                        typename std::list<RegionType>::iterator ri;
			worst=NULL;; 
			worst_err = -1; 
			printf("working set size: %d\n",workingset.size());
			for(ri = regions.begin(); ri != regions.end(); ++ri) if(!(*ri).isd){
				++nr;
				(*ri).UpdateError();
				_ave_error+=(*ri).approx_err;
				_ave_var+=(*ri).approx_var;
				_changed+=(*ri).changed;
				(*ri).changed=0;
				//faceserr.push_back((*ri).worst);
				//push_heap(faceserr.begin(),faceserr.end());
				if((*ri).worst.val*(*ri).size > worst_err){
							worst = (*ri).worst.f;
							worst_err = (*ri).worst.val*(*ri).size;
				}
			}


			_ave_error/=nr;
			_ave_var/=nr;
			_changed/=n_faces;

                        qDebug("%f %f %f-------",_ave_error,_ave_var,_changed);
			//(*regions.begin()).CutOff(worst);

			if(ave_error==-1) itis=false;
			else
			if (_changed<changed) itis=false;
//			else
//				if (_ave_error<ave_error) itis=false;

			ave_error=_ave_error;
			ave_var=_ave_var;
			changed = _changed;
			//return false;
			return true;
			if  (itis && (n_steps!=1)){printf("RELAXED\n"); return true;}

			else return false;

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

	void MergeStep(){
			 TriRegIterator tri;
                         typename RegionType::AdjIterator ai;

				 for(tri = regions.begin(); tri != regions.end(); ++tri)if(!(*tri).isd)
						for(ai = (*tri).adj.begin(); ai != (*tri).adj.end(); ++ai) if(!(*ai)->isd)
							if((*tri).Mergeable(*(*ai)))
                                                                Merge((*tri),*(*ai));


                                 for(tri = regions.begin(); tri != regions.end(); ++tri){
					 for(ai = (*tri).nx_adj.begin(); ai != (*tri).nx_adj.end();++ai)
						 (*tri).adj.push_back(*ai);
                                         (*tri).adj.sort();
                                         (*tri).adj.unique();
                                     }



	}

	void Restart(){
				std::vector<FaceType*>  candi;
                                typename std::vector<RegionType*>::iterator si;
				TriRegIterator ri;
                                typename RegionType::AdjIterator ai;
				facesheap.clear();

				// clean all the regions
                                for(ri = regions.begin(); ri != regions.end(); ++ri){
					(*ri).Clean();
                                        workingset.push_back(&*ri);
                                    }
 
                         if(false && IsRelaxed()){// it's time to place a new seed
				workingset.clear();
				//RegionType * wr = (RegionType*)worst->r;// regions to which the new seed belongs
				FaceError<FaceType>  wrs;
				
				//do{
				//	pop_heap(faceserr.begin(),faceserr.end());
				//	wrs = faceserr.back();
				//	qDebug("ERROR: %f\n",wrs.val);
				//	faceserr.pop_back();
				//}while(((RegionType*)(wrs.f->r))->worst.val != wrs.val);
				


//				RegionType * wr = (RegionType*)wrs.f->r;
                                RegionType * wr = (RegionType*) attr_r[worst];// regions to which the new seed belongs
				wrs.f = worst;
				wrs.val = worst_err;
				printf("worst triangle error %f\n",worst_err);
				workingset.push_back(wr);
				for(ai = wr->adj.begin(); ai != wr->adj.end(); ++ai)// put all its neichbors onthe working set
				 workingset.push_back(*ai);
				CreateRegion(wrs.f);// CreateRegion changes wr->r
                                workingset.push_back((RegionType*)attr_r[wrs.f]);// put the region itself
				// reset state variables
				 ave_error=-1;
			   ave_var=-1;
				 err=0.0; 
				 changed=0;
			 }

			 for(si = workingset.begin(); si != workingset.end(); ++si)
			 {
				 candi.clear();		
                                 (*si)->Refit();            // fit a plane to the region
                                 Restart(**si);              // clear stuff in the region, move the seed to the best fitting to the plabne
                                 Candidates(**si,candi); // take the (three) faces candidatees
				 PushHeap(candi,*(*si));   // put the faces on to the heap
			 }

	}
			void CutOff(){}



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

                        void ComputeShots(){
                            int vpsize[2] = {800,800};
                            for(TriRegIterator tri = regions.begin(); tri != regions.end(); ++tri)
                                (*tri).ComputeShot(vpsize,10000.f / m->bbox.Diag());
                        }
};

#endif
