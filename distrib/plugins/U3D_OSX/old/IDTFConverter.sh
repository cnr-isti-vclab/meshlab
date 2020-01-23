#!/bin/bash
# 
export LD_LIBRARY_PATH=$1:${LD_LIBRARY_PATH}
export U3D_LIBDIR=$1
$1/IDTFConverter.out $2 $3 $4 $5 $6 $7 $8 $9 ${10} "${11}" 

#echo $1/IDTFConverter.out $2 $3 $4 $5 $6 $7 $8 $9 ${10} \"${11}\"> /tmp/dump.txt