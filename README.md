#  pyalpm - python bindings for the libalpm library

[![Build Status](https://travis-ci.org/archlinux/pyalpm.svg?branch=master)](https://travis-ci.com/archlinux/pyalpm) [![Documentation Status](https://readthedocs.org/projects/pyalpm/badge/?version=latest)](https://pyalpm.readthedocs.io/en/latest/?badge=latest)

pyalpm is a C extension for Python 3 that give access to the
libalpm API for package management, which is used, for example
in the Arch Linux distribution.

# Requirements

* Python 3 or later

* libalpm 10 or later

* python-setuptools

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

# Pacman development compatibility

The master branch tracks the most recent pacman release and thus libalpm
release, the [next](https://github.com/archlinux/pyalpm/tree/next) branch
follows [pacman's master branch](https://git.archlinux.org/pacman.git/).
