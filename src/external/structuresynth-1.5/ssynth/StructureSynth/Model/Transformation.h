#pragma once

#include <QString>
#include <QColor>
#include "State.h"
#include "ColorPool.h"
#include "../../SyntopiaCore/Math/Matrix4.h"

namespace StructureSynth {
	namespace Model {	

		class Transformation  {
		public:
			Transformation();
			~Transformation();

			/// 'Applies' the transformation 'T' to this transformation.
			/// (For the matrix this corresponds to matrix multiplication).
			void append(const Transformation& T);
			State apply(const State& s, ColorPool* colorPool) const;

			// The predefined operators
			// Translations
			static Transformation createX(double offset);
			static Transformation createY(double offset);
			static Transformation createZ(double offset);
			
			// Rotations
			static Transformation createRX(double angle);
			static Transformation createRY(double angle);
			static Transformation createRZ(double angle);

			// Plane reflection
			static Transformation createPlaneReflection(SyntopiaCore::Math::Vector3f normal);
			
			// Scaling 
			static Transformation createScale(double x, double y, double z);
		
			// Free transformation 
			static Transformation createMatrix(QVector<double> vals);
		
			// Color stuff
			static Transformation createHSV(float h, float s, float v, float a);
			static Transformation createColor(QString color);
			static Transformation createBlend(QString color, double strength);
		
		
		private:
			// Matrix and Color transformations here.
			SyntopiaCore::Math::Matrix4f matrix;

			// For color alterations
			float deltaH;
			float scaleS;
			float scaleV;
			float scaleAlpha;
			bool absoluteColor;

			// For color blends.
			QColor blendColor;
			double strength;
		};

	}
}

