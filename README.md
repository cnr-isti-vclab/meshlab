# ![MeshLab Logo](src/meshlab/images/eye64.png) MeshLab by Dynart

*A custom fork of [MeshLab](https://www.MeshLab.net) with enhanced selection workflow and usability improvements.*

---

Based on MeshLab — an open source, portable, and extensible system for the processing and editing of unstructured large 3D triangular meshes. Built on top of the [VCGlib](http://www.vcglib.net) C++ mesh processing library developed at the [Visual Computing Lab](http://vcg.isti.cnr.it) of [ISTI - CNR](http://www.isti.cnr.it).

## New Features

### Inverted CTRL Selection Behavior
The default MeshLab selection workflow requires holding CTRL to **add** to an existing selection, while a bare drag **replaces** the selection. This fork inverts that behavior:
- **Drag** now **adds** to the current selection by default
- **CTRL + Drag** starts a **new** selection (clearing the previous one)
- This is configurable via a global parameter (`Invert CTRL Behavior`) accessible in MeshLab settings, so you can switch back to the original behavior at any time

This change applies to all rectangle-based selection tools: **Select Faces**, **Select Vertices**, **Select Connected Components**, and **Select Faces/Vertices inside polyline area**.

### Global Parameter System for Edit Plugins
Edit plugins now support persistent global parameters through the MeshLab settings system. This allows edit tools to expose user-configurable options that are saved across sessions. The `EditPlugin` base class provides `initGlobalParameterList` and `setCurrentGlobalParamSet` for any plugin to register its own settings.

### Lasso Cut Tool (`edit_cut`)
A new editing tool that draws a closed polyline to cut through mesh triangles and select the inside region:
- **Click** to add polyline points
- **Q** — cut edges along the polyline and add inside faces to selection
- **W** — subtract inside faces from selection
- **D** / **A** / **I** — deselect all / select all / invert selection
- **Delete** — delete selected faces (uses MeshLab's standard filter with full undo/redo support)
- **C** — clear polyline, **Backspace** — undo last point, **Escape** — clear polyline

The tool appears in its own **Dynart Tools** toolbar section, separate from the standard Edit toolbar.

### Global Undo/Redo (Ctrl+Z / Ctrl+Y)
MeshLab has no built-in undo system. This fork adds a full-mesh snapshot undo/redo stack that works across **all filters and edit tools**:
- **Ctrl+Z** — undo the last mesh-modifying operation
- **Ctrl+Y** — redo the last undone operation
- Available in the **Edit** menu as Undo/Redo entries
- Supports up to **10 undo levels**
- Covers all filter operations (delete, remesh, smooth, clean, etc.) and the Lasso Cut tool

### Reload Confirmation Dialogs
Added confirmation prompts before reloading meshes (`Reload` and `Reload All`) to prevent accidental loss of unsaved work.

---

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
   https://www.meshlab.net
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
  author       = {Cignoni, Paolo and Muntoni, Alessandro and Ranzuglia, Guido and Callieri, Marco},
  title        = {MeshLab},
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
