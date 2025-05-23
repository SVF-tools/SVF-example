# Using SVF as a Library

This repository shows how to build, install, and use SVF as a library in a project to analyse some
LLVM bitcode files. For more information on setting up SVF, see
[the wiki](https://github.com/svf-tools/SVF/wiki/Setup-Guide#getting-started).

## Building & Installing SVF

This repository builds & installs SVF using its primary build system: CMake. To use SVF with a different
build system, ensure its library files (i.e., `libSvfCore.[a|so]` & `libSvfLLVM.[a|so]`) are properly
defined and available, as well as SVF's header files.

### Prerequisites & Dependencies

SVF relies on the following libraries & tools:

- CMake (>=3.23)
- LLVM (<=16.0.6)
- Z3 (>=4.8.8)

If you've installed SVF's dependencies from the official repositories (e.g., `apt`), SVF's build system
should find them out of the box. If you've manually built & installed the dependencies (particularly
to non-standard installation locations), ensure they are accessible (i.e., by setting the environment
variables `$PATH` and `$LD_LIBRARY_PATH`).

To use a specific LLVM/Z3 instance, define their installation roots in `$LLVM_DIR` and `$Z3_DIR`, or
`$LLVM_HOME` and `$Z3_HOME`.

### Configuring, Building, and Installing SVF

The SVF CMake build system supports multiple build flags; see the top-level `CMakeLists.txt` for more
information about the supported build flags.

To clone SVF to the user's home direction and create a good default configuration to install SVF to the
user's local installation tree, use the following as an example:

```shell
# Can be set globally/persistently (e.g., in ~/.bashrc)
SVF_DIR=${SVF_DIR:-$HOME/SVF}

# Clone SVF
cd $SVF_DIR
git clone https://github.com/SVF-tools/SVF.git ./

# Configure, build, and then install SVF to $SVF_DIR (--verbose outputs during build/install)
cmake -G "Ninja" -S ./ -B Release-build --install-prefix="$(realpath $SVF_DIR)"
cmake --build Release-build --verbose
cmake --install Release-build --verbose
```

### CMake Package Finding

CMake uses `$<PACKAGE>_DIR` variables to locate packages by default; as `$SVF_DIR` is set to the
installation prefix, it should be able to locate SVF's CMake package. If you installed to a
non-standard installation prefix (different from `$SVF_DIR`), ensure CMake can locate the
package by adding the installation prefix to `$CMAKE_PREFIX_PATH`. On most platforms,
this can be done in one of the following ways:

```shell
# For Bash/Zsh shells (add to config (e.g., ~/.profile, ~/.bashrc,
# ~/.zshrc, ~/.zshenv, etc.) to make this setting persistent:
export CMAKE_PREFIX_PATH="$SVF_DIR:$CMAKE_PREFIX_PATH"

# To avoid the trailing ':' if $CMAKE_PREFIX_PATH is unset, use:
export CMAKE_PREFIX_PATH="$SVF_DIR${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"

# For Fish shell (add -U to make this persistent; use -a to append instead):
set -px --path CMAKE_LIBRARY_PATH $SVF_DIR
```

### Without Using the CMake Package

To make SVF's tools available on the command line, or to use SVF from another build system, ensure its
headers and library files are globally locatable. If you installed SVF to one of the default install
prefixes (e.g., `/usr`, `/usr/local`, etc.), this should already be the case. If you installed SVF
to a non-standard location, ensure the appropriate paths are added to their respective variables:

```shell
# Ensure SVF is available; use earlier notation to avoid trailing ':'; flip to append instead
export LD_LIBRARY_PATH="$SVF_DIR/lib:$LD_LIBRARY_PATH"
export PATH="$SVF_DIR/bin:$PATH"

# Or, for Fish (use -U to make persistent; use -a to append instead):
set -Upx LD_LIBRARY_PATH $SVF_DIR/lib
fish_add_path --prepend $SVF_DIR/bin
```

## Building the Executable Example

The top-level `CMakeLists.txt` in this directory shows how to locate & link against SVF's CMake package.
To configure, build, and install the examples locally (to this directory), run the following:

```shell
rm -rf build bin lib data
cmake -G "Ninja" -B build -S ./ --install-prefix=$(pwd)
cmake --build build --verbose
cmake --install build --verbose
```

## Using the Executable Example

The example executable can now be used like this:

```shell
./bin/svf-example ./data/example.ll
```
