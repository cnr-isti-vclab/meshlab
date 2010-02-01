#ifndef FRACTAL_PERTURBATION_H
#define FRACTAL_PERTURBATION_H

#include <vcg/math/base.h>
#include <vcg/math/perlin_noise.h>
#include <common/meshmodel.h>
#include <vcg/complex/trimesh/smooth.h>

using namespace vcg;

template <class ScalarType>
        class FractalPerturbation;

template <class ScalarType>
        class FractalArgs
{
public:
    MeshModel* mesh;
    ScalarType octaves, remainder, l, h, offset, gain, seed, maxHeight;
    int subdivisionSteps, algorithmId, smoothingSteps;
    bool saveAsQuality, displaceSelected;
    ScalarType spectralWeight[21];
    ScalarType scale;

    FractalArgs(MeshModel* mm, int algorithmId, ScalarType seed, ScalarType octaves,
                ScalarType lacunarity, ScalarType fractalIncrement, ScalarType offset,
                ScalarType gain, ScalarType maxHeight, ScalarType scale,
                int smoothingSteps, bool saveAsQuality)
    {
        mesh = mm;
        this->algorithmId = algorithmId;
        this->seed = seed;
        this->octaves = octaves;
        this->remainder = octaves - (int)octaves;
        l = lacunarity;
        h = fractalIncrement;
        this->offset = offset;
        this->gain = gain;
        this->maxHeight = maxHeight;
        this->scale = scale;
        displaceSelected = false;
        this->smoothingSteps = smoothingSteps;
        this->saveAsQuality = saveAsQuality;
        updateSpectralWeights();
    }

    void updateSpectralWeights()
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
class FractalPerturbation
{
public:

    /* This function calculates a fractal perturbation and stores it in a given
       vector. The perturbation is calculated according to the given parameters
       (second argument), on the target mesh (first argument). The result is
       a vector of pairs (vertex_pointer, perturbation) (third parameter).
       Assumption: if the perturbation has to be saved as vertex quality, the
       quality flags must be enabled. */
    template <class MeshType>
            static bool computeFractalPerturbation(
                    MeshType& m,                        // target mesh
                    FractalArgs<ScalarType>& args,      // fractal displacement arguments
                    vcg::CallBackPos* cb)               // progress bar handling
    {
        if(args.saveAsQuality && !tri::HasPerVertexQuality(m)) return false;

        // some typedefs
        typedef typename MeshType::VertexIterator           VertexIterator;
        typedef typename MeshType::VertexPointer            VertexPointer;
        typedef std::pair<VertexPointer, ScalarType>        PertPair;
        typedef typename std::vector<PertPair>::iterator    PertIterator;

        // prepares the mesh for the fractal displacement
        tri::UpdateNormals<MeshType>::PerVertexNormalizedPerFaceNormalized(m);
        if(args.smoothingSteps > 0)
        {
            cb(0, "Smoothing vertex normals..");
            tri::Smooth<MeshType>::VertexNormalLaplacian(m, args.smoothingSteps, false);
        }

        // some variables to manage the progress bar
        char buffer[50];
        int vCount = m.vert.size(), i=0;

        // other variables for scaling and normalization of points
        std::vector<PertPair> pertVector;
        ScalarType factor = args.scale/m.bbox.Diag(), min = 1000.0, max = -1000.0;
        ScalarType seedTraslation = args.seed/factor, perturbation = .0;
        Point3<ScalarType> seedPoint(seedTraslation, seedTraslation, seedTraslation);
        Point3<ScalarType> center = m.bbox.Center(), p;
        Point3<ScalarType> trasl = seedPoint - center;
        PertPair* tmpPair;

        // first loop: calculates the perturbation and create the vector pairs
        for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi, ++i)
        {
            sprintf(buffer, "Calculating perturbation on vertex %d..", i);
            cb(100*i/vCount, buffer);

            if (!(*vi).IsS() && args.displaceSelected) continue;
            p = ((*vi).P() + trasl) * factor;   // scales and normalizes the point

            switch(args.algorithmId)
            {
            case 0: //fBM
                perturbation = FractalPerturbation<ScalarType>::fBM(p, args);
                break;
            case 1: //standard multifractal
                perturbation = FractalPerturbation<ScalarType>::StandardMF(p, args);
                break;
            case 2: //heterogeneous multifractal
                perturbation = FractalPerturbation<ScalarType>::HeteroMF(p, args);
                break;
            case 3: //hybrid multifractal
                perturbation = FractalPerturbation<ScalarType>::HybridMF(p, args);
                break;
            case 4: //ridged multifractal
                perturbation = FractalPerturbation<ScalarType>::RidgedMF(p, args);
                break;
            }

            if (perturbation < min) min = perturbation;
            if (perturbation > max) max = perturbation;

            tmpPair = new PertPair(&(*vi), perturbation);
            pertVector.push_back(*tmpPair);
        }

