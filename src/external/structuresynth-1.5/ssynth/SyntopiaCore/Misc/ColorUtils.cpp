#include "ColorUtils.h"
#include "../Logging/Logging.h"

using namespace SyntopiaCore::Math;
using namespace SyntopiaCore::Logging;


namespace SyntopiaCore {
	namespace Misc {	

		SyntopiaCore::Math::Vector3f ColorUtils::HSVtoRGB(SyntopiaCore::Math::Vector3f hsv) {
			/// Implementation based on: http://en.wikipedia.org/wiki/HSV_color_space
			if (hsv[0] >= 360) hsv[0]-=360;
			int Hi = (int)(hsv[0] / 60) % 6;
			double f = (hsv[0] /  60) - Hi;
			double p = hsv[2]*(1-hsv[1]);
			double q = hsv[2]*(1-f*hsv[1]);
			double t = hsv[2]*(1-(1-f)*hsv[1]);
			if (Hi == 0) return Vector3f(hsv[2],t,p);
			if (Hi == 1) return Vector3f(q,hsv[2],p);
			if (Hi == 2) return Vector3f(p,hsv[2],t);
			if (Hi == 3) return Vector3f(p,q,hsv[2]);
			if (Hi == 4) return Vector3f(t,p,hsv[2]);
			if (Hi == 5) return Vector3f(hsv[2],p,q);
			WARNING("ColorUtils::HSVtoRGB failed");
			return Vector3f(0,0,0);
		}
		
	
	}
}

