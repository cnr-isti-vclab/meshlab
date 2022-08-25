#!/bin/sh

echo "The following people have contributed to libfreenect:" > CONTRIB
echo >> CONTRIB

# note: some exclusions to clean up botched git author tags that got fixed in
# later commits, hopefully we'll be more careful about this in the future
git log --format='%aN <%aE>' | sort | uniq | grep -E -v \
	'brandyn@router|sl203|mwise@bvo|richmattes|kai.*none' | sort >> CONTRIB
