# Windows Scripts

This folder contains a series of scripts to build and deploy MeshLab under a Windows environment.

## Note about the compiler

On Windows, we use MSVC compiler to build MeshLab. Before trying to build MeshLab, you should download and install VisualStudio with the C++ developer package, which contains the MSVC compiler.

Other compilers are not tested.

## Note about Qt

MeshLab requires Qt 5.15. You can both install Qt5 using `choco` or manually in your system. 
If you install Qt manually. you can then give the path of the Qt installation directory to the various scripts, or you can add Qt to your `PATH`.

The `0_setup_env.sh` script won't install qt from `choco` if you pass the argument `dont_install_qt`.

## Dependencies 

Dependencies are automatically installed by the `0_setup_env.sh` script, which uses [`choco`](https://community.chocolatey.org/) as package manager.
Be sure to have `choco` installed before running this script.

Libraries installed by the `0_setup_env.sh` are the following:
  - Required by MeshLab:
    - `wget` (required by these scripts)
	- `ccache` (required by github actions)
	- `cmake`
	- `ninja`
	- `nsis` (for package stage)