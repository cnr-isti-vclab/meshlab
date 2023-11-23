# ![MeshLab Logo](src/meshlab/images/eye64.png) MeshLab


[![BuildMeshLab](https://github.com/cnr-isti-vclab/meshlab/actions/workflows/BuildMeshLab.yml/badge.svg)](https://github.com/cnr-isti-vclab/meshlab/actions/workflows/BuildMeshLab.yml)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5114037.svg)](https://doi.org/10.5281/zenodo.5114037)

This is the official repository for the source and the binaries of [MeshLab](https://www.MeshLab.net).

MeshLab is an open source, portable, and extensible system for the processing and editing of unstructured large 3D triangular meshes. It is aimed to help the processing of the typical not-so-small unstructured models arising in 3D scanning, providing a set of tools for editing, cleaning, healing, inspecting, rendering and converting this kind of meshes.

MeshLab is mostly based on the open source C++ mesh processing library [VCGlib](http://www.vcglib.net) developed at the [Visual Computing Lab](http://vcg.isti.cnr.it) of [ISTI - CNR](http://www.isti.cnr.it). VCG can be used as a stand-alone large-scale automated mesh processing pipeline, while MeshLab makes it easy to experiment with its algorithms interactively.

MeshLab is available for Windows, macOS, and Linux.

# Releases

You can find the last MeshLab release in the [Releases Tab](https://github.com/cnr-isti-vclab/meshlab/releases) for your favourite platform.
You can also test the last nightly version of MeshLab, by downloading the artifacts of the last [Github Actions](https://github.com/cnr-isti-vclab/meshlab/actions) workflow. You can find a guide on how to download it [here](https://github.com/cnr-isti-vclab/meshlab/wiki/How-to-install-the-last-nightly-version).

# Build instructions

We provide a set of scripts that build and deploy MeshLab automatically. All the scripts can be found in the [scripts](https://github.com/cnr-isti-vclab/meshlab/tree/main/scripts) folder.
For specific build instructions see the [src](https://github.com/cnr-isti-vclab/meshlab/blob/main/src/README.md) folder.

# Branches and Pull Requests

On this repository, we keep two relevant branches:

- `main`: we keep here the **last release version of MeshLab**, with just bugfixes, optimizations and changes that do not affect the last released MeshLab binary. In case of bugs caught after a release, this branch is used to publish "post-releases".
- `devel`: we add here new features and functionalities of the software that are going to appear in the next release of MeshLab. Bugfixes pushed in `main` are automatically merged into the `devel` branch.

Before committing a new change, or sending a pull request, please choose the branch that better suits for your change. 
If it is just a bugfix, or an edit that does not modify the software (e.g. documentation typos, changes to the deploy system, ...), use the `main` branch. 
If it is a new feature that will change the behaviour or the usage of the software, use the `devel` branch.

# Structure of the Repository

The MeshLab repository is organized as follows:

* `docs`: doxygen scripts for generating MeshLab documentation. For more details, check the readme [here](https://github.com/cnr-isti-vclab/meshlab/tree/master/docs);
* `sample` and `textures`: a set of files (meshes, images) used for tests;
* `resources`: contains a set of files used by the software and by the deploy system to produce the final MeshLab binary;
* `scripts`: in this folder there is a set of platform-dependent scripts to build and deploy MeshLab. For more details, check the readme [here](https://github.com/cnr-isti-vclab/meshlab/tree/master/scripts/README.md);
* `src`: this folder contains all the source code of MeshLab and its plugins. For more details, check the readme [here](https://github.com/cnr-isti-vclab/meshlab/blob/master/src/README.md);
* `unsupported`: this folder contains a set of old and unsupported MeshLab plugins that are no longer included and built under MeshLab.

# License

 The Meshlab source is released under the [GPL License](LICENSE.txt).

# Copyright

```
   MeshLab
   http://www.meshlab.net
   All rights reserved.

   VCGLib  http://www.vcglib.net                                     o o
   Visual and Computer Graphics Library                            o     o
                                                                  _   O  _
   Paolo Cignoni                                                    \/)\/
   Visual Computing Lab  http://vcg.isti.cnr.it                    /\/|
   ISTI - Italian National Research Council                           |
   Copyright(C) 2005-2021                                             \
```

# References

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5114037.svg)](https://doi.org/10.5281/zenodo.5114037)


Please, when using this tool, cite the references listed in the official web page https://www.meshlab.net/#references according to you needs, or if you are lazy just cite:

```
@software{meshlab,
  author       = {Paolo Cignoni, Alessandro Muntoni, Guido Ranzuglia, Marco Callieri},
  title        = {{MeshLab}},
  publisher    = {Zenodo},
  doi          = {10.5281/zenodo.5114037}
}

@inproceedings {LocalChapterEvents:ItalChap:ItalianChapConf2008:129-136,
  booktitle = {Eurographics Italian Chapter Conference},
  editor    = {Vittorio Scarano and Rosario De Chiara and Ugo Erra},
  title     = {{MeshLab: an Open-Source Mesh Processing Tool}},
  author    = {Cignoni, Paolo and Callieri, Marco and Corsini, Massimiliano and Dellepiane, Matteo and Ganovelli, Fabio and Ranzuglia, Guido},
  year      = {2008},
  publisher = {The Eurographics Association},
  ISBN      = {978-3-905673-68-5},
  DOI       = {10.2312/LocalChapterEvents/ItalChap/ItalianChapConf2008/129-136}
}
```

# Contacts

 - Paolo Cignoni (paolo.cignoni (at) isti.cnr.it)
 - Alessandro Muntoni (alessandro.muntoni (at) isti.cnr.it)

# Feedback

For documented and repeatable bugs, feature requests, etc., please use the [GitHub issues](https://github.com/cnr-isti-vclab/meshlab/issues).

For general questions use [StackOverflow](http://stackoverflow.com/questions/tagged/meshlab).
