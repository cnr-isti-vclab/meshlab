#pragma once

#include "../../SyntopiaCore/Math/Random.h"

namespace StructureSynth {
	namespace Model {	
		
		/// These two independent random number generator streams are used in Structure Synth
		class RandomStreams {
		public:
			static SyntopiaCore::Math::RandomNumberGenerator* Geometry() { return geometry; }
			static SyntopiaCore::Math::RandomNumberGenerator* Color() { return color; }
			static void SetSeed(int seed) { geometry->setSeed(seed); color->setSeed(seed); }
			static void UseOldRandomGenerators(bool useOld) { geometry->useStdLib(useOld); color->useStdLib(useOld); }
		private:
			static SyntopiaCore::Math::RandomNumberGenerator* geometry;
			static SyntopiaCore::Math::RandomNumberGenerator* color;
		};

	}
}

