#pragma once

#include <QString>
#include <QVector>
#include <cmath>
#include "../Math/Vector3.h"

namespace SyntopiaCore {
	namespace Misc {	
	
		
		class ColorUtils {
		public:
			static SyntopiaCore::Math::Vector3f HSVtoRGB(SyntopiaCore::Math::Vector3f hsv);
		};

		
	}
}

