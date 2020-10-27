/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::ScalarArray
 GLSDer <DataPoint, _WFunctor, T>::dtau() const
{
    MULTIARCH_STD_MATH(sqrt);

    Scalar prattNorm2 = Base::prattNorm2();
    Scalar prattNorm  = sqrt(prattNorm2);
    Scalar cfactor    = Scalar(.5) / prattNorm;
    ScalarArray dfield = Base::m_dUc;
    // Recall that tau is the field function at the evaluation point, we thus must take care about
    // its variation when differentiating in space:
    if(this->isScaleDer())
      dfield.template tail<DataPoint::Dim>() += Base::m_ul;

    return (dfield * prattNorm - Base::m_uc * cfactor * Base::dprattNorm2()) / prattNorm2;
}


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::VectorArray
GLSDer <DataPoint, _WFunctor, T>::deta() const
{
  return Base::dNormal();
}


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::ScalarArray
GLSDer <DataPoint, _WFunctor, T>::dkappa() const
{
    MULTIARCH_STD_MATH(sqrt);

    Scalar prattNorm2 = Base::prattNorm2();
    Scalar prattNorm  = sqrt(prattNorm2);
    Scalar cfactor    = Scalar(.5) / prattNorm;

    return Scalar(2.) * (Base::m_dUq * prattNorm - Base::m_uq * cfactor * Base::dprattNorm2()) / prattNorm2;
}


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::ScalarArray
GLSDer <DataPoint, _WFunctor, T>::dtau_normalized() const
{
    return dtau();
}


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::VectorArray
GLSDer <DataPoint, _WFunctor, T>::deta_normalized() const
{
    return Base::m_t * deta();
}


template < class DataPoint, class _WFunctor, typename T>
typename GLSDer <DataPoint, _WFunctor, T>::ScalarArray
GLSDer <DataPoint, _WFunctor, T>::dkappa_normalized() const
{
    return dkappa() * Base::m_t * Base::m_t;
}




template < class DataPoint, class _WFunctor, typename T>
typename GLSGeomVar <DataPoint, _WFunctor, T>::Scalar
GLSGeomVar <DataPoint, _WFunctor, T>::geomVar(  Scalar wtau, 
                                                Scalar weta,
                                                Scalar wkappa ) const
{
    Scalar dtau   = Base::dtau_normalized().col(0)(0);
    Scalar deta   = Base::deta_normalized().col(0).norm();
    Scalar dkappa = Base::dkappa_normalized().col(0)(0);

    return wtau*dtau*dtau + weta*deta*deta + wkappa*dkappa*dkappa;
}
