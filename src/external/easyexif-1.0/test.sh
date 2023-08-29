#!/bin/bash

make

DEMONAME=./demo

if [ ! -e ./demo ]; then
  # check if demo.exe exists, needed on windows
  if [ -e ./demo.exe ]; then
    DEMONAME=./demo.exe
  else
    echo Cannot find executable.
    exit 1
  fi
fi

for jpeg in `ls test-images/*.jpg`; do
  $DEMONAME $jpeg > /tmp/`basename $jpeg`.actual
  diff $jpeg.expected /tmp/`basename $jpeg`.actual > /tmp/diff.out
  if [[ -s /tmp/diff.out ]] ; then
    echo "FAILED ON $jpeg"
    cat /tmp/diff.out
    exit
  fi ;
  echo "PASS $jpeg"
done
