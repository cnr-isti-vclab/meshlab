/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PATATE_COMMON_GL_UTILS_COLOR_
#define _PATATE_COMMON_GL_UTILS_COLOR_


#include "Eigen/Dense"


namespace PatateCommon
{


enum ColorSpace {
    COLOR_NONE,
    COLOR_SRGB,
    COLOR_LINEAR_RGB,
    COLOR_CIE_XYZ,
    COLOR_CIE_LAB,
    COLOR_CIE_LUV
};


// SRGB <-> linear RGB

inline Eigen::Vector3f srgbFromLinearRGB(const Eigen::Vector3f& lrgb)
{
    Eigen::Vector3f srgb = lrgb;
    for(int i=0; i<3; ++i)
        srgb(i) = lrgb(i) > 0.0031308f?
                    1.055f * std::pow(lrgb(i), 1.f/2.4f) - .055f:
                    12.92f * lrgb(i);
    return srgb;
}


inline Eigen::Vector3f linearRGBFromSrgb(const Eigen::Vector3f& srgb)
{
    Eigen::Vector3f lrgb = srgb;
    for(int i=0; i<3; ++i)
        lrgb(i) = lrgb(i) > 0.04045f?
                    std::pow((lrgb(i)+0.055f) / 1.055f, 2.4f):
                    lrgb(i) / 12.92f;
    return lrgb;
}


// linear RGB <-> CIE XYZ

inline Eigen::Vector3f linearRGBFromCieXYZ(const Eigen::Vector3f& cieXYZ) {
    static const Eigen::Matrix3f xyzToRgb = (Eigen::Matrix3f() <<
         3.2406f, -1.5372f, -0.4986f,
        -0.9689f,  1.8758f,  0.0415f,
         0.0557f, -0.2040f,  1.0570f
    ).finished();

    return xyzToRgb * cieXYZ;
}


inline Eigen::Vector3f cieXYZFromLinearRGB(const Eigen::Vector3f& lrgb) {
    static const Eigen::Matrix3f rgbToxyz = (Eigen::Matrix3f() <<
         0.4124f, 0.3576f, 0.1805f,
         0.2126f, 0.7152f, 0.0722f,
         0.0193f, 0.1192f, 0.9505f
    ).finished();

    return rgbToxyz * lrgb;
}


// CIE XYZ <-> CIE LAB

namespace internal {

    template < typename Scalar >
    struct LabHelper {
    static const Scalar thresold;
    static const Scalar thresold2;
    static const Scalar thresold3;
    static const Eigen::Matrix<Scalar, 3, 1> white;

    static inline Scalar f(Scalar v) {
        return (v > thresold3)?
                    std::pow(v, Scalar(1./3.)):
                    Scalar(1.) / (Scalar(3.) * thresold2) * v + Scalar(4. / 29.);
    }

    static inline Scalar fInv(Scalar v) {
        return (v > thresold)?
                    std::pow(v, Scalar(3.)):
                    Scalar(3.) * thresold2 * (v - Scalar(4. / 29.));
    }
    };

    template < typename Scalar >
    const Scalar LabHelper<Scalar>::thresold = Scalar(6. / 29.);
    template < typename Scalar >
    const Scalar LabHelper<Scalar>::thresold2 =
            LabHelper<Scalar>::thresold * LabHelper<Scalar>::thresold;
    template < typename Scalar >
    const Scalar LabHelper<Scalar>::thresold3 =
            LabHelper<Scalar>::thresold2 * LabHelper<Scalar>::thresold;

