#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		class Dot : public Object3D {
		public:
			Dot(SyntopiaCore::Math::Vector3f pos);
			
			virtual QString name() { return "Dot"; }
			
			virtual ~Dot();

			virtual void draw() const;

		private:
			SyntopiaCore::Math::Vector3f pos;
		};

	}
}

