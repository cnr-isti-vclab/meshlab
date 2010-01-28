#ifndef RADIAL_PERTURBATION_H
#define RADIAL_PERTURBATION_H

#include <vcg/space/point3.h>
#include <algorithm>

#define SQRT2 1.42421356

using namespace vcg;

template<class ScalarType>
class RadialPerturbation
{
public:

    static ScalarType Gaussian(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType tmp = profileFactor * dist;
        return (- depth * exp(- pow(tmp/radius, 2)));
    }

    static ScalarType Multiquadric(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType fact1 = sqrt(pow(radius, 2) + pow(dist, 2)) / radius - SQRT2;
        ScalarType fact2 = depth / (1 - SQRT2);
        return - (fact1 * fact2);
    }

    static ScalarType InverseMultiquadric(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType tmp1 = pow(profileFactor, 2) + 1;
        ScalarType tmp2 = sqrt(tmp1);
        ScalarType fact1 = depth / (tmp2 - 1);
        ScalarType tmp3 = sqrt(pow(profileFactor * dist, 2) + pow(radius, 2));
        ScalarType fact2 = 1 - radius * (tmp2 / tmp3);
        return (fact1 * fact2);
    }

    static ScalarType Cauchy(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType squaredDist = pow(vcg::Distance(p, centre), 2);
        ScalarType squaredRadius = pow(radius, 2);
        ScalarType squaredProfile = pow(profileFactor, 2);
        ScalarType fact1 = depth / squaredProfile;
        ScalarType num = squaredRadius * (1 + squaredProfile);
        ScalarType den = squaredRadius + squaredProfile * squaredDist;
        ScalarType fact2 = 1 - num / den;
        return (fact1 * fact2);
    }

    static ScalarType f(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                        ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType radius4 = pow(radius, 4);
        ScalarType tmp1 = std::min(ScalarType(0), dist-radius);
        ScalarType tmp2 = depth * (1 - (1/radius4) * fabs(radius4 - pow(dist, 4))) - depth/2;
        return (std::max(tmp1, tmp2));
    }
};

#endif // RADIAL_PERTURBATION_H
