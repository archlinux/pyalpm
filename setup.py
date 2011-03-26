from distutils.core import setup
from distutils.core import Extension

alpm = Extension('pyalpm',
    extra_compile_args = ["-Wall"],
    libraries = ['alpm'],
    sources = [
        'src/pyalpm.c',
        'src/util.c',
        'src/package.c',
        'src/db.c',
        'src/options.c',
        'src/optionsobject.c'
        ])

setup(name = 'pyalpm',
      version = '0.1',
      ext_modules = [alpm])
