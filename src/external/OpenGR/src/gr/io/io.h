#ifndef _OPENGR_IO_IO_H
#define _OPENGR_IO_IO_H

#include "gr/shared.h"
#include "gr/utils/disablewarnings.h"

#include <fstream>
#include <iostream>
#include <string>

#include <Eigen/Core>

#ifndef _MSC_VER
#include <sys/time.h>
#include <unistd.h>
#endif
#include <stdlib.h>


struct tripple {
  int a;
  int b;
  int c;
  int n1;
  int n2;
  int n3;
  int t1;
  int t2;
  int t3;
  tripple() {}
  tripple(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}
};

class IOManager{
public:
  enum MATRIX_MODE {
      POLYWORKS //! <\brief Matrix file to be loaded and applied to polyworks layers
  };

public:
  // Obj read/write simple functions.
  template<typename Scalar>
  bool ReadObject(const char *name,
                  std::vector<gr::Point3D<Scalar> > &v,
                  std::vector<Eigen::Matrix2f> &tex_coords,
                  std::vector<typename gr::Point3D<Scalar>::VectorType> &normals,
                  std::vector<tripple> &tris,
                  std::vector<std::string> &mtls);           

  template<typename PointRange,
           typename TextCoordRange,
           typename NormalRange,
           typename TrisRange,
           typename MTLSRange>
  bool WriteObject(const char *name,
                   const PointRange &v,
                   const TextCoordRange &tex_coords,
                   const NormalRange &normals,
                   const TrisRange &tris,
                   const MTLSRange &mtls);

  bool WriteMatrix(const std::string& name,
                   const Eigen::Ref<const Eigen::Matrix<double, 4, 4> >& mat,
                   MATRIX_MODE mode);
private:
  template<typename Scalar>
  bool
  ReadPly(const char *name,
          std::vector<gr::Point3D<Scalar> > &v,
          std::vector<typename gr::Point3D<Scalar>::VectorType> &normals);

  /*!
   * \brief ReadPtx
   * \param name
   * \param v
   * \return
   *
   * \note Transformations declared in file are ignored
   *
   * Implementation inspired by
   *            http://github.com/adasta/pcl_io_extra/blob/master/src/ptx_io.cpp
   */
  template<typename Scalar>
  bool
  ReadPtx(const char *name,
          std::vector<gr::Point3D<Scalar> > &v);

  template<typename Scalar>
  bool
  ReadObj(const char *name,
          std::vector<gr::Point3D<Scalar> > &v,
          std::vector<Eigen::Matrix2f> &tex_coords,
          std::vector<typename gr::Point3D<Scalar>::VectorType> &normals,
          std::vector<tripple> &tris,
          std::vector<std::string> &mtls);

  template<typename PointRange, typename NormalRange>
  bool
  WritePly(std::string name,
           const PointRange &v,
           const NormalRange &normals);

  template<typename PointRange,
           typename TexCoordRange,
           typename NormalRange,
           typename TrisRange,
           typename MTLSRange>
  bool
  WriteObj(std::string name,
           const PointRange &v,
           const TexCoordRange &tex_coords,
           const NormalRange &normals,
           const TrisRange &tris,
           const MTLSRange &mtls);
           
  /*!
   * \brief formatPolyworksMatrix Format 4x4 matrice so it can be loaded by polyworks
   * \param mat
   * \param sstr
   * \return
   */
  std::ofstream &
  formatPolyworksMatrix(const Eigen::Ref<const Eigen::Matrix<double, 4, 4> >& mat,
                        std::ofstream &sstr);

  /// Wrapped STBI functions to be used inside template methods
  /// Limits dependency on stb just to compilation of the library by compiling
  /// required stbi methods to object files at library compilation.
  unsigned char*
  stbi_load_(char const *filename, int *x, int *y, int *comp, int req_comp);

  void
  stbi_image_free_(void *retval_from_stbi_load);
  
  const char *
  stbi_failure_reason_(void);

  static const int STBI_rgb = 3;
}; // class IOMananger

#include "io.hpp"
#include "io_ply.h"
#endif // IO_H
