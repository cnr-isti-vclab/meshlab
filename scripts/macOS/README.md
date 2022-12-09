# MacOS Scripts

This folder contains a series of scripts to build and deploy MeshLab under a MacOS x86_64 environment.

Arm (Apple M1) is still not supported. It will be supported soon.
## Note about Qt

MeshLab requires Qt 5.15. You can both install Qt5 using `brew` or manually in your system. 
If you install Qt manually. you can then give the path of the Qt installation directory to the various scripts, or you can add Qt to your `LD_LIBRARY_PATH`.

The `0_setup_env.sh` script won't install qt from `brew` if you pass the argument `dont_install_qt`.

## Dependencies 

Dependencies are automatically installed by the `0_setup_env.sh` script, which uses [`brew`](https://brew.sh/) as package manager.
Be sure to have `brew` installed before running this script.

Libraries installed by the `0_setup_env.sh` are the following:
  - Required by MeshLab:
    - `coreutils` (required by these scripts) 
	- `cmake`
	- `ninja`
	- `libomp`
	- `node` (for package stage, to download appdmg)
  - Optional:
	- `xerces-c` (required by libe57)
	- `cgal` (required by several MeshLab plugins)