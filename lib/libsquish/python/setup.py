from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


cmdclass = {
    'build_ext': build_ext
}

ext = Extension(
    'squish',
    ['squish.pyx'],
    include_dirs=['..'],
    library_dirs=['..'],
    libraries=['squish'],
    extra_link_args=['-fPIC'],
    language='c++',
)

setup(
    name='squish',
    version='1.0',
    author='Mathieu Virbel',
    author_email='mat@kivy.org',
    url='http://txzone.net/',
    license='LGPL',
    description='Python binding for libsquish',
    ext_modules=[ext],
    cmdclass=cmdclass,
)
