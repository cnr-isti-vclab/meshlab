#pragma once

#include "Rule.h"
#include "PrimitiveClass.h"
#include "../../SyntopiaCore/GLEngine/Object3D.h"

namespace StructureSynth {
	namespace Model {	

		using namespace SyntopiaCore::GLEngine;

		/// These are the built-in primitives,
		/// for drawing boxes, spheres and other simple geometric shapes.
		class PrimitiveRule : public Rule {
			public:
				enum PrimitiveType { Box, Sphere, Dot, Grid, Cylinder, Line, Mesh, Template, Other } ;
				
				PrimitiveRule(PrimitiveType type, PrimitiveClass* primitiveClass);
				virtual void apply(Builder* builder) const;
	
				/// Returns a list over rules that this rule references.
				/// (Empty for all PrimitiveRules!)
				virtual QList<RuleRef*> getRuleRefs() const { return QList<RuleRef*>(); }

				/// 'class' is an identifier used for distinguishing between
				/// different forms of the the same PrimiteType.
				/// This is used together with Template Renderers.
				///
				/// For instance 'box::metal' will be parsed in to a 'box' primitive with a 'metal' class identifier.
				void setClass(PrimitiveClass* primitiveClass) { this->primitiveClass = primitiveClass; }
				PrimitiveClass* getClass() { return primitiveClass; }

				
		    protected:
			    PrimitiveClass* primitiveClass;
			private:
				PrimitiveType type;
				
				
		};

		/// Triangle rules are special, since they have explicit coordinate representation.
		class TriangleRule : public PrimitiveRule {
			public:
				
				TriangleRule(SyntopiaCore::Math::Vector3f p1,
					          SyntopiaCore::Math::Vector3f p2,
							   SyntopiaCore::Math::Vector3f p3,
							     PrimitiveClass* primitiveClass);

				virtual void apply(Builder* builder) const;
	
			private:
				SyntopiaCore::Math::Vector3f p1;
				SyntopiaCore::Math::Vector3f p2;
				SyntopiaCore::Math::Vector3f p3;
				
		};

	}
}

