#ifndef FILTER_FUNCTORS_H
#define FILTER_FUNCTORS_H

#include <vcg/space/point3.h>
#include <vcg/math/perlin_noise.h>

#define SQRT2 1.42421356
#define MQCONST 1/(1-SQRT2)

using namespace vcg;

/* Functor that exposes a () operator that is supposed to be used
   with normalized domain and codomain. */
template<class ScalarType>
class Normalized3DFunctor
{
public:
    virtual ScalarType operator()(const Point3<ScalarType>& normalizedPoint) = 0;
};

// ---------------------- blending/radial functors ----------------------------------------
/* Radial functors: the result depends only on the distance between point and origin.
   The origin is simply the (0, 0, 0) point. */
template<class ScalarType>
class RadialFunctor: public Normalized3DFunctor<ScalarType>
{
public:
    RadialFunctor(){
        ScalarType zero = ScalarType(0);
        org = new Point3<ScalarType>(zero, zero, zero);
    }

    RadialFunctor(const Point3<ScalarType>& _org)
    {
        org = new Point3<ScalarType>(_org.X(), _org.Y(), _org.Z());
    }

    ~RadialFunctor() { delete org; }

    ScalarType operator()(const Point3<ScalarType>& normalizedPoint)
    {
        ScalarType dist = vcg::Distance(normalizedPoint, *org);
        return (*this)(dist);
    }
    virtual ScalarType operator()(ScalarType dist) = 0;

private:
    Point3<ScalarType>* org;
};

/* Concrete blending functors - codomain in [1, 0].
   These functors can be used as radial basis functions by inverting
   the sign of the result. */

/* Gaussian blending/radial functor */
template<class ScalarType>
class GaussianBlending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        return (ScalarType)(exp(-pow(2*x, 2)));
    }
};

/* Multiquadric blending/radial functor */
template<class ScalarType>
class MultiquadricBlending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        return (ScalarType)((sqrt(1 + pow(x, 2)) - SQRT2) * MQCONST);
    }
};

/* Another kind of simple blending/radial functor */
template<class ScalarType>
class F3Blending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        if(x>=1) return ScalarType(.0);
        return (ScalarType)(fabs(1-pow(x, 4)));
    }
};

/* Exponential blending/radial functor */
template<class ScalarType>
class ExponentialBlending: public RadialFunctor<ScalarType>
{
private:
    ScalarType falloff;

public:
    ExponentialBlending(ScalarType _falloff = 5.0)
    {
        falloff = _falloff;
    }

    ScalarType operator()(ScalarType x)
    {
        if(x >= 1) return ScalarType(.0);
        return (ScalarType)(exp(-falloff*x));
    }
};

/* Linear blending/radial functor */
template<class ScalarType>
class LinearBlending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        if (x>=1) return ScalarType(.0);
        return (ScalarType)(1-x);
    }
};
// --------------------- end of blending/radial functors -------------------------------

