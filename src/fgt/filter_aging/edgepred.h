#ifndef EDGEPRED_H_
#define EDGEPRED_H_


using namespace vcg;


class AgingEdgePred
{
	public:
		enum {ANGLE, QUALITY};		// predicate type allowed values
		
		AgingEdgePred() {
			lenp = EdgeLen <CMeshO, CMeshO::ScalarType>();
			lenp.thr2 = 1.0;
			this->thVal = 1.0;
			this->type = QUALITY;
		}
		
		AgingEdgePred(int type, float lenTh, float thVal = 0.0) {
			lenp = EdgeLen <CMeshO, CMeshO::ScalarType>();
			lenp.thr2 = lenTh * lenTh;
			this->thVal = thVal;
			this->type = type;
		}
	
		bool operator()(face::Pos<CMeshO::FaceType> ep) const {
			return (lenp(ep) && qaTest(ep));
		}
		
		bool qaTest(face::Pos<CMeshO::FaceType> ep) const {
			return (type==ANGLE?testAngle(ep):(type==QUALITY?testQuality(ep):false));
		}
		
		
	protected:
		EdgeLen<CMeshO, CMeshO::ScalarType> lenp;
		float thVal;
		int type;
		
		bool testAngle(face::Pos<CMeshO::FaceType> ep) const {
			if(ep.f->IsB(ep.z)) return true;
			
			// the angle between the two face normals in degrees
			double ffangle = vcg::Angle(ep.f->N(), ep.f->FFp(ep.z)->N()) * 180 / M_PI;
			
			// the 2 points not shared between the 2 faces
			CVertexO *f1p = ep.f->V2(ep.z);
			CVertexO *f2p = ep.f->FFp(ep.z)->V2(ep.f->FFi(ep.z));
			
			Point3<CVertexO::ScalarType> y, median;
			
			y = ep.f->N().Normalize() ^ ep.f->FFp(ep.z)->N().Normalize();
			median = y ^ (Point3<CVertexO::ScalarType>(f1p->P() - f2p->P()));
			
			/* There are always 2 cases wich produce the same angle value:
						 ___|_		    ____
						|			   |  |
					   -|			   |-
				
			   In the first case the edge lies in a concave area of the mesh
			   while in the second case it lies in a convex area.
			   We need a way to know wich is the current case.
			   This is done comparing ffangle with the angle between the 
			   normal to the current face and the median vector. 
			*/
			return (ffangle-thVal >= -0.001  && vcg::Angle(ep.f->N(), median) * 180 / M_PI < ffangle);
			//return false;
		}
		
		bool testQuality(face::Pos<CMeshO::FaceType> ep) const {
			return (ep.f->V(ep.z)->Q() > thVal && ep.f->V1(ep.z)->Q() > thVal);
		}
};


/*
class AgingFacePred
{
	public:
		AgingFacePred() {
			edgeTest = AgingEdgePred(AgingEdgePred::QUALITY, 1.0, 1.0);
		}

		AgingFacePred(int type, float lenTh, float thVal = 0.0) {
			edgeTest = AgingEdgePred(type, lenTh, thVal);
		}
		
		bool operator()(face::Pos<CMeshO::FaceType> ep) const {
			return (edgeTest(ep) && edgeTest(face::Pos<CMeshO::FaceType>(ep.f, (ep.z+1)%3)) &&
					 edgeTest(face::Pos<CMeshO::FaceType>(ep.f, (ep.z+2)%3)));
		}

		bool qaTest(face::Pos<CMeshO::FaceType> ep) const {
			return (edgeTest.qaTest(ep) && edgeTest.qaTest(face::Pos<CMeshO::FaceType>(ep.f, (ep.z+1)%3)) &&
					 edgeTest.qaTest(face::Pos<CMeshO::FaceType>(ep.f, (ep.z+2)%3)));
		}
		
				
	protected:
		AgingEdgePred edgeTest;
};
*/

#endif /*EDGEPRED_H_*/
