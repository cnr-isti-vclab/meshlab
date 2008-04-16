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
			this->selection = false;
		}
		
		AgingEdgePred(int type, bool sel, float lenTh, float thVal = 0.0) {
			lenp = EdgeLen <CMeshO, CMeshO::ScalarType>();
			lenp.thr2 = lenTh * lenTh;
			this->thVal = thVal;
			this->type = type;
			this->selection = sel;
		}
		
		// Main method to test predicates over edges (used by RefineE function)
		bool operator()(face::Pos<CMeshO::FaceType> ep) const {
			if(selection && !ep.f->IsV() && !ep.f->FFp(ep.z)->IsV()) return false;
			return (lenp(ep) && qaEdgeTest(ep));
		}
		
		// Tests angle predicate over the edge if type is angle; if type is quality 
		// tests quality predicate but only on the first of the two vertexes of the edge
		bool qaVertTest(face::Pos<CMeshO::FaceType> ep) const {
			return (type==ANGLE?testAngle(ep):(type==QUALITY?(ep.f->V(ep.z)->Q()>thVal):false));
		}
		
		// Returns the type of the predicate
		int getType() { return type; }
		
		
	protected:
		EdgeLen<CMeshO, CMeshO::ScalarType> lenp;		// edge len predicate object
		float thVal;					// edge value threshold (angle or quality)
		int type;						// predicate type (angle or quality)
		bool selection;				// working on the selection only
		 
		bool qaEdgeTest(face::Pos<CMeshO::FaceType> ep) const {
			return (type==ANGLE?testAngle(ep):(type==QUALITY?testQuality(ep):false));
		}
		
		bool testAngle(face::Pos<CMeshO::FaceType> ep) const {
			if(ep.f->IsB(ep.z)) return true;
			
			// the angle between the two face normals in degrees
			double ffangle = vcg::Angle(ep.f->N(), ep.f->FFp(ep.z)->N()) * 180 / M_PI;
			
			Point3<CVertexO::ScalarType> edge;		// vector along edge
			double seAngle;		// angle between the edge vector and the normal of one of the two faces
			if(ep.f->V(ep.z) < ep.f->V1(ep.z)) {
				edge = Point3<CVertexO::ScalarType>(ep.f->V1(ep.z)->P() - ep.f->V(ep.z)->P());
				seAngle = vcg::Angle(edge ^ ep.f->N(), ep.f->FFp(ep.z)->N()) * 180 / M_PI;
			}
			else {
				edge = Point3<CVertexO::ScalarType>(ep.f->V(ep.z)->P() - ep.f->V1(ep.z)->P());
				seAngle = vcg::Angle(edge ^ ep.f->FFp(ep.z)->N(), ep.f->N()) * 180 / M_PI;
			}
			
			/* There are always 2 cases wich produce the same angle value:
						 ___|_		    ____
						|			   |  |
					   -|			   |-
				
			   In the first case the edge lies in a concave area of the mesh
			   while in the second case it lies in a convex area.
			   We need a way to know wich is the current case.
			   This is done comparing seAngle with thVal.
			*/
			return (ffangle-thVal >= -0.001  && seAngle-thVal <= 0.001);
		}
		
		bool testQuality(face::Pos<CMeshO::FaceType> ep) const {
			return (ep.f->V(ep.z)->Q() > thVal || ep.f->V1(ep.z)->Q() > thVal);
		}
};


#endif /*EDGEPRED_H_*/
