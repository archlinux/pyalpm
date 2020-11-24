# -*- coding: utf-8 -*-
import os
from setuptools import setup, Extension


os.putenv('LC_CTYPE', 'en_US.UTF-8')

pyalpm_version = '0.9.2'

cflags = ['-Wall', '-Wextra', '-Werror',
          '-Wno-unused-parameter', '-Wno-incompatible-pointer-types',
          '-Wno-cast-function-type', '-std=c99', '-D_FILE_OFFSET_BITS=64']

alpm = Extension('pyalpm',
                 libraries=['alpm'],
                 extra_compile_args=cflags + ['-DVERSION="%s"' % pyalpm_version],
                 language='C',
                 sources=['src/pyalpm.c',
                          'src/util.c',
                          'src/package.c',
                          'src/db.c',
                          'src/options.c',
                          'src/handle.c',
                          'src/transaction.c'],
                 depends=['src/handle.h',
                          'src/db.h',
                          'src/options.h',
                          'src/package.h',
                          'src/pyalpm.h',
                          'src/util.h'])

if __name__ == "__main__":
    setup(version=pyalpm_version,
          ext_modules=[alpm])

# vim: set ts=4 sw=4 et tw=0:
