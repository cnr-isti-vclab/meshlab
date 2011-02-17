#!/bin/bash
# this is a script shell for building a tar archive of all the sources needed to build a running version of the meshlab.
# it should be run in the directory containing the sf meshlab and code directories.
# so your command line should be something like:     source meshlab/src/install/build_src_archive.sh
# when the script ends you should have a tgz with all the needed sources
#
cp meshlab/src/install/building_meshlab.txt ./how_to_compile.txt
find meshlab vcglib ./how_to_compile.txt | egrep -v  -e SVN -e /ui_ -e build -e /moc_ -e /php -e meshlabv11.pro -e /qrc_ -e /test/ -e MeshLabBundle -e docs/ -e /web/ -e\# | egrep -e  [.]cpp$ -e [.]c$ -e  [.]h$ -e  [.]tpp$ -e  [.]pro$ -e [.]pri$ -e [.]ui$ -e [.]png$ -e [.]rc$ -e [.]qrc$ -e [.]vert$ -e [.]frag$ -e [.]rfx$ -e [.]glsl$ -e [.]gdp$ -e [.]txt$ -e [.]vs$ -e [.]fs$ -e [.]rendertemplate$ -e [.]icns$ -e Qt -e images/100mesh.html -e images/eye.ico -e images/facebook.gif -e [.]inl$ -e vcg/Eigen/* > list.txt
echo "meshlab/src/fgt/edit_topo/edit_topomeshbuilder.h" >> list.txt
#
tar -c -v -z  -T ./list.txt -f MeshLabSrc_AllInc_v130_`date +"%Y%m%d"`.tgz 
eg
