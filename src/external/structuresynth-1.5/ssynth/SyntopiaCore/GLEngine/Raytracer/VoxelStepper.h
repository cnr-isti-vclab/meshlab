#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "../Object3D.h"

namespace SyntopiaCore {
	namespace GLEngine {	
		using namespace SyntopiaCore::Math;
	
		/// See here for details about this approach:
		/// http://www.devmaster.net/articles/raytracing_series/part4.php
		class VoxelStepper {
		public:
			VoxelStepper(Vector3f minPos, Vector3f maxPos, int steps) ;
			~VoxelStepper();

			void registerObject(Object3D* obj) ;
			QList<Object3D*>* setupRay(Vector3f pos, Vector3f dir, double& maxT);

			inline double minn(double a, double b, double c) {
				if (a<b) return (a<c ? a : c);
				return (b<c ? b : c);
			}

			QList<Object3D*>* advance(double& maxT);
			void setCopy(bool value) { this->copy = value; }

		private:
			bool copy;
			double currentT;
			double tDeltaX;
			double tDeltaY;
			double tDeltaZ;
			double tMaxX;
			double tMaxY;
			double tMaxZ;
			int stepX;
			int stepY;
			int stepZ;
			int cx;
			int cy;
			int cz;

			Vector3f pos;
			Vector3f dir;
			int steps;
			Vector3f minPos;
			Vector3f maxPos;		
			const Vector3f size;
			QList<Object3D*>* grid;
		};


	}
}