// --------------------- noise functors ------------------------------------------------
/* Noise functor: defines the template algorithm to compute
   the fractal displacement values for terrain generation.
   The basic algorithm relies on perlin noise and takes three parameters:
   - octaves: the number of noise frequency that are considered to
     generate the displacement. Each octave has a precise frequency;
   - lacunarity l: this is the parameter that defines the multiplicative gap
     between successive frequencies, starting from 1.
     Example: if we have a lacunarity of 2, and four octaves, then the
     noise frequencies are: 1 -> 2 -> 4 -> 8. If the lacunarity is 3
     we have the following frequencies: 1 -> 3 -> 9 -> 27;
   - fractal increment h: this parameter defines how "heavy" is the noise
     in each frequency. For each frequency, the noise "weight" is
     calculated as pow(frequency, -h). So each frequency has its
     own "spectral weight". By definition, the lower is h, the higher
     is the noise contribution for each frequency.
   In every terrain generation algorithm we have implemented, spectral
   weights can be pre-computed, in order to provide a faster generation process.
   Every single algorithm provides its own methods to:
   - initialize the noise value;
   - update the noise value at each frequency (e.g. fBM algorithm adds the
     noise contributions of each frequency, while standard multifractal
     algorithm multiplies them with each other.)

   Four of the five terrain generation algorithms take other parameters. These algorithms
   are the standard multifractal, heterogeneous multifractal, hybrid multifractal
   and ridged multifractal ones, and the parameters mentioned above are: offset and gain.
   The offset parameter is added to the noise contribution of each frequency in different
   ways in different algorithms. Basically offset emphasizes the noise contribution at
   each frequency, as fractal increment does. The difference between the two is that
   fractal increment defines the weight of each noise frequency, whereas offset
   is multiplied by this quantity. Because offset is not an exponent as fractal increment
   is, we can use offset for a fine tuning of the noise contribution.
   The gain parameter is used only in the ridged multifractal algorithm, and defines
   how rough the generated terrain will be.

   Detailed algorithms description can be found in:
        Ebert, D.S., Musgrave, F.K., Peachey, D., Perlin, K., and Worley, S.
        Texturing and Modeling: A Procedural Approach.
*/
template<class ScalarType>
class NoiseFunctor: public Normalized3DFunctor<ScalarType>
{
public:
    NoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l)
    {
        octaves = (int)_octaves;
        h = _h;
        l = _l;
        remainder = _octaves - (int)octaves;
        precomputeSpectralWeights();    // precomputes spectral weights
    }

    /* This is the noise template algorithm.
       Concrete noise functors have to define two functions:
       - init(x, y, z, noise): initialization of noise given the coordinates of
         the initial point;
       - update(i, x, y, z, noise): updates the noise value given:
         - the current octave i;
         - (displaced) coordinates of the point x, y and z;
         - the current noise value computed so far (noise).
     */
    ScalarType operator()(const Point3<ScalarType>& p)
    {
        ScalarType x = p.X(), y = p.Y(), z = p.Z(), noise = ScalarType(.0);
        init(x, y, z, noise);

        for(int i=0; i<octaves; i++)
        {
            update(i, x, y, z, noise);
            x *= l; y *= l; z *=l;
        }

        if(remainder != ScalarType(0))
        {
            update(octaves, x, y, z, noise);
            noise *= remainder;
        }
        return noise;
    }

    int octaves;                    // number of octaves
    ScalarType h, l;                // fractal increment and lacunarity
    ScalarType spectralWeight[21];  // spectral weights
    ScalarType remainder;           // octaves remainder

protected:
    virtual inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise) = 0;
    virtual inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise) = 0;

private:
    /* precomputes spectral weights to be used in noise algorithm */
    void precomputeSpectralWeights()
    {
        ScalarType frequency = 1.0;
        for(int i=0; i<=octaves; i++)
        {
            spectralWeight[i] = pow(frequency, -h); // determines how "heavy" is the i-th octave
            frequency *= l;     // calculates the next octave frequency
        }
    }
};

/* FBM noise functor */
template<class ScalarType>
class FBMNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    FBMNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l) {}

    inline void init(ScalarType&/*x*/, ScalarType& /*y*/, ScalarType& /*z*/, ScalarType& noise)
    {
        noise = ScalarType(.0);
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        ScalarType perlin = math::Perlin::Noise(x, y, z);
        noise += (perlin * this->spectralWeight[oct]);
    }
};

/* standard multifractal noise functor */
template<class ScalarType>
class StandardMFNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    StandardMFNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l, ScalarType _offset)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l)
    {
        offset = _offset;
    }

    inline void init(ScalarType&/*x*/, ScalarType& /*y*/, ScalarType& /*z*/, ScalarType& noise)
    {
        noise = ScalarType(1.0);
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        ScalarType perlin = math::Perlin::Noise(x, y, z);
        noise *=  (offset + perlin * this->spectralWeight[oct]);
    }

    ScalarType offset;
};

/* heterogeneous multifractal noise functor */
template<class ScalarType>
class HeteroMFNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    HeteroMFNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l, ScalarType _offset)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l)
    {
        offset = _offset;
    }

    inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        ScalarType perlin = math::Perlin::Noise(x, y, z);
        noise = (offset + perlin) * this->spectralWeight[0];
        x *= this->l; y *= this->l; z*= this->l;
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        int nextOct = oct + 1;
        if (nextOct == this->octaves) return;
        ScalarType perlin = math::Perlin::Noise(x, y, z), increment;
        increment = (offset + perlin) * this->spectralWeight[nextOct] * noise;
        noise += increment;
    }

    ScalarType offset;
};

