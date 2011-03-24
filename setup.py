from distutils.core import setup
from distutils.core import Extension

alpm = Extension('pyalpm',
    extra_compile_args = ["-Wall"],
    libraries = ['alpm'],
    sources = [
        'src/pyalpm.c',
        'src/util.c',
        'src/package.c',
        'src/db.c'
        ])

alpm_options = Extension('_alpmoptions',
                   libraries = ['alpm'],
                   sources = ['src/options.c', 'src/util.c'])

setup(name = 'pyalpm',
      version = '0.1',
      ext_modules = [alpm, alpm_options])
