# MacOS Scripts

This folder contains a series of scripts to build and deploy MeshLab under a MacOS environment.

The follwing scripts are provided:

* `macos_setup_env.sh`: this script installs all the required dependecies that are necessary to build MeshLab and create its DMG in a MacOS machine (tested in 10.15 Catalina). It requires [homebrew](https://brew.sh/) installed.
* `macos_build.sh`: this script compiles MeshLab in a MacOS environment:
	* it requires a properly set Qt environment (see `macos_setup_env.sh`); 
	* without given arguments, all the binaries will be placed in the `meshlab/distrib` folder. You can give as argument the `BUILD_PATH`, and meshlab binaries will be then placed inside `BUILD_PATH/distrib`;
* `macos_build_meshlab_mini.sh`: this script compiles MeshLab mini in a MacOS environment. Works in the same way as the `macos_build.sh` script;
* `macos_deploy.sh`: makes the `distrib/meshlab.app` app a portable version of MeshLab (no Qt and other libraries dependencies):
	* it requires a properly built meshlab `distrib` directory (see `macos_build.sh`);
	* without given arguments, the meshlab.app that will be deployed should be inside `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`;
* `macos_dmg.sh`: this script generates a [DMG](https://en.wikipedia.org/wiki/Apple_Disk_Image) that can be used to install MeshLab; 
	* it requires a properly deployed meshlab `meshlab.app` directory (see `macos_deploy.sh`), a properly set Qt environment, and `appdmg` installed (see `macos_setup_env.sh`);
	* without given arguments, the meshlab.app that will be used should be inside `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`.
* `macos_make_it.sh`: this script builds, deploys and generates a [DMG](https://en.wikipedia.org/wiki/Apple_Disk_Image) that can be used to install MeshLab;
	* it requires a properly set Qt environment and `appdmg` installed (see `macos_setup_env.sh`); 
	* without given arguments, all the binaries will be placed in the `meshlab/distrib` folder. You can give as argument the `BUILD_PATH`, and meshlab binaries and the DMG will be then placed inside `BUILD_PATH/distrib`;

## Examples

Building meshlab on a clean MacOS environment (compiled MeshLab in `meshlab/distrib`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	sh meshlab/install/macos/macos_setup_env.sh
	sh meshlab/install/macos/macos_build.sh

Building and generating DMG on a clean MacOS environment, build directory in `~/build_meshlab`:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	sh meshlab/install/macos/macos_setup_env.sh
	sh meshlab/install/macos/macos_make_it.sh ~/build_meshlab
	
Meshlab*.dmg can be found in `~/build_meshlab/distrib/`.