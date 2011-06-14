# -*- coding: utf-8 -*-
import os
from distutils.core import Extension, setup

os.putenv('LC_CTYPE', 'en_US.UTF-8')

pyalpm_version = '0.5'

cflags = ['-Wall', '-ansi', '-Wdeclaration-after-statement']

alpm = Extension('pyalpm',
    libraries = ['alpm'],
    extra_compile_args = cflags + ['-D_XOPEN_SOURCE=600', '-DVERSION="%s"' % pyalpm_version],
    language = 'C',
    sources = [
        'src/pyalpm.c',
        'src/util.c',
        'src/package.c',
        'src/db.c',
        'src/options.c',
        'src/handle.c',
        'src/transaction.c'
        ],
    depends = [
        'src/handle.h',
        'src/db.h',
        'src/options.h',
        'src/package.h',
        'src/pyalpm.h',
        'src/util.h',
        ])

setup(name = 'pyalpm',
      version = pyalpm_version,
      description = 'libalpm bindings for Python 3',
      author = "Rémy Oudompheng",
      author_email = "remy@archlinux.org",
      url = "http://projects.archlinux.org/users/remy/pyalpm.git",
      packages = ["pycman"],
      scripts = ["scripts/lsoptdepends"] + ["scripts/pycman-" + i
          for i in ['database', 'deptest', 'query', 'remove', 'sync', 'upgrade', 'version']],
      ext_modules = [alpm])

# vim: set ts=4 sw=4 et tw=0:
