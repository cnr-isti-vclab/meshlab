# Linux Scripts

This folder contains a series of scripts to build and deploy MeshLab under a Linux environment.

The following scripts are provided:

* `linux_setup_env_ubuntu.sh`: this script installs all the required dependencies that are necessary to build MeshLab in an Ubuntu distro (tested in 16.04, 18.04). If you never installed Qt and other libraries, you should run it before any other script;
* `linux_build.sh`: this script compiles MeshLab in a Linux environment:
	* it requires a properly set Qt environment (see `linux_setup_env_ubuntu.sh`); 
	* without given arguments, all the binaries will be placed in the `meshlab/distrib` folder. You can give as argument the `BUILD_PATH`, and meshlab binaries will be then placed inside `BUILD_PATH/distrib`;
* `linux_build_meshlab_mini.sh`: this script compiles MeshLab mini in a Linux environment. Works in the same way as the `linux_build.sh` script;
* `linux_deploy.sh`: makes the `distrib` folder a portable version of MeshLab (no Qt and other libraries dependencies):
	* it requires a properly built meshlab `distrib` directory (see `linux_build.sh`);
	* without given arguments, the folder that will be deployed is `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`;
* `linux_appimages.sh`: this script makes the `distrib` folder a portable version of MeshLab (no Qt and other libraries dependencies), and generates an [AppImage](https://appimage.org/) that can be run in a Linux Environment without dependencies; 
	* it requires a properly built meshlab `distrib` directory (see `linux_build.sh`);
	* without given arguments, the folder that will be deployed is `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`.
* `linux_meshlabserver_appimage.sh`: this script generates an [AppImage](https://appimage.org/) of [MeshLabServer](https://github.com/cnr-isti-vclab/meshlab/blob/master/src/meshlabserver/README.md) that can be run in a Linux Environment without dependencies; 
	* it requires a properly built meshlab `distrib` directory (see `linux_build.sh`);
	* without given arguments, the folder that will be deployed is `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`.
* `linux_make_it.sh`: this script builds, deploys and generates an [AppImage](https://appimage.org/) that can be run in a Linux Environment without dependencies;
	* it requires a properly set Qt environment (see `linux_setup_env_ubuntu.sh`); 
	* without given arguments, all the binaries will be placed in the `meshlab/distrib` folder. You can give as argument the `BUILD_PATH`, and meshlab binaries and the AppImage will be then placed inside `BUILD_PATH/distrib`;

__NOTE__: `linux_deploy.sh`, `linux_appimage.sh` and `linux_make_it.sh` use [LinuxDeployQt](https://github.com/probonopd/linuxdeployqt), which allows to deploy applications only running the oldest supported linux distro (see [this](https://github.com/probonopd/linuxdeployqt/issues/340)) in order to guarantee the largest support possible. Therefore, before running these scripts, be sure that your Linux distribution is the oldest supported one. 

## Examples

Building meshlab on a clean Linux environment (compiled MeshLab in `meshlab/distrib`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	bash meshlab/install/linux/linux_setup_env_ubuntu.sh
	bash meshlab/install/linux/linux_build.sh

Building and generating AppImage on a clean Ubuntu 16.04 (last supported distro) environment, build directory in `~/build_meshlab`:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	bash meshlab/install/linux/linux_setup_env_ubuntu.sh
	bash meshlab/install/linux/linux_make_it.sh ~/build_meshlab
	
Meshlab*.AppImage can be found in `~/build_meshlab/distrib/`.
