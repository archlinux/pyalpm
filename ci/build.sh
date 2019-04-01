#!/bin/bash

branch=$(git symbolic-ref --short HEAD)
tmpdir=/tmp/pacman

python3 -m py_compile $(git ls-files '*.py')
sudo mkdir /var/cache/pacman/pkg

if [ "$branch" == "next" ]; then
  ./build-pacman-git.sh $tmpdir
else
  python3 setup.py install
fi

LD_LIBRARY_PATH=$tmpdir/usr/include make test
make pylint
make -C doc html
