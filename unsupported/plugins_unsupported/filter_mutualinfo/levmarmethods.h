/** <b>Levmar Methods -- Interface with the levamr lib </b>

The class is static, so a simple call to <code>calibrate()</code> is
sufficient to get a calibrated shot.<br>

*/

#include <vcg/math/matrix44.h>
#include <vcg/space/point3.h>
#include <vcg/space/point2.h>
#include <vcg/math/shot.h>

#include <list>

#include "../../external/levmar-2.3/lm.h"


struct LevmarCorrelation {
	vcg::Point3f point3d;
	vcg::Point2d point2d;
};

struct LevmarData{
	vcg::Point3f** points3d; 
	vcg::Shot<float>* levmarCam;
};


class LevmarMethods
{
public:
	//Calibration of the shot according to the 2D and 3D points
	static bool calibrate( vcg::Shot<float>* shot,std::list<LevmarCorrelation>* corr,bool p_focal); 

	///Transformation of the camera data between tsai structure and vcg structure
	///True if you want calibrate the focal, false if you want calibrate the extrinsics
	static void Shot2Levmar(vcg::Shot<float>*,double* p,bool p_foc);
	///Transformation of the camera data between tsai structure and vcg structure
	///True if you have calibrated the focal, false if you had calibrated the extrinsics
	static void Levmar2Shot(vcg::Shot<float>*, double *p, bool p_foc);
	
private:
	static void Cam2Levmar(vcg::Shot<float>*);
    static bool createDataSet(std::list<LevmarCorrelation>* corr,vcg::Shot<float>* s, LevmarData* data, double* x,double opts[LM_OPTS_SZ],double info[LM_INFO_SZ]);
	static void estimateExtr(double *p, double *x, int m, int n, void *data);
	static void estimateFocal(double *p, double *x, int m, int n, void *data);

};

