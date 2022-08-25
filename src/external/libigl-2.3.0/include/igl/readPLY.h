#ifndef IGL_READPLY_H
#define IGL_READPLY_H
#include <igl/igl_inline.h>
#include <Eigen/Core>
#include <string>
#include <vector>
#include "tinyply.h"

namespace igl
{
template <
  typename DerivedV,
  typename DerivedF
  >
IGL_INLINE bool readPLY(
  FILE *fp,
  Eigen::PlainObjectBase<DerivedV> & V,
  Eigen::PlainObjectBase<DerivedF> & F
  );


// Read triangular mesh from ply file, filling in vertex positions, normals
  // and texture coordinates, if available
  // also read additional properties associated with vertex,faces and edges 
  // and file comments
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  ply_stream  ply file input stream
  // Outputs:
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
  bool readPLY(
    std::istream & ply_stream,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedF> & E,
    Eigen::PlainObjectBase<DerivedN> & N,
    Eigen::PlainObjectBase<DerivedUV> & UV,

    Eigen::PlainObjectBase<DerivedVD> & VD,
    std::vector<std::string> & Vheader,
    Eigen::PlainObjectBase<DerivedFD> & FD,
    std::vector<std::string> & Fheader,
    Eigen::PlainObjectBase<DerivedED> & ED,
    std::vector<std::string> & Eheader,
    std::vector<std::string> & comments
    );

  // Read triangular mesh from ply file, filling in vertex positions, normals
  // and texture coordinates, if available
  // also read additional properties associated with vertex,faces and edges 
  // and file comments
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  ply_file  ply file name
  // Outputs:
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
  bool readPLY(
    const std::string& ply_file,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedE> & E,
    Eigen::PlainObjectBase<DerivedN> & N,
    Eigen::PlainObjectBase<DerivedUV> & UV,

    Eigen::PlainObjectBase<DerivedVD> & VD,
    std::vector<std::string> & VDheader,

    Eigen::PlainObjectBase<DerivedFD> & FD,
    std::vector<std::string> & FDheader,

    Eigen::PlainObjectBase<DerivedED> & ED,
    std::vector<std::string> & EDheader,
    std::vector<std::string> & comments
    );


  // Read triangular mesh from ply file, filling in vertex positions, normals
  // and texture coordinates, if available
  // also read additional properties associated with vertex,faces and edges 
  // and file comments
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  ply_file  ply file name
  // Outputs:
  //   V  (#V,3) matrix of vertex positions 
  //   F  (#F,3) list of face indices into vertex positions
  //   N  (#V,3) list of normals
  //   UV (#V,2) list of texture coordinates
  //   VD (#V,*) additional vertex data
  //   Vheader (#V) list of vertex data headers
  // Returns true on success, false on errors
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedN,
    typename DerivedUV,
    typename DerivedVD
    >
  bool readPLY(
    const std::string & filename,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedN> & N,
    Eigen::PlainObjectBase<DerivedUV> & UV,
    Eigen::PlainObjectBase<DerivedVD> & VD,
    std::vector<std::string> & Vheader
    );

  // Read triangular mesh from ply file, filling in vertex positions, normals
  // and texture coordinates, if available
  // also read additional properties associated with vertex,faces and edges 
  // and file comments
  //
  // Templates:
  //   Derived from Eigen matrix parameters
  // Inputs:
  //  ply_file  ply file name
  // Outputs:
  //   V  (#V,3) matrix of vertex positions 
  //   F  (#F,3) list of face indices into vertex positions
  //   E  (#E,2) list of edge indices into vertex positions
  //   N  (#V,3) list of normals
  //   UV (#V,2) list of texture coordinates
  //   VD (#V,*) additional vertex data
  //   Vheader (#V) list of vertex data headers
  // Returns true on success, false on errors
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedE,
    typename DerivedN,
    typename DerivedUV
    >
  bool readPLY(
    const std::string & filename,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedE> & E,
    Eigen::PlainObjectBase<DerivedN> & N,
    Eigen::PlainObjectBase<DerivedUV> & UV
    );


  template <
    typename DerivedV,
    typename DerivedF
    >
  bool readPLY(
    const std::string & filename,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F
    );

  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedE
    >
  bool readPLY(
    const std::string & filename,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedE> & E
    );

}

#ifndef IGL_STATIC_LIBRARY
#  include "readPLY.cpp"
#endif
#endif