/* hybrid multifractal noise functor */
template<class ScalarType>
class HybridMFNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    HybridMFNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l, ScalarType _offset)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l)
    {
        offset = _offset;
    }

    inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        perlin = math::Perlin::Noise(x, y, z);
        noise = (offset + perlin);
        weight = noise;
        x *= this->l; y *= this->l; z*= this->l;
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        int nextOct = oct+1;
        if (nextOct == this->octaves) return;
        if (weight > 1.0) weight = 1.0;
        perlin = math::Perlin::Noise(x, y, z);
        signal = (offset + perlin) * this->spectralWeight[nextOct];
        noise += (weight * signal);
        weight *= signal;
    }

    ScalarType offset;
    ScalarType weight, signal, perlin;
};

/* ridged multifractal noise functor */
template<class ScalarType>
class RidgedMFNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    RidgedMFNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l, ScalarType _offset, ScalarType _gain)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l)
    {
        offset = _offset;
        gain = _gain;
    }

    inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        perlin = math::Perlin::Noise(x, y, z);
        signal = pow(offset - fabs(perlin), 2);
        noise = signal;
        weight = ScalarType(0);
        x *= this->l; y *= this->l; z*= this->l;
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        int nextOct = oct + 1;
        if(nextOct == this->octaves) return;
        weight = signal * gain;
        if (weight > 1.0) weight = 1.0;
        if (weight < 0.0) weight = 0.0;
        perlin =  math::Perlin::Noise(x, y, z);
        signal = pow(offset - fabs(perlin), 2) * weight * this->spectralWeight[nextOct];
        noise += signal;
    }

    ScalarType offset, gain;
    ScalarType weight, signal, perlin;
};
// ---------------------- end of noise functors -------------------------------------------

// -------------------------- crater functor ----------------------------------------------
template<class ScalarType>
class CraterFunctor: public Normalized3DFunctor<ScalarType>
{
private:
    RadialFunctor<ScalarType>* radialFunctor;   // the radial functor
    RadialFunctor<ScalarType>* blendingFunctor; // the blending functor
    NoiseFunctor<ScalarType>* noiseFunctor;     // the optional noise functor
    Point3<ScalarType>* origin;                 // (0, 0, 0) point
    ScalarType blendingThreshold, blendingRange;// blending threshold and range
    ScalarType elevationFactor;                 // how much elevated is the crater from zero
    ScalarType maxRadial;                       // the radial value at blending threshold
    bool noiseEnabled, invert;                  // noise and invert flags

public:
    CraterFunctor(RadialFunctor<ScalarType>* _radialFunctor,
                  RadialFunctor<ScalarType>* _blendingFunctor,
                  NoiseFunctor<ScalarType>* _noiseFunctor,
                  ScalarType _blendingThreshold,
                  ScalarType _elevationFactor,
                  bool _noiseEnabled,
                  bool _invert)
    {
        radialFunctor = _radialFunctor;
        blendingFunctor = _blendingFunctor;
        noiseFunctor = _noiseFunctor;
        noiseEnabled = _noiseEnabled;
        invert = _invert;
        blendingThreshold = _blendingThreshold;
        blendingRange = 1 - blendingThreshold;  // blending range is [0, 1 - blending threshold]
        elevationFactor = _elevationFactor;
        maxRadial = -(*radialFunctor)(blendingThreshold) + elevationFactor;
        ScalarType zero = ScalarType(0);
        origin = new Point3<ScalarType>(zero, zero, zero);
    }

    ~CraterFunctor(){delete origin;}

    ScalarType operator()(const Point3<ScalarType>& normalizedPoint)
    {
        // calculates the (0, 1) distance from the origin
        ScalarType x = vcg::Distance(normalizedPoint, *origin), result;
        if (x <= blendingThreshold) // we are in crater "depression"
        {   // so we must apply the radial functor and eventually the noise one
            result = -(*radialFunctor)(x) + elevationFactor;
            if (noiseEnabled)
            {
                result += ((*noiseFunctor)(normalizedPoint) * ScalarType(0.15));
            }
        } else
        {   // blending portion
            result = (*blendingFunctor)((x - blendingThreshold)/blendingRange) * maxRadial;
        }
        return (result * (invert?-1:1));
    }
};
// ---------------------- end of crater functor -------------------------------------------

#endif // FILTER_FUNCTORS_H
