cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Because this build first has to build the pass and THEN the example, allow partial installations
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)

project(
    SVFStandalone
    VERSION 1.0
    DESCRIPTION "An example of how to use SVF as a library in a standalone tool"
    LANGUAGES C CXX)

include(GNUInstallDirs)

message(STATUS "Standalone tool build configuration:
        CMake current source directory:     ${CMAKE_CURRENT_SOURCE_DIR}
        CMake source directory:             ${CMAKE_SOURCE_DIR}
        Project source directory:           ${PROJECT_SOURCE_DIR}
        CMake current binary directory:     ${CMAKE_CURRENT_BINARY_DIR}
        CMake binary directory:             ${CMAKE_BINARY_DIR}
        Project binary directory:           ${PROJECT_BINARY_DIR}")

# Point this to whatever LLVM instance you want to use
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
add_link_options("-fuse-ld=lld")

# Set the C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create an executable for the example program to compile it into a LLVM bitcode file (.ll)
add_executable(example)
target_sources(example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/example.c)

# Enable LTO & create a position independent executable as an example
target_compile_options(example PRIVATE "-flto" "-fPIC")
target_link_options(example PRIVATE "-flto" "-fPIC")

# Enable compiler warnings
target_compile_options(example PRIVATE "-Wall" "-Wextra")
target_link_options(example PRIVATE "-Wall" "-Wextra")

# Add some nice debugging options for nicer output
target_compile_options(
    example
    PRIVATE "-g3"
            "-ggdb3"
            "-gdwarf"
            "-gpubnames"
            "-gcolumn-info"
            "-fdebug-macro"
            "-fstandalone-debug"
            "-fcolor-diagnostics"
            "-fno-discard-value-names"
            "-fno-eliminate-unused-debug-types")
target_link_options(
    example
    PRIVATE
    "-g3"
    "-ggdb3"
    "-gdwarf"
    "-gpubnames"
    "-gcolumn-info"
    "-fdebug-macro"
    "-fstandalone-debug"
    "-fcolor-diagnostics"
    "-fno-discard-value-names"
    "-fno-eliminate-unused-debug-types"
    "-Wl,--demangle"
    "-Wl,--emit-relocs"
    "-Wl,--discard-none"
    "-Wl,--no-gc-sections"
    "-Wl,--export-dynamic"
    "-Wl,--color-diagnostics=always")

install(TARGETS example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)

if(DEFINED LLVM_ROOT OR DEFINED ENV{LLVM_ROOT})
    if(NOT DEFINED LLVM_ROOT)
        set(LLVM_ROOT $ENV{LLVM_ROOT})
    endif()

    message(STATUS "Finding LLVM in custom LLVM root: ${LLVM_ROOT}")
    if(EXISTS "${LLVM_ROOT}/lib/cmake/llvm")
        list(APPEND CMAKE_PREFIX_PATH "${LLVM_ROOT}/lib/cmake/llvm/")
    else()
        list(APPEND CMAKE_PREFIX_PATH "${LLVM_ROOT}")
    endif()
endif()

# Use "HINTS" to prioritise any LLVM instance set in LLVM_DIR over other instances
find_package(
    LLVM
    15.0.7
    CONFIG
    HINTS
    ${LLVM_DIR}
    $ENV{LLVM_DIR}
    PATH_SUFFIXES
    lib
    lib/cmake
    lib/cmake/llvm)
message(STATUS "LLVM STATUS:
        FOUND         ${LLVM_FOUND}
        Version       ${LLVM_PACKAGE_VERSION}
        Definitions   ${LLVM_DEFINITIONS}
        Includes      ${LLVM_INCLUDE_DIRS}
        Libraries     ${LLVM_LIBRARY_DIRS}
        Targets       ${LLVM_TARGETS_TO_BUILD}")

# Set the LLVM header and library paths
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# LLVM is normally built without RTTI; be consistent with that
if(NOT LLVM_ENABLE_RTTI)
    add_compile_options("-fno-rtti")
endif()

if(NOT "${LLVM_FOUND}")
    message(FATAL_ERROR "Failed to find correct LLVM version")
endif()

# Find SVF; technically linking to SVF also links to LLVM due to the transient dependency
# However, since svf-pass uses LLVM itself, it's "nicer" to also explicitly link to LLVM
find_package(SVF CONFIG REQUIRED)

# Define the pass module itself
add_library(SvfPass MODULE)
target_sources(SvfPass PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/svf-pass.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/svf-pass.hpp)

# Define some compiler options for the tool
target_compile_options(
    SvfPass
    PRIVATE "-g3"
            "-ggdb3"
            "-gdwarf"
            "-gpubnames"
            "-gcolumn-info"
            "-ggnu-pubnames"
            "-fdebug-macro"
            "-fstandalone-debug"
            "-fcolor-diagnostics"
            "-fno-discard-value-names"
            "-fno-eliminate-unused-debug-types")
target_link_options(
    SvfPass
    PRIVATE
    "-g3"
    "-ggdb3"
    "-gdwarf"
    "-gpubnames"
    "-gcolumn-info"
    "-ggnu-pubnames"
    "-fdebug-macro"
    "-fstandalone-debug"
    "-fno-discard-value-names"
    "-fno-eliminate-unused-debug-types"
    "-fno-eliminate-unused-debug-symbols"
    "-fcolor-diagnostics"
    "-Wl,--demangle"
    "-Wl,--emit-relocs"
    "-Wl,--discard-none"
    "-Wl,--no-gc-sections"
    "-Wl,--export-dynamic"
    "-Wl,--color-diagnostics=always")

# Enable PIC & LTO on the pass itself
target_compile_options(SvfPass PRIVATE "-fPIC" "-flto=full")
target_link_options(
    SvfPass
    PRIVATE
    "-fPIC"
    "-flto=full"
    "-fuse-ld=lld")

# Enable compiler warnings
target_compile_options(SvfPass PRIVATE "-Wall" "-Wextra")
target_link_options(SvfPass PRIVATE "-Wall" "-Wextra")

# Link the SVF libraries; note that this also links the required LLVM libraries through transitive dependencies
target_link_libraries(SvfPass PRIVATE SVF::SvfCore SVF::SvfLLVM)

# Add arguments to actually run the SVF pass on the module as part of the default pass pipeline(s)
target_compile_options(example PRIVATE "-fpass-plugin=libSvfPass.so")
target_link_options(example PRIVATE "-Wl,--load-pass-plugin=libSvfPass.so")
target_link_directories(example PRIVATE ${CMAKE_SOURCE_DIR}/install/lib)

install(TARGETS SvfPass LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
