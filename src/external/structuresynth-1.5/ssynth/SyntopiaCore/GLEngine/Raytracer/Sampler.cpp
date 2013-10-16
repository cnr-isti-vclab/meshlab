#include "Sampler.h"

namespace SyntopiaCore {
	namespace GLEngine {

		    
		Sampler::Sampler(Math::RandomNumberGenerator* rg) : rg(rg) {
		}

		Sampler::~Sampler() {
		}
			
		namespace {
			
			// This code is based on the method in Physical Based Rendering p. 666, and the Sunflow implementation
			Vector3f concentricSampleDisk(float u1, float u2) {
				float r, angle;
				float sx = 2 * u1 - 1;
				float sy = 2 * u2 - 1;

				if (sx == 0.0 && sy == 0.0)	return Vector3f(0,0,0);
				
				if (sx >= -sy) {
					if (sx > sy) {
						r = sx;
						if (sy > 0.0) angle = sy/r; else angle = 8.0f + sy/r;
					}
					else {
						r = sy;
						angle = 2.0f - sx/r;
					}
				}
				else {
					if (sx <= sy) {
						r = -sx;
						angle = 4.0f - sy/r;
					}
					else {
						r = -sy;
						angle = 6.0f + sx/r;
					}
				}
				angle *= 3.1415 / 4.f;
				return Vector3f(r * cosf(angle),r * sinf(angle),0);
			}

		}
		
		Vector3f StratifiedSampler::getAODirection(int index) {
			if (index>=aoSamples.count()) throw 1;
			return aoSamples[index];
		}

		Vector3f StratifiedSampler::getAASample(int index) {
			if (index>=aaSamples.count()) throw 1;
			return aaSamples[index];
		}

		Vector3f StratifiedSampler::getLensSample(int index) {
			if (index>=lensSamples.count()) throw 1;
			return lensSamples[index];
		}

		// Based on the Physical Based Rendering book
		Vector3f StratifiedSampler::sampleSphere(double u1, double u2) {
			double z = 1.0 - 2.0*u1;
			double r = 0;
			if (1.0-z*z > 0) r = sqrt(1.0-z*z);
			double phi = 2.0 * 3.1415926 * u2;
			double x = r * cos(phi);
			double y = r * sin(phi);
			return Vector3f(x,y,z);
		}

		void StratifiedSampler::prepareSamples(int nAASamplesSqrt, int nAOSamplesSqrt) {
			int nSqrt = nAASamplesSqrt*nAOSamplesSqrt;
			aoSamples = QVector<Vector3f>(nSqrt*nSqrt);
			aaSamples = QVector<Vector3f>(nAASamplesSqrt*nAASamplesSqrt);
			lensSamples = QVector<Vector3f>(nAASamplesSqrt*nAASamplesSqrt);
			int count = 0;
			for (int i = 0; i < nSqrt; i++) {
				for (int j = 0; j < nSqrt; j++) {
					// we need a uniform number in the interval
					// [i/nSqrt;(i+1)/nSqrt]
					double x = rg->getDouble( ((double)i)/(double)nSqrt,((double)(i+1.0))/(double)nSqrt);
					double y = rg->getDouble( ((double)j)/(double)nSqrt,((double)(j+1.0))/(double)nSqrt);
					aoSamples[count++] = sampleSphere(x,y);		
				}	
			}

			count = 0;
			for (int i = 0; i < nAASamplesSqrt; i++) {
				for (int j = 0; j < nAASamplesSqrt; j++) {
					// we need a uniform number in the interval
					// [i/nSqrt;(i+1)/nSqrt]
					double x = rg->getDouble( ((double)i)/(double)nAASamplesSqrt,((double)(i+1.0))/(double)nAASamplesSqrt);
					double y = rg->getDouble( ((double)j)/(double)nAASamplesSqrt,((double)(j+1.0))/(double)nAASamplesSqrt);
					aaSamples[count] = Vector3f(x-0.5,y-0.5,1);
					x = rg->getDouble( ((double)i)/(double)nSqrt,((double)(i+1.0))/(double)nSqrt);
					y = rg->getDouble( ((double)j)/(double)nSqrt,((double)(j+1.0))/(double)nSqrt);
					lensSamples[count++] = concentricSampleDisk(x,y);
				}	
			}
			// We randomize the samples to avoid coherence.
			aaSamples = rg->randomize(aaSamples);
			lensSamples = rg->randomize(lensSamples);

		};


		/// --- Progressive versions...

		Vector3f ProgressiveStratifiedSampler::getAODirection(int index) {
			if (index>=aoSamplesSqrt*aoSamplesSqrt) throw 1;
			int j = index / aoSamplesSqrt;
			int i = index % aoSamplesSqrt;
			double x = rg->getDouble( ((double)i)/(double)aoSamplesSqrt,((double)(i+1.0))/(double)aoSamplesSqrt);
			double y = rg->getDouble( ((double)j)/(double)aoSamplesSqrt,((double)(j+1.0))/(double)aoSamplesSqrt);		
			return sampleSphere(x,y);
		}

		Vector3f ProgressiveStratifiedSampler::getAASample(int ix) {
			if (ix>=(aaSamplesSqrt*aaSamplesSqrt)) throw 1;
			int index = (aaSamplesSqrt*aaSamplesSqrt-1)-aaOrder[ix];
			
			int i = index / aaSamplesSqrt;
			int j = index % aaSamplesSqrt;
			double x = rg->getDouble( ((double)i)/(double)aaSamplesSqrt,((double)(i+1.0))/(double)aaSamplesSqrt);
			double y = rg->getDouble( ((double)j)/(double)aaSamplesSqrt,((double)(j+1.0))/(double)aaSamplesSqrt);
			return Vector3f(x-0.5,y-0.5,1);
		}

		Vector3f ProgressiveStratifiedSampler::getLensSample(int ix) {
			if (ix>=(aaSamplesSqrt*aaSamplesSqrt)) throw 1;
			int index = aaOrder[ix];
			int i = index / aaSamplesSqrt;
			int j = index % aaSamplesSqrt;
			double x = rg->getDouble( ((double)i)/(double)aaSamplesSqrt,((double)(i+1.0))/(double)aaSamplesSqrt);
			double y = rg->getDouble( ((double)j)/(double)aaSamplesSqrt,((double)(j+1.0))/(double)aaSamplesSqrt);
			//return rg->getUniform2D();
			return concentricSampleDisk(x,y);
			
		}

		// Based on the Physical Based Rendering book
		Vector3f ProgressiveStratifiedSampler::sampleSphere(double u1, double u2) {
			double z = 1.0 - 2.0*u1;
			double r = 0;
			if (1.0-z*z > 0) r = sqrt(1.0-z*z);
			double phi = 2.0 * 3.1415926 * u2;
			double x = r * cos(phi);
			double y = r * sin(phi);
			return Vector3f(x,y,z);
		}

		void ProgressiveStratifiedSampler::prepareSamples(int nAASamplesSqrt, int nAOSamplesSqrt) {
			this->aoSamplesSqrt = nAASamplesSqrt*nAOSamplesSqrt;
			this->aaSamplesSqrt = nAASamplesSqrt;
		};

		Sampler* ProgressiveStratifiedSampler::clone(Math::RandomNumberGenerator* rg) { 
			ProgressiveStratifiedSampler* ps = new ProgressiveStratifiedSampler(rg); 
			ps->aaOrder = aaOrder;
			return ps;
		}



	}
}

