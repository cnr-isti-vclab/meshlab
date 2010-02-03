#ifndef FILTER_FUNCTORS_H
#define FILTER_FUNCTORS_H

#include <vcg/space/point3.h>
#include <vcg/math/perlin_noise.h>

#define SQRT2 1.42421356
#define MQCONST 1/(1-SQRT2)

using namespace vcg;

/* functor that exposes a () operator with normalized domain and codomain ([1, 0] ranges) */
template<class ScalarType>
class Normalized3DFunctor
{
public:
    virtual ScalarType operator()(const Point3<ScalarType>& normalizedPoint) = 0;
};

/* radial functors: the result depends only on the distance between point and origin */
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

/* Blending functors - codomain in [1, 0].
   These functors can be used as radial basis functions by inverting the sign of the results. */
template<class ScalarType>
class GaussianBlending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        return (ScalarType)(exp(-pow(2*x, 2)));
    }
};

template<class ScalarType>
class MultiquadricBlending: public RadialFunctor<ScalarType>
{
public:
    ScalarType operator()(ScalarType x)
    {
        return (ScalarType)((sqrt(1 + pow(x, 2)) - SQRT2) * MQCONST);
    }
};

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

/* noise functors */
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
        precomputeSpectralWeights();
    }

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
        for(int i=0; i<=(int)octaves; i++)
        {
            spectralWeight[i] = pow(frequency, -h);
            frequency *= l;
        }
    }
};

template<class ScalarType>
class FBMNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    FBMNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l) {}

    inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        noise = ScalarType(.0);
    }

    inline void update(int oct, ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
    {
        ScalarType perlin = math::Perlin::Noise(x, y, z);
        noise += (perlin * this->spectralWeight[oct]);
    }
};

template<class ScalarType>
class StandardMFNoiseFunctor: public NoiseFunctor<ScalarType>
{
public:
    StandardMFNoiseFunctor(ScalarType _octaves, ScalarType _h, ScalarType _l, ScalarType _offset)
        :NoiseFunctor<ScalarType>(_octaves, _h, _l)
    {
        offset = _offset;
    }

    inline void init(ScalarType&x, ScalarType& y, ScalarType& z, ScalarType& noise)
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


/* crater functor */
template<class ScalarType>
class CraterFunctor: public Normalized3DFunctor<ScalarType>
{
private:
    RadialFunctor<ScalarType>* radialFunctor;
    RadialFunctor<ScalarType>* blendingFunctor;
    NoiseFunctor<ScalarType>* noiseFunctor;
    Point3<ScalarType>* origin;
    ScalarType blendingThreshold, blendingRange;
    ScalarType elevationFactor;
    ScalarType maxRadial;
    bool noiseEnabled, invert;

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
        blendingRange = 1 - blendingThreshold;
        elevationFactor = _elevationFactor;
        maxRadial = -(*radialFunctor)(blendingThreshold) + elevationFactor;
        ScalarType zero = ScalarType(0);
        origin = new Point3<ScalarType>(zero, zero, zero);
    }

    ~CraterFunctor(){delete origin;}

    ScalarType operator()(const Point3<ScalarType>& normalizedPoint)
    {
        ScalarType x = vcg::Distance(normalizedPoint, *origin), result;
        if (x <= blendingThreshold)
        {
            result = -(*radialFunctor)(x) + elevationFactor;
            if (noiseEnabled)
            {
                result += ((*noiseFunctor)(normalizedPoint) * ScalarType(0.15));
            }
        } else
        {
            result = (*blendingFunctor)((x - blendingThreshold)/blendingRange) * maxRadial;
        }
        return (result * (invert?-1:1));
    }
};

#endif // FILTER_FUNCTORS_H
