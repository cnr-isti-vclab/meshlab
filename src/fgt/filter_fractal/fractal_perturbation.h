#ifndef FRACTAL_PERTURBATION_H
#define FRACTAL_PERTURBATION_H

#include <vcg/math/base.h>
#include <vcg/math/perlin_noise.h>
#include <common/meshmodel.h>
#include <vcg/complex/trimesh/smooth.h>
#include "filter_functors.h"

using namespace vcg;

template <class ScalarType>
        class FractalArgs
{
public:
    MeshModel* mesh;
    ScalarType seed, maxHeight, scale;
    int subdivisionSteps, smoothingSteps;
    bool saveAsQuality, displaceSelected;
    NoiseFunctor<ScalarType>* noiseFunctor;

    FractalArgs(MeshModel* mm, int algorithmId, ScalarType seed, ScalarType octaves,
                ScalarType lacunarity, ScalarType fractalIncrement, ScalarType offset,
                ScalarType gain, ScalarType maxHeight, ScalarType scale,
                int smoothingSteps, bool saveAsQuality)
    {
        mesh = mm;
        this->seed = seed;
        this->maxHeight = maxHeight;
        this->scale = scale;
        displaceSelected = false;
        this->smoothingSteps = smoothingSteps;
        this->saveAsQuality = saveAsQuality;

        switch(algorithmId)
        {
        case 0: //fBM
            noiseFunctor = new FBMNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity);
            break;
        case 1: //standard multifractal
            noiseFunctor = new StandardMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
            break;
        case 2: //heterogeneous multifractal
            noiseFunctor = new HeteroMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
            break;
        case 3: //hybrid multifractal
            noiseFunctor = new HybridMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
            break;
        case 4: //ridged multifractal
            noiseFunctor = new RidgedMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset, gain);
            break;
        }
    }

    ~FractalArgs() { delete noiseFunctor; }
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
            perturbation = (*args.noiseFunctor)(p);
            tmpPair = new PertPair(&(*vi), perturbation);
            pertVector.push_back(*tmpPair);

            if (perturbation < min) min = perturbation;
            if (perturbation > max) max = perturbation;
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
                tmpPair->first->Q() += tmpPair->second;
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
};

#endif // FRACTAL_PERTURBATION_H
