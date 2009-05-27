
MeshLab and MeshLab's plugins invokes functions exported by external libraries. You have to compile this libraries before the MeshLab's code. 
To do this:

cd MESHLAB_DIRECTORY/src/external
qmake -recursive external.pro
make

To compile MeshLab and all MeshLab's plugins:

cd MESHLAB_DIRECTORY/src/
qmake -recursive meshlabv12.pro
make

For more and complete info, please, refer to the meshlab's site http://meshlab.sourceforge.net/wiki/index.php/Compiling#Compiling
