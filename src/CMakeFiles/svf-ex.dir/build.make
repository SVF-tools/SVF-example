# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.18.4/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.18.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/charles/UTS/SVF-all/charles-patch/SVF-example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/charles/UTS/SVF-all/charles-patch/SVF-example

# Include any dependencies generated for this target.
include src/CMakeFiles/svf-ex.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/svf-ex.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/svf-ex.dir/flags.make

src/CMakeFiles/svf-ex.dir/svf-ex.cpp.o: src/CMakeFiles/svf-ex.dir/flags.make
src/CMakeFiles/svf-ex.dir/svf-ex.cpp.o: src/svf-ex.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/charles/UTS/SVF-all/charles-patch/SVF-example/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/svf-ex.dir/svf-ex.cpp.o"
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/svf-ex.dir/svf-ex.cpp.o -c /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src/svf-ex.cpp

src/CMakeFiles/svf-ex.dir/svf-ex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/svf-ex.dir/svf-ex.cpp.i"
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src/svf-ex.cpp > CMakeFiles/svf-ex.dir/svf-ex.cpp.i

src/CMakeFiles/svf-ex.dir/svf-ex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/svf-ex.dir/svf-ex.cpp.s"
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src/svf-ex.cpp -o CMakeFiles/svf-ex.dir/svf-ex.cpp.s

# Object files for target svf-ex
svf__ex_OBJECTS = \
"CMakeFiles/svf-ex.dir/svf-ex.cpp.o"

# External object files for target svf-ex
svf__ex_EXTERNAL_OBJECTS =

bin/svf-ex: src/CMakeFiles/svf-ex.dir/svf-ex.cpp.o
bin/svf-ex: src/CMakeFiles/svf-ex.dir/build.make
bin/svf-ex: /Users/charles/node_modules/SVF/Release-build/lib/libSvf.a
bin/svf-ex: /Users/charles/node_modules/SVF/Release-build/lib/CUDD/libCudd.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMBitWriter.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMCore.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMipo.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMIRReader.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMInstCombine.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMInstrumentation.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMTarget.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMLinker.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMAnalysis.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMScalarOpts.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMSupport.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMBitWriter.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMAsmParser.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMInstCombine.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMAggressiveInstCombine.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMVectorize.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMTransformUtils.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMAnalysis.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMObject.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMBitReader.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMMCParser.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMTextAPI.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMProfileData.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMCore.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMRemarks.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMBitstreamReader.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMMC.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMBinaryFormat.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMDebugInfoCodeView.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMDebugInfoMSF.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMSupport.a
bin/svf-ex: /Users/charles/llvm-10.0.0.obj/lib/libLLVMDemangle.a
bin/svf-ex: src/CMakeFiles/svf-ex.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/charles/UTS/SVF-all/charles-patch/SVF-example/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/svf-ex"
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/svf-ex.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/svf-ex.dir/build: bin/svf-ex

.PHONY : src/CMakeFiles/svf-ex.dir/build

src/CMakeFiles/svf-ex.dir/clean:
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src && $(CMAKE_COMMAND) -P CMakeFiles/svf-ex.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/svf-ex.dir/clean

src/CMakeFiles/svf-ex.dir/depend:
	cd /Users/charles/UTS/SVF-all/charles-patch/SVF-example && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/charles/UTS/SVF-all/charles-patch/SVF-example /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src /Users/charles/UTS/SVF-all/charles-patch/SVF-example /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src /Users/charles/UTS/SVF-all/charles-patch/SVF-example/src/CMakeFiles/svf-ex.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/svf-ex.dir/depend
