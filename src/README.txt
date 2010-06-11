Compiling MeshLab
Note:
Some plugins of MeshLab invokes functions exported by external libraries. You have to compile these libraries before attempting to compile the whole MeshLab's code. 

To compile MeshLab and all MeshLab plugins:

cd MESHLAB_DIRECTORY/src/external
qmake -recursive external.pro
make

cd MESHLAB_DIRECTORY/src/
qmake -recursive meshlabv12.pro
make

Alternatively you can directly compile only a subset of the whole meshlab by using the meshlab_mini.pro (give a look at it for more info).

cd MESHLAB_DIRECTORY/src/
qmake -recursive meshlab_mini.pro
make



For more complete info, please, refer to the meshlab wiki:
 http://meshlab.sourceforge.net/wiki/index.php/Compiling#Compiling
