from distutils.core import setup
from distutils.core import Extension

module = Extension('pyalpm',
                   libraries = ['alpm'],
                   sources = ['pyalpm.c'])
                        

setup(name = 'pyalpm',
      version = '0.1',
      ext_modules = [module])
