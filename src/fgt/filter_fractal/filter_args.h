#ifndef FILTER_ARGS_H
#define FILTER_ARGS_H

#include <common/meshmodel.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/math/random_generator.h>

/* This file contains the wrappers for parameters of the filters
   that are defined in filter_fractal.cpp. */

/* FractalArgs encapsulates arguments for CR_FRACTAL_TERRAIN filter
   and FP_FRACTAL_MESH filter. */
class FractalArgs
{
public:
    float octaves, remainder, l, h, offset, gain, seed, heightFactor;
    int subdivisionSteps, algorithmId, smoothingSteps;
    bool saveAsQuality;
    float spectralWeight[21];
    float zoom_window_side, zoom_org_x, zoom_org_y;

    FractalArgs(){}

    void setFields(int algorithmId, float seed, float octaves, float lacunarity, float fractalIncrement,
                   float offset, float gain, float heightFactor)
    {
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
        float frequency = 1.0;
        for(int i=0; i<=(int)octaves; i++)
        {
            spectralWeight[i] = pow(frequency, -h);
            frequency *= l;
        }
    }
};

/* wrapper for FP_CRATERS filter arguments */
class CratersArgs
{
public:
    MeshModel* target_model;
    MeshModel* samples_model;
    CMeshO* target_mesh;
    CMeshO* samples_mesh;
    int algorithm;
    float max_radius, max_depth, min_radius, min_depth, radius_range, depth_range, profile_factor;
    float resolution;

    CratersArgs(MeshModel* target, MeshModel* samples, int alg, float min_r, float max_r,
                float min_d, float max_d, float p_factor, float res)
    {
        generator = new vcg::math::SubtractiveRingRNG();

        target_model = target;
        samples_model = samples;
        target_mesh = &(target->cm);
        samples_mesh = &(samples->cm);
        vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactVertexVector(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactFaceVector(*target_mesh);

        algorithm = alg;
        float target_bb_diag = target_mesh->bbox.Diag();
        max_radius = target_bb_diag * 0.2 * max_r;
        min_radius = target_bb_diag * 0.2 * min_r;
        radius_range = max_radius - min_radius;
        max_depth = target_bb_diag * 0.2 * max_d;
        min_depth = target_bb_diag * 0.2 * min_d;
        depth_range = max_depth - min_depth;
        profile_factor = p_factor;
        resolution = res;
    }

    ~CratersArgs(){ delete generator; }

    /* generates a crater radius within the specified range */
    float generateRadius()
    {
        float rnd = generator->generate01closed();
        return min_radius + radius_range * rnd;
    }

    /* generates a crater depth within the specified range */
    float generateDepth()
    {
        float rnd = generator->generate01closed();
        return min_depth + depth_range * rnd;
    }

private:
    vcg::math::RandomGenerator* generator;
};


#endif // FILTER_ARGS_H
