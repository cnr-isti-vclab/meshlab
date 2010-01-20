#ifndef CRATERS_UTILS_H
#define CRATERS_UTILS_H

#include <vcg/space/point3.h>
#include <vcg/space/line3.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/sphere3.h>
#include <vcg/math/base.h>

template<class MeshType>
class CratersUtils
{
public:
    typedef typename MeshType::FacePointer      FacePointer;
    typedef typename MeshType::VertexPointer    VertexPointer;
    typedef typename MeshType::FaceIterator     FaceIterator;
    typedef typename MeshType::FaceType         FaceType;

    /* given a point, finds the closest face. */
    template<class CoordScalarType>
    static FacePointer getClosestFace(MeshType* m, VertexPointer sampleVertex)
    {
        vcg::Line3<CoordScalarType> ray(sampleVertex->P(), sampleVertex->N() * -1);
        CoordScalarType minDist = -1;
        CoordScalarType bar1, bar2, bar3, dist;
        FacePointer f = 0;
        FacePointer closest = 0;

        for(FaceIterator fi=m->face.begin(); fi!=m->face.end(); ++fi)
        {
            f = &(*fi);
            if(vcg::IntersectionLineTriangle<CoordScalarType>
               (ray, f->P(0), f->P(1), f->P(2), bar1, bar2, bar3))
            {
                dist = vcg::Distance(sampleVertex->P(), f->P(0));
                if (minDist == -1 || dist < minDist)
                {
                    minDist = dist;
                    closest = f;
                }
            }
        }
        return closest;
    }

    /* detectes the faces of the crater */
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
    static void applyRadialPerturbation(MeshType *m, std::vector<FacePointer> &craterFaces,
            VertexPointer centre, ScalarType radius, ScalarType depth)
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
                    perturbation = CratersUtils<MeshType>::applyRadialPerturbation<ScalarType>
                                   (vp->P(), centre->P(), radius, depth);
                    vp->P() += (centre->N() * perturbation);
                }
            }
        }
    }

    template<class ScalarType>
    static ScalarType applyRadialPerturbation(Point3<ScalarType> &p, Point3<ScalarType> &centre,
                                       ScalarType radius, ScalarType depth)
    {
        ScalarType result = .0;
        ScalarType dist = vcg::Distance(p, centre);
        double exponent = - pow((2 * dist / radius), 2);
        result = - depth * exp(exponent);
        return result;
    }


};



#endif // CRATERS_UTILS_H
