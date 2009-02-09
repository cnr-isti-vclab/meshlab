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


/*!
 * Set a quaternion to Identity
 */
void
lib3ds_quat_identity(float c[4]) {
    c[0] = c[1] = c[2] = 0.0f;
    c[3] = 1.0f;
}


/*!
 * Copy a quaternion.
 */
void
lib3ds_quat_copy(float dest[4], float src[4]) {
    int i;
    for (i = 0; i < 4; ++i) {
        dest[i] = src[i];
    }
}


/*!
 * Compute a quaternion from axis and angle.
 *
 * \param c Computed quaternion
 * \param axis Rotation axis
 * \param angle Angle of rotation, radians.
 */
void
lib3ds_quat_axis_angle(float c[4], float axis[3], float angle) {
    double omega, s;
    double l;

    l = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    if (l < LIB3DS_EPSILON) {
        c[0] = c[1] = c[2] = 0.0f;
        c[3] = 1.0f;
    } else {
        omega = -0.5 * angle;
        s = sin(omega) / l;
        c[0] = (float)s * axis[0];
        c[1] = (float)s * axis[1];
        c[2] = (float)s * axis[2];
        c[3] = (float)cos(omega);
    }
}


/*!
 * Negate a quaternion
 */
void
lib3ds_quat_neg(float c[4]) {
    int i;
    for (i = 0; i < 4; ++i) {
        c[i] = -c[i];
    }
}


/*!
 * Compute the conjugate of a quaternion
 */
void
lib3ds_quat_cnj(float c[4]) {
    int i;
    for (i = 0; i < 3; ++i) {
        c[i] = -c[i];
    }
}


/*!
 * Multiply two quaternions.
 *
 * \param c Result
 * \param a,b Inputs
 */
void
lib3ds_quat_mul(float c[4], float a[4], float b[4]) {
    float qa[4], qb[4];
    lib3ds_quat_copy(qa, a);
    lib3ds_quat_copy(qb, b);
    c[0] = qa[3] * qb[0] + qa[0] * qb[3] + qa[1] * qb[2] - qa[2] * qb[1];
    c[1] = qa[3] * qb[1] + qa[1] * qb[3] + qa[2] * qb[0] - qa[0] * qb[2];
    c[2] = qa[3] * qb[2] + qa[2] * qb[3] + qa[0] * qb[1] - qa[1] * qb[0];
    c[3] = qa[3] * qb[3] - qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2];
}


/*!
 * Multiply a quaternion by a scalar.
 */
void
lib3ds_quat_scalar(float c[4], float k) {
    int i;
    for (i = 0; i < 4; ++i) {
        c[i] *= k;
    }
}


/*!
 * Normalize a quaternion.
 */
void
lib3ds_quat_normalize(float c[4]) {
    double l, m;

    l = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]);
    if (fabs(l) < LIB3DS_EPSILON) {
        c[0] = c[1] = c[2] = 0.0f;
        c[3] = 1.0f;
    } else {
        int i;
        m = 1.0f / l;
        for (i = 0; i < 4; ++i) {
            c[i] = (float)(c[i] * m);
        }
    }
}


/*!
 * Compute the inverse of a quaternion.
 */
void
lib3ds_quat_inv(float c[4]) {
    double l, m;

    l = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]);
    if (fabs(l) < LIB3DS_EPSILON) {
        c[0] = c[1] = c[2] = 0.0f;
        c[3] = 1.0f;
    } else {
        m = 1.0f / l;
        c[0] = (float)(-c[0] * m);
        c[1] = (float)(-c[1] * m);
        c[2] = (float)(-c[2] * m);
        c[3] = (float)(c[3] * m);
    }
}


/*!
 * Compute the dot-product of a quaternion.
 */
float
lib3ds_quat_dot(float a[4], float b[4]) {
    return(a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3]);
}


float
lib3ds_quat_norm(float c[4]) {
    return(c[0]*c[0] + c[1]*c[1] + c[2]*c[2] + c[3]*c[3]);
}


void
lib3ds_quat_ln(float c[4]) {
    double om, s, t;

    s = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
    om = atan2(s, c[3]);
    if (fabs(s) < LIB3DS_EPSILON) {
        t = 0.0f;
    } else {
        t = om / s;
    }
    {
        int i;
        for (i = 0; i < 3; ++i) {
            c[i] = (float)(c[i] * t);
        }
        c[3] = 0.0f;
    }
}


void
lib3ds_quat_ln_dif(float c[4], float a[4], float b[4]) {
    float invp[4];

    lib3ds_quat_copy(invp, a);
    lib3ds_quat_inv(invp);
    lib3ds_quat_mul(c, invp, b);
    lib3ds_quat_ln(c);
}


void
lib3ds_quat_exp(float c[4]) {
    double om, sinom;

    om = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
    if (fabs(om) < LIB3DS_EPSILON) {
        sinom = 1.0f;
    } else {
        sinom = sin(om) / om;
    }
    {
        int i;
        for (i = 0; i < 3; ++i) {
            c[i] = (float)(c[i] * sinom);
        }
        c[3] = (float)cos(om);
    }
}


void
lib3ds_quat_slerp(float c[4], float a[4], float b[4], float t) {
    double l;
    double om, sinom;
    double sp, sq;
    float flip = 1.0f;
    int i;

    l = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    if (l < 0) { 
        flip = -1.0f;
        l = -l; 
    }    
    
    om = acos(l);
    sinom = sin(om);
    if (fabs(sinom) > LIB3DS_EPSILON) {
        sp = sin((1.0f - t) * om) / sinom;
        sq = sin(t * om) / sinom;
    } else {
        sp = 1.0f - t;
        sq = t;
    }
    sq *= flip;
    for (i = 0; i < 4; ++i) {
        c[i] = (float)(sp * a[i] + sq * b[i]);
    }
}


void
lib3ds_quat_squad(float c[4], float a[4], float p[4], float q[4], float b[4], float t) {
    float ab[4];
    float pq[4];

    lib3ds_quat_slerp(ab, a, b, t);
    lib3ds_quat_slerp(pq, p, q, t);
    lib3ds_quat_slerp(c, ab, pq, 2*t*(1 - t));
}


void
lib3ds_quat_tangent(float c[4], float p[4], float q[4], float n[4]) {
    float dn[4], dp[4], x[4];
    int i;

    lib3ds_quat_ln_dif(dn, q, n);
    lib3ds_quat_ln_dif(dp, q, p);

    for (i = 0; i < 4; i++) {
        x[i] = -1.0f / 4.0f * (dn[i] + dp[i]);
    }
    lib3ds_quat_exp(x);
    lib3ds_quat_mul(c, q, x);
}


void
lib3ds_quat_dump(float q[4]) {
    printf("%f %f %f %f\n", q[0], q[1], q[2], q[3]);
}

