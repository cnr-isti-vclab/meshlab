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

    /* given a point, finds the closest face using a ray */
    /*
    template<class CoordScalarType>
    static FacePointer getClosestFaceWithRay(MeshType* m, VertexPointer sampleVertex)
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
    */

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

    /*
    template<class ScalarType>
    static bool triangleContainsPoint(FacePointer fp, Point3<ScalarType> &p)
    {
        // bounding box test
        vcg::Box3<ScalarType> bbox;
        fp->GetBBox(bbox);
        if(!bbox.IsIn(p)) return false;

        // coplanar face test
        Point3<ScalarType> v1 = fp->P(0);
        Point3<ScalarType> v2 = fp->P(1);
        Point3<ScalarType> v3 = fp->P(2);

        ScalarType _EPSILON = ScalarType(0.000001);
        ScalarType a = p.X() - v1.X();
        ScalarType b = p.Y() - v1.Y();
        ScalarType c = p.Z() - v1.Z();
        ScalarType d = v2.X() - v1.X();
        ScalarType e = v2.Y() - v1.Y();
        ScalarType f = v2.Z() - v1.Z();
        ScalarType g = v3.X() - v1.X();
        ScalarType h = v3.Y() - v1.Y();
        ScalarType i = v3.Z() - v1.Z();
        ScalarType det = a*e*i + b*f*g + c*d*h - (c*e*g + a*f*h + b*d*i);

        if(fabs(det)<=_EPSILON)
        {
            // barycentric test
            ScalarType a, b, c;
            vcg::InterpolationParameters<FaceType, ScalarType>(*fp, p, a, b, c);
            return (a <= 1 && a >=0 && b <=1 && b>=0 && c<=1 && c>=0);
        }
        return false;
    }
    */
};



#endif // CRATERS_UTILS_H
