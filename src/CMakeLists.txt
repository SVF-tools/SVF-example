add_executable(svf-example svf-ex.cpp)
target_link_libraries(svf-example ${llvm_libs} ${SVF_LIB})
