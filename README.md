# Using SVF as a Library

This repository shows how to use SVF as a library in your own tools; the repository contains an example
of how to use SVF to build standalone tools, as well as how to use SVF in a custom LLVM pass as part
of the LLVM default compiler pass pipeline.

## Assumptions

This example assumes SVF has been built and installed somewhere on the system, and the environment has
been set correctly to locate the SVF package. The following must be true:

- SVF is cloned into `$SVF_DIR`
- SVF has been built as a `Release-build` in `$SVF_DIR/Release-build`
- SVF has been installed into `$SVF_DIR/install`
- `$SVF_DIR/install/bin` has been added to `$PATH`
- `$SVF_DIR/install/lib` has been added to `$LD_LIBRARY_PATH` and `$LD_RUN_PATH` (the latter is mostly optional)
- `$SVF_DIR/install/include` has been added to `$C_INCLUDE_PATH` and `$CPLUS_INCLUDE_PATH`
- `$SVF_DIR/install/lib` (or `$SVF_DIR/install/lib/cmake`/`$SVF_DIR/install/lib/cmake/SVF`) has been added to `$CMAKE_PREFIX_PATH`

## Configuration

This example shows how to use SVF in a custom LLVM pass; as such, the build system is assumed to be LLVM.
Alternatively, `gcc` can be used to compile standalone SVF tools.
