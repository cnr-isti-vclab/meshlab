#ifndef FILTER_ARGS_H
#define FILTER_ARGS_H

#include <common/meshmodel.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/math/random_generator.h>

using namespace vcg;

/* This file contains the wrappers for parameters of the filters
   that are defined in filter_fractal.cpp. */

/* FractalArgs encapsulates arguments for CR_FRACTAL_TERRAIN filter
   and FP_FRACTAL_MESH filter. */
template <class ScalarType>
class FractalArgs
{
public:
    MeshModel* mesh;
    ScalarType octaves, remainder, l, h, offset, gain, seed, heightFactor;
    int subdivisionSteps, algorithmId, smoothingSteps;
    bool saveAsQuality;
    ScalarType spectralWeight[21];
    ScalarType zoom_window_side, zoom_org_x, zoom_org_y;
    ScalarType scale;

    FractalArgs(MeshModel* mm, int algorithmId, ScalarType seed, ScalarType octaves, ScalarType lacunarity,
                ScalarType fractalIncrement, ScalarType offset, ScalarType gain, ScalarType heightFactor)
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
        this->heightFactor = heightFactor;
        zoom_window_side = 1;
        zoom_org_x = 0;
        zoom_org_y = 0;
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

/* wrapper for FP_CRATERS filter arguments */
template <class ScalarType>
class CratersArgs
{
public:
    MeshModel* target_model;
    MeshModel* samples_model;
    CMeshO* target_mesh;
    CMeshO* samples_mesh;
    int algorithm;
    ScalarType max_radius, max_depth, min_radius, min_depth, radius_range, depth_range, profile_factor;
    bool save_as_quality, invert_perturbation, postprocessing_noise;
    FractalArgs<ScalarType>* fArgs;

    CratersArgs(MeshModel* target, MeshModel* samples, int alg, int seed, ScalarType min_r, ScalarType max_r,
                ScalarType min_d, ScalarType max_d, ScalarType p_factor, bool save_as_quality, bool invert,
                bool ppNoise)
    {
        generator = new vcg::math::SubtractiveRingRNG(seed);

        target_model = target;
        samples_model = samples;
        target_mesh = &(target->cm);
        samples_mesh = &(samples->cm);
        vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactVertexVector(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactFaceVector(*target_mesh);

        algorithm = alg;
        this->save_as_quality = save_as_quality;
        invert_perturbation = invert;

        float target_bb_diag = target_mesh->bbox.Diag();
        max_radius = target_bb_diag * 0.1 * max_r;
        min_radius = target_bb_diag * 0.1 * min_r;
        radius_range = max_radius - min_radius;
        max_depth = target_bb_diag * 0.05 * max_d;
        min_depth = target_bb_diag * 0.05 * min_d;
        depth_range = max_depth - min_depth;
        profile_factor = p_factor;

        postprocessing_noise = ppNoise;
        if (postprocessing_noise)
        {
            fArgs = new FractalArgs<ScalarType>(target_model, 0, 1, 8, 2, 0.55, 0, 0, 0.02);
            fArgs->smoothingSteps = 10;
            fArgs->seed = 1;
            fArgs->scale = 1;
        }
    }

    ~CratersArgs(){
        delete generator;

        if(postprocessing_noise)
        {
            delete fArgs;
        }
    }

    /* generates a crater radius within the specified range */
    ScalarType generateRadius()
    {
        ScalarType rnd = generator->generate01closed();
        return min_radius + radius_range * rnd;
    }

    /* generates a crater depth within the specified range */
    ScalarType generateDepth()
    {
        ScalarType rnd = generator->generate01closed();
        return min_depth + depth_range * rnd;
    }

private:
    vcg::math::RandomGenerator* generator;
};


#endif // FILTER_ARGS_H
