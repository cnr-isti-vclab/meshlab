#include "Transformation.h"
#include "ColorPool.h"

#include "../../SyntopiaCore/Math/Matrix4.h"

#include "../../SyntopiaCore/Exceptions/Exception.h"
#include "../../SyntopiaCore/Logging/Logging.h"

#include <QColor>

using namespace SyntopiaCore::Exceptions;
using namespace SyntopiaCore::Logging;

using namespace SyntopiaCore::Math;

namespace StructureSynth {
	namespace Model {	

		Transformation::Transformation() {
			matrix = Matrix4f::Identity();
			deltaH = 0;
			scaleS = 1;
			scaleV = 1;
			scaleAlpha = 1;
			absoluteColor = false;
			strength = 0;
		}

		Transformation::~Transformation() {
		};

		State Transformation::apply(const State& s, ColorPool* colorPool) const {
			State s2(s);
			s2.matrix = s.matrix*matrix; 

			if (absoluteColor) {
				// if the absolute hue is larger than 360, we will choose a random color.
				if (deltaH > 360) {

					QColor c = colorPool->drawColor();
					s2.hsv = Vector3f(c.hue(), c.saturation()/255.0, c.value()/255.0);
					s2.alpha = 1.0;
				} else {
					s2.hsv = Vector3f(deltaH,scaleS,scaleV);
					s2.alpha = scaleAlpha;
				}
			} else {
				float h = s2.hsv[0] + deltaH;
				float sat = s2.hsv[1]*scaleS;
				float v = s2.hsv[2]*scaleV;
				float a = s2.alpha * scaleAlpha;
				if (sat<0) sat=0;
				if (v<0) v=0;
				if (a<0) a=0;
				if (sat>1) sat=1;
				if (v>1) v=1;
				if (a>1) a=1;
				while (h>360) h-=360;
				while (h<0) h+=360;
				s2.hsv = Vector3f(h,sat,v);
				s2.alpha = a;

			}

			if (strength) {
				/*
				// We will blend the two colors (in RGB space)
				QColor original = QColor::fromHsv((int)(s2.hsv[0]),(int)(s2.hsv[1]*255.0),(int)(s2.hsv[2]*255.0));
				double r = original.red() + strength*blendColor.red();
				double g = original.green() + strength*blendColor.green();
				double b = original.blue() + strength*blendColor.blue();
				if (r<0) r=0;
				if (g<0) g=0;
				if (b<0) b=0;
				double max = r;
				if (g>max) max = g;
				if (b>max) max = b;
				if (max > 255) {
					r = r * 255 / max;
					g = g * 255 / max;
					b = b * 255 / max;
				}

				QColor mixed(r,g,b);
				
				
				s2.hsv = Vector3f(mixed.hue(), mixed.saturation()/255.0,mixed.value()/255.0);
				*/

				// We will blend the two colors (in HSV space)
				Vector3f bl = Vector3f(blendColor.hue(), blendColor.saturation()/255.0,blendColor.value()/255.0);
				Vector3f b(s2.hsv[0]+strength*bl[0], s2.hsv[1]+strength*bl[1], s2.hsv[2]+strength*bl[2]);
				b = b/(1+strength);
				while (b[0] < 0) b[0]+= 360;
				while (b[0] > 360) b[0]-= 360;
				if (b[1]>1) b[1]=1;
				if (b[2]>1) b[2]=1;
				if (b[1]<0) b[1]=0;
				if (b[2]<0) b[2]=0;
				s2.hsv = b;
				
				
				
			}

			return s2;
		}


			
		void Transformation::append(const Transformation& t) {
			this->matrix = this->matrix * t.matrix; 
			if (!(t.absoluteColor && absoluteColor)) {
				if (t.absoluteColor) this->absoluteColor = true;
				if (absoluteColor) this->absoluteColor = true;
				
				this->scaleAlpha = this->scaleAlpha * t.scaleAlpha;
				this->deltaH = this->deltaH + t.deltaH;
				this->scaleS = this->scaleS * t.scaleS;
				this->scaleV = this->scaleV * t.scaleV;
			} else  {
				// Mix two absolute colors - this is not possible, so we will just choose one of them
				this->absoluteColor = true;
				this->scaleAlpha = t.scaleAlpha;
				this->deltaH = t.deltaH;
				this->scaleS = t.scaleS;
				this->scaleV = t.scaleV;
			}

			if (t.strength != 0) {
				this->strength = t.strength;
				this->blendColor = t.blendColor;
			}
		}

