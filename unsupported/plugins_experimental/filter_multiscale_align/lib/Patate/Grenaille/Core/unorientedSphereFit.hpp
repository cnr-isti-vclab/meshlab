/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/



#if NEVERDEFINED

MatrixBB cov = MatrixBB::Zero();
VectorDd sumP = VectorDd::Zero();
double sumDotPP = 0.;
double sumOfWeights = 0.;

// the normalization matrix
MatrixBB Q = MatrixBB::Zero();

for(uint i=0 ; i<nofSamples ; ++i)
{
    VectorDd p = pNeighborhood->getNeighbor(i).position().cast<double>();
    VectorDd n = pNeighborhood->getNeighbor(i).normal().cast<double>();
    double w = pNeighborhood->getNeighborWeight(i);

    VectorB basis;
    basis << n, n.dot(p);

    cov += w * basis * basis.transpose();
    sumOfWeights += w;

    MatrixBB q;
    q <<  MatrixDDd::Identity(), p,
        p.transpose(), p.squaredNorm();

    Q += w * q;

    sumP      += w * p;
    sumDotPP  += w * p.squaredNorm();
}
cov /= sumOfWeights;
Q   /= sumOfWeights;            

MatrixBB M = Q.inverse() * cov;
Eigen::EigenSolver<MatrixBB> eig(M);
VectorB eivals = eig.eigenvalues().real();
int maxId = 0;
double l = eivals.maxCoeff(&maxId);
VectorB eivec = eig.eigenvectors().col(maxId).real();

// integrate
uLinear()   = eivec.start<Dim>().cast<Real>();
uQuad()     = 0.5*eivec(Dim);
uConstant() = -(1./sumOfWeights)*(eivec.start<Dim>().dot(sumP) + 0.5*eivec(Dim) * sumDotPP);

#endif

template < class DataPoint, class _WFunctor, typename T>
void 
UnorientedSphereFit<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    // Setup primitive
    Base::resetPrimitive();
    Base::basisCenter() = _evalPos;

    // Setup fitting internal values
    m_matA.setZero();
    //   _matQ.setZero();
    m_sumP.setZero();
    m_sumDotPP = Scalar(0.0);
    m_sumW     = Scalar(0.0);
}

template < class DataPoint, class _WFunctor, typename T>
bool 
UnorientedSphereFit<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    // centered basis
    VectorType q = _nei.pos() - Base::basisCenter();

    // compute weight
    Scalar w = m_w.w(q, _nei);

    if (w > Scalar(0.))
    {
        VectorB basis;
        basis << _nei.normal(), _nei.normal().dot(q);

        m_matA     += w * basis * basis.transpose();
        m_sumP     += w * q;
        m_sumDotPP += w * q.squaredNorm();
        m_sumW     += w;

        /*! \todo Handle add of multiple similar neighbors (maybe user side)*/
        ++(Base::m_nbNeighbors);
        return true;
    }

    return false;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
UnorientedSphereFit<DataPoint, _WFunctor, T>::finalize ()
{
    MULTIARCH_STD_MATH(sqrt);

    // 1. finalize sphere fitting
    Scalar invSumW;
    Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();

    // handle specific configurations
    // With less than 3 neighbors the fitting is undefined
    if(m_sumW == Scalar(0.) || Base::m_nbNeighbors < 3)
    {
        Base::m_ul.setZero();
        Base::m_uc = 0;
        Base::m_uq = 0;
        Base::m_isNormalized = false;
        Base::m_eCurrentState = UNDEFINED;
        return Base::m_eCurrentState;
    }
    else
    {
        invSumW = Scalar(1.) / m_sumW;
    }

    MatrixBB Q;
    Q.template topLeftCorner<Dim,Dim>().setIdentity();
    Q.col(Dim).template head<Dim>() = m_sumP * invSumW;
    Q.row(Dim).template head<Dim>() = m_sumP * invSumW;
    Q(Dim,Dim) = m_sumDotPP * invSumW;
    m_matA *= invSumW;

    MatrixBB M = Q.inverse() * m_matA;
    Eigen::EigenSolver<MatrixBB> eig(M);
    VectorB eivals = eig.eigenvalues().real();
    int maxId = 0;
    Scalar l = eivals.maxCoeff(&maxId);
    VectorB eivec = eig.eigenvectors().col(maxId).real();

    // integrate
    Base::m_ul = eivec.template head<Dim>();
    Base::m_uq = Scalar(0.5) * eivec(Dim);
    Base::m_uc = -invSumW * (Base::m_ul.dot(m_sumP) + m_sumDotPP * Base::m_uq);

    Base::m_isNormalized = false;

    if(Base::m_nbNeighbors < 6)
    {
        Base::m_eCurrentState = UNSTABLE;
    }
    else
    {
        Base::m_eCurrentState = STABLE;
    }

    return Base::m_eCurrentState;
}

