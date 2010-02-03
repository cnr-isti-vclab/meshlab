#ifndef CRATERS_UTILS_H
#define CRATERS_UTILS_H

#include <vcg/space/point3.h>
#include <vcg/space/line3.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/sphere3.h>
#include <vcg/math/base.h>
#include <vcg/space/box3.h>
#include <vcg/space/index/grid_static_ptr.h>
#include "fractal_perturbation.h"
#include "filter_functors.h"


/* wrapper for FP_CRATERS filter arguments */
template <class ScalarType>
        class CratersArgs
{
public:
    RadialFunctor<ScalarType>* radialFunctor;
    RadialFunctor<ScalarType>* blendingFunctor;
    NoiseFunctor<ScalarType>* noiseFunctor;
    CraterFunctor<ScalarType>* craterFunctor;
    MeshModel* target_model;
    MeshModel* samples_model;
    CMeshO* target_mesh;
    CMeshO* samples_mesh;
    int smoothingSteps;
    ScalarType max_radius, max_depth, min_radius, min_depth, radius_range, depth_range;
    bool save_as_quality, postprocessing_noise, successiveImpacts;
    FractalArgs<ScalarType>* fArgs;

    CratersArgs(MeshModel* target, MeshModel* samples, int radial_alg, int seed,
                ScalarType min_r, ScalarType max_r, ScalarType min_d, ScalarType max_d,
                int smoothingSteps, bool save_as_quality, bool invert, bool ppNoise, bool successiveImpacts,
                ScalarType elevation, int blending_alg, ScalarType blendingThreshold)
    {
        generator = new vcg::math::SubtractiveRingRNG(seed);

        switch(radial_alg)
        {
        case 0:
            radialFunctor = new GaussianBlending<ScalarType>();
            break;
        case 1:
            radialFunctor = new MultiquadricBlending<ScalarType>();
            break;
        case 2:
            radialFunctor = new F3Blending<ScalarType>();
            break;
        }

        switch(blending_alg)
        {
        case 0:
            blendingFunctor = new ExponentialBlending<ScalarType>();
            break;
        case 1:
            blendingFunctor = new LinearBlending<ScalarType>();
            break;
        case 2:
            blendingFunctor = new GaussianBlending<ScalarType>();
            break;
        case 3:
            blendingFunctor = new F3Blending<ScalarType>();
            break;
        }

        postprocessing_noise = ppNoise;
        if (ppNoise)
        {
            noiseFunctor = new FBMNoiseFunctor<ScalarType>(8, 0.7, 2);
        }
        craterFunctor = new CraterFunctor<ScalarType>(radialFunctor, blendingFunctor,
            noiseFunctor, blendingThreshold, elevation, ppNoise, invert);

        target_model = target;
        samples_model = samples;
        target_mesh = &(target->cm);
        samples_mesh = &(samples->cm);
        vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactVertexVector(*target_mesh);
        vcg::tri::Allocator<CMeshO>::CompactFaceVector(*target_mesh);

        this->save_as_quality = save_as_quality;
        this->successiveImpacts = successiveImpacts;
        this->smoothingSteps = smoothingSteps;

        float target_bb_diag = target_mesh->bbox.Diag();
        max_radius = target_bb_diag * 0.25 * max_r;
        min_radius = target_bb_diag * 0.25 * min_r;
        radius_range = max_radius - min_radius;
        max_depth = target_bb_diag * 0.25 * max_d;
        min_depth = target_bb_diag * 0.25 * min_d;
        depth_range = max_depth - min_depth;
    }

