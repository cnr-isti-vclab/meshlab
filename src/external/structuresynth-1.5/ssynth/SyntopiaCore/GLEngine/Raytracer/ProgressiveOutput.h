#pragma once

#include <QMutex>
#include <QImage>
#include "SyntopiaCore/Math/Vector3.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		using namespace SyntopiaCore::Math;
	
		class ProgressiveOutput {
		public:
			ProgressiveOutput(int w, int h) : w(w), h(h) {
				mutex = new QMutex();
				colors = new Vector3f[w*h];
				weights = new double[w*h];
				for (int x = 0; x < w; x++) {	
					for (int y = 0; y < h; y++) {
						colors[x+y*w] = Vector3f(0,0,0);
						weights[x+y*w] = 0.0;
					}
				}
			}

			~ProgressiveOutput() {
				delete mutex;
				delete[] colors;
				delete[] weights;
			}

			void addIteration(Vector3f* newColors, double* newWeights) {
				mutex->lock();
				for (int x = 0; x < w; x++) {	
					for (int y = 0; y < h; y++) {	
						colors[x+y*w] = colors[x+y*w] + newColors[x+y*w];
						weights[x+y*w] = weights[x+y*w] + newWeights[x+y*w];
					}
				}
				mutex->unlock();
			};

			void addColumn(int x, Vector3f* newColors) {
				mutex->lock();
				for (int y = 0; y < h; y++) {	
						colors[x+y*w] = colors[x+y*w] + newColors[y];
						weights[x+y*w] = 1.0;
				}
				mutex->unlock();
			};

			// This function is responsible for tonemapping and gamma-conversion.
			// I prefer not converting to sRGB encode 
			// and a simply clipping functions works better than an 'exposure'-simulation
			int encode(float c, float /*exposure*/)
			{
				
				// Enable below for sRGB encoding
				/*
				float out = 1.0f - expf(c * exposure);
				if (out <= 0.0031308f)
				{
					out =  12.92f * out; 
				}
				else
				{
					out = 1.055f * powf(out, 0.4166667f) - 0.055f; // Inverse gamma 2.4
				}
				float out = 1.0f - expf(c * exposure);
				out = powf(c, 1);
				*/
				if (c>1) c = 1; // Simple clipping...
				c *= 255;
				return (int)c;
			}


			QImage getImage() {
				QImage im(w,h, QImage::Format_RGB32);

				//mutex->lock();
				float exposure = -2;
				for (int y = 0; y < h; y++) {	
					QRgb* s = (QRgb*) im.scanLine(y);
					for (int x = 0; x < w; x++) {	
						Vector3f c = colors[x+y*w]/weights[x+y*w];
						*s = qRgb(encode(c.x(),exposure), encode(c.y(),exposure), encode(c.z(),exposure));
						s++;
						//im.setPixel(x,y,qRgb(c.x()*255, c.y()*255, c.z()*255));
					}
				}
				//mutex->unlock();
				return im;
			}

		private:
			int w;
			int h;
			Vector3f* colors;
			double* weights;
			QMutex* mutex;
		};

	}
}

