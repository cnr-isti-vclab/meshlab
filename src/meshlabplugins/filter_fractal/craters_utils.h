#ifndef CRATERS_UTILS_H
#define CRATERS_UTILS_H

#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/index/grid_static_ptr.h>
#include "fractal_utils.h"

/* some utilities needed by the crater generation filter */
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

    /* wrapper for FP_CRATERS filter arguments */
    class CratersArgs
    {
    public:
        RadialFunctor<ScalarType>* radialFunctor;   // The radial functor that is defined according to
                                                    // the chosen radial algorithm.
        RadialFunctor<ScalarType>* blendingFunctor; // Blending functor defined accordingly to
                                                    // the chosen blending algorithm.
        NoiseFunctor<ScalarType>* noiseFunctor;     // Noise functor needed for postprocessing
                                                    // noise application.
        CraterFunctor<ScalarType>* craterFunctor;   // Functor constructed from the combination
                                                    // of the above three functors.
        MeshModel* target_model;
        MeshModel* samples_model;
        MeshType* target_mesh;
        MeshType* samples_mesh;
        int smoothingSteps;         // number of vertex normal smoothing steps (pre-processing)
        ScalarType max_radius, max_depth, min_radius, min_depth, radius_range, depth_range;
        bool save_as_quality, postprocessing_noise, successiveImpacts;

        CratersArgs(MeshModel* target, MeshModel* samples, int radial_alg, int seed,
                    ScalarType min_r, ScalarType max_r, ScalarType min_d, ScalarType max_d,
                    int smoothingSteps, bool save_as_quality, bool invert, bool ppNoise, bool successiveImpacts,
                    ScalarType elevation, int blending_alg, ScalarType blendingThreshold)
        {
            // Builds the random number generator used to generate radiuses and depths
            // for craters.
            generator = new vcg::math::SubtractiveRingRNG(seed);

            // builds the radial functor
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

            // builds the blending functor
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

            // if necessary, builds the noise functor
            postprocessing_noise = ppNoise;
            if (ppNoise)
            {
                noiseFunctor = new FBMNoiseFunctor<ScalarType>(8, 0.7, 2);
            }
            craterFunctor = new CraterFunctor<ScalarType>(radialFunctor, blendingFunctor,
                noiseFunctor, blendingThreshold, elevation, ppNoise, invert);

            // fields assignments
            target_model = target;
            samples_model = samples;
            target_mesh = &(target->cm);
            samples_mesh = &(samples->cm);
            this->save_as_quality = save_as_quality;
            this->successiveImpacts = successiveImpacts;
            this->smoothingSteps = smoothingSteps;
            float target_bb_diag = target_mesh->bbox.Diag();
            max_radius = target_bb_diag * 0.25 * max_r;     // calculates the maximum crater radius
            min_radius = target_bb_diag * 0.25 * min_r;     // calculates the minimum crater radius
            radius_range = max_radius - min_radius;         // calculates the radius range
            max_depth = target_bb_diag * 0.25 * max_d;      // calculates the maximum crater depth
            min_depth = target_bb_diag * 0.25 * min_d;      // calculates the minimum crater depth
            depth_range = max_depth - min_depth;            // calculates the depth range

            // some cleanup operations on target mesh
            vcg::tri::Clean<MeshType>::RemoveUnreferencedVertex(*target_mesh);
            vcg::tri::Allocator<MeshType>::CompactVertexVector(*target_mesh);
            vcg::tri::Allocator<MeshType>::CompactFaceVector(*target_mesh);
        }

        // destructor
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
        vcg::math::RandomGenerator* generator;  // private random number generator
    };


    /* Finds the nearest faces of samples and stores the pairs (sample, nearest_face)
       in the "sfv" vector (third parameter). */
    static void FindSamplesFaces(MeshType *target, MeshType *samples, SampleFaceVector &sfv)
    {
        tri::UpdateNormals<MeshType>::PerFaceNormalized(*target);
        tri::UpdateFlags<MeshType>::FaceProjection(*target);

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

    /* Detectes the faces of a crater starting from a given face, using FF adjacency. */
    static void GetCraterFaces(MeshType *m,              // target mesh
                               FacePointer startingFace, // face under the crater centre
                               VertexPointer centre,     // crater centre
                               ScalarType radius,        // crater radius
                               std::vector<FacePointer> &toFill) // the vector that has to be
                                                         // filled with crater faces
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

    /* Computes the radial perturbation given:
       - args: a CratersArgs object that stores the needed perturbation parameters;
       - centre: a pointer to the centre of the to-be-generated crater;
       - craterFaces: a vector that contains the faces contained in the crater radius;
       - radius: the radius of the to-be-generated crater;
       - depth: the depth of the to-be-generated crater;
       - pertHandle: an handle to a user-defined vertex attribute of type MeshType::ScalarType
         (i.e. s floating point value). This attribute is filled with the computed perturbation.
      */
    static void ComputeRadialPerturbation
            (CratersArgs &args, VertexPointer centre,
             std::vector<FacePointer> &craterFaces, ScalarType radius,
             ScalarType depth, PertHandle& pertHandle)
    {
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*(args.target_mesh));

        typename std::vector<FacePointer>::iterator fi;
        VertexPointer vp;
        ScalarType perturbation = .0;
        Point3<ScalarType> p;

        /* for each crater vertex, calculates the associated perturbation
           and then stores it in the passed per-vertex-attribute handle */
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
                        if(perturbation < 0)  // we are in the crater "depression"
                        {
                            pertHandle[vp] = std::min(perturbation, pertHandle[vp]);
                        } else  // crater "elevation" and/or blending portion
                        {
                            if(pertHandle[vp] == .0)
                            {
                                pertHandle[vp] += perturbation;
                            }
                        }
                    } else {
                        // by adding the perturbation to the one that is already present
                        // we obtain a "crater intersection", so we cannot recognize
                        // which crater is created before each other
                        pertHandle[vp] += perturbation;
                    }
                }
            }
        }
    }

    /* craters generation algorithm */
    static bool GenerateCraters(CratersArgs &args, vcg::CallBackPos *cb)
    {
        // enables per-vertex-quality if needed
        if(args.save_as_quality)
            args.target_model->updateDataMask(MeshModel::MM_VERTQUALITY);

        tri::UpdateFlags<MeshType>::FaceProjection(args.target_model->cm);
        // smoothes vertex normals
        cb(0, "Smoothing vertex normals..");
        tri::Smooth<MeshType>::VertexNormalLaplacian(*(args.target_mesh), args.smoothingSteps, false);

        // finds samples faces
        args.target_model->updateDataMask(MeshModel::MM_FACEFACETOPO);
        args.target_model->updateDataMask(MeshModel::MM_FACEMARK);
        SampleFaceVector sfv;
        CratersUtils<MeshType>::FindSamplesFaces(args.target_mesh, args.samples_mesh, sfv);

        // detectes crater faces and applies the radial perturbation
        int cratersNo = args.samples_mesh->vert.size(), currentCrater = 0;
        char buffer[50];
        typename SampleFaceVector::iterator sfvi;
        SampleFace p;
        ScalarType radius = .0, depth = .0;
        std::vector<FacePointer> craterFaces;

        // per-vertex float attribute
        PertHandle h = tri::Allocator<MeshType>::template AddPerVertexAttribute<ScalarType>
                            (*(args.target_mesh), std::string("perturbation"));
        for(VertexIterator vi=args.target_mesh->vert.begin(); vi!=args.target_mesh->vert.end(); ++vi)
        {
            h[vi] = .0;
        }

        // calculates the perturbation and stores it in the per-vertex-attribute
        for(sfvi=sfv.begin(); sfvi!=sfv.end(); ++sfvi)
        {
            sprintf(buffer, "Generating crater %i...", currentCrater);
            cb(100*(currentCrater++)/cratersNo, buffer);

            p = (*sfvi);
            radius = args.generateRadius();
            depth = args.generateDepth();
            CratersUtils<MeshType>::GetCraterFaces(args.target_mesh, p.second, p.first, radius, craterFaces);
            CratersUtils<MeshType>::ComputeRadialPerturbation(args, p.first, craterFaces, radius, depth, h);
        }

        for(VertexIterator vi=args.target_mesh->vert.begin(); vi!=args.target_mesh->vert.end(); ++vi)
        {
            if(h[vi] == .0) continue;

            if(args.save_as_quality)
            {
                (*vi).Q() = h[vi];
            } else
            {
                (*vi).P() += ((*vi).N() * h[vi]);
            }
        }

        // updates bounding box and normals
        tri::Allocator<MeshType>::DeletePerVertexAttribute(*(args.target_mesh), std::string("perturbation"));
        vcg::tri::UpdateBounding<MeshType>::Box(*(args.target_mesh));
        vcg::tri::UpdateNormals<MeshType>::PerVertexNormalizedPerFaceNormalized(*(args.target_mesh));
        return true;
    }
};

#endif // CRATERS_UTILS_H
