from setuptools import setup, find_packages, Extension
from setuptools.command.build_py import build_py as _build_py
import os

os.environ['CC'] = 'g++'

class build_py(_build_py):
    def run(self):
        self.run_command("build_ext")
        return super().run()

compile_args = ['-std=c++17']
swig_args = ['-c++', '-builtin', '-O']

constants_module = Extension('nwsspc.sharp.calc._constants',
        sources = ['nwsspc/sharp/calc/constants.i'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

interp_module = Extension('nwsspc.sharp.calc._interp',
        sources = ['nwsspc/sharp/calc/interp.i',
                   '../src/sharplib/interp.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

params_module = Extension('nwsspc.sharp.calc._params',
        sources = ['nwsspc/sharp/calc/params.i',
                   '../src/sharplib/profile.cpp', '../src/sharplib/utils.cpp',
                   '../src/sharplib/interp.cpp', '../src/sharplib/winds.cpp',
                   '../src/sharplib/thermo.cpp','../src/sharplib/params.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

parcel_module = Extension('nwsspc.sharp.calc._parcel',
        sources = ['nwsspc/sharp/calc/parcel.i',
                   '../src/sharplib/profile.cpp', '../src/sharplib/utils.cpp',
                   '../src/sharplib/interp.cpp', '../src/sharplib/thermo.cpp',
                   '../src/sharplib/winds.cpp', '../src/sharplib/parcel.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

profile_module = Extension('nwsspc.sharp.calc._profile',
        sources = ['nwsspc/sharp/calc/profile.i',
                   '../src/sharplib/interp.cpp', '../src/sharplib/utils.cpp',
                   '../src/sharplib/thermo.cpp','../src/sharplib/winds.cpp',
                   '../src/sharplib/profile.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

thermo_module = Extension('nwsspc.sharp.calc._thermo',
        sources = ['nwsspc/sharp/calc/thermo.i',
                   '../src/sharplib/utils.cpp',   '../src/sharplib/interp.cpp', 
                   '../src/sharplib/thermo.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

winds_module = Extension('nwsspc.sharp.calc._winds',
        sources=['nwsspc/sharp/calc/winds.i',
                 '../src/sharplib/utils.cpp',   '../src/sharplib/interp.cpp', 
                 '../src/sharplib/winds.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

utils_module = Extension('nwsspc.sharp.calc._utils',
        sources = ['nwsspc/sharp/calc/utils.i',
                   '../src/sharplib/interp.cpp', '../src/sharplib/utils.cpp'],
        include_dirs=['../include'],
        swig_opts = swig_args,
        extra_compile_args = compile_args
    )

setup(
	name="nwsspc_sharp_calc", 
	version="0.0.1",
    cmdclass={'build_py': build_py},
	author="Kelton Halbert",
	author_email="kelton.halbert@noaa.gov",
	description="Used for processing and serving atmospheric sounding data.",
	url="https://github.com/keltonhalbert/SHARP-calc.git",
	classifiers=[
		"Programming Language :: Python :: 3",
		"License :: OSI Approved :: Apache 2.0 License",
		"Operating System :: OS Independent",
	],
	python_requires='>=3.9',
    packages = ["nwsspc.sharp.calc"],
    ext_modules = [constants_module, interp_module, params_module, parcel_module, 
                   profile_module, thermo_module, winds_module, utils_module],
    py_modules = ["nwsspc.sharp.calc.constants", "nwsspc.sharp.calc.interp", 
                  "nwsspc.sharp.calc.params", "nwsspc.sharp.calc.parcel", 
                  "nwsspc.sharp.calc.profile", "nwsspc.sharp.calc.thermo", 
                  "nwsspc.sharp.calc.winds", "nwsspc.sharp.calc.utils"],
)
