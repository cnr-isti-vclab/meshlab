//
// Created by Necip Fazil Yildiran on 06/20/19.
//

#ifndef __POINT_CONVERSION_H__
#define __POINT_CONVERSION_H__

#include <gr/shared.h>

#include "point_extlib1.hpp"
#include "point_extlib2.hpp"

#include <vector>
#include <list>

// create a vector of extlib1::PointType1 from a vector of gr::Point3D 
template<typename Scalar>
inline std::vector<extlib1::PointType1> getExtlib1Points(const std::vector<gr::Point3D<Scalar> >& grPoints)
{
  std::vector<extlib1::PointType1> result;

  for(const auto &p : grPoints)
  {
    extlib1::PointType1 newp = { p.pos()(0), p.pos()(1), p.pos()(2),
                                 p.normal()(0), p.normal()(1), p.normal()(2),
                                 p.rgb()(0), p.rgb()(1), p.rgb()(2) };

    result.push_back(newp);
  }

  return result;
}

// create a list of extlib2::PointType2 from a vector of gr::Point3D 
template<typename Scalar>
inline std::list<extlib2::PointType2> getExtlib2Points(const std::vector<gr::Point3D<Scalar> >& grPoints)
{
  using PointType2 = extlib2::PointType2;

  std::list<PointType2> result;

  float* posBuffer   = new float[grPoints.size()*3];
  float* nBuffer     = new float[grPoints.size()*3];
  float* colorBuffer = new float[grPoints.size()*3];

  int i = 0;
  for(const auto &p : grPoints)
  {
    // fill corresponding section in buffers with p
    for(int j = 0; j < 3; j++) {
      posBuffer[i*3 + j]   = p.pos()(j);

      nBuffer[i*3 + j]     = p.normal()(j);
      colorBuffer[i*3 + j] = p.rgb()(j);
    }

    result.emplace_back( PointType2 { posBuffer, nBuffer, colorBuffer, i++ } );
  }

  return result;
}

#endif