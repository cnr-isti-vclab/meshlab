#include "util.h"
#include <cassert>

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