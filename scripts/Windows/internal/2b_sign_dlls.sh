#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
INSTALL_PATH=$SCRIPTS_PATH/../../install
CERT_FILE=$SCRIPTS_PATH/../../certificate/certificate.pfx
CERT_PSSW=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -cf=*|--cert_file=*)
        CERT_FILE="${i#*=}"
        shift # past argument=value
        ;;
    -cp=*|--cert_pssw=*)
        CERT_PSSW=${i#*=}
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

cd $INSTALL_PATH

CERT_REL=$(realpath --relative-to=$INSTALL_PATH $CERT_FILE)
CERT_WIN=$(echo "$CERT_REL" | sed 's/^\///' | sed 's/\//\\/g')
echo "=== Cert win path: $CERT_WIN" 

for file in $(find $INSTALL_PATH -name '*.dll' -or -name '*.exe');
do
    FILE_REL=$(realpath --relative-to=$INSTALL_PATH $file)
    FILE_WIN=$(echo "$FILE_REL" | sed 's/^\///' | sed 's/\//\\/g')
    echo "=== File win path: $FILE_WIN" 
    signtool.exe sign //fd SHA256 //f $CERT_WIN //p $CERT_PSSW //t http://timestamp.comodoca.com/authenticode $FILE_WIN
done