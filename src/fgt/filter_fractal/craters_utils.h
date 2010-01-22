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
#include "radial_perturbation.h"

template<class MeshType>
class CratersUtils
{
public:
    typedef typename MeshType::FacePointer      FacePointer;
    typedef typename MeshType::FaceIterator     FaceIterator;
    typedef typename MeshType::FaceType         FaceType;
    typedef typename MeshType::VertexPointer    VertexPointer;
    typedef typename MeshType::VertexIterator   VertexIterator;
    typedef typename MeshType::template PerVertexAttributeHandle<FacePointer> FaceHandle;

    /* finds the faces onto which the samples lie and stores them using the given handle */
    template<class ScalarType>
    static void findSamplesFaces(MeshType *target, MeshType *samples, FaceHandle &fh)
    {
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*samples);
        VertexIterator vi;
        Box3<ScalarType> bbox;
        int undiscoveredFaces = samples->vert.size();
        FaceIterator fi = target->face.begin();
        ScalarType a, b, c;

        while(fi!=target->face.end() && undiscoveredFaces>0)
        {
            (*fi).GetBBox(bbox);
            for(vi=samples->vert.begin(); vi!=samples->vert.end(); ++vi)
            {
                if((*vi).IsV()) continue;               // sample's face discovered
                if(!(bbox.IsIn((*vi).P()))) continue;   // sample not contained in the bounding box

                // barycentric test
                vcg::InterpolationParameters<FaceType, ScalarType>((*fi), (*vi).P(), a, b, c);
                if(a <= 1 && a >=0 && b <=1 && b>=0 && c<=1 && c>=0)
                {   // face found
                    (*vi).SetV();
                    fh[vi] = &(*fi);
                    undiscoveredFaces--;
                }
            }
            ++fi;
        }
    }

    /* detectes the faces of the crater starting from a given face */
    template<class CoordScalarType>
    static void SelectCraterFaces(MeshType *m,              // target mesh
                                  FacePointer startingFace, // face under the crater centre
                                  VertexPointer centre,     // crater centre
                                  CoordScalarType radius,    // crater radius
                                  std::vector<FacePointer>* toFill = 0
                                                                     )
    {
        assert(vcg::tri::HasFFAdjacency(*m));
        vcg::tri::UpdateFlags<MeshType>::FaceClearV(*m);
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*m);

        vcg::Sphere3<CoordScalarType> craterSphere(centre->P(), radius); // crater sphere
        std::vector<FacePointer> fl;
        fl.push_back(startingFace);

        if(toFill)
        {
            toFill->clear();
        }
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
                    if(toFill)
                    {
                        toFill->push_back(f);
                    }
                    f->SetS();
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
    static void applyRadialPerturbation(MeshType *m, int algorithm, std::vector<FacePointer> &craterFaces,
                                        VertexPointer centre, ScalarType radius, ScalarType depth, ScalarType profileFactor)
    {
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*m);
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

                    switch(algorithm)
                    {
                    case 0: // gaussian rbf
                        perturbation = RadialPerturbation<ScalarType>::Gaussian
                                       (vp->P(), centre->P(), radius, depth, profileFactor);
                        break;
                    case 1: // multiquadric rbf
                        perturbation = RadialPerturbation<ScalarType>::Multiquadric
                                       (vp->P(), centre->P(), radius, depth, profileFactor);
                        break;
                    case 2: // inverse multiquadric rbf
                        perturbation = RadialPerturbation<ScalarType>::InverseMultiquadric
                                       (vp->P(), centre->P(), radius, depth, profileFactor);
                        break;
                    case 3: // cauchy rbf
                        perturbation = RadialPerturbation<ScalarType>::Cauchy
                                       (vp->P(), centre->P(), radius, depth, profileFactor);
                        break;
                    }
                    vp->P() += (centre->N() * perturbation);
                }
            }
        }
    }
};

#endif // CRATERS_UTILS_H
