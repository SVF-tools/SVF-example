cmake_minimum_required(VERSION 3.23)

project(
  SVF-example
  VERSION 1.0
  DESCRIPTION "An external project example using SVF as a library"
  HOMEPAGE_URL "https://github.com/SVF-tools/SVF-example"
  LANGUAGES C CXX)

# Find the SVF CMake package (pass $SVF_DIR as a (prioritised) hint) Set
# $SVF_DIR to the installation prefix used to install SVF
find_package(
  SVF
  3.0
  REQUIRED
  CONFIG
  HINTS
  ${SVF_DIR}
  $ENV{SVF_DIR})

# Define the primary (minimal) example using SVF as library in an executable
add_executable(svf-example src/svf-example.cpp)

# Only link against SVF; LLVM & Z3 dependencies are resolved internally
target_link_libraries(svf-example PRIVATE SVF::SvfCore SVF::SvfLLVM)

# Set the executable example to install to the local directory (as prefix)
install(TARGETS svf-example RUNTIME DESTINATION bin)

# Generate <prefix>/data/input.ll from src/input.c whenever BinExample is built
add_custom_command(
  TARGET svf-example
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/data
  COMMAND
    clang -S -c -Xclang -disable-O0-optnone -fno-discard-value-names -emit-llvm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/input.c -o
    ${CMAKE_INSTALL_PREFIX}/data/input.ll
  COMMAND opt -S -p=mem2reg ${CMAKE_INSTALL_PREFIX}/data/input.ll -o
          ${CMAKE_INSTALL_PREFIX}/data/input.ll
  COMMENT "→ generating LLVM IR for src/input.c → data/input.ll")
