#ifndef CRATERS_UTILS_H
#define CRATERS_UTILS_H

#include <vcg/space/point3.h>
#include <vcg/space/line3.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/sphere3.h>

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
                                  CoordScalarType radius,   // crater radius
                                  std::vector<FacePointer>* craterFaces // vector of crater faces (output)
                                  )
    {
        assert(vcg::tri::HasFFAdjacency(*m));
        vcg::tri::UpdateFlags<MeshType>::FaceClearV(*m);
        vcg::tri::UpdateFlags<MeshType>::VertexClearV(*m);

        vcg::Sphere3<CoordScalarType> craterSphere(centre->P(), radius); // crater sphere
        std::vector<FacePointer> fl;
        fl.push_back(startingFace);

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
                    craterFaces->push_back(f);
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
};



#endif // CRATERS_UTILS_H
