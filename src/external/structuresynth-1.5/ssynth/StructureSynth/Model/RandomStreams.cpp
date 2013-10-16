#include "RandomStreams.h"


namespace StructureSynth {
	namespace Model {	
		SyntopiaCore::Math::RandomNumberGenerator* RandomStreams::geometry = new SyntopiaCore::Math::RandomNumberGenerator(false);
		SyntopiaCore::Math::RandomNumberGenerator* RandomStreams::color = new SyntopiaCore::Math::RandomNumberGenerator(false);

	}
}

