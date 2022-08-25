#ifndef _OCME_QUANTIZED_PLANE_
#define _OCME_QUANTIZED_PLANE_

#include <vcg/space/plane3.h>
#include <vcg/space/box3.h>

void ToPolar(vcg::Point3f n, float & alpha, float & beta);
vcg::Point3f FromPolar(float   alpha, float   beta);

unsigned int SizeCompressedPlane();
void CompressPlane(vcg::Plane3f p, vcg::Box3f box, char * buf );
void DeCompressPlane(char * buf, vcg::Box3f box , vcg::Plane3f &  p);

template <class S>
unsigned int SizeCompressedPlane(){return 3*sizeof(S);};

template <class S>
void CompressPlane(vcg::Plane3f p, vcg::Box3f box, S * buf ){

	S max_value = std::numeric_limits<S>::max();
	float alpha,beta;
	
	float newoffset = -vcg::Distance(box.Center(),p);
	
	buf [0] = (newoffset* max_value) / box.Diag() ; //newoffset in -Diag()/2 +Diag()/2

	ToPolar(p.Direction(),alpha,beta);

	buf[1] = (alpha* max_value) /(2.f*M_PI);  // alpha in -PI + PI
	buf[2] = (beta * max_value) /(    M_PI);	// beta in -PI/2 +PI/2

}

template <class S>
void DeCompressPlane(S * buf, vcg::Box3f box , vcg::Plane3f &  p){
	S max_value = std::numeric_limits<S>::max();

	float offset = (buf[0] * box.Diag()) / max_value;
	float alpha  = (buf[1] * 2.f * M_PI) /max_value;
	float beta   = (buf[2] *       M_PI) /max_value;

	p.SetDirection(FromPolar(alpha,beta));
	p.SetOffset(offset);
	p.SetOffset(-vcg::Distance(-box.Center(),p));
}

template <class S>
struct Plane3Comp{
	S v[3];

	// this is quite correct. the maximum value for the offset corresponds to a plane with
	// degenerate intersection with the cube (only one corner) or no intersection at all
	void SetNull(){v[0] = std::numeric_limits<S>::max();}
	bool IsNull(){return (v[0] == std::numeric_limits<S>::max());}
};

#endif