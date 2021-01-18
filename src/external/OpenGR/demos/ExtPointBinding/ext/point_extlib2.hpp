#ifndef __POINT_EXTLIB2_H__
#define __POINT_EXTLIB2_H__

//! [extlib2::PointType2]
namespace extlib2
{
  struct PointType2 {
    float* posBuffer;   // position buffer
    float* nBuffer;     // normal buffer
    float* colorBuffer; // color buffer
    int id;             // id (or index)
  };
}
//! [extlib2::PointType2]

#endif