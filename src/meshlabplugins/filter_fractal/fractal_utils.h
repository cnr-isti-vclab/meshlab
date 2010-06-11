#ifndef FRACTAL_UTILS_H
#define FRACTAL_UTILS_H

#include <vcg/math/perlin_noise.h>
#include <common/meshmodel.h>
#include <vcg/complex/trimesh/smooth.h>
#include "filter_functors.h"
#include <vector>

using namespace vcg;



/* This class encapsulates the algorithms used to generate the fractal perturbation
   onto a generic mesh and the grid for terrain generation. */
template<class MeshType>
class FractalUtils
{
public:
    // some typedefs needed by the following algorithms
    typedef typename MeshType::ScalarType               ScalarType;
    typedef typename MeshType::VertexIterator           VertexIterator;
    typedef typename MeshType::VertexPointer            VertexPointer;
    typedef std::pair<VertexPointer, ScalarType>        PertPair;
    typedef typename std::vector<PertPair>::iterator    PertIterator;
    typedef typename MeshType::FaceIterator             FaceIterator;
    typedef typename MeshType::CoordType                CoordType;

    /* This class contains the arguments needed for an application of
       fractal perturbation filter. It constructs the right noise functor
       according to the requested noise algorithm. */
    class FractalArgs
    {
    public:
        MeshModel* mesh;
        ScalarType seed, maxHeight, scale;
        int subdivisionSteps, smoothingSteps;
        bool saveAsQuality, displaceSelected;
        NoiseFunctor<ScalarType>* noiseFunctor;

