cmake_minimum_required(VERSION 3.23)

# Define the SVF example project
project(svf-ex
    VERSION 1.0
    DESCRIPTION "Example project for how to use SVF as an external library"
    HOMEPAGE_URL "https://github.com/SVF-tools/SVF-example"
    LANGUAGES C CXX)

# Configure C & C++ standards for example project (doens't need to match LLVM/SVF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Position Independent Code for generated binaries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to release-build mode
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build mode specific; defaulting to \"Release\" build mode")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add "-g" and no optimisation level if building in debug mode, otherwise add "-O3"
set(CMAKE_C_FLAGS_DEBUG "-g ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "-O3 ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")

# Ignore deprecated warnings; for the rest treat compiler warnings as errors
add_compile_options("-Wall" "-Wextra" "-Wno-deprecated")

# Find LLVM and the necessary build configuration for LLVM 15 (replace with different version if needed)
find_package(LLVM REQUIRED CONFIG HINTS ${LLVM_DIR} ENV LLVM_DIR)
message(STATUS "LLVM STATUS:
    Found:                      ${LLVM_FOUND}
    Version:                    ${LLVM_VERSION}
    Definitions:                ${LLVM_DEFINITIONS}
    Enable RTTI:                ${LLVM_ENABLE_RTTI}
    Enable exceptions:          ${LLVM_ENABLE_EH}
    Include directories:        ${LLVM_INCLUDE_DIRS}
    Library directories:        ${LLVM_LIBRARY_DIRS}
    LLVM targets to build:      ${LLVM_TARGETS_TO_BUILD}")

if(NOT "${LLVM_FOUND}")
    message(FATAL_ERROR "Failed to find supported LLVM version")
endif()

# Add LLVM's include directories and link directory for all targets defined hereafter
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Check if LLVM was built generating the single libLLVM.so shared library file or as separate static libraries
if(LLVM_LINK_LLVM_DYLIB)
    message(STATUS "Linking to LLVM dynamic shared library object")
    set(llvm_libs LLVM)
else()
    message(STATUS "Linking to separate LLVM static libraries")
    llvm_map_components_to_libnames(llvm_libs
        bitwriter
        core
        ipo
        irreader
        instcombine
        instrumentation
        target
        linker
        analysis
        scalaropts
        support
    )
endif()

# Make the "add_llvm_library()" command available and configure LLVM/CMake
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# LLVM is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# The same applies for exception handling
if(NOT LLVM_ENABLE_EH)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
endif()

# Find specifically SVF 2.7 (change if needed) prioritising locations pointed to by $SVF_DIR
find_package(SVF CONFIG HINTS ${SVF_DIR} ENV SVF_DIR)
message(STATUS "SVF STATUS:
    Found:                              ${SVF_FOUND}
    Version:                            ${SVF_VERSION}
    Build mode:                         ${SVF_BUILD_TYPE}
    C++ standard:                       ${SVF_CXX_STANDARD}
    RTTI enabled:                       ${SVF_ENABLE_RTTI}
    Exceptions enabled:                 ${SVF_ENABLE_EXCEPTIONS}
    Install root directory:             ${SVF_INSTALL_ROOT}
    Install binary directory:           ${SVF_INSTALL_BIN_DIR}
    Install library directory:          ${SVF_INSTALL_LIB_DIR}
    Install include directory:          ${SVF_INSTALL_INCLUDE_DIR}
    Install 'extapi.bc' file path:      ${SVF_INSTALL_EXTAPI_FILE}")

# If the SVF CMake package was found, show how to use some "modern" features of this approach; otherwise use old system
if(NOT "${SVF_FOUND}")
    message(STATUS "Found installed SVF instance; importing using modern CMake methods")

    # Print a warning if the build types are mismatched
    if(NOT (${SVF_BUILD_TYPE} STREQUAL ${CMAKE_BUILD_TYPE}))
        message(WARNING "Current & SVF build types don't match (SVF: ${SVF_BUILD_TYPE}, current: ${CMAKE_BUILD_TYPE})!")
    endif()

    # Check that SVF & the found LLVM instance match w.r.t. RTTI/exception handling support
    if(NOT (${SVF_ENABLE_RTTI} STREQUAL ${LLVM_ENABLE_RTTI}))
        message(FATAL_ERROR "SVF & LLVM RTTI support mismatch (SVF: ${SVF_ENABLE_RTTI}, LLVM: ${LLVM_ENABLE_RTTI})!")
    endif()
    if(NOT (${SVF_ENABLE_EXCEPTIONS} STREQUAL ${LLVM_ENABLE_EH}))
        message(FATAL_ERROR "SVF & LLVM exceptions support mismatch (SVF: ${SVF_ENABLE_EXCEPTIONS}, LLVM: ${LLVM_ENABLE_EH})!")
    endif()

    # Include SVF's include directories for all targets & include the library directories to find the library objects
    include_directories(SYSTEM ${SVF_INSTALL_INCLUDE_DIR})
    link_directories(${SVF_INSTALL_LIB_DIR})
else()
    message(STATUS "Failed to find installed SVF instance; using legacy import method")

    if (EXISTS "${SVF_DIR}")
    else()
        set(SVF_DIR $ENV{SVF_DIR})
        if(EXISTS "${SVF_DIR}")
        else()
        message(FATAL_ERROR
            "WARNING: The SVF_DIR var was not set (required for an out-of-source build)!
            Please set this to environment variable to point to the SVF_DIR directory or set this variable to cmake configuration
            (e.g. on linux: export SVF_DIR=/path/to/SVF/dir) or (make the project via: cmake -DSVF_DIR=your_path_to_SVF)")
        endif()
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        MESSAGE (STATUS "building SVF in debug mode")
        if (EXISTS "${SVF_DIR}/Debug-build")
            set(SVF_BIN "${SVF_DIR}/Debug-build")
        else()
            set(SVF_BIN "${SVF_DIR}/Release-build")
        endif()
    else()
        MESSAGE (STATUS "building SVF in release mode")
        set(SVF_BIN "${SVF_DIR}/Release-build")
    endif()
    set(SVF_HEADER "${SVF_DIR}/svf/include")
    set(SVF_LLVM_HEADER "${SVF_DIR}/svf-llvm/include")
    set(SVF_LIB "${SVF_BIN}/svf-llvm/libSvfLLVM.a" "${SVF_BIN}/svf/libSvfCore.a")
    set(SVF_BIN_HEADER "${SVF_BIN}/include")
    include_directories(${SVF_HEADER}
                        ${SVF_LLVM_HEADER}
                        ${SVF_BIN_HEADER})
endif()

# Search for system Z3 with CMake support first; otherwise try to find Z3 downloaded/installed by SVF's build script
find_package(Z3 REQUIRED CONFIG PATHS ${Z3_DIR} ENV Z3_DIR)
message(STATUS "Z3 STATUS:
    Z3 found:                   ${Z3_FOUND}
    Z3 version:                 ${Z3_VERSION}
    Z3 libraries:               ${Z3_LIBRARIES}
    Z3 include directory:       ${Z3_CXX_INCLUDE_DIRS}")

if(Z3_FOUND)
    include_directories(SYSTEM ${Z3_CXX_INCLUDE_DIRS})
else()
    message(STATUS "No system Z3 CMake package found; using SVF's Z3 instance")
    find_library(Z3_LIBRARIES
        NAMES libz3.a libz3.so
        HINTS ${Z3_DIR} ENV Z3_DIR
        PATH_SUFFIXES bin
    )
    find_path(Z3_INCLUDES
        NAMES z3++.h
        HINTS ${Z3_DIR} ENV Z3_DIR
        PATH_SUFFIXES include
    )

    # Ensure this Z3 instance was actually found
    if(NOT Z3_LIBRARIES OR NOT Z3_INCLUDES)
        message(FATAL_ERROR "Failed to find system Z3/SVF's Z3 instance!")
    endif()

    # Getting Z3 from GitHub places compiled library files in /bin, so add that as a search directory
    include_directories(SYSTEM ${Z3_INCLUDES})
    link_directories(${Z3_DIR}/bin)
endif()

add_subdirectory(src)
