

#include "plane_box_quantization.h"

void ToPolar(vcg::Point3f n, float & alpha, float & beta){

	beta = asin(n[1]);
	if( n[1]*n[1] > 0.99) 
		alpha = 0.f; 
	else
		alpha = ((n[2]>0.f)?1.f:-1.f) * acos( n[0] / sqrt(1.f-( n[1]*n[1] ) ));
}

vcg::Point3f FromPolar(float   alpha, float   beta){
	vcg::Point3f n;
	n[1] = sin(beta);
	n[0] = cos(alpha) * cos(beta);
	n[2] = sin(alpha) * cos(beta);
	return n;
};

