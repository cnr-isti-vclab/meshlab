#ifndef _UTIL_H_
#define _UTIL_H_

//this functions return a relative-absolute value conversion perspectively as float and int (rounded to closer integer value)
float relative2AbsoluteValf(float relative_val, float max_val);
int relative2AbsoluteVali(float relative_val, float max_val);

//this functions return a absolute-relative value conversion perspectively as float and int (rounded to closer integer value)
float absolute2RelativeValf(float absolute_val, float max_val);
int absolute2RelativeVali(float absolute_val, float max_val);

#endif