        // defines the effective range and the target range of the perturbation
        ScalarType hmax = args.maxHeight, hmin = (min * hmax) / max;
        ScalarType range1 = max - min, range2 = hmax - hmin;

        i = 0;
        int pairsCount = pertVector.size();

        // second loop: normalizes and applies the perturbation
        for(PertIterator pi=pertVector.begin(); pi!=pertVector.end(); ++pi, ++i)
        {
            sprintf(buffer, "Normalizing perturbation on vertex %d..", i);
            cb(100*i/pairsCount, buffer);

            tmpPair = &(*pi);
            tmpPair->second = (((tmpPair->second - min)/range1) * range2) + hmin;

            if(args.saveAsQuality)
            {
                tmpPair->first->Q() = tmpPair->second;
            } else {
                tmpPair->first->P() += (tmpPair->first->N() * tmpPair->second);
            }
        }

        // if necessary, updates bounding box and normals
        if(!args.saveAsQuality)
        {
            tri::UpdateBounding<MeshType>::Box(m);
            tri::UpdateNormals<MeshType>::PerVertexNormalizedPerFaceNormalized(m);
        }
        return true;
    }

private:
    static ScalarType fBM(Point3<ScalarType> &point, FractalArgs<ScalarType> &args, bool maxPerturbation = false)
    {
        ScalarType noise = .0, x = point[0], y = point[1], z = point[2];
        ScalarType tmpNoise = .0;

        for(int i=0; i<(int)args.octaves; i++)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            noise += (tmpNoise * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            noise += (args.remainder * tmpNoise * args.spectralWeight[(int)args.octaves]);
        }
        return noise;
    }

    static ScalarType StandardMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args, bool maxPerturbation = false)
    {
        ScalarType noise = 1.0, x = point[0], y = point[1], z = point[2];
        ScalarType tmpNoise = .0;

        for(int i=0; i<(int)args.octaves; i++)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            noise *= (args.offset + tmpNoise * args.spectralWeight[i]);
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            noise *= (args.remainder * (tmpNoise * args.spectralWeight[(int)args.octaves] + args.offset));
        }
        return noise;
    }

    static ScalarType HeteroMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args, bool maxPerturbation = false)
    {
        ScalarType noise = .0, x = point[0], y = point[1], z = point[2];
        ScalarType increment = .0, tmpNoise = .0;
        tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
        noise = (args.offset + tmpNoise) * args.spectralWeight[0];
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            increment = (args.offset + tmpNoise) * args.spectralWeight[i] * noise;
            noise += increment;
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            increment = (tmpNoise + args.offset) * args.spectralWeight[(int)args.octaves] * noise;
            increment *= args.remainder;
            noise += increment;
        }
        return noise;
    }

    static ScalarType HybridMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args, bool maxPerturbation = false)
    {
        ScalarType x = point[0], y = point[1], z = point[2];
        ScalarType tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
        ScalarType noise = (args.offset + tmpNoise);
        ScalarType weight = noise, signal = .0;
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            if (weight > 1.0) weight = 1.0;
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            signal = (args.offset + tmpNoise) * args.spectralWeight[i];
            noise += (weight * signal);
            weight *= signal;
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            tmpNoise = (maxPerturbation? 1 : math::Perlin::Noise(x, y, z));
            signal = (args.offset + tmpNoise) * args.spectralWeight[(int)args.octaves];
            noise += (weight * signal * args.remainder);
        }
        return noise;
    }

    static ScalarType RidgedMF(Point3<ScalarType> &point, FractalArgs<ScalarType> &args, bool maxPerturbation = false)
    {
        ScalarType x = point[0], y = point[1], z = point[2];
        ScalarType tmpNoise = (maxPerturbation? .0 : math::Perlin::Noise(x, y, z));
        ScalarType signal = pow(args.offset - fabs(tmpNoise), 2);
        ScalarType noise = signal, weight = .0;
        x *= args.l; y *= args.l; z *= args.l;

        for(int i=1; i<(int)args.octaves; i++)
        {
            weight = signal * args.gain;
            if (weight > 1.0) weight = 1.0;
            if (weight < 0.0) weight = 0.0;
            tmpNoise = (maxPerturbation? .0 : math::Perlin::Noise(x, y, z));
            signal = pow(args.offset - fabs(tmpNoise), 2) * weight * args.spectralWeight[i];
            noise += signal;
            x *= args.l; y *= args.l; z *= args.l;
        }

        if(args.remainder != .0)
        {
            weight = signal * args.gain;
            if (weight > 1.0) weight = 1.0;
            if (weight < 0.0) weight = 0.0;
            tmpNoise = (maxPerturbation? .0 : math::Perlin::Noise(x, y, z));
            signal = pow(args.offset - fabs(tmpNoise), 2) * weight * args.spectralWeight[(int)args.octaves];
            noise += (signal *args.remainder);
        }

        return noise;
    }
};

#endif // FRACTAL_PERTURBATION_H