		// The predefined operators
		// Translations
		Transformation Transformation::createX(double offset) {
			Transformation t;
			t.matrix(0,3) = offset;
			return t;
		}
		
		Transformation Transformation::createY(double offset) {
			Transformation t;
			t.matrix(1,3) = offset;
			return t;
		}

		Transformation Transformation::createZ(double offset) {
			Transformation t;
			t.matrix(2,3) = offset;
			return t;
		}

		Transformation Transformation::createPlaneReflection(SyntopiaCore::Math::Vector3f normal) {
			Transformation t;
			t.matrix = Matrix4f::PlaneReflection(normal);
			return t;
		}
			
			
		// Rotations
		Transformation Transformation::createRX(double angle) {
			Transformation t;
			t.matrix = 
				Matrix4f::Translation(0,0.5,0.5)*
				Matrix4f::Rotation(Vector3f(1,0,0), angle)*
				Matrix4f::Translation(0,-0.5,-0.5);
			return t;
		}

		Transformation Transformation::createRY(double angle) {
			Transformation t;
			t.matrix = 
				Matrix4f::Translation(0.5,0,0.5)*
				Matrix4f::Rotation(Vector3f(0,1,0), angle)*
				Matrix4f::Translation(-0.5,0,-0.5)
				
				; 
			return t;
		}

		Transformation Transformation::createRZ(double angle) {
			Transformation t;
			t.matrix = 
				Matrix4f::Translation(0.5,0.5,0)*
				Matrix4f::Rotation(Vector3f(0,0,1), angle)*
				Matrix4f::Translation(-0.5,-0.5,0);
			return t;
		}

		Transformation Transformation::createHSV(float h, float s, float v, float a) {
			Transformation t;
			t.deltaH = h;
			t.scaleAlpha = a;
			t.scaleS = s;
			t.scaleV = v;

			

			return t;
		}

		Transformation Transformation::createColor(QString color) {

			Transformation t;

			if (color.toLower() != "random") {
				QColor c(color);
				QColor hsv = c.toHsv();
				t.deltaH = hsv.hue();
				t.scaleAlpha = hsv.alpha()/255.0;
				t.scaleS = hsv.saturation()/255.0;
				t.scaleV = hsv.value()/255.0;
				t.absoluteColor = true;
			} else {
				t.deltaH = 1000;
				t.absoluteColor = true;
			}

			//Debug(QString("Abs Color: %1, %2, %3, %4").arg(t.deltaH).arg(t.scaleS).arg(t.scaleV).arg(t.scaleAlpha));

			return t;
		}

		Transformation Transformation::createBlend(QString color, double strength) {
			Transformation t;
			t.blendColor = QColor(color);
			t.strength = strength;
			return t;
		}

		

		Transformation Transformation::createScale(double x, double y, double z) {
			Transformation t;
			t.matrix(0,0) = x;
			t.matrix(1,1) = y;
			t.matrix(2,2) = z;
			t.matrix = 
				Matrix4f::Translation(0.5,0.5,0.5)*
				t.matrix*
				Matrix4f::Translation(-0.5,-0.5,-0.5);
			return t;
		}

		Transformation Transformation::createMatrix(QVector<double> vals) {
			Transformation t;
			t.matrix(0,0) = vals[0];
			t.matrix(0,1) = vals[1];
			t.matrix(0,2) = vals[2];
			t.matrix(1,0) = vals[3];
			t.matrix(1,1) = vals[4];
			t.matrix(1,2) = vals[5];
			t.matrix(2,0) = vals[6];
			t.matrix(2,1) = vals[7];
			t.matrix(2,2) = vals[8];
			t.matrix = 
				Matrix4f::Translation(0.5,0.5,0.5)*
				t.matrix*
				Matrix4f::Translation(-0.5,-0.5,-0.5);
			return t;
			
		}
		
	}
}