    ~CratersArgs(){
        delete radialFunctor;
        delete blendingFunctor;

        if(postprocessing_noise)
            delete noiseFunctor;
        delete craterFunctor;
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


template<class MeshType>
class CratersUtils
{
public:
    typedef typename MeshType::FacePointer          FacePointer;
    typedef typename MeshType::FaceIterator         FaceIterator;
    typedef typename MeshType::FaceType             FaceType;
    typedef typename MeshType::VertexPointer        VertexPointer;
    typedef typename MeshType::VertexIterator       VertexIterator;
    typedef std::pair<VertexPointer, FacePointer>   SampleFace;
    typedef std::vector<SampleFace>                 SampleFaceVector;
    typedef typename MeshType::ScalarType           ScalarType;
    typedef GridStaticPtr<FaceType, ScalarType>     MetroMeshGrid;
    typedef tri::FaceTmark<MeshType>                MarkerFace;
    typedef typename MeshType::template PerVertexAttributeHandle<ScalarType> PertHandle;

    /* Finds the nearest faces of samples and stores the pairs (sample, nearest_face)
       in the "sfv" vector (third parameter). */
    static void FindSamplesFaces(MeshType *target, MeshType *samples, SampleFaceVector &sfv)
    {
        tri::UpdateNormals<CMeshO>::PerFaceNormalized(*target);
        tri::UpdateFlags<CMeshO>::FaceProjection(*target);

        MetroMeshGrid mmg;
        mmg.Set(target->face.begin(), target->face.end());
        MarkerFace markerFunctor;
        markerFunctor.SetMesh(target);
        vcg::face::PointDistanceBaseFunctor<ScalarType> PDistFunct;
        FacePointer nearestFace;
        ScalarType dist_upper_bound = target->bbox.Diag()/10, dist;
        Point3<ScalarType> closest;
        sfv.clear();
        SampleFace* tmpPair;
        int i=0;

        for(VertexIterator vi=samples->vert.begin(); vi!=samples->vert.end(); ++vi, ++i)
        {
            nearestFace = mmg.GetClosest(PDistFunct, markerFunctor, (*vi).P(), dist_upper_bound, dist, closest);
            tmpPair = new SampleFace(&(*vi), nearestFace);
            sfv.push_back(*tmpPair);
        }
    }

    /* Detectes the faces of a crater starting from a given face. */
    static void GetCraterFaces(MeshType *m,              // target mesh
                               FacePointer startingFace, // face under the crater centre
                               VertexPointer centre,     // crater centre
                               ScalarType radius,    // crater radius
                               std::vector<FacePointer> &toFill)
    {
        assert(vcg::tri::HasFFAdjacency(*m));
        vcg::tri::UpdateFlags<MeshType>::FaceClearV(*m);
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*m);

        vcg::Sphere3<ScalarType> craterSphere(centre->P(), radius); // crater sphere
        std::vector<FacePointer> fl;
        fl.push_back(startingFace);

        toFill.clear();
        FacePointer f;
        Point3<ScalarType> dummyPoint;
        std::pair<ScalarType, ScalarType> dummyPair;

        while(!fl.empty())
        {
            f = fl.back();
            fl.pop_back();

            if(!f->IsV())
            {
                f->SetV();
                if(vcg::IntersectionSphereTriangle<ScalarType, FaceType>
                   (craterSphere, *f, dummyPoint, &dummyPair))
                {   // intersection test succedeed
                    toFill.push_back(f);
                    for(int i=0; i<3; i++)
                    {
                        if(!f->FFp(i)->IsV())
                        {
                            fl.push_back(f->FFp(i));
                        }
                    }
                }
            }
        }
    }

    static void GetRadialPerturbation(CratersArgs<ScalarType> &args, VertexPointer centre,
                                      std::vector<FacePointer> &craterFaces, ScalarType radius,
                                      ScalarType depth, PertHandle& pertHandle)
    {
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*(args.target_mesh));

        typename std::vector<FacePointer>::iterator fi;
        VertexPointer vp;
        ScalarType perturbation = .0;
        Point3<ScalarType> p;

        for(fi = craterFaces.begin(); fi!=craterFaces.end(); ++fi)
        {
            for(int i=0; i<3; i++)
            {
                vp = (*fi)->V(i);
                if(!vp->IsV())
                {
                    vp->SetV();
                    p = (vp->P() - centre->P())/radius;
                    perturbation = (*(args.craterFunctor))(p) * depth;

                    // stores the perturbation in the passed handle, according to
                    // the successiveImpacts flag
                    if(args.successiveImpacts)
                    {
                        if(perturbation < 0)
                        {
                            pertHandle[vp] = std::min(perturbation, pertHandle[vp]);
                        } else
                        {
                            if(pertHandle[vp] == .0)
                            {
                                pertHandle[vp] += perturbation;
                            }
                        }
                    } else {
                        pertHandle[vp] += perturbation;
                    }
                }
            }
        }
    }
};

#endif // CRATERS_UTILS_H
