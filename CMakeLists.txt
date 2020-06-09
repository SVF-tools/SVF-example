cmake_minimum_required(VERSION 3.4.3)

if (DEFINED LLVM_DIR)
        set(ENV{LLVM_DIR} "${LLVM_DIR}")
    endif()
    if (DEFINED ENV{LLVM_DIR})
        # We need to match the build environment for LLVM:
        # In particular, we need C++11 and the -fno-rtti flag
        set(CMAKE_CXX_STANDARD 14)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++14 -O0 -fno-rtti -Wno-deprecated")
	else()
		set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++14 -O3 -fno-rtti -Wno-deprecated")
	endif()
	set(CMAKE_C_FLAGS "-fPIC")

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
if (UNIX AND NOT APPLE)
    MESSAGE (STATUS "building SVF on Ubuntu")
    set(SVF_LIB "${SVF_DIR}/libsvf_ubuntu.a")
    set(LLVMCudd "${SVF_DIR}/libCudd_ubuntu.a")
elseif (APPLE)
    MESSAGE (STATUS "building SVF on Mac")
    set(SVF_LIB "${SVF_DIR}/libsvf_mac.a")
    set(LLVMCudd "${SVF_DIR}/libCudd_mac.a")
endif()
set(SVF_HEADER "${SVF_DIR}/include")

if (EXISTS "${SVF_HEADER}" AND EXISTS "${SVF_LIB}")
    message(STATUS "SVF_HEADER: ${SVF_HEADER}")
    message(STATUS "SVF_LIB: ${SVF_LIB}")
else()
    message(FATAL_ERROR "${SVF_LIB} or ${SVF_HEADER} not exists")
    endif()
    include_directories(${SVF_HEADER})



add_subdirectory(src)
