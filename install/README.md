# install folder

**Work in Progress**

This folder contains a series of platform-dependent scripts to build and deploy meshlab.

Every platform folder contains:

* `$platform$-build.[sh/ps1]`: a script that builds MeshLab. Requires a Qt environment properly set, with `qmake` accessible. Binaries will be placed inside `$1/distrib`, where `$1` is the argument that represents the build directory, or in the [distrib](https://github.com/cnr-isti-vclab/meshlab/tree/master/distrib) folder if arguments were not provided.
* `$platform$-deploy.[sh/ps1]`: a script that deploys MeshLab, making sure that the folder will be a self-contained MeshLab, without Qt library dependencies. Requires a properly built MeshLab in the directory passed as an argument, or in the [distrib](https://github.com/cnr-isti-vclab/meshlab/tree/master/distrib) folder if arguments were not provided.
* `$platform$-$installer$.[sh/ps1]`: a script that computes a self-contained package/installer of MeshLab. Requires a properly deployed MeshLab in the directory passed as an argument, or in the [distrib](https://github.com/cnr-isti-vclab/meshlab/tree/master/distrib) folder if arguments were not provided. The result will be saved in the same directory.
* `$platform$-make_it.[sh/ps1]`: a script that computes all the three previous scripts: starting from the source code, it will produce a self-contained package/installer of MeshLab.
* various other files used by the scripts listed above.

See in each platform subfolder for details and other platform-dependent requirements:

- [Linux](https://github.com/cnr-isti-vclab/meshlab/tree/master/install/linux)
- [MaxOS](https://github.com/cnr-isti-vclab/meshlab/tree/master/install/macos)
- [Windows](https://github.com/cnr-isti-vclab/meshlab/tree/master/install/windows)