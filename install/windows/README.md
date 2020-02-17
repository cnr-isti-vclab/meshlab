# Windows Scripts

**WORK IN PROGRESS**

This folder contains a series of scripts to build and deploy MeshLab under a Windows environment.

The follwing scripts are provided:

* `windows_build.ps1`: this script compiles MeshLab in a Windows environment:
	* it requires a properly set Visual Studio (>=2015) and MSVC compiler; 
	* it requires a properly set Qt (>= 5.9) environment; 
	* without given arguments, all the binaries will be placed in the `meshlab/distrib` folder. You can give as argument the `BUILD_PATH`, and meshlab binaries will be then placed inside `BUILD_PATH/distrib`;
* `windows_build_meshlab_mini.ps1`: this script compiles MeshLab mini in a Windows environment. Works in the same way as the `windows_build.ps1` script;
* `windows_deploy.ps1`: makes the `distrib` folder a portable version of MeshLab (no Qt and other libraries dependencies):
	* it requires a properly built meshlab `distrib` directory (see `windows_build.ps1`);
	* `windeployqt.exe` directly accessible by powershell and `VCINSTALLDIR` set to the VC Visual Studio;
	* without given arguments, the folder that will be deployed is `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`;
* `windows_nsis_installer.ps1`: this script generates an installer for MeshLab:
	* it requires a properly built and deployed meshlab `distrib` directory (see `windows_deploy.ps1`);
	* `makensis.exe` directly accessible by powershell;
	* without given arguments, the folder used for creating the installer is `meshlab/distrib`. You can give as argument the `DISTRIB_PATH`;