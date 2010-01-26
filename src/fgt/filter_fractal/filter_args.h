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

/* This class is used as cache for the FRACTAL_MESH filter */
template <class ScalarType>
class FractalMeshCache
{
public:
    QString meshName;
    ScalarType meshSeed;
    unsigned int vertNo;
    std::vector< Point3<ScalarType> >* projectedPoints;

    FractalMeshCache()
    {
        projectedPoints = new std::vector< Point3<ScalarType> >();
        meshName = "";
        meshSeed = -1.0;
        vertNo = -1;
    }

    ~FractalMeshCache()
    {
        projectedPoints ->clear();
        delete projectedPoints;
    }

    void updateCache(FractalArgs<ScalarType> &args)
    {
        std::string str = args.mesh->fileName;
        QString s(str.c_str());
        bool nameMatches = (s == meshName);
        bool vertMatches = (args.mesh->cm.vert.size() == vertNo);
        if(nameMatches && meshSeed == args.seed && vertMatches) return; // we can use the cache content

        projectedPoints->clear();

        // fills the cache
        Box3<ScalarType> bbox = args.mesh->cm.bbox;
        ScalarType minDim = bbox.DimX();
        if (bbox.DimY() < minDim) minDim = bbox.DimY();
        if (bbox.DimZ() < minDim) minDim = bbox.DimZ();
        Point3<ScalarType> center = bbox.Center(), projection;
        Sphere3<ScalarType> bSphere(center, minDim/100);
        Line3<ScalarType> ray;
        ray.SetOrigin(center);

        CMeshO* cm = &(args.mesh->cm);
        CMeshO::VertexIterator vi;
        for(vi = cm->vert.begin(); vi!=cm->vert.end(); ++vi)
        {
            ray.SetDirection((*vi).P() - center);
            IntersectionLineSphere<ScalarType>(bSphere, ray, projection, projection);
            projectedPoints->push_back(projection);
        }

        // stores informations about the cache content
        meshSeed = args.seed;
        meshName = s;
        vertNo = args.mesh->cm.vert.size();
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
        postprocessing_noise = ppNoise;

        float target_bb_diag = target_mesh->bbox.Diag();
        max_radius = target_bb_diag * 0.1 * max_r;
        min_radius = target_bb_diag * 0.1 * min_r;
        radius_range = max_radius - min_radius;
        max_depth = target_bb_diag * 0.05 * max_d;
        min_depth = target_bb_diag * 0.05 * min_d;
        depth_range = max_depth - min_depth;
        profile_factor = p_factor;
    }

    ~CratersArgs(){
        delete generator;
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
