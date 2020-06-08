llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support )
add_executable( wpa wpa.cpp )

set(LLVMCudd "$ENV{LLVM_CUDD}")
target_link_libraries( wpa ${SVF_LIB} ${LLVMCudd} ${llvm_libs})

set_target_properties( wpa PROPERTIES
                       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
