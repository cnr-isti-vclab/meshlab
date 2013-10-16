#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		class Grid : public Object3D {
		public:
			Grid(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1 , 
				 SyntopiaCore::Math::Vector3f dir2, 
				 SyntopiaCore::Math::Vector3f dir3);

			virtual ~Grid();

			virtual QString name() { return "Grid"; }
			
			virtual void draw() const;

		private:
			SyntopiaCore::Math::Vector3f base;
			SyntopiaCore::Math::Vector3f v1;
			SyntopiaCore::Math::Vector3f v2;
			SyntopiaCore::Math::Vector3f v3;
		};

	}
}

