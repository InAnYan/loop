from setuptools import find_packages, setup

setup(
    name="loop_compiler",
    scripts=["loop_compiler/loopc.py", "loop_compiler/looprun.py", "loop_compiler/looptest.py"],
    packages=find_packages(),
)
