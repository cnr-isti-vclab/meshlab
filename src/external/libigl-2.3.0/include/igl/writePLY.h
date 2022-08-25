#ifndef IGL_WRITEPLY_H
#define IGL_WRITEPLY_H
#include <igl/igl_inline.h>
#include <igl/FileEncoding.h>

#include <string>
#include <iostream>
#include <vector>
#include <Eigen/Core>


namespace igl
{
  // write triangular mesh to ply file
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  ply_stream  ply file output stream
  //   V  (#V,3) matrix of vertex positions
  //   F  (#F,3) list of face indices into vertex positions
  //   E  (#E,2) list of edge indices into vertex positions
  //   N  (#V,3) list of normals
  //   UV (#V,2) list of texture coordinates
  //   VD (#V,*) additional vertex data
  //   Vheader (#V) list of vertex data headers
  //   FD (#F,*) additional face data
  //   Fheader (#F) list of face data headers
  //   ED (#E,*) additional edge data
  //   Eheader (#E) list of edge data headers
  //   comments (*) file comments
  //   encoding - enum, to set binary or ascii file format
  // Returns true on success, false on errors
  template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE,
  typename DerivedN,
  typename DerivedUV,
  typename DerivedVD,
  typename DerivedFD,
  typename DerivedED
>
bool writePLY(
  std::ostream & ply_stream,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedE> & E,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV,

  const Eigen::MatrixBase<DerivedVD> & VD,
  const std::vector<std::string> & VDheader,

  const Eigen::MatrixBase<DerivedFD> & FD,
  const std::vector<std::string> & FDheader,

  const Eigen::MatrixBase<DerivedED> & ED,
  const std::vector<std::string> & EDheader,

  const std::vector<std::string> & comments,
  FileEncoding encoding
   );

  // write triangular mesh to ply file
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  filename  ply file name
  //   V  (#V,3) matrix of vertex positions
  //   F  (#F,3) list of face indices into vertex positions
  //   E  (#E,2) list of edge indices into vertex positions
  //   N  (#V,3) list of normals
  //   UV (#V,2) list of texture coordinates
  //   VD (#V,*) additional vertex data
  //   Vheader (#V) list of vertex data headers
  //   FD (#F,*) additional face data
  //   Fheader (#F) list of face data headers
  //   ED (#E,*) additional edge data
  //   Eheader (#E) list of edge data headers
  //   comments (*) file comments
  //   encoding - enum, to set binary or ascii file format
  // Returns true on success, false on errors
template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE,
  typename DerivedN,
  typename DerivedUV,
  typename DerivedVD,
  typename DerivedFD,
  typename DerivedED
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedE> & E,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV,

  const Eigen::MatrixBase<DerivedVD> & VD,
  const std::vector<std::string> & VDheader,

  const Eigen::MatrixBase<DerivedFD> & FD,
  const std::vector<std::string> & FDheader,

  const Eigen::MatrixBase<DerivedED> & ED,
  const std::vector<std::string> & EDheader,

  const std::vector<std::string> & comments,
  FileEncoding encoding
   );

template <
  typename DerivedV,
  typename DerivedF
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F
   );

template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedF> & E
   );


template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedN,
  typename DerivedUV
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV
   );


template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE,
  typename DerivedN,
  typename DerivedUV
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedE> & E,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV
   );


template <
  typename DerivedV,
  typename DerivedF
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  FileEncoding encoding
   );

template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedE> & E,
  FileEncoding encoding
   );

template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedN,
  typename DerivedUV,
  typename DerivedVD
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV,
  const Eigen::MatrixBase<DerivedVD> & VD=Eigen::MatrixXd(0,0),
  const std::vector<std::string> & VDheader={},
  const std::vector<std::string> & comments={}
   );

template <
  typename DerivedV,
  typename DerivedF,
  typename DerivedE,
  typename DerivedN,
  typename DerivedUV,
  typename DerivedVD
>
bool writePLY(
  const std::string & filename,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<DerivedE> & E,
  const Eigen::MatrixBase<DerivedN> & N,
  const Eigen::MatrixBase<DerivedUV> & UV,
  const Eigen::MatrixBase<DerivedVD> & VD=Eigen::MatrixXd(0,0),
  const std::vector<std::string> & VDheader={},
  const std::vector<std::string> & comments={}
   );

}



#ifndef IGL_STATIC_LIBRARY
#  include "writePLY.cpp"
#endif
#endif
