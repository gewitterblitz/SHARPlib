from setuptools import setup, find_packages, Extension
import numpy

# Define common source files needed across extensions
common_sources = [
    '../src/SHARPlib/interp.cpp',
    '../src/SHARPlib/layer.cpp',
    '../src/SHARPlib/parcel.cpp',
    '../src/SHARPlib/thermo.cpp',
    '../src/SHARPlib/winds.cpp',
    '../src/SHARPlib/params/convective.cpp'
]

extensions = [
    Extension(
        'nwsspc.sharp.calc._constants',
        ['nwsspc/sharp/calc/constants.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._interp',
        ['nwsspc/sharp/calc/interp.i'] + common_sources,  # Add source files
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._params',
        ['nwsspc/sharp/calc/params.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._parcel',
        ['nwsspc/sharp/calc/parcel.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._thermo',
        ['nwsspc/sharp/calc/thermo.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._winds',
        ['nwsspc/sharp/calc/winds.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
    Extension(
        'nwsspc.sharp.calc._layer',
        ['nwsspc/sharp/calc/layer.i'],
        include_dirs=['../include', '../external/fmt/include', numpy.get_include()],
        swig_opts=['-c++'],
        extra_compile_args=['-std=c++17']
    ),
]

setup(
    name='nwsspc_sharp_calc',
    version='0.1',
    packages=find_packages(),
    ext_modules=extensions,
)
