#!/bin/bash
#
# Creates a disk image (dmg) on Mac OS X from the command line.
# usage:
#    mkdmg <volname> <vers> <srcdir>
#
# Where <volname> is the name to use for the mounted image, <vers> is the version
# number of the volume and <srcdir> is where the contents to put on the dmg are.
#
# The result will be a file called <volname>-<vers>.dmg

BUNDLEDIRNAME="MeshLabBundle"
DMGNAME=""
DMGFILENAME=`date "+MeshLabDevel_v123_BETA_%Y_%m_%d.dmg"`
VOLNAME="MeshLab"

echo $DMGFILENAME
cd ../distrib

if [ -e $BUNDLEDIRNAME -a -d $BUNDLEDIRNAME ]
then
  echo "-Starting to process:    --" $BUNDLEDIRNAME " --"
else
  echo "Started in the wrong dir: I have not found the " $BUNDLEDIRNAME "folder"
  exit 0
fi

echo du -sk $BUNDLEDIRNAME
#du -sk $BUNDLEDIRNAME 		
du -sk $BUNDLEDIRNAME | sed 's/\([0-9]*\).*/\1/'
# computing needed size
#SIZE=`du -sk ${BUNDLEDIRNAME} | sed -n '/^[0-9]*/s/([0-9]*).*/1/p'`
SIZE=`du -sk $BUNDLEDIRNAME | sed  's/\([0-9]*\).*/\1/'`

echo "Folder is -"$SIZE"-Mbytes"
SIZE=$(((${SIZE}*25)/10000))
echo "we build a dmg of "$SIZE"Mb"
DMG="tmp-MeshLab.dmg"
#hdiutil create "$DMG" -megabytes ${SIZE} -ov -type UDIF

if [ -e /Volumes/$VOLNAME ]
then 
hdiutil eject /Volumes/$VOLNAME
echo "Ejecting volume:" $VOLNAME
fi

if [ -e $DMG ]
then
echo "removing existing DMG:"$DMG
rm -f $DMG 
fi

hdiutil create -size ${SIZE}m -fs HFS+ -volname $VOLNAME -attach $DMG

hdid "$DMG"
cp -R $BUNDLEDIRNAME/* /Volumes/$VOLNAME
hdiutil eject /Volumes/$VOLNAME

exit 0

#if [ $# != 3 ]; then
# echo "usage: mkdmg.sh volname vers srcdir\n\ne.g. You should start it in the install folder mkdmg MeshLabBundle "
# exit 0
#fi

VOL="MesgLab"
VER="$2"
FILES="$3"

DMG="tmp-MeshLab.dmg"
hdiutil create "$DMG" -megabytes ${SIZE} -ov -type UDIF
DISK=`hdid "$DMG" | sed -ne ' /Apple_partition_scheme/ s|^/dev/([^ ]*).*$|1|p'`
newfs_hfs -v "$VOL" /dev/r${DISK}s2
hdiutil eject $DISK

# mount and copy files onto volume
hdid "$DMG"
cp -R "${FILES}"/* "/Volumes/$VOL"
hdiutil eject $DISK

exit 0

# create temporary disk image and format, ejecting when done
SIZE=`du -sk ${FILES} | sed -n '/^[0-9]*/s/([0-9]*).*/1/p'`
SIZE=$((${SIZE}/1000+1))
hdiutil create "$DMG" -megabytes ${SIZE} -ov -type UDIF
DISK=`hdid "$DMG" | sed -ne ' /Apple_partition_scheme/ s|^/dev/([^ ]*).*$|1|p'`
newfs_hfs -v "$VOL" /dev/r${DISK}s2
hdiutil eject $DISK

# mount and copy files onto volume
hdid "$DMG"
cp -R "${FILES}"/* "/Volumes/$VOL"
hdiutil eject $DISK
#osascript -e "tell application "Finder" to eject disk "$VOL"" && 

# convert to compressed image, delete temp image
rm -f "${VOL}-${VER}.dmg"
hdiutil convert "$DMG" -format UDZO -o "${VOL}-${VER}.dmg"
rm -f "$DMG"