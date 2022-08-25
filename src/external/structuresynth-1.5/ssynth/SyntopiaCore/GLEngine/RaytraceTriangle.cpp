#include "RaytraceTriangle.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {
		using namespace SyntopiaCore::Math;


		RaytraceTriangle::RaytraceTriangle(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f n1, Vector3f n2, Vector3f n3) 
			:  p1(p1),p2(p2),p3(p3),n1(n1),n2(n2),n3(n3) , cullBackFaces(true)
		{

				// We will use barycentric coordiantes.
				Vector3f p12 = p2-p1;
				Vector3f p13 = p3-p1;
				Vector3f p312 = (p12/p12.sqrLength())*Vector3f::dot(p12,p13);
				Vector3f pn3 = p13-p312;
				npn3 = pn3/pn3.sqrLength();

				Vector3f p32 = p2-p3;
				Vector3f p31 = p1-p3;
				Vector3f p132 = (p32/p32.sqrLength())*Vector3f::dot(p32,p31);
				Vector3f pn1 = p31-p132;
				npn1 = pn1/pn1.sqrLength();
	
				Vector3f p213 = (p13/p13.sqrLength())*Vector3f::dot(p13,p12);
				Vector3f pn2 = p12-p213;
				npn2 = pn2/pn2.sqrLength();

				if (npn1.sqrLength() * 0 != 0 || npn2.sqrLength() * 0 != 0 || npn3.sqrLength() * 0 != 0 ||
					p13.sqrLength() < 1E-8 || p12.sqrLength() < 1E-8 || p32.sqrLength() < 1E-8) {
					//INFO(QString("Bad Triangle (%0): %1, %2, %3").arg((p3-p2).sqrLength()).arg(p1.toString()).arg(p2.toString()).arg(p3.toString()));
					bad = true;
				} else {
					bad = false;
				}

				n1.normalize();
				n2.normalize();
				n3.normalize();

				n = Vector3f::cross(p13,p12);
			}



			
			void RaytraceTriangle::expandBoundingBox(Vector3f& from,Vector3f& to) {
				Vector3f p = p1;
				for (int i = 0; i < 3; i++) {
					if (i==1) p=p2;
					if (i==2) p=p3;
					if ( p.x() < from.x()) from.x() = p.x();
					if ( p.y() < from.y()) from.y() = p.y();
					if ( p.z() < from.z()) from.z() = p.z();
					if ( p.x() > to.x()) to.x() = p.x();
					if ( p.y() > to.y()) to.y() = p.y();
					if ( p.z() > to.z()) to.z() = p.z();
				}
			}



			//bool RayTraceTriangle::getIntersectionAndNormal(const Vector3f& startPoint, const Vector3f& lineDirection, double& intersection, Vector3f& normal, Vector3f& color, float& alpha ) {

			bool RaytraceTriangle::intersectsRay(RayInfo* ri) {
				if (bad) return false;

				if (cullBackFaces && Vector3f::dot(n,ri->lineDirection)>0) return false;
		
				float is  = Vector3f::dot(n, p1-ri->startPoint)/Vector3f::dot(n, ri->lineDirection);
				Vector3f ip = ri->startPoint +  ri->lineDirection * is;

				float k312 = Vector3f::dot((ip-p1),npn3);
				if (k312 > 1 || k312 <= 0) return false;
				float k132 = Vector3f::dot((ip-p3),npn1);
				if (k132 > 1 || k132 <= 0) return false;
				float k213 = Vector3f::dot((ip-p1),npn2);
				if (k213 > 1 || k213 <= 0) return false;

				ri->intersection = is;
				ri->normal = n3*k312+ n1*k132+n2*k213;
				if (!cullBackFaces && Vector3f::dot( ri->normal , ri->lineDirection)>0) ri->normal=-ri->normal;
				
				float c0 = k312*color3[0]+k132*color1[0]+k213*color2[0];
				float c1 = k312*color3[1]+k132*color1[1]+k213*color2[1];
				float c2 = k312*color3[2]+k132*color1[2]+k213*color2[2];
				float a = 1; //k312*color3[3]+k132*color1[3]+k213*color2[3];
				ri->color[0] = c0;
				ri->color[1] = c1;
				ri->color[2] = c2;
				ri->color[3] = a;
				return true;
			}


			

			RaytraceTriangle::~RaytraceTriangle(void) {  }

			void RaytraceTriangle::Vertex4(Vector3f p1,Vector3f p2,Vector3f p3,Vector3f p4,bool reverse, QVector<RaytraceTriangle>& list, float r, float g, float b, float a) {
			Vector3f n = -Vector3f::cross(p2-p1, p4-p1);
			if (reverse) n=-n;
			n.normalize();
			RaytraceTriangle r1(p1,p2,p4,n,n,n);
			r1.n = n;
			r1.setColor(r,g,b,a);
			list.append(r1);
			RaytraceTriangle r2(p3,p4,p2,n,n,n);
			r2.setColor(r,g,b,a);
			r2.n = n;
			list.append(r2);
		}
	}
}

