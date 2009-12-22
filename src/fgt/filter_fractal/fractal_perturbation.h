#ifndef FRACTAL_PERTURBATION_H
#define FRACTAL_PERTURBATION_H

#include <vcg/math/base.h>
#include <vcg/math/perlin_noise.h>

using namespace vcg;

class FractalArgs
{
public:
    float octaves, remainder, l, h, offset, gain, seed, scale;
    int subdivisionSteps, algorithmId, smoothingSteps;
    bool saveAsQuality;
    float spectralWeight[21];

    FractalArgs(){}

    void setFields(int algorithmId, float seed, float octaves, float lacunarity, float fractalIncrement,
                   float offset, float gain, float scale)
    {
        this->algorithmId = algorithmId;
        this->seed = seed;
        this->octaves = octaves;
        this->remainder = octaves - (int)octaves;
        l = lacunarity;
        h = fractalIncrement;
        this->offset = offset;
        this->gain = gain;
        this->scale = scale;
        updateSpectralWeights();
    }

    void updateSpectralWeights()
    {
        float frequency = 1.0;
        for(int i=0; i<=(int)octaves; i++)
        {
            spectralWeight[i] = pow(frequency, -h);
            frequency *= l;
        }
    }
};

template <class MeshType>
        class FractalPerturbation
{
public:
    typedef typename MeshType::CoordType CoordType;

    static double fBM(CoordType &point, FractalArgs &args)
    {
        double noise = .0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;

        for(int i=0; i<(int)args.octaves; i++)
        {
            noise += (math::Perlin::Noise(x, y, z) * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
            noise += (args.remainder * math::Perlin::Noise(x, y, z) * args.spectralWeight[(int)args.octaves]);

        return noise;
    }

    static double StandardMF(CoordType &point, FractalArgs &args)
    {
        double noise = 1.0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;

        for(int i=0; i<(int)args.octaves; i++)
        {
            noise *= (args.offset + math::Perlin::Noise(x, y, z) * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
            noise *= (args.remainder * (math::Perlin::Noise(x, y, z) *
                                        args.spectralWeight[(int)args.octaves] + args.offset));

        return noise;
    }

    static double HeteroMF(CoordType &point, FractalArgs &args)
    {
        double noise = .0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;
        double increment = .0;
        noise = (args.offset + math::Perlin::Noise(x, y, z)) * args.spectralWeight[0];
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            increment = (args.offset + math::Perlin::Noise(x, y, z)) * args.spectralWeight[i] * noise;
            noise += increment;
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            increment = (math::Perlin::Noise(x, y, z) + args.offset) * args.spectralWeight[(int)args.octaves] * noise;
            increment *= args.remainder;
            noise += increment;
        }
        return noise;
    }

    static double HybridMF(CoordType &point, FractalArgs &args)
    {
        double x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;
        double noise = (args.offset + math::Perlin::Noise(x, y, z));
        double weight = noise, signal = .0;
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            if (weight > 1.0) weight = 1.0;
            signal = (args.offset + math::Perlin::Noise(x, y, z)) * args.spectralWeight[i];
            noise += (weight * signal);
            weight *= signal;
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            signal = (args.offset + math::Perlin::Noise(x, y, z)) * args.spectralWeight[(int)args.octaves];
            noise += (weight * signal * args.remainder);
        }
        return noise;
    }

    static double RidgedMF(CoordType &point, FractalArgs &args)
    {
        double x = point[0], y = point[1], z = point[2];
        double signal = pow(args.offset - fabs(math::Perlin::Noise(x, y, z)), 2);
        double noise = signal, weight = .0;
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            weight = signal * args.gain;
            if (weight > 1.0) weight = 1.0;
            if (weight < 0.0) weight = 0.0;
            signal = pow(args.offset - fabs(math::Perlin::Noise(x, y, z)), 2) * weight * args.spectralWeight[i];
            noise += signal;
            x *= args.l; y *= args.l; z *= args.l;
        }
        return noise;
    }
};

#endif // FRACTAL_PERTURBATION_H
