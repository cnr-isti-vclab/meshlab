// high level interface for MshLoader.h/.cpp

// Copyright (C) 2020 Vladimir Fonov <vladimir.fonov@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distribute
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_READ_MSH_H
#define IGL_READ_MSH_H
#include "igl_inline.h"

#include <Eigen/Core>
#include <string>
#include <vector>


namespace igl 
{
    // read triangle surface mesh and tetrahedral volume mesh from .msh file
    // Inputs:
    //   msh - file name
    // Outputs: 
    //   X  eigen double matrix of vertex positions  #X by 3
    //   Tri  #Tri eigen integer matrix of triangular faces indices into vertex positions
    //   Tet  #Tet eigen integer matrix of tetrahedral indices into vertex positions
    //   TriTag #Tri eigen integer vector of tags associated with surface faces
    //   TetTag #Tet eigen integer vector of tags associated with volume elements
    //   XFields #XFields list of strings with field names associated with nodes
    //   XF      #XFields list of eigen double matrices, fields associated with nodes 
    //   EFields #EFields list of strings with field names associated with elements
    //   TriF    #EFields list of eigen double matrices, fields associated with surface elements
    //   TetF    #EFields list of eigen double matrices, fields associated with volume elements
    // Known bugs: 
    //     only version 2.2 of .msh file is supported (gmsh 3.X)
    //     only triangle surface elements and tetrahedral volumetric elements are supported
    //     only 3D information is supported
    //     only the 1st tag per element is returned (physical) 
    //     same element fields are expected to be associated with surface elements and volumetric elements
    IGL_INLINE bool readMSH(const std::string &msh,
                Eigen::MatrixXd &X,
                Eigen::MatrixXi &Tri,
                Eigen::MatrixXi &Tet,
                Eigen::VectorXi &TriTag,
                Eigen::VectorXi &TetTag,
                std::vector<std::string> &XFields,
                std::vector<Eigen::MatrixXd> &XF,
                std::vector<std::string> &EFields,
                std::vector<Eigen::MatrixXd> &TriF,
                std::vector<Eigen::MatrixXd> &TetF
                );

    // read triangle surface mesh and tetrahedral volume mesh from .msh file
    // ignoring any fields
    // Inputs:
    //   msh - file name
    // Outputs: 
    //   X  eigen double matrix of vertex positions  #X by 3
    //   Tri  #Tri eigen integer matrix of triangular faces indices into vertex positions
    //   Tet  #Tet eigen integer matrix of tetrahedral indices into vertex positions
    //   TriTag #Tri eigen integer vector of tags associated with surface faces
    //   TetTag #Tet eigen integer vector of tags associated with volume elements
    IGL_INLINE bool readMSH(const std::string &msh,
                Eigen::MatrixXd &X,
                Eigen::MatrixXi &Tri,
                Eigen::MatrixXi &Tet,
                Eigen::VectorXi &TriTag,
                Eigen::VectorXi &TetTag
                );
    
    // read triangle surface mesh and tetrahedral volume mesh from .msh file
    // ignoring any fields, and any volumetric elements
    // Inputs:
    //   msh - file name
    // Outputs: 
    //   X  eigen double matrix of vertex positions  #X by 3
    //   Tri  #Tri eigen integer matrix of triangular faces indices into vertex positions
    //   TriTag #Tri eigen integer vector of tags associated with surface faces
    IGL_INLINE bool readMSH(const std::string &msh,
                Eigen::MatrixXd &X,
                Eigen::MatrixXi &Tri,
                Eigen::VectorXi &TriTag
                );
    
    // read triangle surface mesh and tetrahedral volume mesh from .msh file
    // ignoring any fields, and any volumetric elements and tags
    // Inputs:
    //   msh - file name
    // Outputs: 
    //   X  eigen double matrix of vertex positions  #X by 3
    //   Tri  #Tri eigen integer matrix of triangular faces indices into vertex positions
    IGL_INLINE bool readMSH(const std::string &msh,
                Eigen::MatrixXd &X,
                Eigen::MatrixXi &Tri
                );

}


#ifndef IGL_STATIC_LIBRARY
#  include "readMSH.cpp"
#endif

#endif //IGL_READ_MSH_H
