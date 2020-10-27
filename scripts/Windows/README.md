# Windows Scripts

**WORK IN PROGRESS**

This folder contains a series of scripts to build and deploy MeshLab under a Windows environment.
**Note**: scripts are written in bash, therefore they should be run in a linux subsystem environment.

* `1_build.sh`: this script builds MeshLab in a Windows environment:
    * it requires a properly set Visual Studio (>=2015) and MSVC compiler;
	* it requires a properly set Qt environment;
	* `cmake` in the PATH env variable;
	* takes as arguments:
		* the build directory (default: `src/build`): `--build_path=path/to/build`
		* the install directory (default: `src/install`): `--install_path=path/to/install`
		* the number of cores used to build MeshLab (default: `-j4`)`
* `2_deploy.sh`: this script makes the given path a portable version of MeshLab. Takes as argument the path where the output install path of the `1_build.sh` script is placed (default: `src/install`);
* `3_installer.sh`: this script computes, starting from the portable folder of MeshLab, an NSIS installer. Takes as argument the path where the output install path of the `2_deploy.sh` script is placed (default: `src/install`);
    * it requires `makensis.exe` in the PATH env variable.


## Examples

Building MeshLab on Windows, having VisualStudio, MSVC and Qt installed, cmake in the PATH (build placed in `meshlab/src/build`):

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	sh meshlab/scripts/Windows/1_build.sh