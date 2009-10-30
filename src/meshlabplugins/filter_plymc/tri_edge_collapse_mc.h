/*
 *  tri_edge_collapse_mc.h
 *  filter_plymc
 *
 *  Created by Paolo Cignoni on 10/23/09.
 *  Copyright 2009 ISTI - CNR. All rights reserved.
 *
 */
#ifndef __TRI_EDGE_COLLAPSE_MC__
#define __TRI_EDGE_COLLAPSE_MC__

template<class MCTriMesh, class MYTYPE >
// Specialized Simplification classes for removing all small pieces on meshes. 
class MCTriEdgeCollapse: public tri::TriEdgeCollapse< MCTriMesh, MYTYPE> {
	public:
    typedef  typename MCTriMesh::VertexType::EdgeType EdgeType;
    typedef  typename MCTriMesh::VertexType::CoordType CoordType;
    typedef typename MCTriMesh::VertexType::ScalarType ScalarType;
	static Box3f &bb() {
		static Box3f bb; 
		return bb;
    }
	
	inline MCTriEdgeCollapse(  const EdgeType &p, int mark) 
	{
                this->localMark = mark;
                this->pos=p;
                this->_priority = ComputePriority();
		}
	
	
	// The priority is simply the edge lenght, 
	// but we consider collapsing edges that lies on one plane of the MC grid.
	virtual inline ScalarType ComputePriority()
  {
        const CoordType & p0 = this->pos.V(0)->cP();
        const CoordType & p1 = this->pos.V(1)->cP();

        int diffCnt=0;
        if( (p0[0] != p1[0]) ) diffCnt++;
        if( (p0[1] != p1[1]) ) diffCnt++;
        if( (p0[2] != p1[2]) ) diffCnt++;
			
		// non MC plane collapse return highest cost
        if(diffCnt>2) return   this->_priority=std::numeric_limits<float>::max() ;
		
		// collapse on the bbox border. Avoid it. 
        if(p0[0]==bb().min[0] || p0[0]==bb().max[0]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p0[1]==bb().min[1] || p0[1]==bb().max[1]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p0[2]==bb().min[2] || p0[2]==bb().max[2]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[0]==bb().min[0] || p1[0]==bb().max[0]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[1]==bb().min[1] || p1[1]==bb().max[1]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[2]==bb().min[2] || p1[2]==bb().max[2]) return this->_priority=std::numeric_limits<float>::max() ;

        else return this->_priority=Distance(p0,p1);
	}
 
	static float & areaThr(){
			static float t; 
			return t;
		}

  inline void Execute(MCTriMesh &m)
  {	
        const CoordType & p0 = this->pos.V(0)->cP();
        const CoordType & p1 = this->pos.V(1)->cP();
     CoordType MidPoint=(p0+p1)/2.0;
        assert(	(p0[0]==p1[0]) ||
                        (p0[1]==p1[1]) ||
                        (p0[2]==p1[2]) );
      DoCollapse(m, this->pos, MidPoint);
	}
						
						
};

#endif
