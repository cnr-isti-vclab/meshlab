#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export LD_LIBRARY_PATH=$DIR:${LD_LIBRARY_PATH}
$DIR/IDTFConverter $@

#echo $1/IDTFConverter.out $2 $3 $4 $5 $6 $7 $8 $9 ${10} \"${11}\"> /tmp/dump.txt
