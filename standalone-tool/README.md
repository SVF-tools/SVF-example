# SVF in Standalone Tools

This example shows how to integrate SVF into a custom out-of-tree standalone tool. These tools can
be compiled into executable binaries, which can be run on specific LLVM IR files (textual or binary
bitcode). The included `CMakeLists.txt` file shows how to find the SVF CMake package and link
against SVF's libraries.

## SVF

This example assumes the following:
- SVF is cloned into `$SVF_DIR`
- SVF has been built as a `Release-build` in `$SVF_DIR/Release-build`
- SVF has been installed into `$SVF_DIR/install`
- `$SVF_DIR/install/bin` has been added to `$PATH`
- `$SVF_DIR/install/lib` has been added to `$LD_LIBRARY_PATH` and `$LD_RUN_PATH` (the latter is mostly optional)
- `$SVF_DIR/install/include` has been added to `$C_INCLUDE_PATH` and `$CPLUS_INCLUDE_PATH`
- `$SVF_DIR/install/lib` (or `$SVF_DIR/install/lib/cmake`/`$SVF_DIR/install/lib/cmake/SVF`) has been added to `$CMAKE_PREFIX_PATH`
