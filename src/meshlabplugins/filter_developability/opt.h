#ifndef FILTERDEVELOPABILITY_OPT_H
#define FILTERDEVELOPABILITY_OPT_H

#include "mesh_utils.h"
#include "energy.h"
#include "energy_grad.h"

#include <vcg/complex/allocate.h>
#include <vcg/complex/append.h>

#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/topology.h>


/*
 * Gradient method optimizer interface
 */
template<typename MeshType>
class Optimizer
{
public:
    Optimizer(MeshType& m,
              double stepSize) :
        m(m),
        stepSize(stepSize),
        nFunEval(0)
    {
        vAttrStar = vcg::tri::Allocator<MeshType>:: template GetPerVertexAttribute<Star<MeshType>>(m, std::string("Star"));
        vAttrGrad = vcg::tri::Allocator<MeshType>:: template GetPerVertexAttribute<vcg::Point3d>(m, std::string("Gradient"));
        fAttrArea = vcg::tri::Allocator<MeshType>:: template GetPerFaceAttribute<double>(m, std::string("Area"));
    }

    virtual ~Optimizer() {}

    /*
     * Routine that must be called when the mesh geometry and/or topology changes
     */
    virtual void reset() = 0;

    /*
     * Perform an optimization step; returns true iff another step can be performed
     */
    virtual bool step() = 0;

    void updateGradientSqNorm()
    {
        gradSqNorm = 0.0;
        
        for(size_t v = 0; v < m.VN(); v++)
            for(int i = 0; i < 3; i++)
                gradSqNorm += std::pow(vAttrGrad[v][i], 2);
    }

    double getGradientSqNorm() { return gradSqNorm; }
    double getStepSize() { return stepSize; }
    double getEnergy() { return energy; }
    int getNFunEval() { return nFunEval; }

protected:
    MeshType& m;
    AreaFaceAttrHandle<MeshType> fAttrArea;
    StarVertAttrHandle<MeshType> vAttrStar;
    GradientVertAttrHandle<MeshType> vAttrGrad;
    double stepSize;
    double gradSqNorm;
    double energy;
    int nFunEval;
};


/*
 * Gradient method optimization with fixed step size
 */
template<typename MeshType>
class FixedStepOpt : public Optimizer<MeshType>
{
    using Optimizer<MeshType>::m;
    using Optimizer<MeshType>::fAttrArea;
    using Optimizer<MeshType>::vAttrStar;
    using Optimizer<MeshType>::vAttrGrad;
    using Optimizer<MeshType>::stepSize;
    using Optimizer<MeshType>::gradSqNorm;
    using Optimizer<MeshType>::energy;
    using Optimizer<MeshType>::nFunEval;
    using Optimizer<MeshType>::updateGradientSqNorm;

public:
    FixedStepOpt(MeshType& m,
                 int maxFunEval,
                 double eps,
                 double stepSize) :
        Optimizer<MeshType>(m, stepSize),
        maxFunEval(maxFunEval),
        eps(eps)
    {
        reset();
    }

    void reset() override
    {
        updateFaceStars(m, vAttrStar);
        updateNormalsAndAreas(m, fAttrArea);
        energy = combinatorialEnergyGrad(m, fAttrArea, vAttrStar, vAttrGrad);
        updateGradientSqNorm();
    }

    bool step() override
    {
        if(nFunEval >= maxFunEval || gradSqNorm <= eps)
            return false;
        
        for(size_t v = 0; v < m.VN(); v++)
            m.vert[v].P() -= (vAttrGrad[v] * stepSize);

        updateNormalsAndAreas(m, fAttrArea);
        energy = combinatorialEnergyGrad(m, fAttrArea, vAttrStar, vAttrGrad);
        updateGradientSqNorm();
        nFunEval++;

        return true;
    }

private: 
    int maxFunEval;
    double eps;
};


/*
 * Gradient method optimization with backtracking line search (Armijo condition)
 */
template<typename MeshType>
class BacktrackingOpt : public Optimizer<MeshType>
{
    using Optimizer<MeshType>::m;
    using Optimizer<MeshType>::fAttrArea;
    using Optimizer<MeshType>::vAttrStar;
    using Optimizer<MeshType>::vAttrGrad;
    using Optimizer<MeshType>::stepSize;
    using Optimizer<MeshType>::gradSqNorm;
    using Optimizer<MeshType>::energy;
    using Optimizer<MeshType>::nFunEval;
    using Optimizer<MeshType>::updateGradientSqNorm;

public:
    BacktrackingOpt(MeshType& m,
                    int maxFunEval,
                    double eps,
                    double initialStepSize,
                    double minStepSize,
                    double tau,
                    double armijoM1) :
        Optimizer<MeshType>(m, initialStepSize),
        maxFunEval(maxFunEval),
        eps(eps),
        initialStepSize(initialStepSize),
        minStepSize(minStepSize),
        tau(tau),
        armijoM1(armijoM1)
    {
        reset();
    }

    void reset() override
    {
        tmpVP.clear();
        tmpVP.reserve(m.vert.size());
        for(size_t v = 0; v < m.vert.size(); v++)
            tmpVP.push_back(m.vert[v].cP());

        updateFaceStars(m, vAttrStar);
        updateNormalsAndAreas(m, fAttrArea);
        energy = combinatorialEnergyGrad(m, fAttrArea, vAttrStar, vAttrGrad);
        updateGradientSqNorm();
    }

    bool step() override
    {                  
        if(nFunEval >= maxFunEval || gradSqNorm <= eps)
            return false;
        
        double LS_energy, LS_stepSize;
        // compute the current tomography deriv as the dot prod between grad and search direction -grad
        // == minus the squared l2 norm of the gradient
        double tomographyDeriv = -(gradSqNorm);

        for(LS_stepSize = initialStepSize; LS_stepSize > minStepSize; LS_stepSize *= tau)
        {
            for(size_t v = 0; v < m.vert.size(); v++)
                m.vert[v].P() = tmpVP[v] - vAttrGrad[v] * LS_stepSize;

            updateNormalsAndAreas(m, fAttrArea);
            LS_energy = combinatorialEnergy(m, vAttrStar);
            nFunEval++;

            // check Armijo condition
            if(LS_energy <= energy + armijoM1 * LS_stepSize * tomographyDeriv)
                break;

            if(nFunEval >= maxFunEval)
            {
                for(size_t v = 0; v < m.vert.size(); v++)
                    m.vert[v].P() = tmpVP[v];
                    
                updateNormalsAndAreas(m, fAttrArea);

                return false;
            }
        }

        for(size_t v = 0; v < m.vert.size(); v++)
            tmpVP[v] = m.vert[v].cP();

        stepSize = LS_stepSize;
        energy = LS_energy;
        combinatorialEnergyGrad(m, fAttrArea, vAttrStar, vAttrGrad);
        updateGradientSqNorm();
        nFunEval++;

        return true;
    }

private:
    std::vector<vcg::Point3d> tmpVP;
    int maxFunEval;
    double eps;
    double initialStepSize;
    double minStepSize;
    double tau;
    double armijoM1;
};


#endif
