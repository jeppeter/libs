from distutils.core import setup, Extension

setup(name="mandelbrot", version="0.0",
	ext_modules = [Extension("mandelbrot", ["mandelbrot.c"],libraries=['algo'],library_dirs=['.'])])
