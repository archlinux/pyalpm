# -*- coding: utf-8 -*-
import os
from setuptools import setup, Extension


os.putenv('LC_CTYPE', 'en_US.UTF-8')

pyalpm_version = '0.9.2'

PYCMAN_SCRIPTS = ['database', 'deptest', 'query', 'remove', 'sync', 'upgrade', 'version']

if __name__ == "__main__":

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

    with open("README.md", "r") as fh:
        long_description = fh.read()

    setup(name='pyalpm',
          version=pyalpm_version,
          description='libalpm bindings for Python 3',
          long_description=long_description,
          long_description_content_type="text/markdown",
          author="Rémy Oudompheng",
          author_email="remy@archlinux.org",
          url="https://projects.archlinux.org/pyalpm.git",
          packages=["pycman"],
          scripts=["scripts/lsoptdepends"] + [f'scripts/pycman-{p}' for p in PYCMAN_SCRIPTS],
          ext_modules=[alpm],
          classifiers=[
              'Development Status :: 6 - Mature',
              'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
              'Programming Language :: C',
              'Topic :: System :: Software Distribution',
              'Topic :: System :: Systems Administration',
          ])

# vim: set ts=4 sw=4 et tw=0:
