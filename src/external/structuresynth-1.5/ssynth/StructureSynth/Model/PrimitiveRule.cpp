#include "PrimitiveRule.h"
#include "Builder.h"

#include "../../SyntopiaCore/Exceptions/Exception.h"
#include "../../SyntopiaCore/Logging/Logging.h"
#include "../../SyntopiaCore/Misc/ColorUtils.h"

using namespace SyntopiaCore::Exceptions;
using namespace SyntopiaCore::Logging;

#include "../../SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace StructureSynth {
	namespace Model {	
		PrimitiveRule::PrimitiveRule(PrimitiveType type, PrimitiveClass* primitiveClass) : primitiveClass(primitiveClass), type(type) {

			// enum PrimitiveType { Box, Sphere, Point, Cylinder, Line } ;			
			if (type == Box) {
				name = "box";
			} else if (type == Sphere) {
				name = "sphere";
			} else if (type == Dot) {
				name = "dot";
			} else if (type == Grid) {
				name = "grid";
			} else if (type == Cylinder) {
				name = "cylinder";
			} else if (type == Line) {
				name = "line";
			} else if (type == Mesh) {
				name = "mesh";
			} else if (type == Template) {
				name = "template";
			} else if (type == Other) {
				name = "other";
			} else {
				WARNING("PrimitiveRule constructor: unknown PrimitiveType");
			}

		};

		void PrimitiveRule::apply(Builder* b) const {
			if (type == Template) {
				b->getRenderer()->callGeneric(primitiveClass);
				return;
			}

			b->increaseObjectCount();
			b->getRenderer()->setColor(
				SyntopiaCore::Misc::ColorUtils::HSVtoRGB( b->getState().hsv)
			);

			b->getRenderer()->setAlpha( b->getState().alpha );
			
			if (type == Sphere) {
				Vector3f v(0,0,0);
				Vector3f v1 = b->getState().matrix * v;
				Vector3f c =  b->getState().matrix * Vector3f(0.5,0.5,0.5);
				Vector3f c2 = (b->getState().matrix * Vector3f(0.5,0.5,0.0));
				double r =  (c-  c2).length();

				b->getRenderer()->drawSphere(c,r,primitiveClass);
			} else if (type == Mesh) {
				if (b->getState().previous) {
					
					Vector3f v1 = (b->getState().previous->matrix) * Vector3f(0,0,0);
					Vector3f v2 = (b->getState().previous->matrix) * Vector3f(1,0,0);
					Vector3f v3 = (b->getState().previous->matrix) * Vector3f(0,1,0);


					Vector3f u1 = b->getState().matrix * Vector3f(0,0,0);
					Vector3f u2 = b->getState().matrix * Vector3f(1,0,0);
					Vector3f u3 = b->getState().matrix * Vector3f(0,1,0);
					b->getRenderer()->setPreviousColor(
						SyntopiaCore::Misc::ColorUtils::HSVtoRGB( b->getState().previous->hsv));
					b->getRenderer()->setPreviousAlpha(b->getState().previous->alpha);

					b->getRenderer()->drawMesh(v1,v2-v1,v3-v1,u1,u2-u1,u3-u1,primitiveClass);
				} else {
					INFO("No prev");
				}
			} else if (type == Box) {
				Vector3f v(0,0,0);

				Vector3f v1 = b->getState().matrix * v;
				Vector3f v2 = b->getState().matrix * Vector3f(1,0,0);
				Vector3f v3 = b->getState().matrix * Vector3f(0,1,0);
				Vector3f v4 = b->getState().matrix * Vector3f(0,0,1);

				b->getRenderer()->drawBox(v1,v2-v1,v3-v1,v4-v1,primitiveClass);
				
			} else if (type == Grid) {
				Vector3f v(0,0,0);

				Vector3f v1 = b->getState().matrix * v;
				Vector3f v2 = b->getState().matrix * Vector3f(1,0,0);
				Vector3f v3 = b->getState().matrix * Vector3f(0,1,0);
				Vector3f v4 = b->getState().matrix * Vector3f(0,0,1);

				b->getRenderer()->drawGrid(v1,v2-v1,v3-v1,v4-v1,primitiveClass);
			} else if (type == Dot) {
				Vector3f v = b->getState().matrix * Vector3f(0.5,0.5,0.5);
				
				b->getRenderer()->drawDot(v,primitiveClass);
			} else if (type == Line) {
				Vector3f v = b->getState().matrix * Vector3f(0,0.5,0.5);
				Vector3f v2 = b->getState().matrix * Vector3f(1,0.5,0.5);
				
				b->getRenderer()->drawLine(v,v2,primitiveClass);
			}

		};
	
			
			
		TriangleRule::TriangleRule(SyntopiaCore::Math::Vector3f p1,
					          SyntopiaCore::Math::Vector3f p2,
							  SyntopiaCore::Math::Vector3f p3, PrimitiveClass* primitiveClass) 
							    : PrimitiveRule(Other, primitiveClass), p1(p1), p2(p2), p3(p3) {
			name = "Triangle";
		}

		void TriangleRule::apply(Builder* b) const {
			b->increaseObjectCount();
			b->getRenderer()->setColor(
				SyntopiaCore::Misc::ColorUtils::HSVtoRGB( b->getState().hsv)
			);

			b->getRenderer()->setAlpha( b->getState().alpha );
			
			
			Vector3f v1 = b->getState().matrix * p1;
			Vector3f v2 = b->getState().matrix * p2;
			Vector3f v3 = b->getState().matrix * p3;
			
			b->getRenderer()->drawTriangle(v1,v2,v3,primitiveClass);
		}
	
	}
}

