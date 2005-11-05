#include <vcg\simplex\vertexplus\component.h>

namespace vcg {
	namespace vert {


		template <class T> class EmptyCurvature: public T {
		public:
			typedef float CurvatureType;
			CurvatureType &Curv() { assert(0); return CurvatureType(0.0f); }
			static bool HasCurvature()   { return false; }
		};

		template <class A, class T> class Curvature: public T {
		public:
			typedef A CurvatureType;
			CurvatureType &Curv() { return _curvature; }
			static bool HasCurvature()   { return true; }
		private:
			CurvatureType _curvature;    
		};

		template <class T> class Curvaturef: public Curvature<float, T> {};

	} //end namespace vert
} //end namespace vcg
