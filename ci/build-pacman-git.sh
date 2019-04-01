#!/bin/bash
# Simple script to build pyalpm against pacman-git from a checkout

CWD=$(pwd)
PACMAN_DIR=$(pwd)/pacman
if [ -n "$1" ]; then
  TMP_DIR=$1
else
  TMP_DIR=$(mktemp --suffix pacman -d)
fi


if [ -d $PACMAN_DIR ]; then
  cd $PACMAN_DIR && git pull --ff-only
else
  git clone https://git.archlinux.org/pacman.git/ $PACMAN_DIR
fi

cd $PACMAN_DIR

./autogen.sh

./configure --prefix=/usr

make -C lib/libalpm DESTDIR=${TMP_DIR} install-libLTLIBRARIES install-includeHEADERS

cd $CWD
CC=gcc CPATH=$TMP_DIR/usr/include python3 setup.py build
