##Compiling MeshLab
__tl;dr__ 

1. install Qt5.7, 
2. clone vcglib at the same level of meshlab 
3. compile `src/external/external.pro`, 
4. compile `src/meshlab_full.pro`

####Dependencies
You need Qt5.7 and we assume that we have qtcreator installed and you know how to build something from a `*.pro` qt file (either from qtcreator, or from the command line). As external dependencies MeshLab uses the _vcglib_ c++ library for most of the mesh processing tasks; so you need to clone it from github ( http://github.com/cnr-isti-vclab/vcglib/ ) in a folder named `vcglib` at the same level of the folder `meshlab`.  To be clear your folder structure should be something quite similar to:

    yourdevelfolder/
     |
     ├──meshlab
     │   ├──docs
     │   ├──README.md
     │   ├──src
     │   ├──...
     │   └──...
     └──vcglib
         ├──apps
         ├──doc
         ├──eigenlib
         ├──...
         └──...

All the include paths inside the `.pro` assume this relative positioning of the `meshlab` and `vcglib` folders. Please note that, given the fact that the developement of the vcglib and meshlab are often intermixed, if you compile the devel branch of the meshlab repo, you should use the devel branch of the vcglib. 

##### Compiling
MeshLab has a plugin architecture and therefore all the plugins are compiled separately; some of them are harder to be compiled. Dont worry, if a plugin fails to compile just remove it and you lose just that functionality. As a first step you should try to use the `src/meshlab_mini.pro` that contains the info for building meshlab with a minimal set of plugins with no external dependencies and usually compile with no problem.

Some plugins of MeshLab need external libraries. All the required libraries are included in the `meshlab/src/external` folder. You have to compile these libraries before attempting to compile the whole MeshLab. Just use the `meshlab/src/external.pro` file. For OsX 10.12 we kindly provide the already compiled binaries. 

Once you have the required lib (check for lib files in the folder `meshlab/src/external/lib/<your_architecture>`) you can try to compile the whole meshlab using `src/meshlab_full.pro`. 

##### Platform specific notes
On __osx__ some plugins exploit openmp parallelism (screened poisson, isoparametrization) so you need a compiler supporting it and the clang provided by xcode does not support openmp. Install 'clang++-mp-3.9' using macport or modify the .pro accordingly.

