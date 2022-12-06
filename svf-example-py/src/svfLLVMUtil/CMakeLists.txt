project(svfLLVMUtil)
llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support )
file (GLOB SOURCES
   llvm_util.cpp
)
ADD_LIBRARY(svfLLVMUtil SHARED ${SOURCES})

target_link_libraries(svfLLVMUtil ${SVF_LIB} ${llvm_libs})
target_link_libraries(svfLLVMUtil ${Z3_LIBRARIES})
set_target_properties(svfLLVMUtil PROPERTIES
                       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )