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
#include<vcg/simplex/face/topology.h>

class TriEdgeCollapseMCParameter : public BaseParameterClass
{
public:
  Box3f bb;
  bool preserveBBox;
  float areaThr;
  void SetDefaultParams()
  {
    bb.SetNull();
    preserveBBox=true;
    areaThr=0;
  }

  TriEdgeCollapseMCParameter() {SetDefaultParams();}
};



template<class MCTriMesh, class VertexPair, class MYTYPE >
// Specialized Simplification classes for removing all small pieces on meshes. 
class MCTriEdgeCollapse: public tri::TriEdgeCollapse< MCTriMesh, VertexPair, MYTYPE> {

	public:
    typedef  typename MCTriMesh::VertexPointer VertexPointer;
    typedef  typename MCTriMesh::FaceType FaceType;
    typedef  typename MCTriMesh::VertexType::CoordType CoordType;
    typedef typename MCTriMesh::VertexType::ScalarType ScalarType;

  inline MCTriEdgeCollapse(  const VertexPair &p, int mark, BaseParameterClass *pp)
	{
                this->localMark = mark;
                this->pos=p;
                this->_priority = ComputePriority(pp);
  }
	
	
	// The priority is simply the edge lenght, 
	// but we consider collapsing edges that lies on one plane of the MC grid.
  virtual inline ScalarType ComputePriority(BaseParameterClass *_pp)
  {
    TriEdgeCollapseMCParameter *pp=(TriEdgeCollapseMCParameter *)_pp;
    const CoordType & p0 = this->pos.V(0)->cP();
    const CoordType & p1 = this->pos.V(1)->cP();

    int diffCnt=0;
    if( (p0[0] != p1[0]) ) diffCnt++;
    if( (p0[1] != p1[1]) ) diffCnt++;
    if( (p0[2] != p1[2]) ) diffCnt++;

		// non MC plane collapse return highest cost
 //       if(diffCnt>2) return   this->_priority=std::numeric_limits<float>::max() ;
    if(pp->preserveBBox)
        {
          const Box3f &bb=pp->bb;
		// collapse on the bbox border. Avoid it. 
        if(p0[0]==bb.min[0] || p0[0]==bb.max[0]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p0[1]==bb.min[1] || p0[1]==bb.max[1]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p0[2]==bb.min[2] || p0[2]==bb.max[2]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[0]==bb.min[0] || p1[0]==bb.max[0]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[1]==bb.min[1] || p1[1]==bb.max[1]) return this->_priority=std::numeric_limits<float>::max() ;
        if(p1[2]==bb.min[2] || p1[2]==bb.max[2]) return this->_priority=std::numeric_limits<float>::max() ;
      }
    return this->_priority=Distance(p0,p1);
	}
 
  inline void Execute(MCTriMesh &m,BaseParameterClass *)
  {	
        const CoordType & p0 = this->pos.V(0)->cP();
        const CoordType & p1 = this->pos.V(1)->cP();
        std::vector<VertexPointer> starVec0;
        std::vector<VertexPointer> starVec1;
//        int count0=0,count1=0;

        vcg::face::VVStarVF<FaceType>(this->pos.V(0),starVec0);
//        for(size_t i=0;i<starVec.size();++i)
//        {
//          if(	(p0[0]==starVec[i]->cP()[0]) )  count0++;
//          if(	(p0[1]==starVec[i]->cP()[1]) )  count0++;
//          if(	(p0[2]==starVec[i]->cP()[2]) )  count0++;
//        }
        vcg::face::VVStarVF<FaceType>(this->pos.V(1),starVec1);
//        for(size_t i=0;i<starVec.size();++i)
//         {
//           if( (p1[0]==starVec[i]->cP()[0]) )  count1++;
//           if( (p1[1]==starVec[i]->cP()[1]) )  count1++;
//           if( (p1[2]==starVec[i]->cP()[2]) )  count1++;
//         }
        CoordType MidPoint=(p0+p1)/2.0;

        if(starVec0.size()>starVec1.size()) MidPoint=p0;
        if(starVec0.size()<starVec1.size()) MidPoint=p1;

//        assert(	(p0[0]==p1[0]) ||
//                        (p0[1]==p1[1]) ||
//                        (p0[2]==p1[2]) );
//      DoCollapse(m, this->pos, MidPoint);
      vcg::tri::EdgeCollapser<MCTriMesh,VertexPair>::Do(m, this->pos, MidPoint);
	}
						
						
};

#endif