#ifdef TOBEIMPLEMENTED

namespace internal
{

template < class DataPoint, class _WFunctor, typename T, int Type>
void 
OrientedSphereDer<DataPoint, _WFunctor, T, Type>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_dSumN     = VectorArray::Zero();
    m_dSumP     = VectorArray::Zero();

    m_dSumDotPN = ScalarArray::Zero();
    m_dSumDotPP = ScalarArray::Zero();
    m_dSumW     = ScalarArray::Zero();

    m_dUc       = ScalarArray::Zero();
    m_dUq       = ScalarArray::Zero();
    m_dUl       = VectorArray::Zero();
}


template < class DataPoint, class _WFunctor, typename T, int Type>
bool 
OrientedSphereDer<DataPoint, _WFunctor, T, Type>::addNeighbor(const DataPoint  &_nei)
{
    bool bResult = Base::addNeighbor(_nei);
    if(bResult)
    {
        int spaceId = (Type & FitScaleDer) ? 1 : 0;

        ScalarArray w;

        // centered basis
        VectorType q = _nei.pos() - Base::basisCenter();

        // compute weight
        if (Type & FitScaleDer)
            w[0] = Base::m_w.scaledw(q, _nei);

        if (Type & FitSpaceDer){
            VectorType vw = Base::m_w.spacedw(q, _nei);
            for(unsigned int i = 0; i < DataPoint::Dim; i++)
                w[spaceId+i] = vw[i];
        }

        // increment
        m_dSumW     += w;
        m_dSumP     += q * w;
        m_dSumN     += _nei.normal() * w;
        m_dSumDotPN += w * _nei.normal().dot(q);
        m_dSumDotPP += w * q.squaredNorm();

        return true;
    }

    return false;
}


template < class DataPoint, class _WFunctor, typename T, int Type>
FIT_RESULT 
OrientedSphereDer<DataPoint, _WFunctor, T, Type>::finalize()
{
    MULTIARCH_STD_MATH(sqrt);

    Base::finalize();

    // Test if base finalize end on a viable case (stable / unstable)
    if (this->isReady())
    {

        Scalar invSumW = Scalar(1.)/Base::m_sumW;

        Scalar nume  = Base::m_sumDotPN - invSumW*Base::m_sumP.dot(Base::m_sumN);
        Scalar deno  = Base::m_sumDotPP - invSumW*Base::m_sumP.dot(Base::m_sumP);

        ScalarArray dNume = m_dSumDotPN - invSumW*invSumW * ( Base::m_sumW * (
                                                            Base::m_sumN.transpose() * m_dSumP +
                                                            Base::m_sumP.transpose() * m_dSumN ) 
                                                            - m_dSumW*Base::m_sumP.dot(Base::m_sumN) );
        ScalarArray dDeno = m_dSumDotPP - invSumW*invSumW*( Scalar(2.)*Base::m_sumW * Base::m_sumP.transpose()*m_dSumP
                                                            - m_dSumW*Base::m_sumP.dot(Base::m_sumP) );

        m_dUq =  Scalar(.5) * (deno * dNume - dDeno * nume)/(deno*deno);
        m_dUl =  invSumW*((m_dSumN - Scalar(2.)*(m_dSumP*Base::m_uq+Base::m_sumP*m_dUq)) - Base::m_ul*m_dSumW);
        m_dUc = -invSumW*( Base::m_sumP.transpose() * m_dUl + 
                            Base::m_sumDotPP * m_dUq + Base::m_ul.transpose() * m_dSumP +
                            Base::m_uq*m_dSumDotPP + m_dSumW*Base::m_uc);
    }

    return Base::m_eCurrentState;

}


template < class DataPoint, class _WFunctor, typename T, int Type>
bool
OrientedSphereDer<DataPoint, _WFunctor, T, Type>::applyPrattNorm()
{
    if(Base::isNormalized())
        return false; //need original parameters without Pratt Normalization


    MULTIARCH_STD_MATH(sqrt);
    Scalar pn2    = Base::prattNorm2();
    Scalar pn     = sqrt(pn2);

    ScalarArray dpn2   = dprattNorm2();
    ScalarArray factor = Scalar(0.5) * dpn2 / pn;  

    m_dUc = ( m_dUc * pn - Base::m_uc * factor ) / pn2;
    m_dUl = ( m_dUl * pn - Base::m_ul * factor ) / pn2;
    m_dUq = ( m_dUq * pn - Base::m_uq * factor ) / pn2;

    Base::applyPrattNorm();
    return true;
}
  
}// namespace internal

#endif
