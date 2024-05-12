# SVF as an LLVM pass

This example shows how to integrate an SVF analysis routine into an LLVM compiler pass that runs as
part of the default compiler pass pipeline. Alternatively, these passes can be run on individual
modules using LLVM's `opt` command.

## Build dependency

Because the example target (`src/example.c`) will be compiled with a custom SVF analysis run during
the default compiler pass pipeline, the pass module has to be compiled and installed before running
the pass on the example target. These steps have been automated with the `./run_pass.sh` script.

## SVF

This example assumes the following:
- SVF is cloned into `$SVF_DIR`
- SVF has been built as a `Release-build` in `$SVF_DIR/Release-build`
- SVF has been installed into `$SVF_DIR/install`
- `$SVF_DIR/install/bin` has been added to `$PATH`
- `$SVF_DIR/install/lib` has been added to `$LD_LIBRARY_PATH` and `$LD_RUN_PATH` (the latter is mostly optional)
- `$SVF_DIR/install/include` has been added to `$C_INCLUDE_PATH` and `$CPLUS_INCLUDE_PATH`
- `$SVF_DIR/install/lib` (or `$SVF_DIR/install/lib/cmake`/`$SVF_DIR/install/lib/cmake/SVF`) has been added to `$CMAKE_PREFIX_PATH`
