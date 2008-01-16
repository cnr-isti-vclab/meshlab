#ifndef _UTIL_H_
#define _UTIL_H_

#include "const_types.h"



//returns a relative-absolute x value conversion rounded to closer integer value
int relative2AbsoluteVali(float relative_val, float max_val);

//returns a relative-absolute x value conversion rounded to closer integer value
float relative2AbsoluteValf(float relative_val, float max_val);

//returns an absolute-relative x value conversion
int absolute2RelativeVali(float absolute_val, float max_val);

//returns an absolute-relative x value conversion
float absolute2RelativeValf(float absolute_val, float max_val);


#endif