   MeshLab
   http://meshlab.sourceforge.net/
   All rights reserved.

   VCGLib  http://vcg.sf.net                                         o o
   Visual and Computer Graphics Library                            o     o
                                                                  _   O  _
   Copyright(C) 2005-2006                                           \/)\/
   Visual Computing Lab  http://vcg.isti.cnr.it                    /\/|
   ISTI - Italian National Research Council                           |
                                                                      \


This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
(http://www.gnu.org/licenses/gpl.txt) for more details.

--- General Info ---

MeshLab is an open source portable and extendible system for the
processing and editing of unstructured 3D triangular meshes. The system is
aimed to help the processing of the typical not-so-small unstructured
models arising in 3D scanning, providing a set of tools for editing,
cleaning, healing, inspecting, rendering and converting this kind of
meshes.

The project is supported by the European Network of Excellence EPOCH

The system is heavily based on the VCG library developed at the Visual
Computing Lab of ISTI - CNR, for all the core mesh processing tasks. The
MeshLab system started as a part of the FGT course of the Computer Science
department of University of Pisa and most of the code (~15k lines) of the
first versions was written by a handful of students.

--- Main Features ---
# Interactive selection and deletion of portion of the mesh. Even for
  large models.
# Input/output in many formats (PLY, STL, OFF, OBJ, 3DS)
# Extendible plugins based architecture, writing new mesh processing
  functions, colorization filters and support for different file formats is
  quite easy (look at this code sample)
# Cleaning Filters:
    * removal of duplicated, unreferenced vertices, null faces
    * subdivision filters (midpoint, loop and butterfly)
    * coherent normal unification and flipping
# OpenGL Shader based rendering (write your own shader!) compatible with
  Typhoon Labs Shader Designer
# Large rendering (up to 16k x 16k) for high quality printing
# Various Colorization/inspecting Filters
    * Gaussian and mean curvature
    * Border edges
    * Non two-manifold faces
# The history of the all performed cleaning/editing actions can be
  re-played on different meshes or saved and for archival purposes.

--- References ----

Please, when using this tool, cite the following reference:

MeshLab
Visual Computing Lab - ISTI - CNR
http://meshlab.sourceforge.net/

@misc{Meshlab
    author = {Visual Computing Lab  ISTI - CNR},
    title = {MeshLab},
    note = {http://meshlab.sourceforge.net/}
}

For any general question about the use of this software please contact:
Paolo Cignoni ( p.cignoni@isti.cnr.it )

For bugs, feature requests and such, please use the sourceforge service
http://sourceforge.net/tracker/?group_id=149444


-- Authors  --

P. Cignoni and
(in strict alphabetic order)
A. Buzzelli, L. Colombini, G. Gangemi, M. Latronico, F. Mazzanti,
A. Mochi, D. Portelli, D. Vacca, F. Vannini, A. Venturi
