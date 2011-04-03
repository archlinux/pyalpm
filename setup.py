# -*- coding: utf-8 -*-
import os
import setuptools

os.putenv('LC_CTYPE', 'en_US.UTF-8')

alpm = setuptools.Extension('pyalpm',
    libraries = ['alpm'],
    sources = [
        'src/pyalpm.c',
        'src/util.c',
        'src/package.c',
        'src/db.c',
        'src/options.c',
        'src/optionsobject.c'
        ])

setuptools.setup(name = 'pyalpm',
      version = '0.2',
      description = 'libalpm bindings for Python 3',
      author = "Rémy Oudompheng",
      author_email = "remy@archlinux.org",
      url = "http://projects.archlinux.org/users/remy/pyalpm.git",
      packages = ["pycman"],
      scripts = ["scripts/pycman"],
      ext_modules = [alpm])
