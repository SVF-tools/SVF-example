cmake_minimum_required(VERSION 3.4.3)

if (DEFINED ENV{LLVM_DIR})
    # We need to match the build environment for LLVM:
    # In particular, we need C++11 and the -fno-rtti flag
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CMAKE_CXX_FLAGS "-O0 -fno-rtti")
else()
	set(CMAKE_CXX_FLAGS "-O3 -fno-rtti")
endif()

find_package(LLVM REQUIRED CONFIG)
    
    list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
    include(AddLLVM)

    add_definitions(${LLVM_DEFINITIONS})
    include_directories(${LLVM_INCLUDE_DIRS})

else()
    message(FATAL_ERROR "\
WARNING: The LLVM_DIR var was not set (required for an out-of-source build)!\n\
Please set this to environment variable to point to the LLVM build directory\
(e.g. on linux: export LLVM_DIR=/path/to/llvm/build/dir)")
endif()

if (DEFINED ENV{SVF_HOME})
    message(STATUS "SVF_HOME: $ENV{SVF_HOME}")
    set(SVF_HEADER "$ENV{SVF_HOME}/include")
    set(SVF_LIB "$ENV{SVF_HOME}/Release-build/lib")
    if (EXISTS "${SVF_HEADER}" AND EXISTS "${SVF_LIB}")
        message(STATUS "SVF_HEADER: ${SVF_HEADER}")
        message(STATUS "SVF_LIB: ${SVF_LIB}")
    else()
        message(FATAL_ERROR "${SVF_LIB} or ${SVF_HEADER} not exists")
    endif()
    include_directories(${SVF_HEADER})
    link_directories(${SVF_LIB} ${SVF_LIB}/CUDD)
else()
    message(FATAL_ERROR "Please point SVF_HOME to your SVF source folder")
endif()



add_subdirectory(tools)
