#pragma once

#include "Transformation.h"

namespace StructureSynth {
	namespace Model {	

		/// A loop is a transformation which is repeated a number of times:
		/// e.g.: 
		///   20 * { x 1 } R1
		/// will create twenty R1 objects each succesively moved one unit in the x-directions.
		struct TransformationLoop {
			TransformationLoop() {};
			TransformationLoop(int repetitions, Transformation transformation) : repetitions(repetitions), transformation(transformation) {};

			int repetitions;
			Transformation transformation;
		};

	
	}
}

