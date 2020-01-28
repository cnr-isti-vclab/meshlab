# ![MeshLab Logo](src/meshlab/images/eye64.png) MeshLab


[![Actions Status](https://github.com/cnr-isti-vclab/meshlab/workflows/CompileUbuntu/badge.svg)](https://github.com/cnr-isti-vclab/meshlab/actions)
[![Actions Status](https://github.com/cnr-isti-vclab/meshlab/workflows/CompileMacOS/badge.svg)](https://github.com/cnr-isti-vclab/meshlab/actions)
[![Actions Status](https://github.com/cnr-isti-vclab/meshlab/workflows/CompileWindows/badge.svg)](https://github.com/cnr-isti-vclab/meshlab/actions)

This is the official repository for the source and the binaries of MeshLab. 

MeshLab is an open source, portable, and extensible system for the processing and editing of unstructured large 3D triangular meshes. It is aimed to help the processing of the typical not-so-small unstructured models arising in 3D scanning, providing a set of tools for editing, cleaning, healing, inspecting, rendering and converting this kind of meshes. These tools include MeshLab proper, a versatile program with a graphical user interface, and [meshlabserver](https://github.com/cnr-isti-vclab/meshlab/blob/master/src/meshlabserver/meshlabserver.txt), a program that can perform mesh processing tasks in batch mode, without a GUI. 

MeshLab is mostly based on the open source C++ mesh processing library [VCGlib](http://www.vcglib.net) developed at the [Visual Computing Lab](http://vcg.isti.cnr.it) of [ISTI - CNR](http://www.isti.cnr.it). VCG can be used as a stand-alone large-scale automated mesh processing pipeline, while MeshLab makes it easy to experiment with its algorithms interactively.

MeshLab is available for Windows, MacOSX, and Linux.

# Build instructions

MeshLab requires [Qt](https://www.qt.io/) >= 5.9, with "script" and "xmlpatterns" as additional packages.

After setting up the Qt environment:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	cd src
	qmake
	make

Compiling instructions can be found in the [src](https://github.com/cnr-isti-vclab/meshlab/tree/master/src) folder or simply looking at the continous integration setup on [Github Actions](https://github.com/cnr-isti-vclab/meshlab/actions), [travis](https://travis-ci.org/cnr-isti-vclab/meshlab/builds) and [appveyor](https://ci.appveyor.com/project/cignoni/meshlab/history).
 
# License

 The Meshlab source is released under the [GPL License](LICENSE.txt).
 
# Copyright

```
   MeshLab
   http://www.meshlab.net
   All rights reserved.

   VCGLib  http://www.vcglib.net                                         o o
   Visual and Computer Graphics Library                            o     o
                                                                  _   O  _
   Paolo Cignoni                                                    \/)\/
   Visual Computing Lab  http://vcg.isti.cnr.it                    /\/|
   ISTI - Italian National Research Council                           |
   Copyright(C) 2005-2018                                             \
```

# References

Please, when using this tool, cite the following reference:

Meshlab: an open-source mesh processing tool. P. Cignoni, M. Callieri, M. Corsini, M. Dellepiane, F. Ganovelli, G. Ranzuglia
Proceedings of the 2008 Eurographics Italian Chapter Conference, ISBN: 978-3-905673-68-5, pp. 129-136, DOI: 10.2312/LocalChapterEvents/ItalChap/ItalianChapConf2008/129-136

BibTeX format:

```
@inproceedings {LocalChapterEvents:ItalChap:ItalianChapConf2008:129-136,
booktitle = {Eurographics Italian Chapter Conference},
editor = {Vittorio Scarano and Rosario De Chiara and Ugo Erra},
title = {{MeshLab: an Open-Source Mesh Processing Tool}},
author = {Cignoni, Paolo and Callieri, Marco and Corsini, Massimiliano and Dellepiane, Matteo and Ganovelli, Fabio and Ranzuglia, Guido},
year = {2008},
publisher = {The Eurographics Association},
ISBN = {978-3-905673-68-5},
DOI = {10.2312/LocalChapterEvents/ItalChap/ItalianChapConf2008/129-136}
}
```

# Contacts

 - Paolo Cignoni (p.cignoni (at) isti.cnr.it)
 - Guido Ranzuglia (g.ranzuglia (at) isti.cnr.it)

# Feedback

For documented and repeatable bugs, feature requests, etc., please use the [GitHub issues](https://github.com/cnr-isti-vclab/meshlab/issues).

For general questions use [StackOverflow](http://stackoverflow.com/questions/tagged/meshlab).



