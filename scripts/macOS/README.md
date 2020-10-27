# MacOS Scripts

This folder contains a series of scripts to build and deploy MeshLab under a MacOS environment.

* `0_setup_env.sh`: this script installs all the required dependencies that are necessary to build MeshLab and to create its DMG in a MacOS machine (tested in 10.15 Catalina). It requires [homebrew](https://brew.sh/) installed;
* `1_build.sh`: this script builds MeshLab in a MacOS environment:
	* it requires a properly set Qt environment (see `0_setup_env.sh`);
	* takes as arguments:
		* the build directory (default: `src/build`): `--build_path=path/to/build`
		* the install directory (default: `src/install`): `--install_path=path/to/install`
		* the number of cores used to build MeshLab (default: `-j4`)`
* `2_deploy.sh`: this script makes portable a `meshlab.app` appdir. Takes as argument the path where the output install path of the `1_build.sh` script is placed (default: `src/install`);
* `3_dmg.sh`: this script generates a [DMG](https://en.wikipedia.org/wiki/Apple_Disk_Image) that can be used to install MeshLab. Takes as argument the path where the install path of the `2_deploy.sh` script is placed (default: `src/install`);
* `make_it.sh`: this script builds, deploys and generates a [DMG](https://en.wikipedia.org/wiki/Apple_Disk_Image) that can be used to install MeshLab. Arguments are the same of the `1_build.sh` script.

## Examples

Building MeshLab on a clean MacOS environment (build placed in `meshlab/src/build`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	sh meshlab/scripts/macOS/0_setup_env.sh
	sh meshlab/scripts/macOS/1_build.sh

Building and generating a DMG on a clean MacOS environment:
* build directory: `./meshlab-build`
* install directory: `./meshlab-install`
* DMG path: `./`


	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	sh meshlab/scripts/macOS/0_setup_env.sh
	sh meshlab/scripts/macOS/make_it.sh --build_path="./meshlab-build" --install_path="./meshlab-install"