    template < typename Scalar >
    const Eigen::Matrix<Scalar, 3, 1> LabHelper<Scalar>::white =
            Eigen::Matrix<Scalar, 3, 1>(0.95047f, 1.f, 1.08883f);
}




inline Eigen::Vector3f cieLabFromCieXYZ(const Eigen::Vector3f& cieXYZ)
{
    typedef internal::LabHelper<float> LH;

    float fy = LH::f(cieXYZ(1) / LH::white(1));
    return Eigen::Vector3f(
                1.16f * fy - 0.16f,
                5.00f * (LH::f(cieXYZ(0) / LH::white(0)) - fy),
                2.00f * (fy - LH::f(cieXYZ(2) / LH::white(2))));
}


inline Eigen::Vector3f cieXYZFromCieLab(const Eigen::Vector3f& cielab)
{
    typedef internal::LabHelper<float> LH;

    float lf = (cielab(0) + 0.16f) / 1.16f;
    return Eigen::Vector3f(
                LH::white(0) * LH::fInv(lf + cielab(1) / 5.00f),
                LH::white(1) * LH::fInv(lf),
                LH::white(2) * LH::fInv(lf - cielab(2) / 2.00f));
}


// CIE XYZ <-> CIE LUV

inline Eigen::Vector3f cieLuvFromCieXYZ(const Eigen::Vector3f& cieXYZ)
{
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float l = (cieXYZ(1) <= 0.008856452f)?
                    9.03296296296f * cieXYZ(1):
                    1.16f * std::pow(cieXYZ(1), 1.f/3.f) - .16f;
    float d = cieXYZ(0) + 15.f * cieXYZ(1) + 3.f * cieXYZ(2);

    return Eigen::Vector3f(
                l,
                (d > .001f)? 13.f * l * (4.f*cieXYZ(0) / d - wu): 0.f,
                (d > .001f)? 13.f * l * (9.f*cieXYZ(1) / d - wv): 0.f);
}


inline Eigen::Vector3f cieXYZFromCieLuv(const Eigen::Vector3f& cieluv)
{
    const float wu = 0.197839825f;
    const float wv = 0.468336303f;

    float up_13l = cieluv(1) + wu * (13.f * cieluv(0));
    float vp_13l = cieluv(2) + wv * (13.f * cieluv(0));

    float y = (cieluv(0) <= .08f)?
                    cieluv(0) * 0.1107056f:
                    std::pow((cieluv(0)+.16f) / 1.16f, 3.f);
    return Eigen::Vector3f(
                (vp_13l > .001f)? 2.25f * y * up_13l / vp_13l: 0.f,
                y,
                (vp_13l > .001f)? y * (156.f*cieluv(0) - 3.f*up_13l - 20.f*vp_13l) / (4.f * vp_13l): 0.f);
}


inline ColorSpace colorSpaceFromName(const std::string& name, bool* ok=0) {
    if(ok) *ok = true;
    if(     name == "none")       return COLOR_NONE;
    else if(name == "srgb")       return COLOR_SRGB;
    else if(name == "linear_rgb") return COLOR_LINEAR_RGB;
    else if(name == "cie_xyz")    return COLOR_CIE_XYZ;
    else if(name == "cie_lab")    return COLOR_CIE_LAB;
    else if(name == "cie_luv")    return COLOR_CIE_LUV;

    if(ok) *ok = false;
    return COLOR_NONE;
}


inline const char* getColorSpaceName(ColorSpace cs) {
    switch(cs) {
    case COLOR_NONE:        return "none";
    case COLOR_SRGB:        return "srgb";
    case COLOR_LINEAR_RGB:  return "linear_rgb";
    case COLOR_CIE_XYZ:     return "cie_xyz";
    case COLOR_CIE_LAB:     return "cie_lab";
    case COLOR_CIE_LUV:     return "cie_luv";
    }
    return "none";
}


inline Eigen::Vector3f convertColor(const Eigen::Vector3f& fromColor,
                                    ColorSpace from, ColorSpace to)
{
    if(from == COLOR_NONE || to == COLOR_NONE || from == to)
        return fromColor;

    Eigen::Vector3f color = fromColor;

    // To XYZ
    switch(from) {
    case COLOR_NONE:
        // Handled above.
        break;
    case COLOR_SRGB:
        color = linearRGBFromSrgb(color);
        if(to == COLOR_LINEAR_RGB) return color;
        // Fall-through
    case COLOR_LINEAR_RGB:
        color = cieXYZFromLinearRGB(color);
        break;
    case COLOR_CIE_XYZ:
        // Do nothing.
        break;
    case COLOR_CIE_LAB:
        color = cieXYZFromCieLab(color);
        break;
    case COLOR_CIE_LUV:
        color = cieXYZFromCieLuv(color);
        break;
    }

    // From XYZ
    switch(to) {
    case COLOR_NONE:
        // Handled above.
        break;
    case COLOR_SRGB:
    case COLOR_LINEAR_RGB:
        color = linearRGBFromCieXYZ(color);
        if(to == COLOR_SRGB) {
            color = srgbFromLinearRGB(color);
        }
        break;
    case COLOR_CIE_XYZ:
        // Do nothing.
        break;
    case COLOR_CIE_LAB:
        color = cieLabFromCieXYZ(color);
        break;
    case COLOR_CIE_LUV:
        color = cieLuvFromCieXYZ(color);
        break;
    }

    return color;
}


}


#endif
