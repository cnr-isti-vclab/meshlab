# Linux Scripts

This folder contains a series of scripts to build and deploy MeshLab under a Linux environment (tested on Ubuntu 16.04, 18.04 and 20.04).

* `0_setup_env_ubuntu.sh`: this script installs all the required dependencies that are necessary to build MeshLab in an Ubuntu distro (tested in 18.04 and 20.04). If you never installed Qt and other libraries, you should run it before any other script;
* `1_build.sh`: this script builds MeshLab in a Linux environment:
	* it requires a properly set Qt environment (see `0_setup_env_ubuntu.sh`);
	* takes as arguments:
		* the build directory (default: `src/build`): `--build_path=path/to/build`
		* the install directory (default: `src/install`): `--install_path=path/to/install`
		* the number of cores used to build MeshLab (default: `-j4`)`
		* the possibility to build MeshLab with double precision scalar: `--double_precision`
		* optionally, the directory of the custom Qt directory: `--qt_dir=path/to/qt`
* `2_deploy_and_appimage.sh`: this script makes the given path a portable version of MeshLab and creates also an [AppImage](https://appimage.org/) of it. Takes as arguments:
    * the path where the output install path of the `1_build.sh` script is placed (default: `src/install`): `--install_path=path/to/install`
	* optionally, the directory of the custom Qt directory: `--qt_dir=path/to/qt`
* `make_it.sh`: this script builds, deploys and generates an [AppImage](https://appimage.org/) that can be run in a Linux Environment without dependencies. Arguments are the same of the `1_build.sh` script.

## Examples

Building MeshLab on a clean Linux environment (build placed in `meshlab/src/build`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh
	bash meshlab/scripts/Linux/1_build.sh

Building and generating AppImage on a clean Linux environment:
* build directory: `./meshlab-build`
* install directory: `./meshlab-install`
* AppImage path: `./`

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh
bash meshlab/scripts/Linux/make_it.sh --build_path="./meshlab-build" --install_path="./meshlab-install"
```
