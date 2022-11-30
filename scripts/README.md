# MeshLab scripts

This folder contains a series of platform-dependent bash scripts to build and deploy MeshLab.

Every platform folder contains:

* `0_setup_env.sh`: a script that installs automatically the required dependencies;
* `1_build.sh`: a script that builds MeshLab;
* `2_deploy.sh`: a script that deploys MeshLab;
* `3_pack.sh`: a script that computes a platform-dependent self-contained package/installer of MeshLab;
* `make_it.sh`: a script that computes all the three previous scripts;

Note for Windows: scripts are written in bash, therefore they should be run in a linux subsystem environment.

See in each platform subfolder for details and other platform-dependent requirements:
- [Linux](https://github.com/cnr-isti-vclab/meshlab/tree/main/scripts/Linux)
- [MaxOS](https://github.com/cnr-isti-vclab/meshlab/tree/main/scripts/macOS)
- [Windows](https://github.com/cnr-isti-vclab/meshlab/tree/main/scripts/Windows)

## `0_setup_env.sh`

This script installs automatically the required dependencies of MeshLab. Common dependencies are Qt and `cmake`.

The script uses a system package manager to install dependencies, which is:
  - [`apt-get`](https://linux.die.net/man/8/apt-get) on Linux;
  - [`brew`](https://brew.sh/) on macOS;
  - [`choco`](https://community.chocolatey.org/) on Windows.

Please be sure to have these package managers before running this script.

It takes as input the `--dont_install_qt` option, that allows the user to choose whether to install or not qt using the system package manager. It could be useful if Qt is installed using other sources. For example, in our GitHub actions workflows we use a specific action step to install Qt, in order to choose a specific version. 

Please see the readme inside the desired platform folder for system dependent additional arguments and further details.

## `1_build.sh`

This script builds MeshLab. It assumes that the script `0_setup_env.sh` has been already executed and all the dependencies are correctly installed and accessible by the script.

It takes the following input arguments: 
  - `--build_path`: the directory where will be placed the build files; 
    - default: `[meshlab_repo]/build`
    - example: `--build_path=path/to/build`
  - `--install_path`: the directory where final MeshLab binaries, libraries and files will be placed;
    - default: `[meshlab_repo]/install`
    - example: `--install_path=path/to/install`
  - `--double_precision`: if present, this argument will tell to build MeshLab with double floating point precision;
  - `--nighly`: if present, this argument will tell to build MeshLab with a nightly version;
  - `--qt_dir`: the directory where to find Qt, useful when Qt is not in the `PATH`;
    - example: `--qt_dir=path/to/qt`
  - `--ccache`: if present, this argument will tell to use the `ccache` tool to speed up build time (useful for CI builds, be sure to have `ccache` installed).

Example of call: 
```
bash 1_build.sh --build_path=path/to/build --install_path=path/to/install --double_precision
```

## `2_deploy.sh`

This script deploys MeshLab, making sure that the folder will be a self-contained MeshLab, without Qt or any other system libraries dependencies. It assumes that the scripts `0_setup_env.sh` and `1_build.sh` have been  already executed and there is a directory where all the final MeshLab files are placed.

It takes the following input arguments:
  - `--install_path`: the directory where final MeshLab binaries, libraries and files are placed; this will be the path where MeshLab will be deployed. It should be the same argument given to the `1_build.sh` script;
    - default: `[meshlab_repo]/install`
    - example: `--install_path=path/to/install`
  - `--qt_dir`: the directory where to find Qt, useful when Qt is not in the `PATH`; it should be the same argument given to the `1_build.sh` script;
    - example: `--qt_dir=path/to/qt`

Example of call:
```
bash 2_deploy.sh --install_path=path/to/install
```

## `3_pack.sh`

This script generates a platform-dependent self-contained package/installer of MeshLab. It assumes that the scripts `0_setup_env.sh`, `1_build.sh` and `2_deploy.sh` have been already executed, and there is a directory where MeshLab has been deployed. 

It takes the following input arguments:
  - `--install_path`: the directory containing MeshLab deployed; it should be the same argument given to the `2_deploy.sh` script;
    - default: `[meshlab_repo]/install`
    - example: `--install_path=path/to/install`
  - `--packages_path`: the directory where the output package(s) will be placed;
    - default: `[meshlab_repo]/packages`
    - example: `--packages_path=path/to/packages`

Example of call:
```
bash 3_pack.sh --install_path=path/to/install --package_path=path/to/packages
```

## `make_it.sh`

This script computes all the three previous scripts: starting from the source code, it will produce a self-contained package/installer of MeshLab. Input arguments are a union of all the arguments of the previous scripts.

# Examples

The following examples can be run on Linux, macOS and Windows (using linux subsystem).

### Building MeshLab on a clean environment

The build directory will be placed in `meshlab/build`:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	bash meshlab/scripts/[platform]/0_setup_env.sh
	bash meshlab/scripts/[platform]/1_build.sh

### Building, deploying and packaging MeshLab using a custom Qt installation

For example, let assume that Qt 5.15.2 is installed in the path `/opt/Qt`. We assume also that the compiler/architecture used by Qt is `gcc_64` (you can check yours in the directory `path/to/Qt/5.15.2/`). 

* build directory: `meshlab/build`
* AppDir/portable version: `meshlab/install`
* package (installer, dmg, AppImage) path: `meshlab/packages`

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/[platform]/0_setup_env.sh --dont_install_qt
bash meshlab/scripts/[platform]/1_build.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
bash meshlab/scripts/[platform]/2_deploy.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
bash meshlab/scripts/[plarform]/3_pack.sh
```

Or, alternatively:

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/[platform]/0_setup_env.sh --dont_install_qt
bash meshlab/scripts/[platform]/make_it.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
```

### Building, deploying and packaging MeshLab using custom directories

* build directory: `/my/meshlab/build`
* AppDir/portable version: `/my/meshlab/install`
* AppImage path: `/my/meshlab/packages`

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/[platform]/0_setup_env.sh
bash meshlab/scripts/[platform]/make_it.sh --build_path="/my/meshlab/build" --install_path="/my/meshlab/install" --package_path="/my/meshlab/packages"
```
