#!/bin/bash
# this is a script shell for building a tar archive of all the sources needed to build a running version of the meshlab.
# it should be run in the directory containing the sf meshlab and code directories.
# so your command line should be something like:     source meshlab/src/install/build_src_archive.sh
# when the script ends you should have a tgz with all the needed sources
#
find meshlab code/lib/glew sf | egrep -v  -e CVS -e /ui_ -e build -e /moc_ -e /qrc_ -e /test/ -e MeshLabBundle -e docs/ -e /web/ -e /apps/ -e\# | egrep -e  [.]cpp$ -e [.]c$ -e  [.]h$ -e  [.]pro$ -e [.]ui$ -e [.]png$ -e [.]qrc$ -e [.]vert$ -e [.]frag$ -e [.]gdp$ -e [.]txt$ -e [.]icns$ -e images/100mesh.html -e inl$ > list.txt
#
gnutar -c -v -z  -T ./list.txt -f meshlab.tgz 

