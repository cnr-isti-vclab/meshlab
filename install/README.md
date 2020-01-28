# install folder

**Work in Progress**

This folder contains a series of platform-dependent scripts to build and deploy meshlab.

Every platform folder contains:

* `$platform$-build.[sh/ps1]`: a script that builds MeshLab. Requires a Qt environment properly set, with `qmake` accessible. The build will be placed inside the directory passed as an argument, or in the `distrib` folder if arguments were not provided.
* `$platform$-deploy.[sh/ps1]`: a script that deploys MeshLab, making sure that the  folder will be a self-contained MeshLab, without Qt library dependencies. Requires a properly built MeshLab in the directory passed as an argument, or in the `distrib` folder if arguments were not provided.
* `$platform$-install.[sh/ps1]`: a script that computes a self-contained package/installer of MeshLab. Requires a properly deployed MeshLab in the directory passed as an argument, or in the `distrib` folder if arguments were not provided. The result will be saved in the same directory.
* `$platform$-make_it.[sh/ps1]`: a script that computes all the three previous scripts: starting from the source code, it will produce a self-contained package/installer of MeshLab.

See in each platform subfolder if there are some other platform-dependent requirements.