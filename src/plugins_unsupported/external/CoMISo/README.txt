CoMISo Version 1.0 rc1
www.rwth-graphics.de
zimmer@informatik.rwth-aachen.de
bommes@informatik.rwth-aachen.de

Update! Version 1.1
-------------------
For the CoMISo to work only Eigen3 and GMM++ are needed.

What is
-------
This is a short readme file to get you started with the Constrained Mixed-Integer Solver (CoMISo). It shortly overviews the system prerequisites and explains how to build the package. See the examples and the HarmonicExample OpenFlipper plugin for usage examples and ideas.

Requirements
------------
Here is an example of what packages were needed to compile CoMISo on a freshly installed Ubuntu 9.04 system
# sudo apt-get install g++
# sudo apt-get install cmake
# sudo apt-get install libgmm-dev
# sudo apt-get install libboost-dev
# sudo apt-get install libblas-dev
# sudo apt-get install libsuitesparse-dev 
(some other needed libraries such as lapack, are installed as dependencies of the above)

For Windows and Macintosh systems the corresponding packages need to be downloaded and installed.

The cmake build system should enable building the CoMISo library under Windows and Macintosh systems, please let me know if this is (not) the case!

OpenFlipper requirements:
-------------------------
To build OpenFlipper you additionally need to install all the Qt4 packages libqt4-{dev-dbg, dev, network, gui, opengl, opengl-dev, script, scripttools, ...} and also 
# sudo apt-get install libglew1.5-dev
# sudo apt-get install glutg3-dev

Building (Stand alone)
----------------------
Assuming CoMISo was unpacked to the directory SOME_DIRECTORY/CoMISo (where SOME_DIRECTORY should be /PATH_TO_OPENFLIPPER/libs/CoMISo for integration with the OpenFlipper framework) the package is built by creating a build directory, using cmake to create the Makefiles and using make to actually build:

# cd /SOME_DIRECTORY/CoMISo/
# mkdir build
# cd build
# cmake ..
(assuming all needed packages are installed and cmake threw no errors...)
# make

The binaries (examples) and the shared library are found under
/SOME_DIRECTORY/CoMISO/build/Build/bin/
and 
/SOME_DIRECTORY/CoMISO/build/Build/lib/CoMISo/


Building (For use with OpenFlipper)
-----------------------------------
Simply extract/checkout the CoMISo directory to the /PATH_TO_OPENFLIPPER/libs/ directory. The library will be automatically built and you will find the shared library libCoMISo.so under the OpenFlipper build directory.
To use the solver in your Plugin add CoMISo to the CMakeLists.txt of the plugin and you are set, see Plugin-HarmonicExample for an example.

Using
-----
To use the solver library in your applications have a look at the /SOME_DIRECTORY/CoMISo/Examples/ and the sample OpenFlipper plugin (Plugin-HarmonicExample) downloadable from the CoMISo project homepage.

Feedback
--------
We appreciate your feedback! Bugs, comments, questions or patches send them to zimmer@informatik.rwth-aachen.de or bommes@informatik.rwth-aachen.de !
