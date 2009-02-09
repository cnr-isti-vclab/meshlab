/*
    Copyright (C) 1996-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.
    
    This program is free  software: you can redistribute it and/or modify 
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 2.1 of the License, or 
    (at your option) any later version.

    Thisprogram  is  distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
    GNU Lesser General Public License for more details.
    
    You should  have received a copy of the GNU Lesser General Public License
    along with  this program; If not, see <http://www.gnu.org/licenses/>. 
*/
#include "lib3ds_impl.h"


float
lib3ds_math_ease(float fp, float fc, float fn, float ease_from, float ease_to) {
    double s, step;
    double tofrom;
    double a;

    s = step = (float)(fc - fp) / (fn - fp);
    tofrom = ease_to + ease_from;
    if (tofrom != 0.0) {
        if (tofrom > 1.0) {
            ease_to = (float)(ease_to / tofrom);
            ease_from = (float)(ease_from / tofrom);
        }
        a = 1.0 / (2.0 - (ease_to + ease_from));

        if (step < ease_from) s = a / ease_from * step * step;
        else {
            if ((1.0 - ease_to) <= step) {
                step = 1.0 - step;
                s = 1.0 - a / ease_to * step * step;
            } else {
                s = ((2.0 * step) - ease_from) * a;
            }
        }
    }
    return((float)s);
}


void
lib3ds_math_cubic_interp(float *v, float *a, float *p, float *q, float *b, int n, float t) {
    float x, y, z, w;
    int i;

    x = 2 * t * t * t - 3 * t * t + 1;
    y = -2 * t * t * t + 3 * t * t;
    z = t * t * t - 2 * t * t + t;
    w = t * t * t - t * t;
    for (i = 0; i < n; ++i) {
        v[i] = x * a[i] + y * b[i] + z * p[i] + w * q[i];
    }
}
