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
#include "filter_args.h"
#include "radial_perturbation.h"
#include "fractal_perturbation.h"

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

    template<class ScalarType>
    static void FindSamplesFaces(MeshType *target, MeshType *samples, SampleFaceVector &sfv)
    {
        vcg::Box3<ScalarType> bbox;
        float a, b, c;
        int undiscoveredFaces = samples->vert.size();

        vcg::tri::UpdateFlags<CMeshO>::VertexClearV(*samples);
        FaceIterator fi = target->face.begin();
        while(fi!=target->face.end() && undiscoveredFaces>0)
        {
            (*fi).GetBBox(bbox);

            for(VertexIterator vi=samples->vert.begin(); vi!=samples->vert.end(); ++vi)
            {
                if((*vi).IsV()) continue;
                if(!bbox.IsIn((*vi).P())) continue;

                vcg::InterpolationParameters<FaceType, ScalarType>((*fi), (*vi).P(), a, b, c);
                if (a>=0 && a<=1 && b>=0 && b<=1 && c>=0 && c<=1)
                {
                    (*vi).SetV();
                    sfv.push_back( SampleFace(&(*vi), &(*fi)));
                    undiscoveredFaces--;
                }
            }
            ++fi;
        }
    }


    /* detectes the faces of the crater starting from a given face */
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

        for(fi = craterFaces.begin(); fi!=craterFaces.end(); ++fi)
        {
            for(int i=0; i<3; i++)
            {
                vp = (*fi)->V(i);
                if(!vp->IsV())
                {
                    vp->SetV();

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
                        perturbation = RadialPerturbation<ScalarType>::InverseMultiquadric
                                       (vp->P(), centre->P(), radius, depth, args.profile_factor);
                        break;
                    case 3: // cauchy rbf
                        perturbation = RadialPerturbation<ScalarType>::Cauchy
                                       (vp->P(), centre->P(), radius, depth, args.profile_factor);
                        break;
                    }

                    // limits the perturbation to negative values
                    if(perturbation > 0) perturbation = 0;

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
