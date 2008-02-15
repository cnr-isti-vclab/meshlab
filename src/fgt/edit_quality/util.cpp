#include "util.h"
#include <cmath>

//returns a relative-absolute x value conversion rounded to closer integer value
float relative2AbsoluteValf(float relative_val, float max_val)
{	return (relative_val * max_val);	}

//returns a relative-absolute x value conversion rounded to closer integer value
int relative2AbsoluteVali(float relative_val, float max_val)
{	return (int)(relative2AbsoluteValf(relative_val, max_val)+0.5f);	}

//returns an absolute-relative x value conversion
float absolute2RelativeValf(float absolute_val, float max_val)
{	assert(max_val!=0);	return (absolute_val / max_val);	}

//returns an absolute-relative x value conversion
int absolute2RelativeVali(float absolute_val, float max_val)
{	return (int)(absolute2RelativeValf(absolute_val, max_val)+0.5f);	}

float relative2QualityValf(float relative_val, float min_q, float max_q, float exp)
{ 
	assert( (relative_val>=-1.0f) && (relative_val<=2.0f) );
	assert( (exp>=0) && (exp<=1) );
	assert(min_q<=max_q);
	relative_val = pow( relative_val, exp );
	return ( relative_val * (max_q - min_q) ) + min_q;
}