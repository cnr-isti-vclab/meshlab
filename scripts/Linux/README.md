# Linux Scripts

This folder contains a series of scripts to build and deploy MeshLab under a Linux environment (tested on Ubuntu 18.04, 20.04 and 22.04).

## Note about Qt

Old versions of Ubuntu (< than 22.04) are shipped with old versions of Qt, but MeshLab requires Qt 5.15.
Therefore, you must install manually Qt separatelly in your system. You can then given the path of the Qt installation
directory to the various scripts.
You can check an example of this scenario in the examples section.

## Scripts

* `0_setup_env_ubuntu.sh`: this script installs all the required dependencies that are necessary to build MeshLab in an Ubuntu distro (tested in 18.04, 20.04 and 22.04). If you never installed Qt and other libraries, you should run it before any other script;
   Arguments accepted by this script are the following:
   * `--dont_install_qt`: does not install qt libraries. You should use this option if you are running a version of ubuntu older than 22.04, and install then manually Qt.
   * `--dowload_cgal_and_boost_src`: downloads sources of cgal and boost libraries instead of using the system packages. It may be useful if you are running a version of ubuntu older than 20.04 (before this version, CGAL was not header only).
 
* `1_build.sh`: this script builds MeshLab in a Linux environment:
	* it requires a properly set Qt environment (see `0_setup_env_ubuntu.sh`);
	* takes as arguments:
		* the build directory (default: `src/build`): `--build_path=path/to/build`
		* the install directory (default: `src/install`): `--install_path=path/to/install`
		* the number of cores used to build MeshLab (default: `-j4`)`
		* the possibility to build MeshLab with double precision scalar: `--double_precision`
		* optionally, the path of the custom Qt directory: `--qt_dir=path/to/qt`
* `2_deploy_and_appimage.sh`: this script makes the given path a portable version of MeshLab and creates also an [AppImage](https://appimage.org/) of it. Takes as arguments:
    * the path where the output install path of the `1_build.sh` script is placed (default: `src/install`): `--install_path=path/to/install`
	* optionally, the directory of the custom Qt directory: `--qt_dir=path/to/qt`
* `make_it.sh`: this script builds, deploys and generates an [AppImage](https://appimage.org/) that can be run in a Linux Environment without dependencies. Arguments are the same of the `1_build.sh` script.

## Examples

Building MeshLab on a clean Linux environment (build placed in `meshlab/src/build`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh
	bash meshlab/scripts/Linux/1_build.sh

Building and generating AppImage MeshLab on an old Ubuntu version with Qt installed in the path `/opt/Qt`: 

* build directory: `meshlab/src/build`
* AppDir/portable version: `meshlab/src/install`
* AppImage path: `./`

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh --dont_install_qt
bash meshlab/scripts/Linux/1_build.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
bash meshlab/scripts/Linux/2_deploy_and_appimage.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
```

Or, alternatively:

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh --dont_install_qt
bash meshlab/scripts/Linux/make_it.sh --qt_dir=/opt/Qt/5.15.2/gcc_64
```

Building and generating AppImage on a clean Linux environment, using 16 cores and custom directories:

* build directory: `./meshlab-build`
* AppDir/portable version: `./meshlab-install`
* AppImage path: `./`

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
bash meshlab/scripts/Linux/0_setup_env_ubuntu.sh
bash meshlab/scripts/Linux/make_it.sh --build_path="./meshlab-build" --install_path="./meshlab-install" -j16
```
