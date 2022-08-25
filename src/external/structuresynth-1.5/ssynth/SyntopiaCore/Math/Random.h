#pragma once

#include <QString>
#include <QVector>
#include <QStringList>
#include <cmath>
#include <random>
#include <memory>

#include "Vector3.h"

namespace SyntopiaCore {
	namespace Math {	

		/// A simple class for generating random numbers
		/// It is possible to have multiple independent streams, if the underlying RNG is the Mersenne Twister.
		/// If set to useStdLib, the CStdLib 'rand' and 'srand' functions are used - these are not independent - not even with multiple instances of this class.
		class RandomNumberGenerator {
		public:
			RandomNumberGenerator(bool useOldLibrary = false) : uniformCounter2D(0), uniformCounter3D(0) { if (!useOldLibrary) { rng.reset(new std::mt19937()); } setSeed(0); };

			// This is only useful for backward compatibility.
			// The Mersenne Twister is much better since it allows multiple independent streams.
			void useStdLib(bool useOldLibrary) {
				rng.reset();
				if (!useOldLibrary) {
					rng.reset(new std::mt19937());
				}
				setSeed(lastSeed);
			};

			QVector<int> getRandomIndices(int count); 

			// Returns a vector, where the elements are ranked randomly.
			template <typename T>
			QVector<T> randomize(QVector<T> list) {
				QVector<int> indices = getRandomIndices(list.count());
				QVector<T> copy(list.count());
				for (int i = 0; i < list.count(); i++) copy[i] = list[indices[i]];
				return copy;
			}
		

			bool isUsingStdLib() { return (rng == nullptr); }

			// Returns a double in the interval [0;1]
			double getDouble() { 
				if (rng) {
					std::uniform_real_distribution<> d{0.0, 1.0};
					return d(*rng);
					//return static_cast<double>((*rng)()) / static_cast<double>(std::numeric_limits<uint_fast32_t>::max());
				} else {
					return rand()/(double)RAND_MAX;
					/*
					This one would be more correct, but the old cstdlib rand is implemented for backward compatibility:
					return  (double)rand() / ((double)(RAND_MAX)+(double)(1)) ; // There are reasons for the multiple (double) casts, see: http://members.cox.net/srice1/random/crandom.html
					*/
				}
			};    

			// Normal distributed number with mean zero.
			double getNormal(double variance) {
				/// Note: requires MT RNG!
				std::normal_distribution<> d{0, std::sqrt(variance)};
				return d(*rng);
			}

			double getDouble(double min, double max) {
				if (rng) {
					std::uniform_real_distribution<> d{min, max};
					return d(*rng);
				}
				return getDouble()*(max-min)+min;
			}
			
			
			// Returns an integer between 0 and max (both inclusive).
			int getInt(int max) { 
				if (rng) {
					std::uniform_int_distribution<> d{0, max};
					return d(*rng);
				} else {
					return rand() % (max+1); // Probably not very good, use mersenne instead
				}
			}

			int getInt() { 
				if (rng) {
					std::uniform_int_distribution<> d;
					return d(*rng);
				} else {
					return rand();
				}
			}

			void setSeed(int seed) { 
				lastSeed = seed; 
				if (rng) {
					rng->seed(seed);
				} else {
					srand(seed);
				}
			};
			
			// Return uniform samples on either unit disc (z=0) or unit sphere
			// Uses Monto-carlo sampling which might be slow
			Vector3f getUniform2D();
			Vector3f getUniform3D();

			// Uses precalculated tables.
			// Initialized on first use (so init before using in multithreaded code)
			Vector3f getUniform2DFromTable();
			Vector3f getUniform3DFromTable();
			void randomizeUniformCounter(); // use this to avoid coherence between different rg's
			void setUniformCounter2D(int value) { uniformCounter2D = value; }
			void setUniformCounter3D(int value) { uniformCounter3D = value; }
		private:
			int lastSeed;
			std::unique_ptr<std::mt19937> rng;
			int uniformCounter2D;
			int uniformCounter3D;
			
		};

	}
}

