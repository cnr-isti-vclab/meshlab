/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

uniform float zoom;
uniform float pointRadius;
uniform float halfLineWidth;
uniform bool showWireframe;
uniform vec4 wireframeColor;
uniform vec4 pointColor;

in vec3 frag_linearBasis;
in vec2 position;
flat in vec2 vertices[3];
flat in vec2 normEdges[3];


float irlerp(in vec3 vx, in vec3 v1, in vec3 v2)
{
    float alpha = acos(clamp(dot(v1, vx), -1., 1.));
    float beta = acos(clamp(dot(v1, v2), -1., 1.));
    return alpha / beta;
}


vec4 quadraticInterp(in vec4 colors[6])
{
    return
        colors[0] * frag_linearBasis.x * (2. * frag_linearBasis.x - 1.) +
        colors[1] * frag_linearBasis.y * (2. * frag_linearBasis.y - 1.) +
        colors[2] * frag_linearBasis.z * (2. * frag_linearBasis.z - 1.) +
        colors[3] * 4. * frag_linearBasis.y * frag_linearBasis.z +
        colors[4] * 4. * frag_linearBasis.z * frag_linearBasis.x +
        colors[5] * 4. * frag_linearBasis.x * frag_linearBasis.y;
}

float diffuse(in vec3 n, in vec3 l)
{
    return clamp(dot(n, l), 0., 1.);
}


///////////////////////////////////////////////////////////////////////////////
// Color space conversion

const int COLOR_NONE       = 0;
const int COLOR_SRGB       = 1;
const int COLOR_LINEAR_RGB = 2;
const int COLOR_CIE_XYZ    = 3;
const int COLOR_CIE_LAB    = 4;
const int COLOR_CIE_LUV    = 5;


// SRGB <-> linear rgbToxyz

vec3 srgbFromLinearRGB(const in vec3 linear)
{
    vec3 srgb = linear;
    srgb[0] = (linear[0] > 0.0031308)?
              1.055 * pow(linear[0], 1./2.4) - .055f:
              12.92 * linear[0];
    srgb[1] = (linear[1] > 0.0031308)?
              1.055 * pow(linear[1], 1./2.4) - .055f:
              12.92 * linear[1];
    srgb[2] = (linear[2] > 0.0031308)?
              1.055 * pow(linear[2], 1./2.4) - .055f:
              12.92 * linear[2];
    return srgb;
}

vec3 linearRGBFromSrgb(const in vec3 srgb)
{
    vec3 linear = srgb;
    linear[0] = (linear[0] > 0.04045)?
                pow((linear[0]+0.055) / 1.055, 2.4):
                linear[0] / 12.92;
    linear[1] = (linear[1] > 0.04045)?
                pow((linear[1]+0.055) / 1.055, 2.4):
                linear[1] / 12.92;
    linear[2] = (linear[2] > 0.04045)?
                pow((linear[2]+0.055) / 1.055, 2.4):
                linear[2] / 12.92;
    return linear;
}


// Linear RGB <-> Cie XYZ

const mat3 xyzToRgb = mat3(
     3.2406, -1.5372, -0.4986,
    -0.9689,  1.8758,  0.0415,
     0.0557, -0.2040,  1.0570
);

vec3 linearRGBFromCieXYZ(const in vec3 cieXYZ) {
    return transpose(xyzToRgb) * cieXYZ;
}

mat3 rgbToxyz = mat3(
     0.4124, 0.3576, 0.1805,
     0.2126, 0.7152, 0.0722,
     0.0193, 0.1192, 0.9505
);

vec3 cieXYZFromLinearRGB(const in vec3 lrgb) {
    return transpose(rgbToxyz) * lrgb;
}


// Cie XYZ <-> Cie Lab

float cieLabF(const in float v) {
    return (v > 0.008856452)?
                pow(v, 1./3.):
                1. / (3. * 0.042806183) * v + (4. / 29.);
}

float cieLabFInv(const in float v) {
    return (v > 0.206896552)?
                pow(v, 3.):
                3. * 0.042806183 * (v - (4. / 29.));
}

const vec3 cieLabWhite = vec3(0.95047, 1, 1.08883);

vec3 cieLabFromCieXYZ(const in vec3 cieXYZ)
{
    float fy = cieLabF(cieXYZ[1] / cieLabWhite[1]);
    return vec3(
            1.16 * fy - 0.16,
            5.00 * (cieLabF(cieXYZ[0] / cieLabWhite[0]) - fy),
            2.00 * (fy - cieLabF(cieXYZ[2] / cieLabWhite[2])));
}


vec3 cieXYZFromCieLab(const in vec3 cielab)
{
    float lf = (cielab[0] + 0.16) / 1.16;
    return vec3(
            cieLabWhite[0] * cieLabFInv(lf + cielab[1] / 5.00),
            cieLabWhite[1] * cieLabFInv(lf),
            cieLabWhite[2] * cieLabFInv(lf - cielab[2] / 2.00));
}


// Cie XYZ <-> Cie Luv

vec3 cieLuvFromCieXYZ(const in vec3 cieXYZ)
{
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float l = (cieXYZ[1] <= 0.008856452f)?
                    9.03296296296f * cieXYZ[1]:
                    1.16f * pow(cieXYZ[1], 1.f/3.f) - .16f;
    float d = cieXYZ[0] + 15.f * cieXYZ[1] + 3.f * cieXYZ[2];

    return vec3(
                l,
                (d > .001f)? 13.f * l * (4.f*cieXYZ[0] / d - wu): 0.f,
                (d > .001f)? 13.f * l * (9.f*cieXYZ[1] / d - wv): 0.f);
}

vec3 cieXYZFromCieLuv(const in vec3 cieluv)
{
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float up_13l = cieluv[1] + wu * (13.f * cieluv[0]);
    float vp_13l = cieluv[2] + wv * (13.f * cieluv[0]);

    float y = (cieluv[0] <= .08f)?
                    cieluv[0] * 0.1107056f:
                    pow((cieluv[0]+.16f) / 1.16f, 3.f);
    return vec3(
                (vp_13l > .001f)? 2.25f * y * up_13l / vp_13l: 0.f,
                y,
                (vp_13l > .001f)? y * (156.f*cieluv[0] - 3.f*up_13l - 20.f*vp_13l) / (4.f * vp_13l): 0.f);
}


// General color conversion

vec3 convertColor(in vec3 fromColor, in int from, in int to)
{
    if(from == COLOR_NONE || to == COLOR_NONE || from == to)
        return fromColor;

    vec3 color = fromColor;

    // To XYZ
    if(from == COLOR_SRGB) {
        color = linearRGBFromSrgb(color);
        from = COLOR_LINEAR_RGB;
        if(to == COLOR_LINEAR_RGB) return color;
    }
    if(from == COLOR_LINEAR_RGB) {
        color = cieXYZFromLinearRGB(color);
    }
    if(from == COLOR_CIE_LAB) {
        color = cieXYZFromCieLab(color);
    }
    if(from == COLOR_CIE_LUV) {
        color = cieXYZFromCieLuv(color);
    }

    // From XYZ
    if(to < COLOR_CIE_XYZ) {
        color = linearRGBFromCieXYZ(color);
        if(to == COLOR_SRGB) {
            color = srgbFromLinearRGB(color);
        }
    } else if(to == COLOR_CIE_LAB) {
        color = cieLabFromCieXYZ(color);
    } else if(to == COLOR_CIE_LUV) {
        color = cieLuvFromCieXYZ(color);
    }

    return color;
}