        FractalArgs(MeshModel* mm, int algorithmId, ScalarType seed, ScalarType octaves,
                    ScalarType lacunarity, ScalarType fractalIncrement, ScalarType offset,
                    ScalarType gain, ScalarType maxHeight, ScalarType scale,
                    int smoothingSteps, bool saveAsQuality)
        {
            mesh = mm;
            this->seed = seed;
            this->maxHeight = maxHeight;
            this->scale = scale;
            displaceSelected = false;
            this->smoothingSteps = smoothingSteps;
            this->saveAsQuality = saveAsQuality;

            switch(algorithmId)
            {
            case 0: //fBM
                noiseFunctor = new FBMNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity);
                break;
            case 1: //standard multifractal
                noiseFunctor = new StandardMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
                break;
            case 2: //heterogeneous multifractal
                noiseFunctor = new HeteroMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
                break;
            case 3: //hybrid multifractal
                noiseFunctor = new HybridMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset);
                break;
            case 4: //ridged multifractal
                noiseFunctor = new RidgedMFNoiseFunctor<ScalarType>(octaves, fractalIncrement, lacunarity, offset, gain);
                break;
            }
        }

        ~FractalArgs() { delete noiseFunctor; }
    };


    /* This function calculates a fractal perturbation and stores it in a given
       vector. The perturbation is calculated according to the given parameters
       (second argument), on the target mesh (first argument). The result is
       a vector of pairs (vertex_pointer, perturbation) (third parameter).
       Assumption: if the perturbation has to be saved as vertex quality, the
       quality flags must be enabled. */
    static bool ComputeFractalPerturbation(
            MeshType& m,            // target mesh
            FractalArgs& args,      // fractal displacement arguments
            vcg::CallBackPos* cb)   // progress bar handling
    {
        if(args.saveAsQuality && !tri::HasPerVertexQuality(m)) return false;

        // prepares the mesh for the fractal displacement
        tri::UpdateNormals<MeshType>::PerVertexNormalizedPerFaceNormalized(m);
        if(args.smoothingSteps > 0)
        {
            cb(0, "Smoothing vertex normals..");
            tri::Smooth<MeshType>::VertexNormalLaplacian(m, args.smoothingSteps, false);
        }

        // some variables to manage the progress bar
        char buffer[50];
        int vCount = m.vert.size(), i=0;

        // other variables for scaling and normalization of points
        std::vector<PertPair> pertVector;
        ScalarType factor = args.scale/m.bbox.Diag(), min = 1000.0, max = -1000.0;
        ScalarType seedTraslation = args.seed/factor, perturbation = .0;
        Point3<ScalarType> seedPoint(seedTraslation, seedTraslation, seedTraslation);
        Point3<ScalarType> center = m.bbox.Center(), p;
        Point3<ScalarType> trasl = seedPoint - center;
        PertPair* tmpPair;

        // first loop: calculates the perturbation and create the vector pairs
        for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi, ++i)
        {
            sprintf(buffer, "Calculating perturbation on vertex %d..", i);
            cb(100*i/vCount, buffer);

            if (!(*vi).IsS() && args.displaceSelected) continue;
            p = ((*vi).P() + trasl) * factor;   // scales and normalizes the point
            perturbation = (*args.noiseFunctor)(p);
            tmpPair = new PertPair(&(*vi), perturbation);
            pertVector.push_back(*tmpPair);

            if (perturbation < min) min = perturbation;
            if (perturbation > max) max = perturbation;
        }

        // defines the effective range and the target range of the perturbation
        ScalarType hmax = args.maxHeight, hmin = (min * hmax) / max;
        ScalarType range1 = max - min, range2 = hmax - hmin;

        i = 0;
        int pairsCount = pertVector.size();

        // second loop: normalizes and applies the perturbation
        for(PertIterator pi=pertVector.begin(); pi!=pertVector.end(); ++pi, ++i)
        {
            sprintf(buffer, "Normalizing perturbation on vertex %d..", i);
            cb(100*i/pairsCount, buffer);

            tmpPair = &(*pi);
            tmpPair->second = (((tmpPair->second - min)/range1) * range2) + hmin;

            if(args.saveAsQuality)
            {
                tmpPair->first->Q() += tmpPair->second;
            } else {
                tmpPair->first->P() += (tmpPair->first->N() * tmpPair->second);
            }
        }

        // if necessary, updates bounding box and normals
        if(!args.saveAsQuality)
        {
            tri::UpdateBounding<MeshType>::Box(m);
            tri::UpdateNormals<MeshType>::PerVertexNormalizedPerFaceNormalized(m);
        }
        return true;
    }

    /* Generates a squared grid of triangles whose side will be passed back in the
       gridSide parameter. The subSteps parameter represents the number of subdivision
       steps during the grid generation. */
    static void GenerateGrid(MeshType& m, int subSteps, float& gridSide)
    {
        m.Clear();
        int k = (int)(pow(2.0f, subSteps)), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;
        ScalarType x = .0, y = .0;
        gridSide = k2;

        vcg::tri::Allocator<MeshType>::AddVertices(m, vertexCount);
        vcg::tri::Allocator<MeshType>::AddFaces(m, faceCount);

        // generates the vertex coords
        VertexIterator vi;
		std::vector<VertexPointer> ivp(vertexCount);
        for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
        {
            x = (i%k2);
            y = (i/k2);
            (*vi).P() = CoordType(x, y, ScalarType(0));
            ivp[i++] = &*vi;
        }

        // defines the vertexes of each face
        FaceIterator fi = m.face.begin();
        int evenFace[3] = {0, 1, k2}, oddFace[3] = {1, k2+1, k2};
        for(i=0; i<k; i++)
        {
            for(j=0; j<k; j++)
            {
                (*fi).V(0) = ivp[evenFace[0]++];
                (*fi).V(1) = ivp[evenFace[1]++];
                (*fi).V(2) = ivp[evenFace[2]++];
                ++fi;
                (*fi).V(0) = ivp[oddFace[0]++];
                (*fi).V(1) = ivp[oddFace[1]++];
                (*fi).V(2) = ivp[oddFace[2]++];
                if(fi != m.face.end())++fi;
            }
            evenFace[0]++; evenFace[1]++; evenFace[2]++;
            oddFace[0]++;  oddFace[1]++;  oddFace[2]++;
        }

        // updates the created mesh bounding box
        vcg::tri::UpdateBounding<MeshType>::Box(m);
    }
};

#endif // FRACTAL_PERTURBATION_H
