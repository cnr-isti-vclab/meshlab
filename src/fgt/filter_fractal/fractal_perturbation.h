#ifndef FRACTAL_PERTURBATION_H
#define FRACTAL_PERTURBATION_H

#include <vcg/math/base.h>
#include <vcg/math/perlin_noise.h>

#include "filter_args.h"

using namespace vcg;

template<class ScalarType>
class FractalPerturbation
{
public:

    static ScalarType computeFractalPerturbation(FractalArgs<ScalarType> &args, Point3<ScalarType> &point)
    {
        ScalarType perturbation = .0;
        switch(args.algorithmId)
        {
        case 0: //fBM
            perturbation = FractalPerturbation<ScalarType>::fBM(point, args);
            break;
        case 1: //standard multifractal
            perturbation = FractalPerturbation<ScalarType>::StandardMF(point, args);
            break;
        case 2: //heterogeneous multifractal
            perturbation = FractalPerturbation<ScalarType>::HeteroMF(point, args);
            break;
        case 3: //hybrid multifractal
            perturbation = FractalPerturbation<ScalarType>::HybridMF(point, args);
            break;
        case 4: //ridged multifractal
            perturbation = FractalPerturbation<ScalarType>::RidgedMF(point, args);
            break;
        }

        return perturbation;
    }

    static ScalarType fBM(Point3<ScalarType> &point, FractalArgs<ScalarType> &args)
    {
        ScalarType noise = .0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;

        for(int i=0; i<(int)args.octaves; i++)
        {
            noise += (math::Perlin::Noise(x, y, z) * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
            noise += (args.remainder * math::Perlin::Noise(x, y, z) * args.spectralWeight[(int)args.octaves]);

        return noise * args.heightFactor;
    }

    static ScalarType StandardMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args)
    {
        ScalarType noise = 1.0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;

        for(int i=0; i<(int)args.octaves; i++)
        {
            noise *= (args.offset + math::Perlin::Noise(x, y, z) * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
            noise *= (args.remainder * (math::Perlin::Noise(x, y, z) *
                                        args.spectralWeight[(int)args.octaves] + args.offset));

        return noise * args.heightFactor;
    }

    static ScalarType HeteroMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args)
    {
        ScalarType noise = .0, x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;
        ScalarType increment = .0;
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
        return noise * args.heightFactor;
    }

    static ScalarType HybridMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args)
    {
        ScalarType x = point[0]+args.seed, y = point[1]+args.seed, z = point[2]+args.seed;
        ScalarType noise = (args.offset + math::Perlin::Noise(x, y, z));
        ScalarType weight = noise, signal = .0;
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
        return noise * args.heightFactor;
    }

    static ScalarType RidgedMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args)
    {
        ScalarType x = point[0] + args.seed, y = point[1] + args.seed, z = point[2] + args.seed;
        ScalarType signal = pow(args.offset - fabs(math::Perlin::Noise(x, y, z)), 2);
        ScalarType noise = signal, weight = .0;
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
        return noise * args.heightFactor;
    }
};

#endif // FRACTAL_PERTURBATION_H
