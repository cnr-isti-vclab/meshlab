#scripts folder

This folder contains a series of platform-dependent scripts to build and deploy MeshLab.

Every platform folder contains:

* `1_build.sh`: a script that builds MeshLab. Requires a Qt environment properly set, with `cmake` accessible. Takes as inputh the build directory (default: `src/build`), the install directory (default: `src/install`) and the number of cores used to build MeshLab (default: `-j4`). Example of call: `bash 1_build.sh --build_path=path/to/build --install_path=path/to/install -j8`
* `2_deploy.sh`: a script that deploys MeshLab, making sure that the folder will be a self-contained MeshLab, without Qt library dependencies. Requires a properly built MeshLab in the directory passed as an argument (default: `src/install`). Example of call: `bash 2_deploy.sh --install_path=path/to/deploy`
* `3_$installer$.sh`: a script that computes a platform-dependent self-contained package/installer of MeshLab. Requires a properly deployed MeshLab in the directory passed as an argument (default: `src/install`). The result will be saved in the parent directory of the given argument.
* `make_it.sh`: a script that computes all the three previous scripts: starting from the source code, it will produce a self-contained package/installer of MeshLab (arguments are the same of `1_build.sh`). Example of call: `bash makeit.sh --build_path=path/to/build --install_path=path/to/install -j8`

Note for Windows: scripts are written in bash, therefore they should be run in a linux subsystem environment.
See in each platform subfolder for details and other platform-dependent requirements:
- [Linux](https://github.com/cnr-isti-vclab/meshlab/tree/master/scripts/Linux)
- [MaxOS](https://github.com/cnr-isti-vclab/meshlab/tree/master/scripts/macOS)
- [Windows](https://github.com/cnr-isti-vclab/meshlab/tree/master/scripts/Windows)
