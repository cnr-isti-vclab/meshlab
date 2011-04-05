#include <vcg/math/matrix44.h>
#include <vcg/space/point3.h>
 
/**
Compute a scaling transformation that bring PMov point as close as possible to Pfix
*/ 
bool ComputeScalingMatchMatrix(vcg::Matrix44d &res,
							   std::vector<vcg::Point3d> &Pfix,		
							   std::vector<vcg::Point3d> &Pmov);

/**
Compute a rototranslation + scaling transformation that bring PMov point as close as possible to Pfix
*/ 


bool ComputeRotoTranslationScalingMatchMatrix(vcg::Matrix44d &res,
 						std::vector<vcg::Point3d> &Pfix,		
						std::vector<vcg::Point3d> &Pmov);