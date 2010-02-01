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

#define SQRT2 1.42421356

using namespace vcg;

template<class ScalarType>
class RadialPerturbation
{
public:

    static ScalarType Gaussian(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType tmp = profileFactor * dist;
        return (- depth * exp(- pow(tmp/radius, 2)));
    }

    static ScalarType Multiquadric(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                               ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType fact1 = sqrt(pow(radius, 2) + pow(dist, 2)) / radius - SQRT2;
        ScalarType fact2 = depth / (1 - SQRT2);
        return - (fact1 * fact2);
    }

    static ScalarType f(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                        ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        ScalarType dist = vcg::Distance(p, centre);
        ScalarType radius4 = pow(radius, 4);
        ScalarType tmp1 = std::min(ScalarType(0), dist-radius);
        ScalarType tmp2 = depth * (1 - (1/radius4) * fabs(radius4 - pow(dist, 4))) - depth/2;
        return (std::max(tmp1, tmp2));
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
            ScalarType heightFactor = target_mesh->bbox.Diag() * 0.02;
            fArgs = new FractalArgs<ScalarType>(target_model, 0, 1, 8, 2, 0.55, 0, 0, heightFactor, 1, 1, false);
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
    typedef typename MeshType::ScalarType           MeshScalarType;
    typedef GridStaticPtr<FaceType, MeshScalarType> MetroMeshGrid;
    typedef tri::FaceTmark<MeshType>                MarkerFace;

    /* Finds the nearest faces of samples and stores the pairs (sample, nearest_face)
       in the "sfv" vector (third parameter). */
    template<class ScalarType>
    static void FindSamplesFaces(MeshType *target, MeshType *samples, SampleFaceVector &sfv)
    {
        tri::UpdateNormals<CMeshO>::PerFaceNormalized(*target);
        tri::UpdateFlags<CMeshO>::FaceProjection(*target);

        MetroMeshGrid mmg;
        mmg.Set(target->face.begin(), target->face.end());
        MarkerFace markerFunctor;
        markerFunctor.SetMesh(target);
        vcg::face::PointDistanceBaseFunctor<MeshScalarType> PDistFunct;
        FacePointer nearestFace;
        MeshScalarType dist_upper_bound = target->bbox.Diag()/10, dist;
        Point3<MeshScalarType> closest;
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
    template<class CoordScalarType>
    static void GetCraterFaces(MeshType *m,              // target mesh
                               FacePointer startingFace, // face under the crater centre
                               VertexPointer centre,     // crater centre
                               CoordScalarType radius,    // crater radius
                               std::vector<FacePointer> &toFill)
    {
        assert(vcg::tri::HasFFAdjacency(*m));
        vcg::tri::UpdateFlags<MeshType>::FaceClearV(*m);
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*m);

        vcg::Sphere3<CoordScalarType> craterSphere(centre->P(), radius); // crater sphere
        std::vector<FacePointer> fl;
        fl.push_back(startingFace);

        toFill.clear();
        FacePointer f;
        Point3<CoordScalarType> dummyPoint;
        std::pair<CoordScalarType, CoordScalarType> dummyPair;

        while(!fl.empty())
        {
            f = fl.back();
            fl.pop_back();

            if(!f->IsV())
            {
                f->SetV();
                if(vcg::IntersectionSphereTriangle<CoordScalarType, FaceType>
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

    template<class ScalarType>
    static void applyRadialPerturbation(CratersArgs<ScalarType> &args, VertexPointer centre,
                                        std::vector<FacePointer> &craterFaces, ScalarType radius,
                                        ScalarType depth)
    {
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*(args.target_mesh));
        if(args.save_as_quality)
        {
            args.target_model->updateDataMask(MeshModel::MM_VERTQUALITY);
        }

        typename std::vector<FacePointer>::iterator fi;
        VertexPointer vp;
        ScalarType perturbation = .0;
        Point3<ScalarType> p;
        Point3<ScalarType> center = args.target_mesh->bbox.Center();
        ScalarType diag = args.target_mesh->bbox.Diag();

        for(fi = craterFaces.begin(); fi!=craterFaces.end(); ++fi)
        {
            for(int i=0; i<3; i++)
            {
                vp = (*fi)->V(i);
                if(!vp->IsV())
                {
                    vp->SetV();
                    p = vp->P();

                    // applies the radial perturbation
                    switch(args.algorithm)
                    {
                    case 0: // gaussian rbf
                        perturbation = RadialPerturbation<ScalarType>::Gaussian
                                       (vp->P(), centre->P(), radius, depth, args.profile_factor);
                        break;
                    case 1: // multiquadric rbf
                        perturbation = RadialPerturbation<ScalarType>::Multiquadric
                                       (vp->P(), centre->P(), radius, depth, args.profile_factor);
                        break;
                    case 2: // inverse multiquadric rbf
                        perturbation = RadialPerturbation<ScalarType>::f
                                       (vp->P(), centre->P(), radius, depth, args.profile_factor);
                    }

                    // applies the post-processing noise to the temporary point
                    // to obtain the fractal perturbation
                    /*
                    if (args.postprocessing_noise)
                    {
                        p = (p-center)/diag;
                        perturbation += (FractalPerturbation<ScalarType>::
                                         computeFractalPerturbation(*(args.fArgs), p) * args.fArgs->maxHeight);
                    }
                    */

                    // if necessary, inverts the perturbation
                    if(args.invert_perturbation)
                    {
                        perturbation = -perturbation;
                    }

                    // applies the perturbation to the current vertex or stores it
                    // as vertex quality
                    if(args.save_as_quality)
                    {
                        vp->Q() += perturbation;
                    } else {
                        vp->P() += (centre->N() * perturbation);
                    }
                }
            }
        }
    }
};



#endif // CRATERS_UTILS_H
