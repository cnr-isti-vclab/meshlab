#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("freenect", ["freenect.pyx"],
                         libraries=['usb-1.0', 'freenect', 'freenect_sync'],
                         runtime_library_dirs=['/usr/local/lib', '/usr/local/lib64', '/usr/lib/'],
                         extra_compile_args=['-fPIC', '-I', '../../include/',
                                             '-I', '/usr/include/libusb-1.0/',
                                             '-I', '/usr/local/include/libusb-1.0',
                                             '-I', '/usr/local/include',
                                             '-I', '../c_sync/'])]
setup(
  name = 'freenect',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)
