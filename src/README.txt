Compiling MeshLab
Note:
Some plugins of MeshLab invokes functions exported by external libraries. You have to compile these libraries before attempting to compile the whole MeshLab's code. 

To compile MeshLab and all MeshLab plugins:
First compile the needed external libraries 

cd MESHLAB_DIRECTORY/src/external
qmake -recursive external.pro
make

then compile MeshLab and its plugins
cd MESHLAB_DIRECTORY/src/
qmake -recursive meshlab_full.pro
make

Alternatively you can directly compile only a subset of the whole meshlab by using the meshlab_mini.pro. This minimal subset does not require any external library (give a look at the .pro itself for more info).

cd MESHLAB_DIRECTORY/src/
qmake -recursive meshlab_mini.pro
make




For more complete info, please, refer to the meshlab wiki:
 http://meshlab.sourceforge.net/wiki/index.php/Compiling#Compiling
