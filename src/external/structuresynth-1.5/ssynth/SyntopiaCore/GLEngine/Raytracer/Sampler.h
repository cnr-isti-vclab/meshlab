#pragma once

#include "SyntopiaCore/Math/Random.h"
#include "SyntopiaCore/Math/Vector3.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		using namespace SyntopiaCore::Math;

		class Filter {
		public:
			Filter() {};
			virtual float getWeight(float xSqr, float ySqr) = 0;
			virtual int getExtent() = 0;
		};

		class BoxFilter : public Filter {
		public:
			BoxFilter() {};
			virtual float getWeight(float, float) { return 1.0; }
			virtual int getExtent() { return 0;	};
		};

		class GaussianFilter : public Filter {
		public:
			GaussianFilter(double ext, double alpha) : extent(extent), alpha(alpha) {
				this->extent = int(0.5+ext);
				s =  -exp(-alpha*(ext*ext));
			};

			virtual float getWeight(float xSqr, float ySqr) {
				return (gaussian(xSqr)*gaussian(ySqr));
			};

			float gaussian(float v) {
				float a= exp(-alpha*v)+s;
				return a > 0 ? a : 0;
			}

			virtual int getExtent() { return extent;	};
		private:
			int extent;
			double s;
			double alpha;
		};

		class TriangleFilter : public Filter {
		public:
			TriangleFilter(double halfwidth) : halfwidth(halfwidth) {
				this->extent = int(halfwidth-0.5+1);
			};

			virtual float getWeight(float xSqr, float ySqr) {
				return (triangle(sqrt(xSqr))*triangle(sqrt(ySqr)));
			};

			float triangle(float v) {
				float a= 1-v/halfwidth;
				return a > 0 ? a : 0;
			}

			virtual int getExtent() { return extent;	};
		private:
			int extent;
			double halfwidth;
		};


		// A simple sampler. Draws uniform numbers, but no stratification.
		class Sampler {
		public:
			Sampler(Math::RandomNumberGenerator* rg);
			virtual ~Sampler();
			virtual Vector3f getAASample(int /*index*/) { return Vector3f(rg->getDouble(-0.5,0.5), rg->getDouble(-0.5,0.5),1.0); }
			virtual Vector3f getAODirection(int /*index*/) { return rg->getUniform3D(); }
			virtual Vector3f getLensSample(int /*index*/) { return rg->getUniform2D(); }
			virtual void prepareSamples(int /*nSamplesSqrt*/, int /*nAOSamplesSqrt*/) {};
			virtual Sampler* clone(Math::RandomNumberGenerator* rg) { return new Sampler(rg); }
		protected:
			Math::RandomNumberGenerator* rg;
		};

		// Stratified sampling
		class StratifiedSampler : public Sampler {
		public:
			StratifiedSampler(Math::RandomNumberGenerator* rg) : Sampler(rg) {};
			virtual Vector3f getAASample(int index);
			virtual Vector3f getAODirection(int index);
			virtual Vector3f getLensSample(int index);
			Vector3f sampleSphere(double u1, double u2);
			virtual void prepareSamples(int nSamplesSqrt, int nAOSamplesSqrt);
			virtual Sampler* clone(Math::RandomNumberGenerator* rg) { return new StratifiedSampler(rg); }

		private:
			QVector<Vector3f> aoSamples;
			QVector<Vector3f> aaSamples;
			QVector<Vector3f> lensSamples;
		};

		// Stratified sampling
		class ProgressiveStratifiedSampler : public Sampler {
		public:
			ProgressiveStratifiedSampler(Math::RandomNumberGenerator* rg) : 
			  Sampler(rg) {}
			virtual Vector3f getAASample(int index);
			virtual Vector3f getAODirection(int index);
			virtual Vector3f getLensSample(int index);
			Vector3f sampleSphere(double u1, double u2);
			virtual void prepareSamples(int nSamplesSqrt, int nAOSamplesSqrt);
			virtual Sampler* clone(Math::RandomNumberGenerator* rg);
			void setAAOrder(QVector<int> aaOrder) { this->aaOrder = aaOrder; }
		private:
			int aoSamplesSqrt;
			int aaSamplesSqrt;
			QVector<int> aaOrder;
			
		};


	}
}

