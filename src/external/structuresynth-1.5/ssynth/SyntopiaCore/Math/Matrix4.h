#pragma once

#include <QString>
#include <QVector>

#include "Vector3.h"

namespace SyntopiaCore {
	namespace Math {	

		/// A simple class for representing 4x4 Matrices
		/// The internal representation has the rows increasing fastest: index = row + col*4;
		template <class scalar> class Matrix4 {
		public:
			/// Constructor (inits to zero value).
			Matrix4() { for (unsigned int i = 0; i < 16; i++) v[i] = 0; };

			/// Construct from a string (with 3x3 entries) - such as "[1 0 0 0 1 0 0 0 1]"
			Matrix4(QString input, bool& succes2) {
				for (unsigned int i = 0; i < 16; i++) v[i] = 0;
				v[0] = 1; v[5] = 1; v[10] = 1; v[15] = 1; 

				input.remove('[');
				input.remove(']');

				QStringList sl = input.split(" ");
				if (sl.size() != 9) { succes2 = false; return; }

				bool succes = false;
				float f = 0;
				f = sl[0].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[0] = f;
				f = sl[1].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[4] = f;
				f = sl[2].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[8] = f;
				f = sl[3].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[1] = f;
				f = sl[4].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[5] = f;
				f = sl[5].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[9] = f;
				f = sl[6].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[2] = f;
				f = sl[7].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[6] = f;
				f = sl[8].toFloat(&succes); if (!succes) { succes2 = false; return; }; v[10] = f;

				succes2 = true;
			}


			/// Identity matrix
			static Matrix4<scalar> Identity() { 
				Matrix4<scalar> m;
				m.v[0] = 1; m.v[5] = 1; m.v[10] = 1; m.v[15] = 1; 
				return m;
			};

			static Matrix4<scalar> ScaleMatrix(scalar s) { 
				Matrix4<scalar> m;
				m.v[0] = s; m.v[5] = s; m.v[10] = s; m.v[15] = 1; 
				return m;
			};

			/// at(row, col) return a copy of the value.
			scalar at(int row, int col) const { return v[row+col*4]; }
			scalar at(int index) const { return v[index]; }
			scalar operator() (int row, int col) const { return v[row+col*4]; }
			scalar operator() (int index) const { return v[index]; }

			/// getRef(row, col) returns a reference (for writing into matrix)
			scalar& getRef(int row, int col) { return v[row+col*4];  }
			scalar& getRef(int index) { return v[index];  }
			scalar& operator() (int row, int col) { return v[row+col*4]; }
			scalar& operator() (int index) { return v[index]; }

			/// Internal representation (can be used in OpenGL functions)
			scalar* getArray(void) { return v; }

			static Matrix4<scalar> Translation(scalar x,scalar y,scalar z) {
				Matrix4<scalar> m;
				m(0,3) = x;
				m(1,3) = y;
				m(2,3) = z;
				m(0,0) = 1;
				m(1,1) = 1;
				m(2,2) = 1;
				m(3,3) = 1;
				return  m;
			};

			static Matrix4<scalar> PlaneReflection(Vector3<scalar> n) {
				n.normalize();
				Matrix4<scalar> m;
				m(0,0) = 1.0 - 2.0*n.x()*n.x(); m(1,0) = -2.0*n.y()*n.x();       m(2,0) = -2.0*n.z()*n.x();
				m(0,1) = -2.0*n.x()*n.y();      m(1,1) = 1.0 - 2.0*n.y()*n.y();  m(2,1) = -2.0*n.z()*n.y();
				m(0,2) = -2.0*n.x()*n.z();      m(1,2) = -2.0*n.y()*n.z();       m(2,2) = 1.0 - 2.0*n.z()*n.z();
				m(3,3) = 1;
				return m;
			};

			/// Rotation about axis with angle
			/// Taken from http://www.gamedev.net/reference/articles/605/math3d.h
			static Matrix4<scalar> Rotation(Vector3<scalar> axis, scalar angle) {

				Matrix4<scalar> m;

				scalar c = cos(angle);
				scalar s = sin(angle);
				// One minus c (short name for legibility of formulai)
				scalar omc = (1 - c);

				if (axis.length() != 1) axis.normalize();

				scalar x = axis[0];
				scalar y = axis[1];
				scalar z = axis[2];	
				scalar xs = x * s;
				scalar ys = y * s;
				scalar zs = z * s;
				scalar xyomc = x * y * omc;
				scalar xzomc = x * z * omc;
				scalar yzomc = y * z * omc;

				m.v[0] = x*x*omc + c;
				m.v[1] = xyomc + zs;
				m.v[2] = xzomc - ys;
				m.v[3] = 0;

				m.v[4] = xyomc - zs;
				m.v[5] = y*y*omc + c;
				m.v[6] = yzomc + xs;
				m.v[7] = 0;

				m.v[8] = xzomc + ys;
				m.v[9] = yzomc - xs;
				m.v[10] = z*z*omc + c;
				m.v[11] = 0;

				m.v[12] = 0;
				m.v[13] = 0;
				m.v[14] = 0;
				m.v[15] = 1;

				return m;	
			};

			Matrix4<scalar> operator* (const Matrix4<scalar>& rhs) const {
				Matrix4<scalar> m;
				for (int x=0; x<4; x++) {
					for (int y=0;y <4; y++) {
						for (int i=0; i<4; i++) m.getRef(x,y) += (this->at(x,i) * rhs.at(i,y));
					}
				}
				return m;
			};

			Vector3<scalar> operator* (const Vector3<scalar>& rhs) const {
				Vector3<scalar> v; // Is initialized to zeros.
				for (int i=0; i<3; i++) {
					for (int j = 0; j < 3; j++) {
						v[i] += this->at(i,j)*rhs[j];
					}
					v[i] += this->at(i,3);
				}

				return v;
			};

			QString toString() {
				QString s = QString(" Row1 = [%1 %2 %3 %4], Row2 = [%5 %6 %7 %8]").
					arg(at(0,0)).arg(at(0,1)).arg(at(0,2)).arg(at(0,3)).
					arg(at(1,0)).arg(at(1,1)).arg(at(1,2)).arg(at(1,3));
				QString s2 = QString(" Row3 = [%1 %2 %3 %4], Row4 = [%5 %6 %7 %8]").
					arg(at(2,0)).arg(at(2,1)).arg(at(2,2)).arg(at(2,3)).
					arg(at(3,0)).arg(at(3,1)).arg(at(3,2)).arg(at(3,3));
				return s+s2;
			}

			// Only return the 3x3 part of the matrix.
			QString toStringAs3x3() {
				QString s = QString("[%1 %2 %3 %4 %5 %6 %7 %8 %9]").
					arg(at(0,0)).arg(at(0,1)).arg(at(0,2)).
					arg(at(1,0)).arg(at(1,1)).arg(at(1,2)).
					arg(at(2,0)).arg(at(2,1)).arg(at(2,2))
					;
				return s;
			}

		private:

			scalar v[16];
		};

		typedef Matrix4<float> Matrix4f ;
		typedef Matrix4<double> Matrix4d ;
	}
}

