#!/bin/sh

DATE=$(date --rfc-3339=date)
MLABURL=https://meshlab.svn.sourceforge.net/svnroot/meshlab/trunk
VCGURL=https://vcg.svn.sourceforge.net/svnroot/vcg/trunk
MLABREV=$(svn info ${MLABURL} | sed -n 's/^Revision:[ ]*\([0-9]*\)/\1/p')
TMP=/tmp
MLABARCH=meshlab-snapshot-svn${MLABREV}
LIBARCH=meshlab-externals-svn${MLABREV}
MLABDIR=$1
LIBDIR=$LIBARCH

CURDIR=$(pwd)

mkdir -p $TMP/$MLABDIR
svn export --force $MLABURL $TMP/$MLABDIR
svn export --force $VCGURL $TMP/$MLABDIR

cd $TMP/$MLABDIR

mv meshlab/src/external $TMP/$LIBDIR/meshlab/src
rm -rf CVSROOT meshlab/web meshlab/src/old meshlab/src/sample meshlab/src/meshlabplugins/edit_phototexturing meshlab/src/meshlab/textures meshlab/src/meshlab/plugins/U3D_OSX meshlab/src/meshlab/plugins/U3D_W32 meshlab/src/external
mkdir -p meshlab/src/meshlab/plugins/U3D_W32
mkdir -p meshlab/src/meshlab/plugins/U3D_OSX
mkdir -p meshlab/src/external
mkdir -p $TMP/$LIBDIR/meshlab/src

rm -rf vcglib/docs
rm -rf vcglib/apps/gcc_make vcglib/apps/msvc vcglib/apps/test
mkdir -p $TMP/$LIBDIR/vcglib/vcg
mv vcglib/vcg/Eigen $TMP/$LIBDIR/vcglib/vcg
cd ..
tar -zcf $CURDIR/$MLABARCH.tar.gz $MLABDIR
tar -zcf $CURDIR/$LIBARCH.tar.gz $LIBDIR

rm -rdf $TMP/$MLABDIR

cd $CURDIR
