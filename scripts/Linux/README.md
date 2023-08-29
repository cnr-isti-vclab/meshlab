# Linux Scripts

This folder contains a series of scripts to build and deploy MeshLab under a Linux environment (tested on Ubuntu 18.04, 20.04 and 22.04).

## Note about Qt

Old versions of Ubuntu (< than 22.04) are shipped with old versions of Qt, but MeshLab requires Qt 5.15.
Therefore, you must install manually Qt separatelly in your system. You can then give the path of the Qt installation
directory to the various scripts, or you can add Qt to your `LD_LIBRARY_PATH`.

The `0_setup_env.sh` script won't install qt from apt if you pass the argument `dont_install_qt`.

## Dependencies 

Dependencies are automatically installed by the `0_setup_env.sh` script, which uses [`apt-get`](https://linux.die.net/man/8/apt-get) as package manager.
Be sure to have `apt-get` installed before running this script.

Libraries installed by the `0_setup_env.sh` are the following:
  - Required by MeshLab:
    - `mesa-common-dev` 
	- `libglu1-mesa-dev`
	- `cmake`
	- `ninja-build`
	- `patchelf` (for deploy stage)
  - Required by Qt5:
    - `libxcb-icccm4-dev` 
	- `libxcb-image0-dev` 
	- `libxcb-keysyms1-dev` 
	- `libxcb-render-util0-dev` 
	- `libxcb-xinerama0-dev`
  - Optional:
    - `libgmp-dev` and `libmpfr-dev` (required by cgal)
	- `libxerces-c-dev` (required by libe57)
	- `libcgal-dev` and `libboost-all-dev` (required by several MeshLab plugins)

The script won't install `cgal` and `boost` (and then they will be downloaded by `cmake` during configuration) if you pass the argument `--dont_install_cgal_and_boost`. 

## Note about deployment and AppImage packaging on Linux

To deploy MeshLab and make it portable, we use the tool [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy). The authors of this tool suggest to use the last still-supported LTS version of Linux when deploying and generating an AppImage, in order to provide wide support of the application.