#  pyalpm - python bindings for the libalpm library

[![Build Status](https://travis-ci.org/archlinux/pyalpm.svg?branch=master)](https://travis-ci.com/archlinux/pyalpm) [![Documentation Status](https://readthedocs.org/projects/pyalpm/badge/?version=latest)](https://pyalpm.readthedocs.io/en/latest/?badge=latest)

pyalpm is a C extension for Python 3 that give access to the
libalpm API for package management, which is used, for example
in the Arch Linux distribution.

# Requirements

* Python 3 or later

* libalpm 13

* python-setuptools

* python-pkgconfig

* python-sphinx (optional, to build the docs)

# Building

Building is as easy as running make

	make

Building the docs:

	make -C doc html

# Testing

Required test dependency:

* python-pytest
* gcovr (optional, generating C coverage) 

Unit tests can be run with:

	make test

Coverage for Python code can be generated as following:

	make open-coverage

Coverage for the CPython code can be generated as following:

	make open-ext-coverage

# Releasing

1. Bump version in setup.py
2. commit
3. git tag

## Uploading to PyPI

1. python3 setup.py sdist
2. twine upload -s dist/* --verbose

# Pacman development compatibility

The master branch tracks the most recent pacman release and thus libalpm
release, the [next](https://gitlab.archlinux.org/archlinux/pyalpm/-/tree/next) branch
follows [pacman's master branch](https://gitlab.archlinux.org/pacman/pacman.git).

Building against pacman-git is as simple as:

	git clone https://gitlab.archlinux.org/pacman/pacman.git
	mkdir build
	cd build && meson .. && ninja
	PKG_CONFIG_PATH=/path/to/pacman/build/meson-uninstalled make build

Running tests against pacman-git:

	LD_LIBRARY_PATH=/path/to/pacman/build/ make test
