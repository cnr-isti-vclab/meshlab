
   VCGLib  http://vcg.sf.net                                         o o
   Visual and Computer Graphics Library                            o     o
                                                                  _   O  _
   Copyright(C) 2005-2006                                           \/)\/
   Visual Computing Lab  http://vcg.isti.cnr.it                    /\/|
   ISTI - Italian National Research Council                           |
                                                                      \
   MeshLab 0.4 2005/12/24
   http://meshlab.sourceforge.net/
   All rights reserved.


This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
(http://www.gnu.org/licenses/gpl.txt) for more details.

--- General Info ---

MeshLab is an extendible mesh processing system, aimed to the user
assisted cleaning, filtering, editing and rendering of unstructured 3D
triangular meshes.
The tool found its natural use for the processiing of the meshes that usually 
arise in 3d scanning. The system is heavily based on
the VCG library (vcg.sf.net). The current version of Meshlab was developed during 
the FGT (Foundation of Three-dimensional Graphics) course of the Computer Science 
department ofUniversity of Pisa. 

--- Main Features ---

Input/output in many formats (PLY, STL, OFF, OBJ)
Extendible plugins based architecture, writing new mesh 
processing functions, filters and format support is quite easy.
Cleaning Filters
	Removal of duplicated, unreferenced vertices, null faces
	Subdivision filters
Opengl Shader based rendering 
Large snapshot for hi quality printing
Various Colorization Filters 
	Gaussian Curvature
	Border edges

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


-- Authors	--

P. Cignoni and
(in strict alphabetic order)
A. Buzzelli, L. Colombini, G. Gangemi, M. Latronico, F. Mazzanti,
A. Mochi, D. Portelli, D. Vacca, F. Vannini, A. Venturi

--- Extra Credits ---

Thanks goes to
Christopher January, Dave Brondsema, Christian Ehrlicher, Ralf Habacker
for their unofficial patch for Qt4 ( http://sourceforge.net/projects/qtwin ).
